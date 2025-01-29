#include "camera.h"

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane)
	: fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane),
    position(0.0f), front(0.0f), up(0.0f), isPerspective(true) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    glm::mat4 view = glm::lookAt(position, position + front, up);
    return view;
}



glm::mat4 Camera::getProjectionMatrix() const
{
    if (isPerspective) {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
    else {
        float halfHeight = nearPlane * tan(glm::radians(fov) / 2);
        float halfWidth = halfHeight * aspectRatio;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
    }
}

void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    isPerspective = true;
    this->fov = fov;
    this->aspectRatio = aspectRatio;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
    isPerspective = false;
    this->aspectRatio = (right - left) / (top - bottom);
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}



