# Luogu Helper - Windows 版本使用指南

这是洛谷辅助工具的 Windows 兼容版本，提供与 Linux/macOS 版本相同的功能。

## 系统要求

- **操作系统**: Windows 10/11
- **编译器**: MinGW-w64 (g++) 或 MSVC (cl.exe)
- **Python**: Python 3.x
- **rclone** (可选，用于 Google Drive 同步)

## 安装

### 方法 1：使用 Git 克隆（推荐）

```cmd
rem 克隆仓库
git clone https://github.com/sunny-boy-fqy/luogu-helper.git
cd luogu-helper
```

### 方法 2：下载 ZIP

从 GitHub 下载 ZIP 文件并解压。

### 编译器配置

确保你的编译器在 PATH 中：

```cmd
rem 测试 g++ 是否可用
g++ --version

rem 或测试 MSVC 是否可用（需要 Visual Studio 开发环境）
where cl
```

## 快速开始

### 1. 创建新题目

```cmd
rem 从洛谷题号创建
new.bat P5736
new.bat T721517

rem 创建空白目录
new.bat my_contest_problem
```

### 2. 获取题目信息

```cmd
cd P5736
get_problem.bat
rem 创建 problem.md 文件
```

### 3. 获取题解

```cmd
cd P5736
get_solve.bat
rem 创建 std.cpp 文件
```

**需要设置 luogu cookie 和 uid**，参看 [环境变量](#环境变量) 部分。

### 4. 本地评测

```cmd
cd P5736
judge.bat
rem 编译并测试 main.cpp
```

### 5. 对排工具（压力测试）

```cmd
cd P3834

rem 首先获取或复制 std.cpp
get_solve.bat

rem 然后运行对排
checker.bat [测试次数] [超时时间]
checker.bat 100 2    rem 运行 100 组测试，每组超时 2 秒
```

### 6. 代码打包

```cmd
cd P5736
bundle.bat
rem 打包 main.cpp，自动展开 include 文件夹中的头文件
bundle.bat my.cpp
rem 打包指定的源文件
```

运行后会自动复制到剪贴板，如果失败则保存为 `bundled_code.cpp`。

## 目录结构

```
luogu-helper/
├── new.bat            # 创建新题目
├── clear.bat          # 上传到 Google Drive 并清理
├── sync.bat           # 从 Google Drive 下载
├── std/
│   ├── main.cpp       # 默认代码模板
│   └── gen.cpp        # 默认数据生成器模板
├── include/           # 头文件库
│   ├── datagen.hpp    # 数据生成库
│   ├── qio.hpp        # 快速 IO 工具
│   ├── modint.hpp     # 模整数类
│   └── ...            # 其他头文件
├── tools/
│   ├── judge.bat      # 本地评测器
│   ├── checker.bat    # 对排工具
│   ├── bundle.bat     # 代码打包
│   ├── get_problem.bat # 获取题目描述
│   ├── get_solve.bat  # 获取 AC 题解
│   ├── fetch_problem.py # Python 脚本（跨平台）
│   ├── bundle.py      # 打包脚本（跨平台）
│   └── judge.cpp      # 评测器源码
└── .vscode/           # VSCode 配置
```

## 环境变量

### LUOGU_COOKIE

用于访问私有题目和题解。设置方法：

**方法 1：命令行设置（临时）**
```cmd
set LUOGU_COOKIE=_uid=12345; __client_id=...
```

**方法 2：系统环境变量（永久）**
1. 右键"此电脑" → "属性" → "高级系统设置"
2. 点击"环境变量"
3. 在"用户变量"中添加：
   - 变量名：`LUOGU_COOKIE`
   - 变量值：`_uid=12345; __client_id=...`

**方法 3：创建 cookie 文件**
在用户目录创建 `~\.luogu_cookie` 文件（即 `%USERPROFILE%\.luogu_cookie`），内容为 cookie 字符串。

## Google Drive 同步

### 安装 rclone

1. 从 https://rclone.org/downloads/ 下载 Windows 版本
2. 解压到一个文件夹（如 `C:\rclone`）
3. 添加到 PATH：系统属性 → 环境变量 → 编辑 PATH → 添加路径
4. 验证安装：打开 CMD，输入 `rclone version`

### 配置 Google Drive

```cmd
rclone config
```

按以下步骤操作：
1. 输入 `n` 创建新远程
2. 名称：`gdrive`（必须与脚本中一致）
3. 选择对应 Google Drive 的编号
4. client_id 和 client_secret 留空
5. 选择范围：`1`（完全访问权限）
6. Root folder ID：留空
7. Service Account：留空
8. 编辑高级配置：`n`
9. 使用自动配置：`y`（会打开浏览器授权）

验证：
```cmd
rclone listremotes    rem 应该显示：gdrive:
rclone lsd gdrive:    rem 列出 Drive 根目录
```

### 使用同步

```cmd
rem 上传核心文件到 Google Drive 并清理本地题目目录
clear.bat

rem 从 Google Drive 下载核心文件
sync.bat
```

## VSCode 配置

推荐在 `.vscode/settings.json` 中添加：

```json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "windows-gcc-x64"
}
```

如果使用 MSVC：
```json
{
    "C_Cpp.default.cppStandard": "c++17",
    "C_Cpp.default.intelliSenseMode": "windows-msvc-x64"
}
```

## 常见问题

### 1. "g++ 不是内部或外部命令"

解决方案：安装 MinGW-w64 并添加到 PATH。

```cmd
rem 下载 MinGW-w64
rem 添加 g++ 路径到系统 PATH 环境变量
rem 例如：C:\mingw64\bin
```

### 2. "python 不是内部或外部命令"

解决方案：安装 Python 并添加到 PATH。

```cmd
rem 测试 Python 安装
python --version

rem 如果是 python3 或 py 命令
py --version
```

如果 Python 命令是 `py`，需要修改脚本中的 `python` 为 `py`。

### 3. 剪贴板复制不工作

Windows 使用 `clip` 命令复制到剪贴板。如果失败：
- 检查是否可以运行 `echo test | clip`
- 失败时，代码会保存到 `bundled_code.cpp`

### 4. 对排工具超时问题

Windows 的 `timeout` 命令与 Linux 不同。checker.bat 当前不支持精确超时控制。如需精确超时，考虑：
- 使用 WSL（Windows Subsystem for Linux）
- 使用第三方工具如 `timeout.exe`

### 5. 路径中的空格问题

如果路径包含空格，确保使用引号：
```cmd
new.bat "P12345"
cd "My Problem"
```

## WSL 替代方案

如果你使用 WSL（Windows Subsystem for Linux），可以直接使用原版 Linux 脚本：

```bash
# 在 WSL 中
chmod +x new clear sync tools/*
./new P5736
```

## 许可证

MIT License - 自由使用和修改

## 贡献

欢迎贡献！请确保：
- 批处理脚本与 Bash 脚本功能一致
- 保持脚本的跨版本兼容性（MinGW 和 MSVC）
- 更新文档

---

**祝编码愉快！** 🚀