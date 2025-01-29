#include "VulkanContext.h"
#include <set>

void VulkanContext::init(Window& window, std::vector<const char*>& deviceExtensions)
{
	createInstance();
	window.createSurface(contextData.instance);
	pickPhysicalDevice(window, deviceExtensions);
	createLogicalDevice(window, deviceExtensions);
}

void VulkanContext::cleanup()
{
	vkDestroyDevice(contextData.device, nullptr);
	vkDestroyInstance(contextData.instance, nullptr);
}

void VulkanContext::createInstance()
{
	// Set application information
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello World For Renderer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.apiVersion = VK_API_VERSION_1_0;

	// Get extensions for glfw
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Set instance information
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	// Create instance
	if (vkCreateInstance(&createInfo, nullptr, &contextData.instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vulkan instance!");
	}
}

void VulkanContext::pickPhysicalDevice(Window& window, std::vector<const char*>& deviceExtensions)
{
	// Count and record GPUs with Vulkan support
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(contextData.instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(contextData.instance, &deviceCount, devices.data());

	// Find a suitable physcial device
	for (const auto& device : devices) {
		if (isDeviceSuitable(device, window, deviceExtensions)) {
			contextData.physicalDevice = device;
			break;
		}
	}
	if (contextData.physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Fail to find a suitable GPU");
	}
}

void VulkanContext::createLogicalDevice(Window& window, std::vector<const char*>& deviceExtensions)
{
	// Create the queue createInfo for logical device based on the physical device
	QueueFamilyIndices indices = findQueueFamilies(contextData.physicalDevice, window);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Device features needed
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.sampleRateShading = VK_TRUE;

	// Create logical device
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (vkCreateDevice(contextData.physicalDevice, &createInfo, nullptr, &contextData.device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device");
	}

	// Store the queue
	vkGetDeviceQueue(contextData.device, indices.graphicsFamily, 0, &contextData.graphicsQueue);
	vkGetDeviceQueue(contextData.device, indices.presentFamily, 0, &contextData.presentQueue);
}

bool VulkanContext::isDeviceSuitable(VkPhysicalDevice device, Window& window, std::vector<const char*>& deviceExtensions)
{
	// Check DeviceProperties and Features suitable
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	bool DevicePropertiesSuitable =
		((deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			|| (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU))
		&& deviceFeatures.geometryShader;

	// Find a suitable queueFamily
	QueueFamilyIndices indices = findQueueFamilies(device, window);
	bool QueueFamilySuitable = indices.isComplete();

	// Check Extension Support
	bool extensionsSupported = checkDeviceExtensionSupport(device, deviceExtensions);

	// Check swapChain support
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, window);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return DevicePropertiesSuitable
		&& QueueFamilySuitable
		&& extensionsSupported
		&& swapChainAdequate;
}

QueueFamilyIndices VulkanContext::findQueueFamilies(VkPhysicalDevice device, Window& window)
{
	QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;

	// Record the queueFamily information
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	int properQueueIndex = 0;
	
	// Find a suitable queueFamily
	for (const auto& queueFamily : queueFamilies) {
		// Check Graphic support
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = properQueueIndex;
		}
		// Check present ability
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, properQueueIndex, window.getSurface(), &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = properQueueIndex;
		}
		// Check if indice is complete
		if (indices.isComplete()) {
			break;
		}
		properQueueIndex++;
	}
	this->queueFamilyIndices = indices;
	return indices;
}

bool VulkanContext::checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*>& deviceExtensions)
{
	// Record extensions
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// Check if all requiredExtensions available
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanContext::querySwapChainSupport(VkPhysicalDevice device, Window& window)
{
	// Get capabilities
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, window.getSurface(), &details.capabilities);

	// Get formats
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, window.getSurface(), &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, window.getSurface(), &formatCount, details.formats.data());
	}

	// Get presentModes
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, window.getSurface(), &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, window.getSurface(), &presentModeCount, details.presentModes.data());
	}
	this->swapchainSupportDetails = details;
	return details;
}
