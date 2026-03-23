import os
import argparse
from modelscope.hub.snapshot_download import snapshot_download
from datasets import load_from_disk, concatenate_datasets

def parse_args():
    parser = argparse.ArgumentParser(description="从ModelScope下载RedPajama-1T")
    # 数据配置
    parser.add_argument("--subset", type=str, default="wikipedia",
                        help="RedPajama子集：common_crawl/c4/github/books/arxiv/wikipedia/stack_exchange")
    parser.add_argument("--save_dir", type=str, default="./redpajama_modelscope",
                        help="数据保存根目录")
    parser.add_argument("--cache_dir", type=str, default="~/.cache/modelscope",
                        help="ModelScope缓存目录")
    return parser.parse_args()

def download_redpajama_from_modelscope(args):
    # 1. 定义ModelScope上的RedPajama-1T地址
    # 注意：ModelScope上的RedPajama-1T已按子集拆分，对应地址如下
    subset2repo = {
        "common_crawl": "AI-ModelScope/RedPajama-Data-1T-common_crawl",
        "c4": "AI-ModelScope/RedPajama-Data-1T-c4",
        "github": "AI-ModelScope/RedPajama-Data-1T-github",
        "books": "AI-ModelScope/RedPajama-Data-1T-books",
        "arxiv": "AI-ModelScope/RedPajama-Data-1T-arxiv",
        "wikipedia": "AI-ModelScope/RedPajama-Data-1T-wikipedia",
        "stack_exchange": "AI-ModelScope/RedPajama-Data-1T-stack_exchange"
    }
    
    # 校验子集合法性
    if args.subset not in subset2repo:
        raise ValueError(f"无效子集！支持的子集：{list(subset2repo.keys())}")
    
    # 2. 下载指定子集（自动处理分片/解压）
    repo_id = subset2repo[args.subset]
    save_path = os.path.join(args.save_dir, args.subset)
    print(f"开始下载 {args.subset} 子集，保存路径：{save_path}")
    
    # 核心下载函数（ModelScope官方）
    snapshot_download(
        repo_id=repo_id,
        cache_dir=args.cache_dir,
        local_dir=save_path,
        local_dir_use_symlinks=False,  # 禁用软链接，直接复制文件
        timeout=300,  # 超时时间（大文件需延长）
        force_download=False  # 已下载则跳过
    )
    
    # 3. 加载并验证数据
    print(f"验证 {args.subset} 子集数据完整性...")
    # ModelScope下载的RedPajama已保存为datasets格式，可直接加载
    dataset = load_from_disk(save_path)
    print(f"✅ 数据验证成功！")
    print(f"  - 样本总数：{len(dataset)}")
    print(f"  - 数据列：{dataset.column_names}")
    print(f"  - 第一条样本预览：\n{dataset[0]['text'][:200]}...")
    
    return dataset

def download_full_redpajama(args):
    """下载全量RedPajama-1T（所有子集）并合并"""
    all_datasets = []
    subsets = ["common_crawl", "c4", "github", "books", "arxiv", "wikipedia", "stack_exchange"]
    
    for subset in subsets:
        args.subset = subset
        ds = download_redpajama_from_modelscope(args)
        all_datasets.append(ds)
    
    # 合并所有子集
    full_dataset = concatenate_datasets(all_datasets)
    full_save_path = os.path.join(args.save_dir, "full_redpajama_1T")
    full_dataset.save_to_disk(full_save_path)
    print(f"📦 全量RedPajama-1T合并完成！保存路径：{full_save_path}")
    print(f"  - 总样本数：{len(full_dataset)}")

if __name__ == "__main__":
    args = parse_args()
    # 下载单个子集（默认wikipedia）
    download_redpajama_from_modelscope(args)
    
    # 如需下载全量数据，取消注释以下行（需约5TB磁盘空间）
    # download_full_redpajama(args)