#ifndef AABB_H
#define AABB_H
#include "../ECS/components/TransformComponent.h"
class AABB
{
public:
    static bool isCollision(TransformComponent *a, TransformComponent *b);
    static bool isCollision(float x, float y, float w, float h, TransformComponent *b);
    static bool isCollision(
        float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

    static bool isTotalCollision(TransformComponent *a, TransformComponent *b);
    static bool isTotalCollision(float x, float y, float w, float h, TransformComponent *b);
    static bool isTotalCollision(
        float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

private:
    AABB();
};

#endif // AABB_H
