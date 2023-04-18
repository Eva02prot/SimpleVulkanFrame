#include "VulkanTest.h"
#include "VulkanApp.h"
#include "iostream"

void VulkanTest::run() {
    //initWindows();
    initVulkan();
    //mainLoop();
    //cleanup();
}

void VulkanTest::initVulkan()
{
    VulkanApp* app = new VulkanApp();
    app->createInstance();
    app->createDevice();
    app->createCommandPools();
    app->getQueues();
}

int main() {
    VulkanTest helloWorld;
    helloWorld.run();
    return 0;
}