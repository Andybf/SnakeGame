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
Text* text;

uint startTime = 0;
uint finishTime = 0;
ushort updateStep = 0;
const ushort movementSteps = 12;
const float updateRate = 1.0f/movementSteps;

float objectSize = 1;
float hitboxDivisor = 2.50f;

uint points = 2;
uint pointsToMake = 0;
std::vector<glm::vec2> fillMap;

char title[16] = "SnakeGame";
unsigned short height = 600;
unsigned short width = 600;

float yaw = 45.0f;
float pitch = -89.9f;

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
    bool isControlEnabled = true;
} player;

struct Fruit {
    Entity* entity;
    struct Hitbox hitbox;
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

std::map<std::string, glm::vec2> spriteMap {
    {"background", glm::vec2(0.00f, 0.25f)},
    {"wallCorner", glm::vec2(0.00f, 0.50f)},
    {"wall",       glm::vec2(0.25f, 0.50f)},
    {"fruit",      glm::vec2(0.00f, 0.75f)},
    {"fruitRot",   glm::vec2(0.25f, 0.75f)},
    {"snakeBody",  glm::vec2(0.50f, 0.25f)},
    {"snakeHead",  glm::vec2(0.50f, 0.50f)},
    {"snakeTongue",glm::vec2(0.75f, 0.50f)},
    {"snakeTail",  glm::vec2(0.75f, 0.00f)},
    {"snakeFruit", glm::vec2(0.75f, 0.25f)},
};

void initializeSystem();
void prepareGameStart();
void startGameLoop();
void mainLoop();
void runGameLogic();
void renderGameScenery();
void gameOverLoop();
float randBetween(float min, float max);

void onKeyDown(int key);
void onWindowResize(int newWidth, int newHeight);

#endif /* snakeGame_h */
