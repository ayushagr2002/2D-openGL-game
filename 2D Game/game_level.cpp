#include "game_level.h"

#include <fstream>
#include <sstream>
#define BRICKS_PER_WALL 25

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    // load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // read each line from level file
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) // read each word separated by spaces
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Bricks)
        if (!tile.Destroyed)
            tile.Draw(renderer);

    for (GameObject &coin : this->Coins)
        if (!coin.IsCollected)
            coin.Draw(renderer);

    for (GameObject &enemy : this->Enemies)
        enemy.Draw(renderer);

    Door.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    // calculate dimensions
    srand(time(0));
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); // note we can index vector at [0] since this function is only called if height > 0
    // float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height;
    float unit_width = levelWidth / BRICKS_PER_WALL, unit_height = levelHeight / BRICKS_PER_WALL;

    for (unsigned y = 0; y < BRICKS_PER_WALL; y++)
    {

        glm::vec2 pos(0, unit_height * y);
        glm::vec2 size(unit_width, unit_height);
        GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
        obj.IsSolid = true;
        this->Bricks.push_back(obj);

        glm::vec2 posright(unit_width * (BRICKS_PER_WALL - 1), unit_height * y);
        GameObject obj2(posright, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
        obj2.IsSolid = true;
        this->Bricks.push_back(obj2);
    }

    for (unsigned x = 0; x < BRICKS_PER_WALL; x++)
    {

        glm::vec2 pos(unit_width * x, 0);
        glm::vec2 size(unit_width, unit_height);
        GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
        obj.IsSolid = true;
        this->Bricks.push_back(obj);

        glm::vec2 posdown(unit_width * x, unit_height * (BRICKS_PER_WALL - 1));
        GameObject BottomWall(posdown, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
        BottomWall.IsSolid = true;
        this->Bricks.push_back(BottomWall);
    }

    for (unsigned y = 1; y < height - 1; y++)
    {
        for (unsigned x = 1; x < width - 1; x++)
        {

            if (tileData[y][x] == 0)
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject Wall(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                Wall.IsSolid = true;
                this->Bricks.push_back(Wall);
            }
        }       
    }

    while (this->Coins.size() <= this->level * 15)
    {
        int xcor = rand() % (width - 2) + 1;
        int ycor = rand() % (height - 3) + 2;

        if (tileData[ycor][xcor])
        {
            glm::vec2 pos(unit_width * xcor, unit_height * ycor);
            glm::vec2 size(unit_width, unit_height);
            GameObject coin(pos, size, ResourceManager::GetTexture("coin"));
            coin.IsCollected = false;
            this->Coins.push_back(coin);
        }
    }

    while(this->rowsfilled.size() < this->level * 7)
    {
        int xcor = rand() % (width - 2) + 1;
        int ycor = rand() % (height - 3) + 2;
        if (tileData[ycor][xcor] && this->rowsfilled.find(unit_height * ycor) == this->rowsfilled.end())
        {
            glm::vec2 pos(unit_width * xcor, unit_height * ycor);
            glm::vec2 size(unit_width, unit_height);
            printf("Here");
            EnemyObject enemy(pos, size, ResourceManager::GetTexture("enemy"));
            this->Enemies.push_back(enemy);
            this->rowsfilled[unit_height * ycor]++;
        }
    }

    GameObject door(glm::vec2(levelWidth / 2.0f, unit_height), glm::vec2(30.0f, 30.0f), ResourceManager::GetTexture("door"));
    this->Door = door;
}