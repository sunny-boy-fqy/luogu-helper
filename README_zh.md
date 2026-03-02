# Luogu Helper - 洛谷辅助工具

一个为算法竞赛（OI/ACM）开发打造的综合工具集，**专为 VSCode 优化**。

## 功能特性

- **题目初始化**：快速创建题目目录，包含标准模板
- **数据获取**：从洛谷抓取题目描述、样例数据和题解
- **本地测试**：内置评测器和对排工具，支持本地调试
- **数据生成**：强大的随机数据生成库，支持多种数据类型
- **云端同步**：通过 Google Drive 备份和同步代码
- **跨平台支持**：支持 Linux/macOS 和 Windows

## 快速开始

### 安装

```bash
# 克隆仓库
git clone https://github.com/sunny-boy-fqy/luogu-helper.git
cd luogu-helper

# Linux/macOS：添加执行权限
chmod +x clear new sync tools/*
```

### 推荐：VSCode 集成

本工具集与 VSCode 配合使用效果最佳。推荐安装以下扩展：
- C/C++ Extension Pack
- Competitive Programming Helper

添加到 `.vscode/settings.json`：
```json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64"
}
```

## 命令说明

### 1. 创建新题目：`./new`

```bash
# 从洛谷题号创建
./new P5736
./new T721517

# 创建空白目录
./new my_contest_problem
```

这将创建一个包含以下文件的新目录：
- `main.cpp` - 你的解答代码模板
- `gen.cpp` - 数据生成器（从 std/ 复制）
- `data/` - 样例测试数据（如果从洛谷抓取成功）
- `info` - 题目元数据

### 2. 获取题目信息：`./get_problem`

在题目目录中运行，获取完整题目描述：
```bash
cd P5736
./get_problem   # 创建 problem.md
```

### 3. 获取题解：`./get_solve`

从洛谷获取最高评分的 AC 题解：
```bash
cd P5736
./get_solve     # 创建 std.cpp
```

**改进**：如果题解作者将代码拆分成多个小块，脚本会智能检测并尝试下一篇题解；如果没有更多题解，则使用第一篇的最后一个代码块。

### 4. 本地评测：`./judge`

使用样例数据测试你的代码：
```bash
cd P5736
./judge         # 编译并测试 main.cpp
```

### 5. 对排工具（压力测试）：`./checker`

使用随机数据对比你的代码和标准代码：
```bash
cd P3834

# 首先，获取或复制 std.cpp
./get_solve

# 或者从其他来源复制
cp /path/to/solution.cpp std.cpp

# 然后运行对排
./checker [测试次数] [超时时间]
./checker 100 2    # 运行 100 组测试，每组超时 2 秒
```

**对排功能**：
- 自动编译 gen.cpp、main.cpp 和 std.cpp
- 生成随机测试数据
- 对比输出结果（忽略空格差异）
- 第一个错误出现时停止，并保存失败样例到 `fail.in`
- 每 10 组测试报告进度

### 6. 数据生成器：`./gen`

生成自定义测试数据：
```bash
cd P5736
./gen > test.in    # 生成到文件
./gen | ./main     # 直接管道输入到程序
```

根据题目需求自定义 `gen.cpp`。参考 `std/gen.cpp` 中的示例。

### 7. Google Drive 同步：`./clear` & `./sync`

这些命令允许你将模板和工具备份到 Google Drive，并在多台设备间同步。

```bash
# 上传核心文件到 Google Drive 并清理本地题目目录
./clear

# 从 Google Drive 下载核心文件
./sync
```

#### 设置 rclone

**步骤 1：安装 rclone**

**Linux：**
```bash
# 方法 1：使用 snap
sudo snap install rclone

# 方法 2：下载二进制文件
curl https://rclone.org/install.sh | sudo bash

# 方法 3：使用包管理器
# Debian/Ubuntu
sudo apt install rclone

# Fedora
sudo dnf install rclone

# Arch
sudo pacman -S rclone
```

**macOS：**
```bash
# 使用 Homebrew
brew install rclone

# 或从 https://rclone.org/downloads/ 下载
```

**Windows：**
1. 从 https://rclone.org/downloads/ 下载
2. 解压到文件夹（如 `C:\rclone`）
3. 添加到 PATH：系统属性 → 环境变量 → 编辑 PATH → 添加 `C:\rclone`
4. 验证：打开 CMD，输入 `rclone version`

**步骤 2：配置 Google Drive**

```bash
# 开始配置
rclone config
```

按以下步骤操作：
1. 输入 `n` 创建新远程
2. 名称：`gdrive`（必须与脚本中一致）
3. 选择 `13` 对应 Google Drive（或实际显示的编号）
4. client_id 和 client_secret 留空（直接回车）
5. 选择范围：`1`（完全访问权限）
6. Root folder ID：留空
7. Service Account：留空
8. 编辑高级配置：`n`
9. 使用自动配置：`y`（桌面环境）或 `n`（无图形界面/SSH）
   - 如果选 `y`：浏览器会打开，登录 Google 并授权
   - 如果选 `n`：访问提供的 URL，获取代码，粘贴回来
10. 确认：`y`

验证配置：
```bash
rclone listremotes          # 应该显示：gdrive:
rclone lsd gdrive:          # 列出 Drive 根目录
```

**步骤 3：创建同步文件夹（可选）**

默认脚本同步到 `gdrive:OI/program`。你可以创建此文件夹：
```bash
rclone mkdir gdrive:OI/program
```

或修改 `clear` 和 `sync` 脚本中的 `REMOTE` 变量来更改路径。

**步骤 4：测试同步**

```bash
# 测试上传（试运行）
rclone sync . gdrive:OI/program --dry-run

# 测试下载（试运行）
rclone sync gdrive:OI/program . --dry-run
```

**同步内容包括：**
- `std/` - 模板文件
- `tools/` - 所有脚本和工具
- `include/` - 头文件库
- `.vscode/` - VSCode 设置
- `new`、`clear`、`sync` 脚本
- 题目目录中的所有 `main.cpp`

**不同步的内容：**
- 题目目录（只同步其中的 `main.cpp`）
- 编译后的二进制文件
- 测试数据输出
- 个人解答文件

#### 常见问题

**"gdrive not found" 错误：**
```bash
# 重新运行配置
rclone config

# 检查 'gdrive' 是否在列表中
rclone listremotes
```

**Token 过期：**
```bash
# 刷新 token
rclone config reconnect gdrive:
```

**速率限制：**
Google Drive 有 API 限制。如果触发限制，等待几分钟或在同步命令中添加 `--tpslimit 1`。

**大文件：**
对于超过 100MB 的文件，rclone 可能需要特殊处理。使用 `--drive-chunk-size 128M` 参数。

## 数据生成库

包含 `datagen.hpp`，提供强大的随机数据生成能力：

```cpp
#include "../include/datagen.hpp"
using namespace datagen;

int main() {
    init_rand();  // 初始化随机种子
    
    // 基础随机数
    int x = random_int(1, 100);
    double y = random_double(0.0, 1.0);
    
    // 数组和序列
    auto arr = random_array(10, 1, 100);           // 随机数组
    auto perm = random_permutation(10);             // 随机排列
    auto inc = random_increasing(10, 1, 100);       // 严格递增序列
    auto seq = sequence_from_function(10, [](int i) { return i * i; });
    
    // 字符串
    string s = random_string(10, LOWERCASE);
    string word = random_word(3, 8);
    string sentence = random_sentence(10);
    string para = random_paragraph(5);
    
    // 图
    auto tree = random_tree(100);                   // 无权树
    auto wtree = random_tree(100, true, 1, 100);   // 带权树
    auto graph = random_connected_graph(100, 200);  // 连通图
    auto dag = random_dag(50, 100);                 // 有向无环图
    auto bip = random_bipartite(20, 30, 100);       // 二分图
    
    // 向量和矩阵
    auto vecs = random_vectors(10, 3, -100, 100);   // 10 个三维向量
    auto mat = random_matrix(5, 5, 0, 1);           // 5x5 矩阵
    
    // 几何
    auto pts = random_points(10, -100, 100, -100, 100);
    auto poly = random_convex_polygon(10, -100, 100, -100, 100);
    auto simple = random_simple_polygon(10, -100, 100, -100, 100);
    
    // 多边形计算
    long long area2 = polygon_area2(poly);          // 两倍面积
    double perim = polygon_perimeter(poly);         // 周长
    
    // 输出辅助
    print_array(arr);
    print_graph(100, edges, true);                  // 带权重
    print_points(pts);
    print_polygon(poly);
    
    return 0;
}
```

### 库函数分类

| 分类 | 函数 |
|------|------|
| **随机数** | `random_int()`, `random_double()`, `random_bool()` |
| **数组** | `random_array()`, `random_permutation()`, `random_increasing()`, `random_decreasing()`, `random_non_decreasing()`, `sequence_from_function()` |
| **字符串** | `random_string()`, `random_word()`, `random_sentence()`, `random_paragraph()`, `random_palindrome()` |
| **图** | `random_tree()`, `random_connected_graph()`, `random_dag()`, `random_bipartite()` |
| **向量** | `random_vectors()`, `random_matrix()` |
| **几何** | `random_points()`, `random_convex_polygon()`, `random_simple_polygon()`, `polygon_area2()`, `polygon_perimeter()` |
| **输出** | `print_array()`, `print_matrix()`, `print_graph()`, `print_tree()`, `print_points()`, `print_polygon()` |

## 目录结构

```
luogu-helper/
├── clear              # 上传到 Google Drive 并清理
├── sync               # 从 Google Drive 下载
├── new                # 创建新题目
├── std/
│   ├── main.cpp       # 默认代码模板
│   └── gen.cpp        # 默认数据生成器模板
├── include/
│   ├── datagen.hpp    # 数据生成库
│   ├── qio.hpp        # 快速 IO 工具
│   └── ...            # 其他算法模板
├── tools/
│   ├── judge          # 本地评测器
│   ├── checker        # 对排工具
│   ├── get_problem    # 获取题目描述
│   ├── get_solve      # 获取 AC 题解
│   └── fetch_problem.py
└── .vscode/           # VSCode 配置
```

## Windows 支持

Windows 用户使用 `.bat` 版本的脚本：

```cmd
new.bat P5736
checker.bat 100 2
judge.bat
```

或者使用 WSL（Windows Subsystem for Linux）获得完整的 Linux 兼容性。

## 环境变量

- `LUOGU_COOKIE`：设置洛谷会话 cookie，用于访问私有题目

```bash
export LUOGU_COOKIE="_uid=12345; __client_id=..."
```

或者在用户目录创建 `~/.luogu_cookie` 文件存放 cookie。

## 使用技巧

1. **使用 VSCode 代码片段**：将常用的代码模式添加到 VSCode 代码片段中
2. **自定义 gen.cpp**：将 `std/gen.cpp` 复制到题目目录并修改
3. **对排调试**：充分利用 `./checker` 捕获边界情况
4. **保留 std.cpp**：始终保留一份正确的参考代码用于对比
5. **定期备份**：经常运行 `./clear` 备份进度到 Google Drive

## 系统要求

- **Linux/macOS**：bash, g++, python3, rclone（用于同步）
- **Windows**：MinGW-w64 或 MSVC, python3, 批处理支持
- **所有平台**：C++17 编译器, git（可选）

## 许可证

MIT License - 自由使用和修改，助力你的算法竞赛之路！

## 贡献指南

欢迎贡献！请确保：
- 代码遵循现有风格
- 脚本保持可移植性（无硬编码路径）
- 为新功能添加测试
- 更新文档

---

**祝编码愉快！** 🚀
