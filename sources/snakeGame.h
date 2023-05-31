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
#define ARG_TILEMAP_WIDTH "-tw"
#define ARG_TILEMAP_HEIGHT "-th"

#define TEX_PIXEL_POINT 0.03125 // 1px / 32px

extern Camera* camera;
extern Projection* projection;
extern Entity* background;
extern Text* gameInterface;
extern Text* centerText;
extern Text* creditsText;

extern std::map<std::string, glm::vec2> spriteMap;

struct Hitbox {
    float x;
    float y;
};

extern struct Game {
    uint startTime;
    uint finishTime;
    uint points;
    uint pointsToMake;
    uint record;
    ushort updateStep;
    ushort movementSteps;
    ushort windowHeight;
    ushort windowWidth;
    uchar tilemapWidth;
    uchar tilemapHeight;
    float updateRate;
    float objectSize;
    bool isGameplayInterrupted;
} game;

extern struct Player {
    Entity* entity;
    struct Hitbox hitbox;
    std::vector<GLfloat> spriteLocations;
    std::vector<glm::mat4> positionTargetsMat4;
    std::vector<glm::mat4> matrices;
    glm::vec4 orientation;
    glm::vec3 direction;
    glm::vec3 newDirection;
} player;

extern struct Fruit {
    Entity* entity;
    struct Hitbox hitbox;
    uchar enabledSprite;
    std::vector<GLfloat> sprite1;
    std::vector<GLfloat> sprite2;
} fruit;

extern struct Barrier {
    Entity* north;
    Entity* northEast;
    Entity* east;
    Entity* southEast;
    Entity* south;
    Entity* southWest;
    Entity* west;
    Entity* northWest;
} barrier;

extern struct Direction {
    glm::vec3 left;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 down;
} direction;


void processProgramArguments(int argc, const char * argv[]);
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
void onMouseDown(int button, int x, int y);
void onTouchDown(float x, float y, uint fingerId);
void onTouchMove(float x, float y, float dx, float dy, uint fingerId);
void onTouchUp(float x, float y, uint fingerId);
void onWindowResize(int newWidth, int newHeight);

void arrowUp();
void arrowDown();
void arrowLeft();
void arrowRight();

#ifdef __EMSCRIPTEN__
    using namespace emscripten;
    EMSCRIPTEN_BINDINGS(module) {
        emscripten::function("arrowUp", &arrowUp, allow_raw_pointers());
        emscripten::function("arrowDown", &arrowDown, allow_raw_pointers());
        emscripten::function("arrowLeft", &arrowLeft, allow_raw_pointers());
        emscripten::function("arrowRight", &arrowRight, allow_raw_pointers());
    }
#endif

#endif /* snakeGame_h */
