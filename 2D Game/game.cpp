
#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "text_renderer.h"
#include <bits/stdc++.h>

#define BRICKS_PER_WALL 25

// Game-related State data
SpriteRenderer *Renderer;
GameObject *Player;
TextRenderer  *Text;

bool CheckCollision(GameObject &one, GameObject &two)
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
    delete Renderer;
    delete Player;
}

void Game::Init()
{
    // load shaders
    ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                      static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Shader temp = ResourceManager::GetShader("sprite");
    Renderer = new SpriteRenderer(temp);
    // load textures
    ResourceManager::LoadTexture("background.jpg", false, "background");
    ResourceManager::LoadTexture("awesomeface.png", true, "face");
    ResourceManager::LoadTexture("block.png", false, "block");
    ResourceManager::LoadTexture("block_solid.png", false, "block_solid");
    ResourceManager::LoadTexture("player.png", true, "player");
    ResourceManager::LoadTexture("coin.png", true, "coin");
    ResourceManager::LoadTexture("enemy.png", true, "enemy");
    ResourceManager::LoadTexture("door.png", false, "door");
    // load levels
    GameLevel one;
    one.level = 1;
    one.Load("levels/one.lvl", this->Width, this->Height);
    GameLevel two;
    two.level = 2;
    two.Load("levels/two.lvl", this->Width, this->Height);
    GameLevel three;
    three.level = 3;
    three.Load("levels/three.lvl", this->Width, this->Height);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Level = 0;
    // configure game objects
    glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y - ((float)this->Height / BRICKS_PER_WALL) - 2.0f);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("player"));
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("OCRAEXT.TTF", 24);
}

void Game::CollectCoins()
{
    for(GameObject &coin : this->Levels[this->Level].Coins)
    {
        if(!coin.IsCollected)
        {
            if(CheckCollision(*Player, coin))
            {
                coin.IsCollected = true;
                CoinsCollected++;
            }
        }
    }
}


void Game::MoveEnemies(float dt, float width)
{
    for(EnemyObject &enemy: this->Levels[this->Level].Enemies)
    {
        for(GameObject &wall : this->Levels[this->Level].Bricks)
        {
            if(wall.Position.y == enemy.Position.y)
            {
                if(enemy.enemyvelocity < 0.0f)
                if(enemy.Position.x >= wall.Position.x && enemy.Position.x <= wall.Position.x + wall.Size.x) {
                    enemy.enemyvelocity = -enemy.enemyvelocity;
                }

                if(enemy.enemyvelocity > 0.0f)
                if(enemy.Position.x + enemy.Size.x >= wall.Position.x && enemy.Position.x + enemy.Size.x <= wall.Position.x + wall.Size.x) {
                    enemy.enemyvelocity = -enemy.enemyvelocity;
                }
            }
        }
        enemy.Position.x += dt*enemy.enemyvelocity;
    }
}

void Game::CheckEnemyContact()
{
    for(EnemyObject &enemy : this->Levels[this->Level].Enemies)
    {
        if(CheckCollision(*Player, enemy))
        {
            this->State = GAME_LOST;
        }
    }
}

void Game::CheckExit()
{
    if(Player->Position.x >= this->Levels[this->Level].Door.Position.x && Player->Position.x + Player->Size.x <= this->Levels[this->Level].Door.Position.x + this->Levels[this->Level].Door.Size.x && Player->Position.y <= this->Levels[this->Level].Door.Position.y + this->Levels[this->Level].Door.Size.y)
    {
        this->Level++;
        Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y - ((float)this->Height / BRICKS_PER_WALL) - 2.0f);
    }
}

bool Game::CheckCollisionA(float dt)
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (Player->Position.x - PLAYER_VELOCITY * dt >= box.Position.x && Player->Position.x - PLAYER_VELOCITY * dt <= box.Position.x + box.Size.x)
        {
            if ((Player->Position.y >= box.Position.y && Player->Position.y <= box.Position.y + box.Size.y) || (Player->Position.y + Player->Size.y >= box.Position.y && Player->Position.y + Player->Size.y <= box.Position.y + box.Size.y))
                return true;
        }
    }
    return false;
}

bool Game::CheckCollisionD(float dt)
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (Player->Position.x + Player->Size.x + PLAYER_VELOCITY * dt >= box.Position.x && Player->Position.x + Player->Size.x + PLAYER_VELOCITY * dt <= box.Position.x + box.Size.x)
        {
            if ((Player->Position.y >= box.Position.y && Player->Position.y <= box.Position.y + box.Size.y) || (Player->Position.y + Player->Size.y >= box.Position.y && Player->Position.y + Player->Size.y <= box.Position.y + box.Size.y))
                return true;
        }
    }
    return false;
}

bool Game::CheckCollisionW(float dt)
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (Player->Position.y - PLAYER_VELOCITY * dt >= box.Position.y && Player->Position.y - PLAYER_VELOCITY * dt <= box.Position.y + box.Size.y)
        {
            if ((Player->Position.x >= box.Position.x && Player->Position.x <= box.Position.x + box.Size.x) || (Player->Position.x + Player->Size.x >= box.Position.x && Player->Position.x + Player->Size.x <= box.Position.x + box.Size.x))
                return true;
        }
    }

    return false;
}

bool Game::CheckCollisionS(float dt)
{
    for (GameObject &box : this->Levels[this->Level].Bricks)
    {
        if (Player->Position.y + Player->Size.y + PLAYER_VELOCITY * dt >= box.Position.y && Player->Position.y + Player->Size.y + PLAYER_VELOCITY * dt <= box.Position.y + box.Size.y)
        {
            if ((Player->Position.x >= box.Position.x && Player->Position.x <= box.Position.x + box.Size.x) || (Player->Position.x + Player->Size.x >= box.Position.x && Player->Position.x + Player->Size.x <= box.Position.x + box.Size.x))
                return true;
        }
    }

    return false;
}

void Game::Update(float dt)
{
    this->CollectCoins();
    this->MoveEnemies(dt, this->Width);
    this->CheckEnemyContact();
    this->CheckExit();
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= ((float)this->Width / BRICKS_PER_WALL))
            {
                if (!CheckCollisionA(dt))
                    Player->Position.x -= velocity;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x - ((float)this->Width / BRICKS_PER_WALL))
                if (!CheckCollisionD(dt))
                    Player->Position.x += velocity;
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Player->Position.y >= ((float)this->Height / BRICKS_PER_WALL))
                if (!CheckCollisionW(dt))
                    Player->Position.y -= velocity;
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Player->Position.y <= this->Height - Player->Size.y - ((float)this->Height / BRICKS_PER_WALL))
                if (!CheckCollisionS(dt))
                    Player->Position.y += velocity;
        }
    }
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        // draw backgrounds
        Texture2D temp = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(temp, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        // draw level
        this->Levels[this->Level].Draw(*Renderer);
        // draw player
        Player->Draw(*Renderer);
        std::string s = "Level: ";
        s = s + std::to_string(this->Level + 1);
        std::string coins = "Coins: ";
        coins = coins + std::to_string(this->CoinsCollected);
        std::string timeelapsed = "Time Elapsed: ";
        std::string finalstring =  s + " " + coins;
        Text->RenderText(finalstring , 5.0f, 5.0f, 1.0f);
    }
    else if(this->State == GAME_LOST)
    {
        Texture2D temp = ResourceManager::GetTexture("background");
        Renderer->DrawSprite(temp, glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        Text->RenderText("GAME OVER", 5.0f, 5.0f, 1.0f);
    }
}