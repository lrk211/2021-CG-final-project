#include "Camera.h"

glm::mat4 Camera::GetViewMatrix()
{
  return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
{
  float velocity = MovementSpeed * deltaTime;
  if (direction == FORWARD && glm::vec3((Position + Front * velocity)).y >= 0)
    Position += Front * velocity;
  if (direction == BACKWARD&& glm::vec3((Position - Front * velocity)).y >= 0)
    Position -= Front * velocity;
  if (direction == LEFT&& glm::vec3((Position - Right * velocity)).y >= 0)
    Position -= Right * velocity;
  if (direction == RIGHT &&glm::vec3((Position + Right * velocity)).y >= 0)
    Position += Right * velocity;
}

void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
  xoffset *= this->MouseSensitivity;
  yoffset *= this->MouseSensitivity;

  this->Yaw += xoffset;
  this->Pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch)
  {
    if (this->Pitch > 89.0f)
      this->Pitch = 89.0f;
    if (this->Pitch < -89.0f)
      this->Pitch = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Eular angles
  this->updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
  if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
    this->Zoom -= yoffset;
  if (this->Zoom <= 1.0f)
    this->Zoom = 1.0f;
  if (this->Zoom >= 45.0f)
    this->Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  front.y = sin(glm::radians(this->Pitch));
  front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
  this->Front = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
  this->Up = glm::normalize(glm::cross(this->Right, this->Front));
}