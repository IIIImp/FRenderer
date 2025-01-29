#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Camera {
public:
    Camera(float fov, float aspectRatio, float nearPlane, float farPlane);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);
    float cameraSpeed = 0.1f;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    float pitch = 0.0f;
    float yaw = 0.0f;
    bool isMousePressed = false;
    bool isFirstMouse = true;
    float deltaTime;

    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }

private:
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    bool isPerspective;

};

#endif