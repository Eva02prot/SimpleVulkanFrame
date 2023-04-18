#include "VulkanApp.h"

PFN_vkCreateDebugUtilsMessengerEXT  pfnVkCreateDebugUtilsMessengerEXT;
PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT;

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessageFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void*)
{
    static std::string message{};
    std::string newMessage = pCallbackData->pMessage;
    if (message != newMessage)
    {
        std::cout << "\n" << newMessage << "\n";
        message = std::move(newMessage);
    }

    return false;
}

VulkanApp::VulkanApp()
{
}

VulkanApp::~VulkanApp()
{
}

void VulkanApp::createInstance()
{
    // https://github.com/KhronosGroup/Vulkan-Hpp#extensions--per-device-function-pointers
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = this->dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    checkValidationLayers(this->instanceLayers);
    checkExtensions(this->instanceExtensions);
    vk::ApplicationInfo applicationInfo("Vulkan Light Bakery", 1, "Asama", 1, VK_API_VERSION_1_2);
    vk::InstanceCreateInfo instanceCreateInfo(vk::InstanceCreateFlags(), &applicationInfo,
        uint32_t(this->instanceLayers.size()), this->instanceLayers.data(),
        uint32_t(this->instanceExtensions.size()), this->instanceExtensions.data());
    this->instance = vk::createInstanceUnique(instanceCreateInfo);

    VULKAN_HPP_DEFAULT_DISPATCHER.init(this->instance.get());

    initDebugReportCallback();

	D:\CplusCode\VulkanTest\out\build\x64 - Debug\VulkanTest	D : \CplusCode\VulkanTest\out\build\x64 - Debug\VulkanApp.cpp.obj	1

}

void VulkanApp::checkValidationLayers(const std::vector<const char*>& layersToCheck)
{
    auto availableLayers = vk::enumerateInstanceLayerProperties();
    assert(availableLayers.size());

    auto checkValidationLayer = [&availableLayers](const char* wantedLayerName)
    {
        auto compareNames = [&wantedLayerName](const vk::LayerProperties& props)
        {
            std::string temp = props.layerName;
            return temp == std::string(wantedLayerName);
        };

        if (std::find_if(availableLayers.begin(), availableLayers.end(), compareNames) == availableLayers.end())
        {
            throw std::runtime_error("layer not found");
        }
    };

    std::for_each(layersToCheck.begin(), layersToCheck.end(), checkValidationLayer);
}

void VulkanApp::checkExtensions(const std::vector<const char*>& extensionsToCheck)
{
    auto availableExtensions = vk::enumerateInstanceExtensionProperties();
    assert(availableExtensions.size());

    auto checkExtension = [&availableExtensions](const char* wantedExtensionName)
    {
        auto compareNames = [&wantedExtensionName](const vk::ExtensionProperties& props)
        {
            std::string temp = props.extensionName;
            return temp == std::string(wantedExtensionName);
        };

        if (std::find_if(availableExtensions.begin(), availableExtensions.end(), compareNames) == availableExtensions.end())
        {
            throw std::runtime_error("extension not found");
        }
    };

    std::for_each(extensionsToCheck.begin(), extensionsToCheck.end(), checkExtension);
}

void VulkanApp::initDebugReportCallback()
{
    assert(this->instance.get());

    pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        this->instance.get().getProcAddr("vkCreateDebugUtilsMessengerEXT"));
    assert(pfnVkCreateDebugUtilsMessengerEXT);

    pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        this->instance.get().getProcAddr("vkDestroyDebugUtilsMessengerEXT"));
    assert(pfnVkDestroyDebugUtilsMessengerEXT);

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    this->debugMessenger = this->instance.get().createDebugUtilsMessengerEXTUnique(
        vk::DebugUtilsMessengerCreateInfoEXT({}, severityFlags, messageTypeFlags, &debugMessageFunc));
}

void VulkanApp::createDevice(size_t physicalDeviceIdx)
{
    assert(this->instance.get());

    std::vector<vk::PhysicalDevice> devices = this->instance.get().enumeratePhysicalDevices();
    assert(devices.size());
    this->physicalDevice = devices[physicalDeviceIdx];
    this->physicalDeviceMemoryProperties = this->physicalDevice.getMemoryProperties();

    setQueueFamilyIndices();
    float queuePriorities = 1.f;
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(3);
    for (auto& info : queueCreateInfos)
    {
        info.setQueueCount(1).setPQueuePriorities(&queuePriorities);
    }
    queueCreateInfos[0]
        .setQueueFamilyIndex(this->queueFamilyIndex.graphics);
    queueCreateInfos[1]
        .setQueueFamilyIndex(this->queueFamilyIndex.transfer);
    queueCreateInfos[2]
        .setQueueFamilyIndex(this->queueFamilyIndex.compute);

    vk::DeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo
        .setQueueCreateInfos(queueCreateInfos)
        .setPEnabledExtensionNames(this->deviceExtensions)
        .setPEnabledLayerNames(this->deviceLayers);

    auto c = vk::StructureChain<
        vk::DeviceCreateInfo,
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceRayTracingPipelineFeaturesKHR,
        vk::PhysicalDeviceAccelerationStructureFeaturesKHR,
        vk::PhysicalDeviceVulkan12Features>{
            deviceCreateInfo,
            vk::PhysicalDeviceFeatures2(),
            vk::PhysicalDeviceRayTracingPipelineFeaturesKHR(),
            vk::PhysicalDeviceAccelerationStructureFeaturesKHR(),
            vk::PhysicalDeviceVulkan12Features()
    };

    this->physicalDevice.getFeatures2(&c.get<vk::PhysicalDeviceFeatures2>());
    this->device = this->physicalDevice.createDeviceUnique(c.get<vk::DeviceCreateInfo>());

    VULKAN_HPP_DEFAULT_DISPATCHER.init(this->device.get());
}

void VulkanApp::setQueueFamilyIndices()
{
    auto queueFamilies = this->physicalDevice.getQueueFamilyProperties();

    for (int i{}; i < queueFamilies.size(); ++i)
    {
        auto& prop = queueFamilies[i];
        if (prop.queueCount > 0)
        {
            if ((prop.queueFlags & vk::QueueFlagBits::eGraphics) && this->queueFamilyIndex.graphics == -1)
            {
                this->queueFamilyIndex.graphics = i;
            }
            else if ((prop.queueFlags & vk::QueueFlagBits::eTransfer) && this->queueFamilyIndex.transfer == -1)
            {
                this->queueFamilyIndex.transfer = i;
            }
            else if ((prop.queueFlags & vk::QueueFlagBits::eCompute) && this->queueFamilyIndex.compute == -1)
            {
                this->queueFamilyIndex.compute = i;
            }
        }
    }

    if (queueFamilyIndex.graphics != -1)
    {
        auto& computeIdx = this->queueFamilyIndex.compute;
        auto& transferIdx = this->queueFamilyIndex.transfer;
        computeIdx = computeIdx == -1 ? this->queueFamilyIndex.graphics : computeIdx;
        transferIdx = transferIdx == -1 ? this->queueFamilyIndex.graphics : transferIdx;
    }
    else
    {
        throw std::runtime_error("could not find graphics queue!");
    }
}

void VulkanApp::createCommandPools()
{
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    poolInfo.setQueueFamilyIndex(this->queueFamilyIndex.graphics);
    this->commandPool.graphics = this->device.get().createCommandPoolUnique(poolInfo, nullptr);
    poolInfo.setQueueFamilyIndex(this->queueFamilyIndex.transfer);
    this->commandPool.transfer = this->device.get().createCommandPoolUnique(poolInfo, nullptr);
    poolInfo.setQueueFamilyIndex(this->queueFamilyIndex.compute);
    this->commandPool.compute = this->device.get().createCommandPoolUnique(poolInfo, nullptr);
}

void VulkanApp::getQueues()
{
    this->queue.graphics = this->device.get().getQueue(this->queueFamilyIndex.graphics, 0);
    this->queue.transfer = this->device.get().getQueue(this->queueFamilyIndex.transfer, 0);
    this->queue.compute = this->device.get().getQueue(this->queueFamilyIndex.compute, 0);
}
