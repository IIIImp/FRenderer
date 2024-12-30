#ifndef INSTANCEMANAGER_H
#define INSTANCEMANAGER_H


#include <GLFW/glfw3.h>

#include "vulkan/vulkan.h"
#include "FRender.h"

class InstanceManager {
public:
	static InstanceManager& getInstanceManager();

	GLFWwindow* getWindow() const;
	VkInstance getVkInstance() const;
	VkSurfaceKHR getSurface() const;

private:
	InstanceManager();
	~InstanceManager();

	VkInstance vkInstance;
	GLFWwindow* window;
	VkSurfaceKHR surface;

	void createInstance();
	void initGLFW();
	void createSurface();

	InstanceManager(const InstanceManager&) = delete;
	InstanceManager& operator=(const InstanceManager&) = delete;
};

#endif

