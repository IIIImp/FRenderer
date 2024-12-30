#define GLFW_INCLUDE_VULKAN

#include "InstanceManager.h"

#include <vector>
#include <iostream>


InstanceManager::InstanceManager()
{
	initGLFW();
	createInstance();
	createSurface();
}

InstanceManager::~InstanceManager()
{
	vkDestroySurfaceKHR(vkInstance, surface, nullptr);

	vkDestroyInstance(InstanceManager::vkInstance, nullptr);

	glfwDestroyWindow(window);
	glfwTerminate();
}

InstanceManager& InstanceManager::getInstanceManager()
{
	static InstanceManager instanceManager;
	return instanceManager;
}

GLFWwindow* InstanceManager::getWindow() const
{
	return window;
}

VkInstance InstanceManager::getVkInstance() const
{
	return InstanceManager::vkInstance;
}

VkSurfaceKHR InstanceManager::getSurface() const
{
	return surface;
}

void InstanceManager::createInstance()
{
	// Set application information
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello World For Renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Set create instance information
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Get extension for glfw
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::cout << glfwExtensionCount << std::endl;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	// Get extension list
	if (DEBUG) {
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensionList(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionList.data());
		std::cout << "available extensions:" << std::endl;
		for (const auto& extension : extensionList) {
			std::cout << "\t" << extension.extensionName << std::endl;
		}
	}

	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance");
	}
}

void InstanceManager::initGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Bind to Vulkan instead of OpenGL	
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);    // Set window resizable

	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("Falied to create window");
	}
}

void InstanceManager::createSurface()
{
	if (glfwCreateWindowSurface(vkInstance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Falied to create window surface");
	}
}

