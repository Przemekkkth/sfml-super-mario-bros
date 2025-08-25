#include "AABB.h"

bool AABB::isCollision(TransformComponent *a, TransformComponent *b)
{
    const float x1 = a->getPosition().x + a->getHitbox().position.x;
    const float y1 = a->getPosition().y + a->getHitbox().position.y;
    const float w1 = a->getHitbox().size.x;
    const float h1 = a->getHitbox().size.y;
    return isTotalCollision(x1, y1, w1, h1, b);
}

bool AABB::isCollision(float x, float y, float w, float h, TransformComponent *b)
{
    const float x2 = b->getPosition().x + b->getHitbox().position.x;
    const float y2 = b->getPosition().y + b->getHitbox().position.y;
    const float w2 = b->getHitbox().size.x;
    const float h2 = b->getHitbox().size.y;
    return isCollision(x, y, w, h, x2, y2, w2, h2);
}

bool AABB::isCollision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    const bool overlapXLeft = (x1 <= x2 + w2);
    const bool overlapXRight = (x1 + w1 >= x2);
    const bool overlapYTop = (y1 <= y2 + h2);
    const bool overlapYBottom = (y1 + h1 >= y2);

    return overlapXLeft && overlapXRight && overlapYTop && overlapYBottom;
}

bool AABB::isTotalCollision(TransformComponent *a, TransformComponent *b)
{
    const float x1 = a->getPosition().x + a->getHitbox().position.x;
    const float y1 = a->getPosition().y + a->getHitbox().position.y;
    const float w1 = a->getHitbox().size.x;
    const float h1 = a->getHitbox().size.y;
    return isTotalCollision(x1, y1, w1, h1, b);
}

bool AABB::isTotalCollision(float x, float y, float w, float h, TransformComponent *b)
{
    const float x2 = b->getPosition().x + b->getHitbox().position.x;
    const float y2 = b->getPosition().y + b->getHitbox().position.y;
    const float w2 = b->getHitbox().size.x;
    const float h2 = b->getHitbox().size.y;
    return isTotalCollision(x, y, w, h, x2, y2, w2, h2);
}

bool AABB::isTotalCollision(
    float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    const bool overlapXLeft = x1 < x2 + w2;
    const bool overlapXRight = x1 + w1 > x2;
    const bool overlapYTop = y1 < y2 + h2;
    const bool overlapYBottom = y1 + h1 > y2;

    return overlapXLeft && overlapXRight && overlapYTop && overlapYBottom;
}

AABB::AABB() {}
