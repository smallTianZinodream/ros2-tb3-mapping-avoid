cmake_minimum_required(VERSION 3.8)
project(param_api_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

# 参数服务端
add_executable(param_api_cpp src/param_api_cpp.cpp)
ament_target_dependencies(param_api_cpp rclcpp)

# 参数客户端
add_executable(param_client src/param_client.cpp)
ament_target_dependencies(param_client rclcpp)

install(TARGETS param_api_cpp param_client
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
