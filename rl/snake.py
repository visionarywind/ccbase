import pygame
import random
import numpy as np

# 初始化游戏
pygame.init()

# 游戏参数
WIDTH, HEIGHT = 400, 400  # 窗口大小
GRID_SIZE = 20  # 格子大小
GRID_WIDTH = WIDTH // GRID_SIZE  # 横向格子数
GRID_HEIGHT = HEIGHT // GRID_SIZE  # 纵向格子数

# 颜色定义
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)    # 食物
GREEN = (0, 255, 0)  # 蛇头
BLUE = (0, 0, 255)   # 蛇身

# 方向定义（上、右、下、左）
UP = (0, -1)
RIGHT = (1, 0)
DOWN = (0, 1)
LEFT = (-1, 0)

# 创建窗口
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("强化学习玩贪吃蛇")
clock = pygame.time.Clock()


class SnakeEnv:
    def __init__(self):
        self.reset()  # 初始化游戏状态
    
    def reset(self):
        # 重置蛇的位置（初始在中间，长度为1）
        self.snake = [(GRID_WIDTH // 2, GRID_HEIGHT // 2)]
        self.head = self.snake[0]
        self.direction = RIGHT  # 初始方向向右
        self.food = self.generate_food()  # 生成食物
        self.score = 0
        self.done = False  # 游戏是否结束
        return self.get_state()  # 返回初始状态
    
    def generate_food(self):
        # 随机生成食物（不在蛇身上）
        while True:
            food = (random.randint(0, GRID_WIDTH-1), random.randint(0, GRID_HEIGHT-1))
            if food not in self.snake:
                return food
    
    def get_state(self):
        # 状态表示（简化版）：蛇头周围的情况 + 食物相对位置
        head_x, head_y = self.head
        food_x, food_y = self.food
        
        # 1. 蛇头周围是否有障碍物（墙或自身）
        danger_front = self.is_collision((head_x + self.direction[0], head_y + self.direction[1]))
        danger_right = self.is_collision((head_x + RIGHT[0], head_y + RIGHT[1]))
        danger_left = self.is_collision((head_x + LEFT[0], head_y + LEFT[1]))
        
        # 2. 食物相对位置（上/下/左/右）
        food_up = 1 if food_y < head_y else 0
        food_down = 1 if food_y > head_y else 0
        food_left = 1 if food_x < head_x else 0
        food_right = 1 if food_x > head_x else 0
        
        # 3. 当前方向（用独热编码表示）
        dir_up = 1 if self.direction == UP else 0
        dir_right = 1 if self.direction == RIGHT else 0
        dir_down = 1 if self.direction == DOWN else 0
        dir_left = 1 if self.direction == LEFT else 0
        
        # 合并状态（共11个特征）
        state = (danger_front, danger_right, danger_left,
                 food_up, food_down, food_left, food_right,
                 dir_up, dir_right, dir_down, dir_left)
        return state
    
    def is_collision(self, pos):
        # 判断位置是否碰撞（撞墙或撞自身）
        x, y = pos
        return (x < 0 or x >= GRID_WIDTH or  # 撞墙
                y < 0 or y >= GRID_HEIGHT or  # 撞墙
                pos in self.snake)  # 撞自身
    
    def step(self, action):
        # 根据动作更新方向（0:直走, 1:右转, 2:左转）
        if action == 0:  # 直走（方向不变）
            new_dir = self.direction
        elif action == 1:  # 右转（顺时针转90度）
            if self.direction == UP:
                new_dir = RIGHT
            elif self.direction == RIGHT:
                new_dir = DOWN
            elif self.direction == DOWN:
                new_dir = LEFT
            else:  # LEFT
                new_dir = UP
        else:  # 左转（逆时针转90度）
            if self.direction == UP:
                new_dir = LEFT
            elif self.direction == LEFT:
                new_dir = DOWN
            elif self.direction == DOWN:
                new_dir = RIGHT
            else:  # RIGHT
                new_dir = UP
        self.direction = new_dir
        
        # 移动蛇头
        new_head = (self.head[0] + self.direction[0], self.head[1] + self.direction[1])
        
        # 判断是否碰撞（惩罚）
        if self.is_collision(new_head):
            self.done = True
            reward = -10  # 撞墙/自身：大惩罚
        else:
            # 移动蛇身
            self.snake.insert(0, new_head)
            self.head = new_head
            # 判断是否吃到食物（奖励）
            if self.head == self.food:
                self.score += 1
                reward = 10  # 吃到食物：大奖励
                self.food = self.generate_food()  # 重新生成食物
            else:
                self.snake.pop()  # 没吃到食物，尾部前进
                reward = 0  # 普通移动：无奖励
        
        return self.get_state(), reward, self.done
    
    def render(self):
        # 绘制游戏画面
        screen.fill(BLACK)
        # 画蛇头
        pygame.draw.rect(screen, GREEN, (self.head[0]*GRID_SIZE, self.head[1]*GRID_SIZE, GRID_SIZE-1, GRID_SIZE-1))
        # 画蛇身
        for segment in self.snake[1:]:
            pygame.draw.rect(screen, BLUE, (segment[0]*GRID_SIZE, segment[1]*GRID_SIZE, GRID_SIZE-1, GRID_SIZE-1))
        # 画食物
        pygame.draw.rect(screen, RED, (self.food[0]*GRID_SIZE, self.food[1]*GRID_SIZE, GRID_SIZE-1, GRID_SIZE-1))
        pygame.display.update()


class QLearningAgent:
    def __init__(self):
        self.epsilon = 0.9  # 探索率（90%概率选已知最优动作，10%随机探索）
        self.alpha = 0.1    # 学习率（新经验对Q值的影响程度）
        self.gamma = 0.9    # 折扣因子（未来奖励的重要性）
        self.q_table = {}   # Q表：key是状态，value是动作对应的Q值
    
    def get_action(self, state):
        # 根据当前状态选动作（ε-贪婪策略）
        if state not in self.q_table:
            self.q_table[state] = [0.0, 0.0, 0.0]  # 初始化3个动作的Q值（直走/右转/左转）
        
        # 有ε概率随机探索，否则选Q值最高的动作
        if random.random() < self.epsilon:
            return np.argmax(self.q_table[state])  # 选最优动作
        else:
            return random.choice([0, 1, 2])  # 随机探索
    
    def learn(self, state, action, reward, next_state, done):
        # 更新Q表（核心！Q-learning公式）
        if state not in self.q_table:
            self.q_table[state] = [0.0, 0.0, 0.0]
        if next_state not in self.q_table:
            self.q_table[next_state] = [0.0, 0.0, 0.0]
        
        # 当前Q值
        current_q = self.q_table[state][action]
        # 下一状态的最大Q值（如果游戏结束，下一状态价值为0）
        next_max_q = np.max(self.q_table[next_state]) if not done else 0
        # 计算目标Q值（贝尔曼方程）
        target_q = reward + self.gamma * next_max_q
        # 更新Q值：当前Q值 + 学习率*(目标Q值 - 当前Q值)
        self.q_table[state][action] = current_q + self.alpha * (target_q - current_q)


# 训练AI
def train(episodes=1000):
    env = SnakeEnv()
    agent = QLearningAgent()
    total_rewards = []  # 记录每局的总奖励
    
    for episode in range(episodes):
        state = env.reset()
        done = False
        total_reward = 0
        
        while not done:
            # 1. 选动作
            action = agent.get_action(state)
            # 2. 执行动作，得到反馈
            next_state, reward, done = env.step(action)
            # 3. 学习（更新Q表）
            agent.learn(state, action, reward, next_state, done)
            # 4. 更新状态和总奖励
            state = next_state
            total_reward += reward
            
            # 每100局可视化一次（加快训练速度）
            if episode % 100 == 0:
                env.render()
                clock.tick(10)  # 控制帧率
        
        total_rewards.append(total_reward)
        # 打印训练进度
        if episode % 100 == 0:
            print(f"第{episode}局，总奖励：{total_reward}，蛇长度：{len(env.snake)}")
    
    print("训练完成！")
    pygame.quit()
    return agent, total_rewards


# 运行训练（1000局）
if __name__ == "__main__":
    train(1000000)
