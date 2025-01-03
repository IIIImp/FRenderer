#define GLFW_INCLUDE_VULKAN

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>  
#include "InstanceManager.h"
#include "FRender.h"
#include "DeviceManager.h"
#include "SwapchainManager.h"
#include "PipelineManager.h"
#include "FramebufferManager.h"
#include "CommandBuffersManager.h"
#include "RenderManager.h"

VulkanContext context;
InstanceManager& instanceManager = InstanceManager::getInstanceManager(context);
DeviceManager deviceManager(context);
SwapchainManager swapchainManager(context);
PipelineManager pipelineManager(context);
FramebufferManager framebufferManager(context);
CommandBufferManager commandBufferManager(context);
RenderManager renderManager(context);

void mainLoop() {
	while (!glfwWindowShouldClose(context.window)) {
		glfwPollEvents();
		renderManager.drawFrame();
	}
	vkDeviceWaitIdle(context.device);
}
int main() {
	mainLoop();
}