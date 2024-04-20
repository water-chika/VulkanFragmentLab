cmake_minimum_required(VERSION 3.20)

project(VulkanFragmentLab)

add_subdirectory(glfwpp)

find_package(Vulkan REQUIRED)

add_executable(VulkanFragmentLab
    main.cpp)

set_target_properties(VulkanFragmentLab PROPERTIES CXX_STANDARD 23)

target_link_libraries(VulkanFragmentLab PUBLIC glfwpp)
