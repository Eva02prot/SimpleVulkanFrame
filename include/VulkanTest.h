#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

class VulkanTest
{
public:
	void run(); // 开始运行

private:
	void initVulkan();  // 初始化Vulkan实例、包括检测扩展、校验层、初始化debug等 

	void initWindows();  // 初始化glfw的窗口

	void mainLoop();    // 主循环

	void cleanup();   // 释放所有申请的资源

	GLFWwindow* m_window = nullptr;

	VkInstance m_vkInstance;



	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
};