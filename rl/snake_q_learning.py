import pygame
import random
import numpy as np

# 初始化游戏
pygame.init()

# 环境参数
GRID_SIZE = 50  # 每个格子大小
WORLD_SIZE = 10  # 10x10的格子世界
SCREEN_SIZE = GRID_SIZE * WORLD_SIZE
screen = pygame.display.set_mode((SCREEN_SIZE, SCREEN_SIZE))
pygame.display.set_caption("Q-learning应对随机食物")
clock = pygame.time.Clock()

# 颜色定义
WHITE = (255, 255, 255)    # 空地
RED = (255, 0, 0)          # 食物
GREEN = (0, 255, 0)        # AI
GRAY = (100, 100, 100)     # 墙壁（边界）

# 动作定义（上下左右）
UP = 0
RIGHT = 1
DOWN = 2
LEFT = 3
ACTIONS = [UP, RIGHT, DOWN, LEFT]


class FoodWorld:
    def __init__(self):
        self.reset()

    def reset(self):
        # 随机初始化AI位置（不在边界）
        self.agent_pos = (
            random.randint(1, WORLD_SIZE-2),
            random.randint(1, WORLD_SIZE-2)
        )
        # 随机生成食物（不在AI位置）
        self.food_pos = self._generate_food()
        self.done = False
        return self.get_state()

    def _generate_food(self):
        # 确保食物不在AI当前位置
        while True:
            food = (
                random.randint(1, WORLD_SIZE-2),  # 食物也不在边界，避免AI卡墙
                random.randint(1, WORLD_SIZE-2)
            )
            if food != self.agent_pos:
                return food

    def get_state(self):
        # 状态定义：AI位置 + 食物相对位置（核心！应对食物随机性）
        ax, ay = self.agent_pos
        fx, fy = self.food_pos

        # 食物相对AI的方向（上/下/左/右）
        food_up = 1 if fy < ay else 0
        food_down = 1 if fy > ay else 0
        food_left = 1 if fx < ax else 0
        food_right = 1 if fx > ax else 0

        # AI是否靠近墙壁（上下左右是否有墙）
        wall_up = 1 if ay == 0 else 0
        wall_down = 1 if ay == WORLD_SIZE-1 else 0
        wall_left = 1 if ax == 0 else 0
        wall_right = 1 if ax == WORLD_SIZE-1 else 0

        # 合并状态（共8个特征，全是相对信息，与绝对位置无关）
        return (food_up, food_down, food_left, food_right,
                wall_up, wall_down, wall_left, wall_right)

    def step(self, action):
        ax, ay = self.agent_pos
        # 根据动作移动AI
        if action == UP:
            new_pos = (ax, ay - 1)
        elif action == RIGHT:
            new_pos = (ax + 1, ay)
        elif action == DOWN:
            new_pos = (ax, ay + 1)
        else:  # LEFT
            new_pos = (ax - 1, ay)

        # 奖励机制
        if new_pos == self.food_pos:
            # 吃到食物：大奖励，食物刷新
            reward = 10
            self.agent_pos = new_pos
            self.food_pos = self._generate_food()  # 食物随机刷新（关键！）
        elif (new_pos[0] < 0 or new_pos[0] >= WORLD_SIZE or
              new_pos[1] < 0 or new_pos[1] >= WORLD_SIZE):
            # 撞墙：大惩罚，回合结束
            reward = -10
            self.done = True
        else:
            # 普通移动：小惩罚（鼓励尽快找到食物）
            reward = -0.1
            self.agent_pos = new_pos

        return self.get_state(), reward, self.done

    def render(self):
        # 绘制环境
        screen.fill(WHITE)
        # 画墙壁（边界）
        for i in range(WORLD_SIZE):
            pygame.draw.rect(screen, GRAY, (i*GRID_SIZE, 0, GRID_SIZE, GRID_SIZE))  # 上边界
            pygame.draw.rect(screen, GRAY, (i*GRID_SIZE, (WORLD_SIZE-1)*GRID_SIZE, GRID_SIZE, GRID_SIZE))  # 下边界
            pygame.draw.rect(screen, GRAY, (0, i*GRID_SIZE, GRID_SIZE, GRID_SIZE))  # 左边界
            pygame.draw.rect(screen, GRAY, ((WORLD_SIZE-1)*GRID_SIZE, i*GRID_SIZE, GRID_SIZE, GRID_SIZE))  # 右边界
        # 画AI
        ax, ay = self.agent_pos
        pygame.draw.rect(screen, GREEN, (ax*GRID_SIZE, ay*GRID_SIZE, GRID_SIZE-2, GRID_SIZE-2))
        # 画食物
        fx, fy = self.food_pos
        pygame.draw.rect(screen, RED, (fx*GRID_SIZE, fy*GRID_SIZE, GRID_SIZE-2, GRID_SIZE-2))
        pygame.display.update()


class QLearningAgent:
    def __init__(self):
        self.epsilon = 0.9  # 90%选最优动作，10%探索
        self.alpha = 0.1    # 学习率
        self.gamma = 0.9    # 折扣因子
        self.q_table = {}   # 存储状态-动作的Q值

    def get_action(self, state):
        # 初始化状态的Q值（如果没见过这个状态）
        if state not in self.q_table:
            self.q_table[state] = [0.0 for _ in ACTIONS]
        # ε-贪婪策略选动作
        if random.random() < self.epsilon:
            return np.argmax(self.q_table[state])  # 选Q值最高的动作
        else:
            return random.choice(ACTIONS)  # 随机探索

    def learn(self, state, action, reward, next_state, done):
        # 确保状态在Q表中
        if state not in self.q_table:
            self.q_table[state] = [0.0 for _ in ACTIONS]
        if next_state not in self.q_table:
            self.q_table[next_state] = [0.0 for _ in ACTIONS]
        # Q值更新公式（贝尔曼方程）
        current_q = self.q_table[state][action]
        next_max_q = np.max(self.q_table[next_state]) if not done else 0
        target_q = reward + self.gamma * next_max_q
        self.q_table[state][action] = current_q + self.alpha * (target_q - current_q)


def train(episodes=1000):
    env = FoodWorld()
    agent = QLearningAgent()
    for episode in range(episodes):
        state = env.reset()
        done = False
        total_reward = 0
        steps = 0

        while not done and steps < 200:  # 限制每局最大步数，避免无限循环
            # 选动作
            action = agent.get_action(state)
            # 执行动作，获取反馈（食物可能已随机刷新）
            next_state, reward, done = env.step(action)
            # 更新Q表
            agent.learn(state, action, reward, next_state, done)
            # 迭代
            state = next_state
            total_reward += reward
            steps += 1

            # 每100局可视化一次，观察AI行为
            if episode % 100 == 0:
                env.render()
                clock.tick(10)  # 控制速度，方便观察

        # 打印训练进度
        if episode % 100 == 0:
            print(f"第{episode}局 | 总奖励: {total_reward:.1f} | 步数: {steps}")

    print("训练结束！")
    pygame.quit()


if __name__ == "__main__":
    train(1000)  # 训练1000局