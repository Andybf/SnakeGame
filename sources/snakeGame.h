//
//  main.h
//  SnakeGame
//
//  Created by Anderson Bucchianico on 22/04/23.
//

#ifndef snakeGame_h
#define snakeGame_h

#include <Icosphere.hpp>

#define ARG_WINDOW_WIDTH  "-ww"
#define ARG_WINDOW_HEIGHT "-wh"

#define TEX_PIXEL_POINT 0.03125 // 1px / 32px

Camera* camera;
Projection* projection;
Entity* background;
Text* gameInterface;
Text* centerText;
Text* creditsText;

uint startTime = 0;
uint finishTime = 0;
ushort updateStep = 0;
const ushort movementSteps = 12;
const float updateRate = 1.0f/movementSteps;

float objectSize = 1;
bool isGameplayInterrupted = true;

uint points = 0;
uint pointsToMake = 0;
uint record = 0;
std::vector<glm::vec2> fillMap;

ushort windowHeight = 630;
ushort windowWidth = 600;

std::map<std::string, glm::vec2> spriteMap {
    {"background", glm::vec2(0.00f, 0.25f)},
    {"wallCorner", glm::vec2(0.00f, 0.50f)},
    {"wall",       glm::vec2(0.25f, 0.50f)},
    {"fruit",      glm::vec2(0.00f, 0.75f)},
    {"fruitShine", glm::vec2(0.25f, 0.75f)},
    {"snakeBody",  glm::vec2(0.50f, 0.25f)},
    {"snakeHead",  glm::vec2(0.50f, 0.50f)},
    {"snakeTongue",glm::vec2(0.75f, 0.50f)},
    {"snakeDead",  glm::vec2(0.75f, 0.75f)},
    {"snakeTail",  glm::vec2(0.75f, 0.00f)},
    {"snakeFruit", glm::vec2(0.75f, 0.25f)},
};

struct Hitbox {
    float x;
    float y;
};

struct Player {
    Entity* entity;
    struct Hitbox hitbox;
    std::vector<GLfloat> spriteLocations;
    std::vector<glm::mat4> positionTargetsMat4;
    std::vector<glm::mat4> matrices;
    glm::vec4 orientation = glm::vec4(0,0,1, 0);
    glm::vec3 direction = glm::vec3(0, 0, 0);
    glm::vec3 newDirection = glm::vec3(0,0,0);
} player;

struct Fruit {
    Entity* entity;
    struct Hitbox hitbox;
    uchar enabledSprite = 1;
    std::vector<GLfloat> sprite1 = {
        spriteMap["fruit"].x,       spriteMap["fruit"].y+0.25f,
        spriteMap["fruit"].x+0.25f, spriteMap["fruit"].y+0.25f,
        spriteMap["fruit"].x,       spriteMap["fruit"].y,
        spriteMap["fruit"].x+0.25f, spriteMap["fruit"].y,
    };
    std::vector<GLfloat> sprite2 = {
        spriteMap["fruitShine"].x,       spriteMap["fruitShine"].y+0.25f,
        spriteMap["fruitShine"].x+0.25f, spriteMap["fruitShine"].y+0.25f,
        spriteMap["fruitShine"].x,       spriteMap["fruitShine"].y,
        spriteMap["fruitShine"].x+0.25f, spriteMap["fruitShine"].y,
    };
} fruit;

struct Barrier {
    Entity* north;
    Entity* northEast;
    Entity* east;
    Entity* southEast;
    Entity* south;
    Entity* southWest;
    Entity* west;
    Entity* northWest;
} barrier;

struct Direction {
    glm::vec3 left = glm::vec3(-updateRate, 0, 0);
    glm::vec3 right = glm::vec3(+updateRate, 0, 0);
    glm::vec3 up = glm::vec3(0, updateRate, 0);
    glm::vec3 down = glm::vec3(0, -updateRate, 0);
} direction;

void initializeSystem();
void titleScreenLoop();
void prepareGameStart();
void startGameLoop();
void mainLoop();
void runGameLogic();

void renderGameScenery();
void drawBarriers();
void drawBackground();
void drawGameInterface();

void prepareForGameOver();
void gameOverLoop();
float randBetween(float min, float max);

void onKeyDown(int key);
void onWindowResize(int newWidth, int newHeight);

#endif /* snakeGame_h */
