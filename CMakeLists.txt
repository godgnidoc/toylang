cmake_minimum_required(VERSION 3.28)

project($ENV{PROJECT_NAME} CXX)

# 设置编译器选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-pthread -fPIC -Wall -Wextra -Werror=delete-non-virtual-dtor -Werror=return-type")
set(CMAKE_BUILD_TYPE $ENV{BUILD_TYPE})

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED)

# 搜索全部源代码
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

# 添加头文件搜索路径
include_directories(include)

# 添加可执行程序目标
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE
  fmt::fmt-header-only 
  spdlog::spdlog_header_only 
  nlohmann_json::nlohmann_json)

# 添加测试目标
enable_testing()
file(GLOB_RECURSE TEST_FILES test/*.cpp)
add_executable(${PROJECT_NAME}_test ${TEST_FILES} ${SOURCE_FILES})
target_compile_definitions(${PROJECT_NAME}_test PRIVATE UNIT_TEST)
target_link_libraries(${PROJECT_NAME}_test 
  GTest::GTest GTest::Main 
  fmt::fmt-header-only 
  spdlog::spdlog_header_only 
  nlohmann_json::nlohmann_json)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)