#ifndef ENEMYOBJECT_H
#define ENEMYOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <bits/stdc++.h>

#include "game_object.h"
#include "texture.h"


class EnemyObject : public GameObject
{
public:
    // constructor(s)
    float enemyvelocity;
    EnemyObject();
    EnemyObject(glm::vec2 pos, glm::vec2 Size, Texture2D sprite);
    glm::vec2 Move(float dt, unsigned int window_width);
};

#endif