第一步：
用多 agent 并行开发 SkillCentral 项目，项目路径 D:/workbuddy/SkillCentral。

技术要求：
- C++ 17 + Qt 6 + CMake + SQLite
- Qt 安装路径：F:\Qt
- 数据库使用 Qt SQL 模块（QSqlDatabase）

分三批执行：

第一批（先执行，完成后才能继续）：
  子 agent 1：实现数据库模块（DatabaseManager 类）
    - 创建 library.db，实现所有表（skills/tags/skill_tags/agents/skill_agents/settings）
    - 实现 CRUD 接口
    - 实现备份（exportBackup）和恢复（importBackup）
    - 写单元测试
    
  子 agent 2：实现 Skill 数据模型（Skill/SkillManager 类）
    - 解析 SKILL.md 提取描述
    - 实现导入逻辑（本地文件夹复制到 D:\skillLibrary\skills\）
    - 实现导出逻辑（打包 ZIP）
    - 实现验证（检查 SKILL.md 是否存在）

第一批两个 agent 完成后，告诉我，我再启动第二批。

要求：
- 使用 TDD 方式开发
- 每个 agent 完成后运行测试验证
- 代码符合 C++ 17 标准，使用 Qt 信号槽机制

第二步：
第一批已完成，现在启动第二批：

子 agent 3：实现 Agent 管理模块（AgentManager 类）
  - agents 表 CRUD
  - 启用 skill：复制文件夹到 agent 的 skill 路径
  - 禁用 skill：删除 agent 路径中的 skill 文件夹（需确认对话框）
  - 启动时自动扫描 agent 路径

子 agent 4：实现标签管理模块（TagManager 类）
  - tags/skill_tags 表 CRUD
  - 自动识别标签（解析 SKILL.md 标题和触发词）
  - 标签云数据接口（按使用频率排序）

子 agent 5：实现导入处理器（ImportHandler 类）
  - 本地文件夹导入
  - ZIP 导入（使用 Qt 的 QuaZip 或 QZipReader）
  - GitHub 导入（使用 QNetworkAccessManager 下载）
  - 重复检测（同名 skill 处理：覆盖/重命名/取消）

第二批三个 agent 并行执行，完成后告诉我。


第三步：
第二批已完成，现在启动第三批（UI 层，高度并行）：

子 agent 6：主窗口 + 卡片布局（MainWindow + SkillCard 类）
  - 主窗口：搜索框、标签云、工具栏
  - SkillCard：大卡片（250x200）和小卡片两种尺寸
  - 实时搜索过滤（不需要点按钮）
  - 按频率排序

子 agent 7：侧边栏预览（SkillSidebar 类）
  - 点击卡片滑出侧边栏
  - 渲染 SKILL.md（Markdown 格式，使用 QTextBrowser）
  - 显示标签列表、启用的 Agent 列表
  - 操作按钮：编辑/删除/导出

子 agent 8：设置对话框（SettingsDialog 类）
  - 中央库路径设置
  - Agent 管理（添加/编辑/启用/禁用）
  - 备份设置（自动备份频率）
  - 主题切换（浅色/深色）
  - 关于页面

三个 agent 并行执行，完成后集成测试。