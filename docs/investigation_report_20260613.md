# SkillCentral 异常文件夹来源调查报告

## 调查时间
2026-06-13

## 调查结论

### 1. 异常文件夹的来源

用户在 `D:/skillLibrary/skills/` 中看到的"异常文件夹"实际上是从以下 agent 的 skills 目录扫描并导入的：

#### 来自 WorkBuddy (Agent ID: 1)
- **路径**: `C:/Users/Dongmiao/.workbuddy/skills/`
- **Enabled**: 1 (已启用)
- **导入的 skills**:
  - 测试驱动开发（TDD）← `test-driven-development`
  - 代码审查接收 ← `receiving-code-review`
  - 花叔Design · Huashu-Design ← `huashu-design`
  - 派发并行代理 ← `dispatching-parallel-agents`
  - 请求代码审查 ← `requesting-code-review`
  - 使用 Git 工作树 ← `using-git-worktrees`
  - 头脑风暴：将想法转化为设计 ← `brainstorming`
  - 完成开发分支 ← `finishing-a-development-branch`
  - 完成前验证 ← `verification-before-completion`
  - 系统化调试 ← `systematic-debugging`
  - 执行计划 ← `executing-plans`
  - 子代理驱动开发 ← `subagent-driven-development`
  - 编写计划 ← `writing-plans`
  - Skill Discovery And Feedback ← `skills-vote`

#### 来自 Claude Code (Agent ID: 2)
- **路径**: `C:\Users\Dongmiao\.claude\skills`
- **Enabled**: 1 (已启用)
- **导入的 skills**:
  - 花叔Design · Huashu-Design (重复)

#### 来自 Codex (Agent ID: 3)
- **路径**: `C:\Users\Dongmiao\.codex\skills\skills`
- **Enabled**: 1 (已启用)
- **导入的 skills**:
  - Academic Paper — Academic Paper Writing Agent Team
  - Academic Paper Reviewer v1.9.0 — Multi-Perspective Academic Paper Review Agent Team
  - Academic Pipeline v3.8.2 — Full Academic Research Workflow Orchestrator
  - Andrej Karpathy Skills - Claude Code 行为准则
  - anysearch-skill
  - Chinese Novelist: 中文小说创作助手
  - Deep Research — Universal Academic Research Agent Team

---

### 2. 问题根源

#### 2.1 自动扫描机制
从源码 `DatabaseManager::initializeDatabase()` 可以看到，系统预设了以下 agents：
```cpp
QVector<QPair<QString, QString>> presetAgents = {
    {"WorkBuddy", "C:/Users/Dongmiao/.workbuddy/skills/"},
    {"Claude Code", "~/.claude/skills/"},
    {"Codex", ""},
    {"MimoCode", ""},
    {"Trae", ""},
    {"OpenCode", ""}
};
```

当 `AgentManager::scanAllAgents()` 被调用时，它会扫描所有 enabled agents 的 path，并将找到的 skills 导入到中央库 `D:/skillLibrary/skills/`。

#### 2.2 扫描逻辑
`AgentManager::scanAgentPath()` 的关键逻辑（第 247-394 行）：
1. 读取 agent 路径下的所有子目录
2. 检查每个子目录是否包含 `SKILL.md`
3. 如果包含，解析 skill 名称
4. 检查中央库是否已存在该 skill
5. 如果不存在，调用 `SkillManager::importFromFolder()` 导入
6. 导入时会将 skill 复制到 `D:/skillLibrary/skills/`

---

### 3. 重复导入问题

#### 3.1 数据库中的重复记录
从 `skills` 表可以看到，很多 skills 有重复的记录和带括号编号的版本：

| 原始名称 | 重复名称 | ID (原始) | ID (重复) |
|---------|---------|-----------|-----------|
| 花叔Design · Huashu-Design | 花叔Design · Huashu-Design (1) | 54 | 28 |
| Academic Paper — Academic Paper Writing Agent Team | Academic Paper — Academic Paper Writing Agent Team (1) | 55 | 29 |
| Academic Pipeline v3.8.2 | Academic Pipeline v3.8.2 (1) | 56 | 30 |
| Andrej Karpathy Skills | Andrej Karpathy Skills (1) | 57 | 31 |
| anysearch-skill | anysearch-skill (1) | 58 | 32 |
| Deep Research | Deep Research (1) | 60 | 33 |
| 派发并行代理 | 派发并行代理 (1) | 63 | 36 |
| ... | ... | ... | ... |

#### 3.2 重复原因
根据 `SkillManager::importFromFolder()` 的逻辑（第 74-77 行）：
```cpp
// Check if display name already exists in database
if (checkDuplicate(skillName)) {
    qInfo() << "Duplicate display name in DB, generating unique:" << skillName;
    skillName = generateUniqueName(skillName);  // 生成 "name (1)"
}
```

但当文件夹名已存在时，也会生成唯一的文件夹名（第 66-71 行）：
```cpp
// Ensure folderName is unique in file system
QString baseFolderName = folderName;
int folderCounter = 1;
while (QDir(QDir::cleanPath(m_libraryPath + "/" + folderName)).exists()) {
    folderName = baseFolderName + "_" + QString::number(folderCounter);
    folderCounter++;
}
```

**问题**: 显示名称和文件夹名的去重逻辑不一致，可能导致数据库记录重复但文件夹名不同。

---

### 4. 用户级 skills 目录的内容

#### 4.1 WorkBuddy skills 目录
路径: `C:/Users/Dongmiao/.workbuddy/skills/`

包含以下 skills（部分列表）：
- `academic-paper-reviewer` (符号链接)
- `academic-pipeline` (符号链接)
- `andrej-karpathy-skills/`
- `anysearch-skill/`
- `brainstorming/`
- `chinese-novelist-skill/`
- `dispatching-parallel-agents/`
- `executing-plans/`
- `finishing-a-development-branch/`
- `huashu-design/`
- `receiving-code-review/`
- `requesting-code-review/`
- `skills-vote/`
- `subagent-driven-development/`
- `systematic-debugging/`
- `test-driven-development/`
- `using-git-worktrees/`
- `verification-before-completion/`
- `writing-plans/`

#### 4.2 Claude Code skills 目录
路径: `C:/Users/Dongmiao/.claude/skills/`

包含：
- `huashu-design/` (与 WorkBuddy 重复)

#### 4.3 Codex skills 目录
路径: `C:/Users/Dongmiao/.codex/skills/skills/`

包含：
- `academic-paper/`
- `academic-paper-reviewer/`
- `academic-pipeline/`
- `andrej-karpathy-skills/`
- `anysearch-skill/`
- `chinese-novelist-skill/`
- `deep-research/`

---

### 5. 为什么这些文件夹会出现在中央库？

根据当前的设计，这是**正常行为**：
1. SkillCentral 会扫描所有已启用的 agents 的 skills 目录
2. 如果发现新的 skills（包含 SKILL.md 的目录），会自动导入到中央库
3. 导入的 skills 会被复制到 `D:/skillLibrary/skills/`

**如果用户认为这些文件夹"不应该存在"，可能的原因：**
1. 这些 skills 是 WorkBuddy/Claude Code/Codex 内置的，不应该被导入到中央库
2. 用户不希望这些 skills 出现在中央库中
3. 用户可能误以为中央库应该只包含特定的 skills

---

## 建议的解决方案

### 方案 1: 禁用不需要的 agents
如果不需要从某个 agent 导入 skills，可以在 SkillCentral 中禁用该 agent：
- 打开 SkillCentral
- 进入 Agent 管理界面
- 将 WorkBuddy/Claude Code/Codex 设为 `enabled = 0`

### 方案 2: 修改 agent 的 skills 路径
如果 agent 的 skills 路径不正确，可以修改：
```sql
UPDATE agents SET path = '' WHERE name = 'WorkBuddy';
```

### 方案 3: 添加过滤规则
在 `AgentManager::scanAgentPath()` 中添加过滤规则，跳过某些 skills 或目录。

### 方案 4: 清理重复记录
清理数据库中的重复 records 和文件夹：
1. 查找重复的 skill 名称
2. 保留最新的记录，删除旧记录
3. 删除对应的文件夹

### 方案 5: 文档和用户界面改进
1. 为用户提供清晰的文档，说明哪些 agents 的 skills 会被自动导入
2. 在 UI 中添加"导入来源"列，显示每个 skill 是从哪个 agent 导入的
3. 添加"清理重复 skills"功能

---

## 技术细节

### 数据库表结构

#### skills 表
```sql
CREATE TABLE skills (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE,
    path TEXT,
    description TEXT,
    frequency INTEGER DEFAULT 1,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    last_used DATETIME
)
```

#### agents 表
```sql
CREATE TABLE agents (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE,
    path TEXT,
    enabled BOOLEAN DEFAULT 1,
    icon TEXT
)
```

#### skill_agents 表
```sql
CREATE TABLE skill_agents (
    skill_id INTEGER,
    agent_id INTEGER,
    enabled BOOLEAN DEFAULT 0,
    PRIMARY KEY (skill_id, agent_id),
    FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE,
    FOREIGN KEY (agent_id) REFERENCES agents(id) ON DELETE CASCADE
)
```

### 关键源码文件
- `D:/workbuddy/SkillCentral/src/database/DatabaseManager.cpp` - 数据库初始化和预设 agents
- `D:/workbuddy/SkillCentral/src/models/AgentManager.cpp` - Agent 管理和 skills 扫描
- `D:/workbuddy/SkillCentral/src/models/SkillManager.cpp` - Skill 导入和管理

---

## 附录: 完整数据库记录

### skills 表 (部分)
| ID | name | path |
|----|------|------|
| 27 | Chinese Novelist: 中文小说创作助手 | D:/skillLibrary/skills/Chinese Novelist- 中文小说创作助手 |
| 28 | 花叔Design · Huashu-Design (1) | D:/skillLibrary/skills/花叔Design · Huashu-Design |
| 36 | 派发并行代理 (1) | D:/skillLibrary/skills/派发并行代理 |
| 37 | 执行计划 (1) | D:/skillLibrary/skills/执行计划 |
| ... | ... | ... |

### agents 表
| ID | name | path | enabled |
|----|------|------|---------|
| 1 | WorkBuddy | C:/Users/Dongmiao/.workbuddy/skills/ | 1 |
| 2 | Claude Code | C:\Users\Dongmiao\.claude\skills | 1 |
| 3 | Codex | C:\Users\Dongmiao\.codex\skills\skills | 1 |
| 4 | MimoCode | (空) | 0 |
| 5 | Trae | (空) | 0 |
| 7 | OpenCode | (空) | 0 |

### skill_agents 表 (部分)
| skill_id | agent_id | enabled |
|----------|----------|---------|
| 27 | 1 | 1 |
| 27 | 3 | 1 |
| 28 | 2 | 1 |
| 36 | 1 | 1 |
| 37 | 1 | 1 |
| ... | ... | ... |

---

## 调查工具

本调查使用了以下工具：
- `ls` - 列出目录内容
- `python` + `sqlite3` - 读取 SQLite 数据库
- `Read` - 读取源码文件
- `Glob` - 搜索文件

调查过程中没有修改任何代码或数据。
