#include "window.h"
#include <stdexcept>
#include <iostream>

Window::Window(uint32_t width, uint32_t height, const char* title) : width(width), height(height)
{
	createWindow(title);
}

void Window::cleanup(VkInstance instance)
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::cameraMoveCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) {
		throw std::runtime_error("Failed");
	}
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
		case GLFW_KEY_W: camera->position += camera->cameraSpeed * camera->front; break; // 前移
		case GLFW_KEY_S: camera->position -= camera->cameraSpeed * camera->front; break; // 后退
		case GLFW_KEY_A: camera->position -= glm::normalize(glm::cross(camera->front, camera->up)) * camera->cameraSpeed; break; // 左移
		// case GLFW_KEY_A: camera->move(-glm::normalize(glm::cross(camera->front, camera->up)), camera->cameraSpeed, camera->deltaTime); break;
		case GLFW_KEY_D: camera->position += glm::normalize(glm::cross(camera->front, camera->up)) * camera->cameraSpeed; break; // 右移
		case GLFW_KEY_SPACE: camera->position += camera->cameraSpeed * camera->up; break; // 向上
		case GLFW_KEY_C: camera->position -= camera->cameraSpeed * camera->up; break; // 向下
		}
	}
}

void Window::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) {
		throw std::runtime_error("Failed");
	}
	if (!camera->isMousePressed) {
		return;
	}

	static float lastX = 400; // 初始化为屏幕中心
	static float lastY = 300; // 初始化为屏幕中心

	xpos = (float)xpos;
	ypos = (float)ypos;

	if (camera->isFirstMouse) {
		lastX = xpos;
		lastY = ypos;
		camera->isFirstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // 注意：y 坐标是反向的
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.5f; // 鼠标灵敏度
	xOffset *= sensitivity;
	yOffset *= sensitivity;
	camera->yaw += xOffset;
	camera->pitch += yOffset;
	if (camera->pitch > 89.0f) camera->pitch = 89.0f;
	if (camera->pitch < -89.0f) camera->pitch = -89.0f;
	if (camera->yaw > 180.0f) camera->yaw -= 360.0f;
	if (camera->yaw < -180.0f) camera->yaw += 360.0f;

	camera->updateCameraVectors();

}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (!camera) {
		throw std::runtime_error("Failed");
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			camera->isMousePressed = true;  // 按下左键时，开始旋转
		}
		else if (action == GLFW_RELEASE) {
			camera->isMousePressed = false;  // 松开左键时，停止旋转
			camera->isFirstMouse = true;
		}
	}
}


bool Window::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::createWindow(const char* title)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Bind to Vulkan instead of OpenGL	
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);    // Set window resizable
	


	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("Falied to create window");
	}
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

void Window::createSurface(VkInstance instance)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}
