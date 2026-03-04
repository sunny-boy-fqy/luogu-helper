# Luogu Helper - 洛谷辅助工具

C++ 算法竞赛开发工具集，为洛谷 OJ 优化，支持 Linux/macOS/Windows。

## 功能

- **题目管理** - 从洛谷抓取题目、样例、题解
- **本地测试** - 评测器 + 压力测试工具
- **数据生成** - 随机数据生成库（数组、图、几何等）
- **代码打包** - 自动展开 `#include` 头文件
- **云端同步** - Google Drive 备份

## 快速开始

### 安装

**Linux/macOS:**
```bash
git clone https://github.com/sunny-boy-fqy/luogu-helper.git
cd luogu-helper
chmod +x new clear sync tools/*
```

**Windows:**
```cmd
git clone https://github.com/sunny-boy-fqy/luogu-helper.git
cd luogu-helper
```

或从 [Releases](https://github.com/sunny-boy-fqy/luogu-helper/releases) 下载对应平台的压缩包。

### 系统要求

- **编译器**: g++ (Linux), clang++ (macOS), MinGW-w64/MSVC (Windows)
- **Python**: 3.x (可选，用于构建可执行文件)
- **C++ 标准**: C++17

## 使用方法

### 创建题目

```bash
./new P1000          # Linux/macOS
new.bat P1000        # Windows
```

创建 `P1000/` 目录，包含：
- `main.cpp` - 代码模板
- `gen.cpp` - 数据生成器
- `data/` - 样例数据

### 测试代码

```bash
./judge              # 用样例测试
./checker 100 2      # 压力测试：100组数据，2秒超时
```

### 获取题目/题解

```bash
./get_problem        # 获取题目描述 → problem.md
./get_solve          # 获取高赞题解 → std.cpp
```

需要配置洛谷 Cookie（见下文）。

### 打包代码

```bash
./bundle             # 展开头文件到剪贴板
./bundle my.cpp      # 打包指定文件
```

## 配置洛谷 Cookie

**方法 1: 环境变量**
```bash
export LUOGU_COOKIE="_uid=12345; __client_id=..."
```

**方法 2: 配置文件**
```bash
./config_cookie      # 交互式配置
```

Cookie 保存在 `~/.luogu_cookie`。

## 头文件库

`include/` 目录包含 14 个算法模板：

| 头文件 | 功能 |
|--------|------|
| `qio.hpp` | 快速 I/O (mmap 优化) |
| `modint.hpp` | 模整数运算 |
| `datagen.hpp` | 数据生成 |
| `bint.hpp` | 高精度整数 |
| `BIT.hpp` | 树状数组 |
| `treap.hpp` | Treap 平衡树 |
| `LCT.hpp` | Link-Cut Tree |
| `AC_trie.hpp` | AC 自动机 |
| `Dij.hpp` | Dijkstra |
| `mat.hpp` | 矩阵运算 |
| `comb.hpp` | 组合数学 |
| `geometry.hpp` | 计算几何 |
| `fast_sort.hpp` | 快速排序 |
| `math_constants.hpp` | 数学常数 |

**使用示例:**
```cpp
#include "../include/modint.hpp"
#include "../include/qio.hpp"

using mint = modint<998244353>;

int main() {
    int n;
    cin >> n;  // 已替换为高速版本
    
    mint ans = 1;
    for (int i = 1; i <= n; i++) ans *= i;
    cout << ans << '\n';
    
    return 0;
}
```

提交前运行 `./bundle` 展开头文件。

## 数据生成

`include/datagen.hpp` 提供丰富的随机数据生成函数：

```cpp
#include "../include/datagen.hpp"
using namespace datagen;

int main() {
    init_rand();  // 初始化随机种子
    
    // 数组
    auto arr = random_array(10, 1, 100);
    auto perm = random_permutation(10);
    
    // 图
    auto tree = random_tree(100);
    auto graph = random_connected_graph(100, 200);
    
    // 字符串
    string s = random_string(10, LOWERCASE);
    
    // 几何
    auto pts = random_points(10, -100, 100, -100, 100);
    auto poly = random_convex_polygon(10, -100, 100, -100, 100);
    
    // 输出
    print_array(arr);
    print_graph(100, tree, true);  // 带权重
}
```

完整 API 见 `std/gen.cpp` 模板。

## Google Drive 同步

需要 [rclone](https://rclone.org/) 配置远程 `gdrive`。

```bash
./clear   # 上传模板/工具，清理题目
./sync    # 从 Drive 下载
```

同步到 `gdrive:OI/program`。

## 目录结构

```
luogu-helper/
├── new, clear, sync        # 主命令 (Linux/macOS)
├── *.bat                   # Windows 版本
├── include/                # 头文件库 (14个)
├── std/                    # 模板 (main.cpp, gen.cpp)
├── tools/                  # 工具脚本
│   ├── judge              # 评测器
│   ├── checker            # 压力测试
│   ├── bundle             # 代码打包
│   ├── get_problem        # 获取题目
│   └── get_solve          # 获取题解
└── .vscode/               # VSCode 配置
```

## 常见问题

**Q: Windows 下找不到编译器？**  
A: 安装 MinGW-w64 或 Visual Studio，确保 `g++` 或 `cl` 在 PATH 中。

**Q: Cookie 无效？**  
A: 检查 `_uid` 和 `__client_id` 是否正确，重新运行 `./config_cookie`。

**Q: `./bundle` 无法复制到剪贴板？**  
A: Linux 需要安装 `xsel` 或 `xclip`。代码会自动保存到 `bundled_code.cpp`。

**Q: macOS 下 `timeout` 命令找不到？**  
A: 安装 coreutils: `brew install coreutils`，使用 `gtimeout`。

## 许可证

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！

---

**祝编码愉快！** 🚀