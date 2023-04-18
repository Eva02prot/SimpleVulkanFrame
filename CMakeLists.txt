project(VulkanTest)
cmake_minimum_required(VERSION 3.21) 
set(CMAKE_CXX_STANDARD 20)

set(GLSL_VALIDATOR "glslangValidator")

set(VK_SDK_PATH "D:/Vulkan")
set(VK_SDK_INCLUDE ${VK_SDK_PATH}/Include)
set(VK_SDK_LIB ${VK_SDK_PATH}/Lib/vulkan-1.lib)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE HEADER_FILES ${PROJECT_SOURCE_DIR} "include/*.h")
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR} "src/*.cpp")

add_executable(VulkanTest ${SOURCE_FILES})

target_link_libraries(VulkanTest PRIVATE glm::glm)
target_link_libraries(VulkanTest PRIVATE glfw)
target_link_libraries(VulkanTest PRIVATE ${VK_SDK_LIB})

target_include_directories(VulkanTest PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(VulkanTest PRIVATE ${VK_SDK_INCLUDE})