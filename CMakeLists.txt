cmake_minimum_required(VERSION 3.16)
project(SkillCentral VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt6 路径 - 请根据实际安装版本调整 6.x.y
set(Qt6_DIR "F:/Qt/6.11.1/mingw_64/lib/cmake/Qt6" CACHE PATH "Qt6 CMake 路径")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Concurrent Test)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)

# === 源文件 ===
set(SOURCES
    # 数据库模块
    src/database/DatabaseManager.cpp
    # 模型模块
    src/models/Skill.cpp
    src/models/SkillManager.cpp
    src/models/AgentManager.cpp
    # UI 模块 - 卡片与主页
    src/ui/SkillCard.cpp
    src/ui/CardWidget.cpp
    src/ui/MainWindow.cpp
    # UI 模块 - 侧边栏与渲染
    src/ui/MarkdownRenderer.cpp
    src/ui/SkillSidebar.cpp
    # UI 模块 - 对话框
    src/ui/AgentDialog.cpp
    src/ui/TagManagerDialog.cpp
    src/ui/SettingsDialog.cpp
    # 主程序入口
    src/main.cpp
)

# === 头文件 ===
set(HEADERS
    # 数据库模块
    include/database/DatabaseManager.h
    # 模型模块
    include/models/Skill.h
    include/models/SkillManager.h
    include/models/AgentManager.h
    # UI 模块
    include/ui/SkillCard.h
    include/ui/CardWidget.h
    include/ui/MainWindow.h
    include/ui/MarkdownRenderer.h
    include/ui/SkillSidebar.h
    include/ui/AgentDialog.h
    include/ui/TagManagerDialog.h
    include/ui/SettingsDialog.h
)

# === 主程序 ===
add_executable(SkillCentral ${SOURCES} ${HEADERS})
target_link_libraries(SkillCentral Qt6::Core Qt6::Widgets Qt6::Sql Qt6::Concurrent)

# === 测试 ===
enable_testing()

# DatabaseManager 测试
add_executable(test_DatabaseManager
    tests/test_DatabaseManager.cpp
    src/database/DatabaseManager.cpp
    include/database/DatabaseManager.h
)
target_include_directories(test_DatabaseManager PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_DatabaseManager Qt6::Core Qt6::Test Qt6::Sql)
add_test(NAME DatabaseManagerTest COMMAND test_DatabaseManager)

# SkillManager 测试
add_executable(test_SkillManager
    tests/test_SkillManager.cpp
    src/models/Skill.cpp
    src/models/SkillManager.cpp
    include/models/Skill.h
    include/models/SkillManager.h
    src/database/DatabaseManager.cpp
    include/database/DatabaseManager.h
)
target_include_directories(test_SkillManager PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_SkillManager Qt6::Core Qt6::Test Qt6::Sql)
add_test(NAME SkillManagerTest COMMAND test_SkillManager)

# AgentManager 测试
add_executable(test_AgentManager
    tests/test_AgentManager.cpp
    src/models/AgentManager.cpp
    src/models/SkillManager.cpp
    src/models/Skill.cpp
    include/models/AgentManager.h
    include/models/SkillManager.h
    include/models/Skill.h
    src/database/DatabaseManager.cpp
    include/database/DatabaseManager.h
)
target_include_directories(test_AgentManager PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_AgentManager Qt6::Core Qt6::Test Qt6::Sql)
add_test(NAME AgentManagerTest COMMAND test_AgentManager)

# === 部署 Qt 依赖（windeployqt）===
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET SkillCentral POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:SkillCentral>"
            WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
    endif()
endif()
