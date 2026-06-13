# SkillCentral

> 统一管理所有 AI Agent 的 Skill —— 一键启用、跨工具同步、可视化浏览

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.5+-green.svg)](https://www.qt.io/)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

---

## 痛点

| 问题 | SkillCentral 的解决方案 |
|------|------------------------|
| 安装了 skill 容易忘记 | 卡片式浏览，一眼看清所有 skill |
| 换 agent 工具要重装 skill | 一键启用/禁用，自动复制到对应目录 |
| skill 太多，缺乏管理 | 标签系统 + 实时搜索 + 频率分级 |
| 担心 skill 丢失 | 自动备份 + 一键恢复 |

## 截图

```
┌──────────────────────────────────────────────────────────────┐
│  SkillCentral              [搜索 skill 名称或标签 _______]  │
├──────────────────────────────────────────────────────────────┤
│  [#全部] [#设计] [#研究] [#PDF] [#学术]          [管理标签] │
├──────────────────────────────────────────────────────────────┤
│  [全选] [批量启用] [批量禁用] [批量删除]  [切换到小卡片]    │
├──────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────┐  ┌─────────────────┐                   │
│  │ ●常用 huashu    │  │ ●一般 deep-     │                   │
│  │  用HTML做高保真  │  │  research       │                   │
│  │  原型、交互Demo  │  │  深度研究...    │                   │
│  │  #设计 #原型     │  │  #研究 #学术    │                   │
│  │  [W][CL]        │  │  [W][CL][M]     │                   │
│  └─────────────────┘  └─────────────────┘                   │
│                                                              │
└──────────────────────────────────────────────────────────────┘
```

## 功能特性

### Skill 管理
- **导入**：本地文件夹、ZIP 包、GitHub 仓库、拖拽导入
- **导出**：单个/批量导出为 ZIP
- **浏览**：大卡片 / 小卡片切换，卡片式布局
- **搜索**：实时搜索 skill 名称和标签，支持多关键词 AND 匹配
- **删除**：右键卡片一键删除（同时清理所有 Agent 副本）
- **重复检测**：导入时自动检测，支持覆盖/重命名/取消

### Agent 管理
- **预置 Agent**：WorkBuddy、Claude Code、Codex、MimoCode、Trae、OpenCode
- **自定义 Agent**：添加任意 Agent，配置名称和 skill 存放路径
- **一键启用/禁用**：点击卡片底部图标，自动复制/删除 Agent 目录中的 skill
- **彩色图标**：每个 Agent 独立配色，直观区分

### 标签系统
- **手动管理**：右键卡片添加/删除标签
- **标签云**：顶部标签栏快速筛选
- **标签管理**：重命名、合并、清空所有标签

### 频率分级
- **四级频率**：常用 (绿)、一般 (橙)、很少 (灰)、废弃 (红)
- **点击切换**：点击频率圆点或文字直接修改
- **频率筛选**：按频率级别快速筛选

### 批量操作
- 全选/反选当前显示的 skill
- 批量启用/禁用到指定 Agent
- 批量删除
- 批量打标签

### 侧边栏预览
- 点击卡片右侧滑出详情面板
- 显示完整 SKILL.md 渲染内容（Markdown）
- 显示标签列表和启用的 Agent
- 编辑/删除/导出快捷操作

### 设置
- **中央库路径**：自定义 skill 存储位置
- **Agent 管理**：添加、编辑、启用/禁用、删除 Agent
- **自动备份**：支持每天/每周/每月自动备份
- **主题切换**：浅色 / 深色主题

## 技术栈

| 组件 | 技术 |
|------|------|
| 语言 | C++ 17 |
| UI 框架 | Qt 6 (Widgets) |
| 数据库 | SQLite 3 (Qt SQL) |
| 构建工具 | CMake 3.16+ |
| 编译器 | MinGW / MSVC / GCC |

## 构建

### 前置要求

- Qt 6.5 或更高版本
- CMake 3.16 或更高版本
- 支持 C++17 的编译器

### 编译步骤

```bash
# 克隆仓库
git clone https://github.com/your-username/SkillCentral.git
cd SkillCentral

# 配置（根据你的 Qt 安装路径调整）
cmake -B build -G "MinGW Makefiles" \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64" \
  -DQt6_DIR="C:/Qt/6.x.x/mingw_64/lib/cmake/Qt6"

# 编译
cmake --build build -j 8

# 运行
./build/bin/SkillCentral.exe
```

### 运行测试

```bash
cd build/bin
./test_DatabaseManager.exe
./test_SkillManager.exe
./test_AgentManager.exe
```

## 项目结构

```
SkillCentral/
├── CMakeLists.txt              # CMake 构建配置
├── README.md
├── docs/
│   └── REQUIREMENTS.md         # 需求规格文档
├── include/
│   ├── database/
│   │   └── DatabaseManager.h   # 数据库管理
│   ├── models/
│   │   ├── Skill.h             # Skill 数据模型
│   │   ├── SkillManager.h      # Skill 业务逻辑
│   │   └── AgentManager.h      # Agent 管理
│   └── ui/
│       ├── MainWindow.h        # 主窗口
│       ├── SkillCard.h         # Skill 卡片组件
│       ├── CardWidget.h        # 卡片网格容器
│       ├── SkillSidebar.h      # 详情侧边栏
│       ├── MarkdownRenderer.h  # Markdown 渲染器
│       ├── AgentDialog.h       # Agent 编辑对话框
│       ├── TagManagerDialog.h  # 标签管理对话框
│       └── SettingsDialog.h    # 设置对话框
├── src/
│   ├── main.cpp                # 程序入口
│   ├── database/
│   │   └── DatabaseManager.cpp
│   ├── models/
│   │   ├── Skill.cpp
│   │   ├── SkillManager.cpp
│   │   └── AgentManager.cpp
│   └── ui/
│       ├── MainWindow.cpp
│       ├── SkillCard.cpp
│       ├── CardWidget.cpp
│       ├── SkillSidebar.cpp
│       ├── MarkdownRenderer.cpp
│       ├── AgentDialog.cpp
│       ├── TagManagerDialog.cpp
│       └── SettingsDialog.cpp
├── tests/
│   ├── test_DatabaseManager.cpp
│   ├── test_SkillManager.cpp
│   └── test_AgentManager.cpp
└── resources/
    └── icons/                  # Agent 图标
```

## 数据库

SQLite 数据库位于中央库目录，包含以下表：

| 表名 | 说明 |
|------|------|
| `skills` | Skill 元数据（名称、路径、描述、频率） |
| `tags` | 标签定义 |
| `skill_tags` | Skill-标签多对多关系 |
| `agents` | Agent 配置（名称、路径、启用状态） |
| `skill_agents` | Skill-Agent 启用关系 |
| `settings` | 应用设置 |

## 使用场景

1. **初次使用**：设置 → Agent 管理 → 配置各 Agent 的 skill 存放路径 → 启用
2. **导入 skill**：文件 → 添加 Skill，选择包含 SKILL.md 的文件夹
3. **启用到 Agent**：点击卡片底部的 Agent 图标（点亮 = 启用）
4. **搜索和筛选**：使用搜索框或标签云快速定位
5. **批量操作**：勾选多个 skill，批量启用/禁用/删除/打标签
6. **备份**：设置 → 备份 → 立即备份，或开启自动备份

## 开发计划

- [x] 第一阶段：核心功能（数据库、卡片布局、导入/导出、搜索筛选）
- [x] 第二阶段：增强功能（批量操作、侧边栏预览、备份恢复）
- [x] 第三阶段：完善体验（拖拽导入、主题切换、Agent 图标）
- [ ] 第四阶段：测试与发布（集成测试、打包发布、用户文档）

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

MIT License

## 作者

Dongmiao
