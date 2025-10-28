import pygame
import numpy as np
import random
from collections import deque
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader

# 豆包生成，代码不能pass

# ==============================
# 1. 贪吃蛇游戏环境（核心逻辑）
# ==============================
class SnakeGame:
    def __init__(self, grid_size=(15, 15), block_size=30, speed=100):
        # 初始化Pygame
        pygame.init()
        self.grid_size = grid_size  # (高度H, 宽度W)，单位：格子数
        self.block_size = block_size  # 每个格子的像素大小
        self.screen_width = self.grid_size[1] * self.block_size
        self.screen_height = self.grid_size[0] * self.block_size
        self.screen = pygame.display.set_mode((self.screen_width, self.screen_height))
        pygame.display.set_caption("DQN Snake")
        
        # 颜色定义
        self.BLACK = (0, 0, 0)
        self.WHITE = (255, 255, 255)
        self.RED = (255, 0, 0)    # 食物
        self.GREEN = (0, 255, 0)  # 蛇头
        self.DARK_GREEN = (0, 200, 0)  # 蛇身
        self.BLUE = (0, 0, 255)   # 边界
        
        # 游戏状态初始化
        self.reset()
        self.clock = pygame.time.Clock()
        self.speed = speed  # 游戏帧率（控制训练速度）

    def reset(self):
        """重置游戏状态（新回合开始）"""
        # 蛇初始位置：中心偏左，长度3，方向向右
        center_x = self.grid_size[0] // 2
        center_y = self.grid_size[1] // 3
        self.snake_body = [
            (center_x, center_y),          # 蛇头
            (center_x, center_y - 1),      # 蛇身1
            (center_x, center_y - 2)       # 蛇身2
        ]
        self.direction = "right"  # 初始方向：右
        self.prev_direction = "right"  # 上一步方向（用于判断倒退）
        self.food_pos = self._generate_food()  # 生成食物
        self.collided = False  # 是否碰撞
        self.score = 0  # 当前分数
        self.step_count = 0  # 步数（防止无限循环）
        # 初始蛇头到食物的曼哈顿距离
        self.prev_distance = self._get_manhattan_distance()
        return self._get_state()

    def _generate_food(self):
        """生成不与蛇身重叠的食物位置"""
        while True:
            food_x = random.randint(1, self.grid_size[0] - 2)  # 避开边界
            food_y = random.randint(1, self.grid_size[1] - 2)
            if (food_x, food_y) not in self.snake_body:
                return (food_x, food_y)

    def _get_manhattan_distance(self):
        """计算蛇头到食物的曼哈顿距离"""
        head_x, head_y = self.snake_body[0]
        food_x, food_y = self.food_pos
        return abs(head_x - food_x) + abs(head_y - food_y)

    def _get_state(self):
        """获取游戏状态（网格特征+数值特征），适配DQN输入"""
        H, W = self.grid_size
        # 1. 网格特征矩阵 (1, H, W)：墙=-1，蛇头=1，蛇身=0.5，食物=2，空地=0
        grid = np.zeros((1, H, W), dtype=np.float32)
        # 标记边界（墙）
        grid[0, 0, :] = -1.0
        grid[0, H-1, :] = -1.0
        grid[0, :, 0] = -1.0
        grid[0, :, W-1] = -1.0
        # 标记蛇身（倒序避免蛇头被覆盖）
        for i, (x, y) in enumerate(reversed(self.snake_body)):
            if i == len(self.snake_body) - 1:
                grid[0, x, y] = 1.0  # 蛇头
            else:
                grid[0, x, y] = 0.5  # 蛇身
        # 标记食物
        fx, fy = self.food_pos
        grid[0, fx, fy] = 2.0

        # 2. 数值特征：蛇头-食物距离(dx,dy)、方向独热码、周围障碍物
        head_x, head_y = self.snake_body[0]
        dx = self.food_pos[0] - head_x
        dy = self.food_pos[1] - head_y
        # 方向独热编码（up=0, down=1, left=2, right=3）
        dir_map = {"up": 0, "down": 1, "left": 2, "right": 3}
        dir_onehot = np.eye(4)[dir_map[self.direction]]
        # 周围4个方向是否有障碍物（1=墙/蛇身，0=空地/食物）
        obstacles = [
            1 if grid[0, head_x-1, head_y] in (-1.0, 0.5) else 0,  # 上
            1 if grid[0, head_x+1, head_y] in (-1.0, 0.5) else 0,  # 下
            1 if grid[0, head_x, head_y-1] in (-1.0, 0.5) else 0,  # 左
            1 if grid[0, head_x, head_y+1] in (-1.0, 0.5) else 0   # 右
        ]

        # 转换为Tensor（batch维度=1）
        grid_tensor = torch.tensor(grid)
        numeric_tensor = torch.tensor([dx, dy, *dir_onehot, *obstacles], dtype=torch.float32).unsqueeze(0)
        return grid_tensor, numeric_tensor

    def _check_collision(self, head_pos):
        """检查头部位置是否碰撞（墙或自身）"""
        x, y = head_pos
        H, W = self.grid_size
        # 碰撞墙
        if x <= 0 or x >= H-1 or y <= 0 or y >= W-1:
            return True
        # 碰撞自身
        if head_pos in self.snake_body[1:]:
            return True
        return False

    def step(self, action):
        """执行动作（0=直行，1=左转，2=右转），返回新状态、奖励、是否结束"""
        self.step_count += 1
        self.prev_direction = self.direction  # 记录上一步方向（用于判断倒退）
        
        # 1. 将相对动作转换为绝对方向
        dir_order = {
            "up": ["up", "left", "right"],    # 动作0=直行，1=左转（左），2=右转（右）
            "down": ["down", "right", "left"],
            "left": ["left", "down", "up"],
            "right": ["right", "up", "down"]
        }
        self.direction = dir_order[self.direction][action]

        # 2. 计算新蛇头位置
        head_x, head_y = self.snake_body[0]
        if self.direction == "up":
            new_head = (head_x - 1, head_y)
        elif self.direction == "down":
            new_head = (head_x + 1, head_y)
        elif self.direction == "left":
            new_head = (head_x, head_y - 1)
        else:  # right
            new_head = (head_x, head_y + 1)

        # 3. 检查碰撞
        self.collided = self._check_collision(new_head)
        if self.collided:
            reward = -20.0  # 碰撞惩罚
            new_state = self._get_state()
            return new_state, reward, True  # 游戏结束

        # 4. 移动蛇（添加新头部）
        self.snake_body.insert(0, new_head)
        # 5. 判断是否吃到食物
        current_distance = self._get_manhattan_distance()
        if new_head == self.food_pos:
            self.score += 10  # 吃食物加分
            reward = 10.0     # 吃食物奖励
            self.food_pos = self._generate_food()  # 重新生成食物
            self.prev_distance = self._get_manhattan_distance()  # 更新距离
        else:
            # 没吃到食物：删除尾部，计算距离奖励和倒退惩罚
            self.snake_body.pop()
            # 距离奖励：靠近食物+0.1，远离-0.05
            distance_reward = 0.1 if current_distance < self.prev_distance else -0.05
            # 倒退惩罚：与上一步反向且非必要（食物不在反方向）
            reverse_map = {"up": "down", "down": "up", "left": "right", "right": "left"}
            is_reverse = (self.direction == reverse_map[self.prev_direction])
            is_food_in_reverse = (
                (self.prev_direction == "up" and head_x > self.food_pos[0]) or
                (self.prev_direction == "down" and head_x < self.food_pos[0]) or
                (self.prev_direction == "left" and head_y > self.food_pos[1]) or
                (self.prev_direction == "right" and head_y < self.food_pos[1])
            )
            reverse_penalty = -1.0 if (is_reverse and not is_food_in_reverse) else 0.0
            # 存活奖励（避免过早放弃）
            survival_reward = 0.01
            # 总奖励
            reward = distance_reward + reverse_penalty + survival_reward
            self.prev_distance = current_distance  # 更新距离

        # 6. 防止无限循环（超过200步没吃食物则结束）
        if self.step_count > 200 * len(self.snake_body):
            reward = -5.0
            return self._get_state(), reward, True

        # 7. 返回新状态、奖励、游戏是否结束
        new_state = self._get_state()
        return new_state, reward, False

    def render(self):
        """可视化游戏画面"""
        # 填充背景
        self.screen.fill(self.BLACK)
        # 绘制边界
        pygame.draw.rect(
            self.screen, self.BLUE,
            (0, 0, self.screen_width, self.block_size)
        )
        pygame.draw.rect(
            self.screen, self.BLUE,
            (0, self.screen_height - self.block_size, self.screen_width, self.block_size)
        )
        pygame.draw.rect(
            self.screen, self.BLUE,
            (0, 0, self.block_size, self.screen_height)
        )
        pygame.draw.rect(
            self.screen, self.BLUE,
            (self.screen_width - self.block_size, 0, self.block_size, self.screen_height)
        )
        # 绘制蛇身
        for i, (x, y) in enumerate(self.snake_body):
            color = self.GREEN if i == 0 else self.DARK_GREEN
            pygame.draw.rect(
                self.screen, color,
                (y * self.block_size, x * self.block_size, self.block_size - 1, self.block_size - 1)
            )
        # 绘制食物
        fx, fy = self.food_pos
        pygame.draw.rect(
            self.screen, self.RED,
            (fy * self.block_size, fx * self.block_size, self.block_size - 1, self.block_size - 1)
        )
        # 绘制分数
        font = pygame.font.SysFont(None, 30)
        score_text = font.render(f"Score: {self.score}", True, self.WHITE)
        self.screen.blit(score_text, (10, 10))
        # 更新画面
        pygame.display.update()
        self.clock.tick(self.speed)

    def close(self):
        """关闭游戏窗口"""
        pygame.quit()


# ==============================
# 2. DQN网络（卷积+全连接融合特征）
# ==============================
class SnakeDQN(nn.Module):
    def __init__(self, grid_size=(15, 15), numeric_dim=10):
        """
        Args:
            grid_size: 游戏网格尺寸 (H, W)
            numeric_dim: 数值特征维度（dx, dy + 方向4维 + 障碍物4维 = 10）
        """
        super(SnakeDQN, self).__init__()
        H, W = grid_size
        # 卷积层：提取网格空间特征
        self.conv1 = nn.Conv2d(in_channels=1, out_channels=16, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(in_channels=16, out_channels=32, kernel_size=3, padding=1)
        self.pool = nn.AvgPool2d(kernel_size=2, stride=2)  # 下采样减半
        # 卷积层输出维度：32 * (H//2) * (W//2)
        self.conv_out_dim = 32 * (H // 2) * (W // 2)
        
        # 全连接层：融合卷积特征与数值特征
        self.fc1 = nn.Linear(self.conv_out_dim + numeric_dim, 256)
        self.fc2 = nn.Linear(256, 128)
        self.fc3 = nn.Linear(128, 3)  # 3个动作（0=直行，1=左转，2=右转）

    def forward(self, grid, numeric):
        """前向传播：输入(grid, numeric)，输出3个动作的Q值"""
        # 卷积层：ReLU激活 + 池化
        x = F.relu(self.conv1(grid))
        x = F.relu(self.conv2(x))
        x = self.pool(x)
        # 展平卷积特征（适配全连接层）
        x = x.view(x.size(0), -1)  # (batch_size, conv_out_dim)
        # 融合数值特征（batch_size, conv_out_dim + numeric_dim）
        x = torch.cat([x, numeric], dim=1)
        # 全连接层：ReLU激活
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        # 输出Q值（无激活，因为Q值可正可负）
        return self.fc3(x)


# ==============================
# 3. 经验回放缓冲区（稳定训练）
# ==============================
class ReplayBuffer:
    def __init__(self, capacity=100000):
        self.buffer = deque(maxlen=capacity)  # 固定容量，满则删除旧经验

    def add(self, state, action, reward, next_state, done):
        """添加经验：(状态, 动作, 奖励, 下一状态, 是否结束)"""
        # 状态和下一状态均为 (grid_tensor, numeric_tensor)，需转为numpy存储（节省内存）
        state_grid = state[0].numpy()
        state_numeric = state[1].numpy()
        next_state_grid = next_state[0].numpy()
        next_state_numeric = next_state[1].numpy()
        self.buffer.append((state_grid, state_numeric, action, reward, next_state_grid, next_state_numeric, done))

    def sample(self, batch_size=64):
        """采样一批经验，转换为Tensor用于训练（修复通道维度）"""
        batch = random.sample(self.buffer, batch_size)
        # 解包批量数据
        state_grids, state_numerics, actions, rewards, next_state_grids, next_state_numerics, dones = zip(*batch)
        
        # 关键修复：拼接后添加通道维度（axis=1），确保形状为 (batch_size, 1, H, W)
        # 1. 处理状态网格（state_grid）
        state_grids_np = np.concatenate(state_grids, axis=0)  # 拼接后：(batch_size, H, W)
        state_grids_np = np.expand_dims(state_grids_np, axis=1)  # 加通道维度：(batch_size, 1, H, W)
        state_grid_tensor = torch.tensor(state_grids_np, dtype=torch.float32)
        
        # 2. 处理下一状态网格（next_state_grid）——同样加通道维度
        next_state_grids_np = np.concatenate(next_state_grids, axis=0)  # (batch_size, H, W)
        next_state_grids_np = np.expand_dims(next_state_grids_np, axis=1)  # (batch_size, 1, H, W)
        next_state_grid_tensor = torch.tensor(next_state_grids_np, dtype=torch.float32)
        
        # 3. 处理其他特征（数值特征、动作、奖励等——原逻辑不变）
        state_numeric_tensor = torch.tensor(np.concatenate(state_numerics, axis=0), dtype=torch.float32)
        action_tensor = torch.tensor(actions, dtype=torch.long).unsqueeze(1)  # (batch_size, 1)
        reward_tensor = torch.tensor(rewards, dtype=torch.float32).unsqueeze(1)  # (batch_size, 1)
        next_state_numeric_tensor = torch.tensor(np.concatenate(next_state_numerics, axis=0), dtype=torch.float32)
        done_tensor = torch.tensor(dones, dtype=torch.float32).unsqueeze(1)  # (batch_size, 1)
        
        return (state_grid_tensor, state_numeric_tensor, action_tensor, reward_tensor,
                next_state_grid_tensor, next_state_numeric_tensor, done_tensor)

    def __len__(self):
        """返回缓冲区当前经验数量"""
        return len(self.buffer)


# ==============================
# 4. DQN训练器（核心训练逻辑）
# ==============================
class DQNTrainer:
    def __init__(self, grid_size=(15, 15), lr=1e-4, gamma=0.99, epsilon_start=1.0, epsilon_end=0.1, epsilon_decay=10000):
        """
        Args:
            lr: 学习率
            gamma: 折扣因子（未来奖励权重）
            epsilon: ε-greedy探索率（从start衰减到end，共decay步）
        """
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        print(f"使用设备: {self.device}")
        
        # 初始化游戏环境、DQN网络（主网络+目标网络）
        self.env = SnakeGame(grid_size=grid_size)
        self.numeric_dim = 10  # dx, dy + 方向4维 + 障碍物4维
        self.policy_net = SnakeDQN(grid_size=grid_size, numeric_dim=self.numeric_dim).to(self.device)  # 主网络（实时更新）
        self.target_net = SnakeDQN(grid_size=grid_size, numeric_dim=self.numeric_dim).to(self.device)  # 目标网络（定期更新）
        self.target_net.load_state_dict(self.policy_net.state_dict())  # 初始参数同步
        
        # 经验回放缓冲区、优化器、损失函数
        self.replay_buffer = ReplayBuffer(capacity=100000)
        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=lr)
        self.loss_fn = nn.MSELoss()  # Q值回归用MSE损失
        
        # ε-greedy参数
        self.epsilon = epsilon_start
        self.epsilon_end = epsilon_end
        self.epsilon_decay = epsilon_decay
        self.epsilon_step = (epsilon_start - epsilon_end) / epsilon_decay
        
        # 训练超参数
        self.gamma = gamma  # 折扣因子
        self.batch_size = 64  # 批量大小
        self.target_update_freq = 200  # 目标网络更新频率（每200步更新一次）
        self.train_steps = 0  # 训练总步数
        self.episodes = 1000  # 总训练回合数
        self.save_freq = 100  # 模型保存频率（每100回合保存一次）
        self.model_path = "snake_dqn.pth"  # 模型保存路径

    def select_action(self, state):
        """ε-greedy选择动作：探索（随机）或利用（选Q值最大的动作）"""
        # 状态转为Tensor并移到设备
        grid = state[0].to(self.device)
        numeric = state[1].to(self.device)
        
        # 探索：随机选择动作（0-2）
        if random.random() < self.epsilon:
            return random.randint(0, 2)
        # 利用：选择Q值最大的动作
        else:
            with torch.no_grad():  # 推理阶段不计算梯度
                q_values = self.policy_net(grid, numeric)
                return q_values.argmax(dim=1).item()  # 返回Q值最大的动作索引

    def update_epsilon(self):
        """衰减探索率ε"""
        self.epsilon = max(self.epsilon_end, self.epsilon - self.epsilon_step)

    def update_policy(self):
        """更新主网络（DQN核心更新逻辑）"""
        # 缓冲区经验不足批量大小时，不更新
        if len(self.replay_buffer) < self.batch_size:
            return 0.0
        
        # 1. 采样批量经验
        (state_grid, state_numeric, actions, rewards,
         next_state_grid, next_state_numeric, dones) = self.replay_buffer.sample(self.batch_size)
        # 移到设备
        state_grid = state_grid.to(self.device)
        state_numeric = state_numeric.to(self.device)
        actions = actions.to(self.device)
        rewards = rewards.to(self.device)
        next_state_grid = next_state_grid.to(self.device)
        next_state_numeric = next_state_numeric.to(self.device)
        dones = dones.to(self.device)

        # 2. 计算当前Q值（主网络预测）：Q(s,a) = 主网络输出中动作a对应的Q值
        current_q = self.policy_net(state_grid, state_numeric).gather(1, actions)

        # 3. 计算目标Q值：r + γ * max_a(Q'(s',a))（目标网络预测，且终端状态Q=0）
        with torch.no_grad():
            # 目标网络预测下一状态的最大Q值
            next_max_q = self.target_net(next_state_grid, next_state_numeric).max(dim=1, keepdim=True)[0]
            # 目标Q值：终端状态（done=1）时，目标Q=r（无未来奖励）
            target_q = rewards + self.gamma * next_max_q * (1 - dones)

        # 4. 计算损失（MSE）并反向传播
        loss = self.loss_fn(current_q, target_q)
        self.optimizer.zero_grad()  # 清空梯度
        loss.backward()  # 反向传播计算梯度
        self.optimizer.step()  # 更新参数

        return loss.item()  # 返回当前损失值

    def train(self):
        """完整训练循环"""
        print("开始训练DQN贪吃蛇...")
        for episode in range(1, self.episodes + 1):
            # 重置游戏状态（新回合）
            state = self.env.reset()
            total_reward = 0.0
            total_loss = 0.0
            step_in_episode = 0
            done = False

            while not done:
                # 1. 选择动作
                action = self.select_action(state)
                # 2. 执行动作，获取新状态、奖励、是否结束
                next_state, reward, done = self.env.step(action)
                # 3. 添加经验到缓冲区
                self.replay_buffer.add(state, action, reward, next_state, done)
                # 4. 更新主网络
                loss = self.update_policy()
                total_loss += loss if loss != 0 else 0.0
                # 5. 更新探索率
                self.update_epsilon()
                # 6. 计数与状态转移
                self.train_steps += 1
                step_in_episode += 1
                total_reward += reward
                state = next_state
                # 7. 可视化（每步绘制画面，可注释加速训练）
                self.env.render()

                # 8. 定期更新目标网络（同步主网络参数）
                if self.train_steps % self.target_update_freq == 0:
                    self.target_net.load_state_dict(self.policy_net.state_dict())

            # 回合结束：打印统计信息
            avg_loss = total_loss / step_in_episode if step_in_episode > 0 else 0.0
            print(f"回合 {episode:4d} | 步数: {step_in_episode:4d} | 总奖励: {total_reward:6.2f} | "
                  f"平均损失: {avg_loss:.4f} | ε: {self.epsilon:.3f} | 分数: {self.env.score}")

            # 定期保存模型
            if episode % self.save_freq == 0:
                torch.save(self.policy_net.state_dict(), self.model_path)
                print(f"模型已保存到 {self.model_path}")

        # 训练结束：关闭游戏窗口
        self.env.close()
        print("训练完成！")

    def test(self, model_path="snake_dqn.pth"):
        """测试训练好的模型（无探索，纯利用）"""
        print(f"加载模型 {model_path} 进行测试...")
        # 加载模型参数
        self.policy_net.load_state_dict(torch.load(model_path, map_location=self.device))
        self.policy_net.eval()  # 切换到评估模式（禁用Dropout等）

        # 测试10个回合
        for episode in range(1, 11):
            state = self.env.reset()
            total_reward = 0.0
            step_in_episode = 0
            done = False

            while not done:
                # 选择Q值最大的动作（无探索）
                grid = state[0].to(self.device)
                numeric = state[1].to(self.device)
                with torch.no_grad():
                    q_values = self.policy_net(grid, numeric)
                    action = q_values.argmax(dim=1).item()
                # 执行动作
                next_state, reward, done = self.env.step(action)
                # 统计与可视化
                total_reward += reward
                step_in_episode += 1
                state = next_state
                self.env.render()

            print(f"测试回合 {episode:2d} | 步数: {step_in_episode:4d} | 总奖励: {total_reward:6.2f} | 分数: {self.env.score}")

        # 测试结束
        self.env.close()
        print("测试完成！")


# ==============================
# 5. 运行入口（训练或测试）
# ==============================
if __name__ == "__main__":
    # 初始化训练器（可调整网格大小、学习率等参数）
    trainer = DQNTrainer(
        grid_size=(15, 15),  # 网格尺寸（H=15行，W=15列）
        lr=1e-4,             # 学习率
        gamma=0.99,          # 折扣因子
        epsilon_start=1.0,   # 初始探索率
        epsilon_end=0.1,     # 最终探索率
        epsilon_decay=10000  # 探索率衰减步数
    )

    # 选择模式：训练（train）或测试（test）
    mode = "train"  # "train" 或 "test"
    if mode == "train":
        trainer.train()
    elif mode == "test":
        # 测试前需先训练（或下载预训练模型）
        trainer.test(model_path="snake_dqn.pth")