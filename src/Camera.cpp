#include "Camera.h"
#include "ECS/components/TransformComponent.h"
#include "Globals.h"

Camera Camera::instance;

void Camera::setCameraX(float x)
{
    m_cameraX = x;
}

void Camera::setCameraY(float y)
{
    m_cameraY = y;
}

void Camera::increaseCameraX(float value)
{
    m_cameraX += value;
}

void Camera::updateCameraMin()
{
    m_cameraMinX = m_cameraX;
}

void Camera::setCameraLeft(float x)
{
    m_cameraMinX = x;
}

void Camera::setCameraRight(float x)
{
    setCameraX(x - GLOBALS::WINDOW_WIDTH);
}

void Camera::setCameraFrozen(bool val)
{
    m_frozen = val;
}

void Camera::setCameraMinX(float x)
{
    m_cameraMinX = x;
}

void Camera::setCameraMaxX(float x)
{
    m_cameraMaxX = x;
}

float Camera::getCameraX()
{
    return m_cameraX;
}

float Camera::getCameraY()
{
    return m_cameraY;
}

float Camera::getCameraCenterX()
{
    return getCameraX() + (GLOBALS::WINDOW_WIDTH / 2.0f);
}

float Camera::getCameraCenterY()
{
    return getCameraY() + (GLOBALS::WINDOW_HEIGHT / 2.0f);
}

float Camera::getCameraLeft()
{
    return getCameraX();
}

float Camera::getCameraRight()
{
    return getCameraX() + GLOBALS::WINDOW_WIDTH;
}

float Camera::getCameraMinX()
{
    return m_cameraMinX;
}

float Camera::getCameraMaxX()
{
    return m_cameraMaxX;
}

bool Camera::inCameraRange(TransformComponent *position)
{
    return inCameraXRange(position) && inCameraYRange(position);
}

bool Camera::inCameraXRange(TransformComponent *positionComponent)
{
    return positionComponent->getPosition().x
                   + (positionComponent->getSize().x * GLOBALS::SCALED_CUBE_SIZE)
               >= getCameraX()
           && positionComponent->getPosition().x <= getCameraX() + GLOBALS::WINDOW_WIDTH;
}

bool Camera::inCameraYRange(TransformComponent *positionComponent)
{
    return positionComponent->getPosition().y
                   + (positionComponent->getSize().y * GLOBALS::SCALED_CUBE_SIZE)
               >= getCameraY()
           && positionComponent->getPosition().y <= getCameraY() + GLOBALS::WINDOW_HEIGHT;
}

bool Camera::isFrozen()
{
    return m_frozen;
}
