/******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "enemy_object.h"

#define ENEMY_VELOCITY 100.0f

EnemyObject::EnemyObject(glm::vec2 pos, glm::vec2 Size, Texture2D sprite)
{
    this->Position = pos;
    this->Size = Size;
    this->Sprite = sprite;
    this->enemyvelocity = 100.0f;
}

glm::vec2 EnemyObject::Move(float dt, unsigned int window_width)
{
    this->Position.x += this->enemyvelocity * dt;
    if (this->Position.x <= 0.0f)
    {
        this->enemyvelocity = -this->enemyvelocity;
        this->Position.x = 0.0f;
    }
    else if (this->Position.x + this->Size.x >= window_width)
    {
        this->enemyvelocity = -this->enemyvelocity;
        this->Position.x = window_width - this->Size.x;
    }
    return this->Position;
}