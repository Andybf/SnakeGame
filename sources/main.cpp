//
//  main.cpp
//  SnakeGame
//
//  Created by Anderson Bucchianico on 19/04/23.
//

#include "snakeGame.h"

#define TEX_PIXEL_POINT 0.03125 // 1px / 32px

Camera* camera;
Projection* projection;
SolidObject* player;
SolidObject* fruit;
Entity* background;
Entity* wall;
Text* text;

uint startTime = 0;
ushort updateStep = 0;
const ushort movementSteps = 24;
const float updateRate = 1.0f/movementSteps;

std::vector<glm::vec3> playerBodySegTarget;
std::vector<glm::vec3> playerBodySegBase;
std::vector<glm::mat4> playerMatrices;
glm::vec3 playerDirection = glm::vec3(0, 0, 0);
glm::vec3 newPlayerDirection = glm::vec3(0,0,0);
glm::vec3 lastPlayerPosition = glm::vec3(0);
float objectSize = 1;
bool shouldRunGameLogic = false;

uint points = 2;
uint pointsToMake = 0;

char title[16] = "SnakeGame";
unsigned short height = 800;
unsigned short width = 800;

float yaw = 45.0f;
float pitch = -89.9f;

void initializeSystem();
void prepareGameStart();
void startGameLoop();
void mainLoop();
void runGameLogic();
void render();
void gameOverLoop();
float randBetween(float min, float max);

void onKeyDown(int key);
void onKeyUp(int key);

int main(int argc, const char * argv[]) {
    Loader::File::setExecutableSystemPath( (char*)argv[0] );
    
    initializeSystem();
}

void initializeSystem() {
    srand((uint)time(NULL));
    
    Core::Window::initialize(width, height, title);
    Core::Window::setLoopCallback(startGameLoop);
    Core::Render::initialize(Core::Window::getRenderContext());
    Core::Render::enableTransparency();
    
    Input::setKeyUpCallbackFunction(onKeyUp);
    Input::setKeyDownCallbackFunction(onKeyDown);
    
    Loader::Texture::setFilteringMode(GL_NEAREST);
    GLuint gameTexturesId = Loader::Texture::load2d(Loader::Texture::fromFile("snake-game-sprites.png"));
    
    GLuint programForMultipleDraws = Loader::Shader::fromFile("gameObjectInstanced.vs", "gameObjectInstanced.fs");
    GLuint programForOneDraw = Loader::Shader::fromFile("gameObject.vs", "gameObject.fs");
    
    player = new SolidObject();
    player->setPosition(glm::vec3(0.5, 0.5, 0));
    player->relatedShaderId = programForMultipleDraws;
    player->relatedTexturesId[0] = gameTexturesId;
    player->hitbox = HitboxQuad{
        player->getPosition().x-(objectSize/2), player->getPosition().y-(objectSize/2),
        objectSize, objectSize
    };
    Mesh* playerModel = new Mesh;
    Creator::Quad::make(playerModel, objectSize, objectSize);
    Creator::Quad::addTexCoords(playerModel, 0.0f, 0.0f, TEX_PIXEL_POINT*8, TEX_PIXEL_POINT*8);
    playerModel->map["bodyInstance"] = ModelSubdata{
        .attribDivisor = 1,
        .dimensions = 4,
        .locations = 4,
        .stride = sizeof(glm::mat4),
    };
    playerModel->map["bodyInstance"].list.resize(256*16);
    player->buffer = Loader::Model::fromMesh(playerModel, player->relatedShaderId);
    
    glm::mat4 headMatrix = glm::mat4(1.0f);
    headMatrix = glm::translate(headMatrix, glm::vec3(0.5f, 0.5f, 0));
    
    glm::mat4 tailMatrix = glm::mat4(1.0f);
    tailMatrix = glm::translate(tailMatrix, glm::vec3(0.5f, 1.5f, 0));
    
    playerMatrices.insert(playerMatrices.end(), {
        headMatrix, tailMatrix
    });
    Loader::Model::updateBufferSubDataMatrix(player->buffer, "bodyInstance", playerMatrices);

    playerBodySegBase.reserve(256*12);
    playerBodySegBase.push_back(glm::vec3(0.5f, 0.5f, 0));
    playerBodySegBase.push_back(glm::vec3(0.5f, 1.5f, 0));
    playerBodySegTarget.reserve(256*12);
    playerBodySegTarget.push_back(glm::vec3(0.0f, 0.0f, 0));
    playerBodySegTarget.push_back(glm::vec3(0.5f, 0.5f, 0));
    
    text = new Text();
    text->relatedTexturesId[0] = Loader::Texture::load2d(Loader::Texture::fromFile("fontmap.png"));
    text->relatedShaderId = Loader::Shader::fromFile("text.vs", "text.fs");
    text->setPosition(glm::vec3((width/2)-30, (height/2) ,1.0f));
    text->setScale(glm::vec3(20.0f));
    text->createModel();
    
    background = new Entity();
    background->setPosition(glm::vec3(-10.5,-10.5, 0));
    background->relatedTexturesId[0] = gameTexturesId;
    background->relatedShaderId = programForOneDraw;
    Mesh* bkgModel = new Mesh;
    float size = 21;
    Creator::Grid::addVertices2d(bkgModel, size, size);
    Creator::Grid::addTexCoords(bkgModel, 0.0f, 0.25f, 0.25f, 0.25f);
    Creator::Grid::addCounterClockwiseIndices(bkgModel, size, size);
    background->buffer = Loader::Model::fromMesh(bkgModel, background->relatedShaderId);
    
    fruit = new SolidObject();
    fruit->setPosition(glm::vec3(-5.5,-5.5,0));
    fruit->hitbox = HitboxQuad{0, 0, 0, 0};
    fruit->relatedTexturesId[0] = gameTexturesId;
    fruit->relatedShaderId = programForOneDraw;
    fruit->hitbox = HitboxQuad{
        fruit->getPosition().x-(objectSize/2), fruit->getPosition().y-(-objectSize/2),
        objectSize, objectSize
    };
    Mesh* fruitModel = new Mesh;
    Creator::Quad::make(fruitModel, 1, 1);
    Creator::Quad::addTexCoords(fruitModel, 0.0f, 0.75f, 0.25f, 0.25f);
    fruit->buffer = Loader::Model::fromMesh(fruitModel, fruit->relatedShaderId);
    
    wall = new Entity();
    wall->setPosition(glm::vec3(-9,9,0));
    wall->relatedTexturesId[0] = gameTexturesId;
    wall->relatedShaderId = programForOneDraw;
    Mesh* wallModel = new Mesh;
    Creator::Quad::make(wallModel, 18, 1);
    Creator::Quad::addTexCoords(wallModel, 0.25f, 0.50f, 0.25f, 0.25f);
    wall->buffer = Loader::Model::fromMesh(wallModel, wall->relatedShaderId);
    
    camera = new Camera();
    camera->setPosition(glm::vec3(0,0,10));
    
    projection = new Projection();
    projection->setPerspecProjection(90, Core::Window::getAspectRatio(), 0.1f, 100.0f);
    projection->setOrthographic(0, width, height, 0);
    
    prepareGameStart();
    
    Core::Render::enableFaceCulling();
    Core::Window::startLoop();
}

void prepareGameStart() {
    newPlayerDirection = glm::vec3(0, -updateRate, 0);
    playerDirection = newPlayerDirection;
    startTime = Core::Window::getTimeElapsed();
}

void startGameLoop() {
    if (Core::Window::getTimeElapsed() - startTime > 1500) {
        text->setPosition(glm::vec3(10, 13.0f ,1.0f));
        text->setScale(glm::vec3(16.0f));
        Core::Window::setLoopCallback(mainLoop);
    }
    Core::Shader::setActiveProgram(background->relatedShaderId);
    Core::Shader::setUniformTexture(background->relatedTexturesId[0], background->textureType, 0);
    Core::Shader::setUniformModelViewProjection(background, camera->getMatrix(), projection->getPerspective());
    Core::Render::drawElements(background);
    
    Core::Shader::setActiveProgram(text->relatedShaderId);
    sprintf(text->content, "Ready");
    text->processContents((uchar*) text->content);
    Loader::Model::updateBufferSubData(text->buffer, "charInfo", text->getStringInfo());
    Core::Shader::setUniformTexture(text->relatedTexturesId[0], text->textureType, 0);
    Core::Shader::setUniformModelProjection(text, projection->getOrthographic());
    Core::Render::drawElementsInstanced(text, text->getStringSize());
}
 
void mainLoop() {
    runGameLogic();
    Core::Shader::setActiveProgram(background->relatedShaderId);
    Core::Shader::setUniformTexture(background->relatedTexturesId[0], background->textureType, 0);
    Core::Shader::setUniformModelViewProjection(background, camera->getMatrix(), projection->getPerspective());
    Core::Render::drawElements(background);
    
    Core::Shader::setActiveProgram(wall->relatedShaderId);
    Core::Shader::setUniformTexture(wall->relatedTexturesId[0], wall->textureType, 0);
    Core::Shader::setUniformModelViewProjection(wall, camera->getMatrix(), projection->getPerspective());
    Core::Render::drawElements(wall);
    
    Core::Shader::setActiveProgram(fruit->relatedShaderId);
    Core::Shader::setUniformTexture(fruit->relatedTexturesId[0], fruit->textureType, 0);
    Core::Shader::setUniformModelViewProjection(fruit, camera->getMatrix(), projection->getPerspective());
    Core::Render::drawElements(fruit);
    
    Core::Shader::setActiveProgram(player->relatedShaderId);
    Core::Shader::setUniformTexture(player->relatedTexturesId[0], player->textureType, 0);
    Core::Shader::setUniformMatrix("viewMatrix", camera->getMatrix());
    Core::Shader::setUniformMatrix("projectionMatrix", projection->getPerspective());
    Core::Render::drawElementsInstanced(player, points);
    
    Core::Shader::setActiveProgram(text->relatedShaderId);
    sprintf(text->content,
            "SnakeGame Information\n"
            "Points: %d\n"
            "Time Elapsed: %d\n"
            "Update: % 0.2f\n"
            "base:   % 0.2f % 0.2f % 0.2f\n"
            "target: % 0.2f % 0.2f % 0.2f\n",
            points,
            Core::Window::getTimeElapsed()/1000,
            updateRate*updateStep,
            playerBodySegBase[1].x, playerBodySegBase[1].y, playerBodySegBase[1].z,
            playerBodySegTarget[1].x, playerBodySegTarget[1].y, playerBodySegTarget[1].z
    );
    text->processContents((uchar*) text->content);
    Loader::Model::updateBufferSubData(text->buffer, "charInfo", text->getStringInfo());
    Core::Shader::setUniformTexture(text->relatedTexturesId[0], text->textureType, 0);
    Core::Shader::setUniformModelProjection(text, projection->getOrthographic());
    Core::Render::drawElementsInstanced(text, text->getStringSize());
}

void runGameLogic() {
    updateStep++;
    if (updateStep <= movementSteps) {
        for (int c=1; c<playerMatrices.size(); c++) {
            if (playerBodySegBase[c] == playerBodySegBase[c-1]) {
                continue;
            }
            float weight = updateRate*updateStep;
            glm::vec3 result = Util::Color::mix(playerBodySegBase[c], playerBodySegTarget[c], weight);
            playerMatrices[c] = glm::translate(result);
        }
        playerMatrices[0] = glm::translate(playerMatrices[0], playerDirection);
        player->hitbox.x = playerMatrices[0][3].x-(objectSize/2);
        player->hitbox.y = playerMatrices[0][3].y+(objectSize/2);
        Loader::Model::updateBufferSubDataMatrix(player->buffer, "bodyInstance", playerMatrices);
    }
    
    if (updateStep >= movementSteps*2) {
        updateStep = 0;
        points += pointsToMake;
        playerDirection = newPlayerDirection;
        
        playerBodySegBase[0] = playerMatrices[0][3];
        for (int c=1; c<playerMatrices.size(); c++) {
            playerBodySegBase[c] = playerMatrices[c][3];
            playerBodySegTarget[c] = playerMatrices[c-1][3];
        }
        
        if (pointsToMake == 1) {
            glm::mat4 newSegment = playerMatrices.back();
            playerMatrices.push_back(newSegment);
            playerBodySegBase.push_back(playerBodySegBase.back());
            playerBodySegTarget.push_back(playerBodySegTarget.back());
        }
        pointsToMake = 0;
        
        if (Physics::Colision::calc(player->hitbox, fruit->hitbox)) {
            int newX = Util::Rand::intBetween(-9,9);
            int newY = Util::Rand::intBetween(-9,9);
            fruit->setPosition(glm::vec3(newX+(objectSize/2), newY+(objectSize/2), 0));
            fruit->hitbox.x = fruit->getPosition().x-(objectSize/2);
            fruit->hitbox.y = fruit->getPosition().y+(objectSize/2);
            pointsToMake = 1;
        }
    }
}

void render() {
}

void onKeyDown(int key) {
    switch (key) {
        case IC_INPUT_ARROW_RIGHT:
            newPlayerDirection = glm::vec3(+updateRate, 0, 0);
            player->setOrientation(90, glm::vec3(0,0,1));
            break;
        case IC_INPUT_ARROW_LEFT:
            newPlayerDirection = glm::vec3(-updateRate, 0, 0);
            player->setOrientation(-90, glm::vec3(0,0,1));
            break;
        case IC_INPUT_ARROW_DOWN:
            newPlayerDirection = glm::vec3(0, -updateRate, 0);
            player->setOrientation(0, glm::vec3(0,0,1));
            break;
        case IC_INPUT_ARROW_UP:
            newPlayerDirection = glm::vec3(0, updateRate, 0);
            player->setOrientation(180, glm::vec3(0,0,1));
            break;
        default:
            break;
    }
}

void onKeyUp(int key) {
    
}
