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
Entity* player;
Entity* background;
Entity* wall;
Entity* fruit;
Text* text;

char title[16] = "SnakeGame";
unsigned short height = 640;
unsigned short width = 640;

float yaw = 45.0f;
float pitch = -89.9f;

void initLoop();
void mainLoop();
void gameOverLoop();

void onKeyDown(int key);
void onKeyUp(int key);

int main(int argc, const char * argv[]) {
    FileLoader::setExecutableSystemPath( (char*)argv[0] );
    
    initializeSystem();
}

void initializeSystem() {
    Window::initialize(width, height, title);
    Window::setLoopCallback(mainLoop);
    Render::initialize(Window::getRenderContext());
    Render::enableTransparency();
    
    Input::setKeyUpCallbackFunction(onKeyUp);
    Input::setKeyDownCallbackFunction(onKeyDown);
    
    TextureLoader::setTextureFiltering(GL_NEAREST);
    GLuint gameTexturesId = TextureLoader::load2d(TextureLoader::loadFromFile("snake-game-sprites.png"));
    
    GLuint programForMultipleDraws = ShaderLoader::load("gameObjectInstanced.vs", "gameObjectInstanced.fs");
    GLuint programForOneDraw = ShaderLoader::load("gameObject.vs", "gameObject.fs");
    
    text = new Text();
    text->relatedTexturesId[0] = TextureLoader::load2d(TextureLoader::loadFromFile("fontmap.png"));
    text->relatedShaderId = ShaderLoader::load("text.vs", "text.fs");
    text->setPosition(glm::vec3(5,41.0f,1.0f));
    text->setScale(glm::vec3(16.0f));
    text->createModel();
    
    player = new Entity();
    player->setPosition(glm::vec3(0, 0, 0));
    player->relatedShaderId = programForMultipleDraws;
    player->relatedTexturesId[0] = gameTexturesId;
    Model* playerModel = new Model;
    ModelCreator::Quad::create(playerModel, 1, 1);
    ModelCreator::Quad::addTexCoords(playerModel, 0.0f, 0.0f, TEX_PIXEL_POINT*8, TEX_PIXEL_POINT*8);
    playerModel->map["bodySegmentPosition"] = ModelSubdata{};
    playerModel->map["bodySegmentPosition"].dimensions = 2;
    playerModel->map["bodySegmentPosition"].attribDivisor = 1;
    playerModel->map["bodySegmentPosition"].list.insert(playerModel->map["bodySegmentPosition"].list.end(), {
        0.0f, 0.0f,
    });
    playerModel->map["bodySegmentTexCoords"] = ModelSubdata{};
    playerModel->map["bodySegmentTexCoords"].dimensions = 2;
    playerModel->map["bodySegmentTexCoords"].attribDivisor = 1;
    playerModel->map["bodySegmentTexCoords"].list.insert(playerModel->map["bodySegmentTexCoords"].list.end(), {
        0.5f, 0.5f,
    });
    player->buffer = ModelLoader::loadFromModel(playerModel, player->relatedShaderId);
    
    background = new Entity();
    background->setPosition(glm::vec3(-10.5,-10.5, 0));
    background->relatedTexturesId[0] = gameTexturesId;
    background->relatedShaderId = programForOneDraw;
    Model* bkgModel = new Model;
    float size = 21;
    ModelCreator::Grid::addVertices2d(bkgModel, size, size);
    ModelCreator::Grid::addTexCoords(bkgModel, 0.0f, 0.25f, 0.25f, 0.25f);
    ModelCreator::Grid::addCounterClockwiseIndices(bkgModel, size, size);
    background->buffer = ModelLoader::loadFromModel(bkgModel, background->relatedShaderId);
    
    fruit = new Entity();
    fruit->setPosition(glm::vec3(-5,-5,0));
    fruit->relatedTexturesId[0] = gameTexturesId;
    fruit->relatedShaderId = programForOneDraw;
    Model* fruitModel = new Model;
    ModelCreator::Quad::create(fruitModel, 1, 1);
    ModelCreator::Quad::addTexCoords(fruitModel, 0.0f, 0.75f, 0.25f, 0.25f);
    fruit->buffer = ModelLoader::loadFromModel(fruitModel, fruit->relatedShaderId);
    
    wall = new Entity();
    wall->setPosition(glm::vec3(-9,9,0));
    wall->relatedTexturesId[0] = gameTexturesId;
    wall->relatedShaderId = programForOneDraw;
    Model* wallModel = new Model;
    ModelCreator::Quad::create(wallModel, 18, 1);
    ModelCreator::Quad::addTexCoords(wallModel, 0.25f, 0.50f, 0.25f, 0.25f);
    wall->buffer = ModelLoader::loadFromModel(wallModel, wall->relatedShaderId);
    
    camera = new Camera();
    camera->setPosition(glm::vec3(0,0,10));
    
    projection = new Projection();
    projection->setPerspecProjection(90, Window::getAspectRatio(), 0.1f, 100.0f);
    projection->setOrthographic(0, width, height, 0);
    Render::enableFaceCulling();
    Window::startLoop();
}
 
void mainLoop() {
    
    Shader::setActiveProgram(background->relatedShaderId);
    Shader::setUniformTexture(background->relatedTexturesId[0], background->textureType, 0);
    Shader::setUniformModelViewProjection(background, camera->getMatrix(), projection->getPerspective());
    Render::drawElements(background);
    
    Shader::setActiveProgram(wall->relatedShaderId);
    Shader::setUniformTexture(wall->relatedTexturesId[0], wall->textureType, 0);
    Shader::setUniformModelViewProjection(wall, camera->getMatrix(), projection->getPerspective());
    Render::drawElements(wall);
    
    Shader::setActiveProgram(fruit->relatedShaderId);
    Shader::setUniformTexture(fruit->relatedTexturesId[0], fruit->textureType, 0);
    Shader::setUniformModelViewProjection(fruit, camera->getMatrix(), projection->getPerspective());
    Render::drawElements(fruit);
    
    Shader::setActiveProgram(player->relatedShaderId);
    Shader::setUniformTexture(player->relatedTexturesId[0], player->textureType, 0);
    Shader::setUniformModelViewProjection(player, camera->getMatrix(), projection->getPerspective());
    Render::drawElementsInstanced(player, 1);
    
    Shader::setActiveProgram(text->relatedShaderId);
    sprintf(text->content,"SnakeGame Information %d\n",Window::getElapsedTime()/1000);
    text->processContents((uchar*) text->content);
    ModelLoader::updateBufferSubData(text->buffer, "charInfo", text->getStringInfo());
    Shader::setUniformTexture(text->relatedTexturesId[0], text->textureType, 0);
    Shader::setUniformModelProjection(text, projection->getOrthographic());
    Render::drawElementsInstanced(text, text->getStringSize());
}

void onKeyDown(int key) {
    switch (key) {
        case 'w':
            
            break;
        case 'a':
            
            break;
        case 's':
            
            break;
        case 'd':
            
            break;
            
        default:
            break;
    }
}

void onKeyUp(int key) {
    
}
