#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_STORAGE_SHARED_EXPORT
#define VULKAN_HPP_STORAGE_SHARED
#define VULKAN_HPP_NO_NODISCARD_WARNINGS
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include <cassert>

class VulkanApp
{
public:
    VulkanApp();
    virtual ~VulkanApp();
    void createInstance();
    void createDevice(size_t physicalDeviceIdx = 0);
    void createCommandPools();
    void getQueues();

protected:
	vk::UniqueInstance    instance;
	vk::PhysicalDevice    physicalDevice;
	vk::UniqueDevice      device;

    vk::DynamicLoader  dl;

    void pushExtentionsForGraphics();
    void initDebugReportCallback();

    struct CommandPool
    {
        vk::UniqueCommandPool graphics;
        vk::UniqueCommandPool transfer;
        vk::UniqueCommandPool compute;
    } commandPool;

    struct QueueFamilyIndex
    {
        uint32_t graphics = -1;
        uint32_t transfer = -1;
        uint32_t compute = -1;
    } queueFamilyIndex;
    struct Queue
    {
        vk::Queue graphics;
        vk::Queue transfer;
        vk::Queue compute;
    } queue;

    void setQueueFamilyIndices();

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
    uint32_t getMemoryType(const vk::MemoryRequirements& memoryRequirements, const vk::MemoryPropertyFlags& memoryProperties);

private:
    vk::UniqueDebugUtilsMessengerEXT debugMessenger;
    std::vector<const char*> instanceLayers;
    std::vector<const char*> instanceExtensions;
    std::vector<const char*> deviceLayers;
    std::vector<const char*> deviceExtensions;

    void checkValidationLayers(const std::vector<const char*>& layersToCheck);

    void checkExtensions(const std::vector<const char*>& extensionsToCheck);
};
