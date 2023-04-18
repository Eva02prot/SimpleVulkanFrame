#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

class VulkanTest
{
public:
	void run(); // ��ʼ����

private:
	void initVulkan();  // ��ʼ��Vulkanʵ�������������չ��У��㡢��ʼ��debug�� 

	void initWindows();  // ��ʼ��glfw�Ĵ���

	void mainLoop();    // ��ѭ��

	void cleanup();   // �ͷ������������Դ

	GLFWwindow* m_window = nullptr;

	VkInstance m_vkInstance;



	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
};