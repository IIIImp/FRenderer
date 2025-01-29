#ifndef WINDOW_H
#define WINDOW_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <memory>
#include "app/camera.h"

class Window {
public:
    Window(uint32_t width, uint32_t height, const char* title);
    
    void cleanup(VkInstance instance);

    void createSurface(VkInstance instance);

    GLFWwindow* getWindow() const { return window; }

    VkSurfaceKHR getSurface() const { return surface; }

    static void cameraMoveCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    bool shouldClose() const;

    void pollEvents();
    float deltaTime;

private:
    GLFWwindow* window = nullptr;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    uint32_t width;
    uint32_t height;

    void createWindow(const char* title);
};

#endif