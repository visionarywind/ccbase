from transformers import LlamaModel, LlamaConfig
import torch

# 创建模型
config = LlamaConfig()
config.num_hidden_layers=2
config.num_attention_heads=1
print(config)
model = LlamaModel(config)

# 检查不同层的q_proj权重是否相同
layer_0_q_weight = model.layers[0].self_attn.q_proj.weight
layer_1_q_weight = model.layers[1].self_attn.q_proj.weight

# 验证它们是不同的张量
print(layer_0_q_weight is layer_1_q_weight)  # False
print(layer_0_q_weight.shape == layer_1_q_weight.shape)  # True (形状相同但值不同)

# 检查数值是否相同
print(torch.equal(layer_0_q_weight, layer_1_q_weight))  # False

print(layer_0_q_weight.shape, layer_0_q_weight)
print(layer_1_q_weight.shape, layer_1_q_weight)
