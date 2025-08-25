#ifndef CAMERA_H
#define CAMERA_H

class TransformComponent;
class Camera
{
public:
    static Camera& GetInstance()
    {
        return instance;
    }

    void setCameraX(float x);
    void setCameraY(float y);
    void increaseCameraX(float value);
    void updateCameraMin();
    void setCameraLeft(float x);
    void setCameraRight(float x);
    void setCameraFrozen(bool val);
    void setCameraMinX(float x);
    void setCameraMaxX(float x);

    float getCameraX();
    float getCameraY();
    float getCameraCenterX();
    float getCameraCenterY();
    float getCameraLeft();
    float getCameraRight();
    float getCameraMinX();
    float getCameraMaxX();

    bool inCameraRange(TransformComponent* position);
    bool inCameraXRange(TransformComponent* position);
    bool inCameraYRange(TransformComponent* position);
    bool isFrozen();

private:
    Camera() = default;
    Camera(const Camera&) = delete;

    static Camera instance;

    float m_cameraX = 0.0f, m_cameraY = 0.0f;
    float m_cameraMinX = 0.0f, m_cameraMaxX = 0.0f;
    bool m_frozen = false;
};
#endif // CAMERA_H
