//
//  main.cpp
//  SnakeGame
//
//  Created by Anderson Bucchianico on 19/04/23.
//

#include "snakeGame.h"

int main(int argc, const char * argv[]) {
    Loader::File::setExecutableSystemPath( (char*)argv[0] );
    for (int c=0; c<argc; c++) {
        
        if (strstr(argv[c], ARG_WINDOW_WIDTH)) {
            width = atoi(argv[c] + strlen(ARG_WINDOW_WIDTH));
        }
        if (strstr(argv[c], ARG_WINDOW_HEIGHT)) {
            height = atoi(argv[c] + strlen(ARG_WINDOW_HEIGHT));
        }
    }
    initializeSystem();
}

void initializeSystem() {
    srand((uint)time(NULL));
    
    Core::Window::enableWindowResizable();
    Core::Window::initialize(width, height, title);
    Core::Window::setLoopCallback(startGameLoop);
    Core::Renderer::initialize(Core::Window::getRenderContext());
    Core::Renderer::enableTransparency();
    
    Input::setKeyDownCallbackFunction(onKeyDown);
    Input::setWindowEventResizeCallbackFunction(onWindowResize);
    
    Loader::Texture::setFilteringMode(GL_NEAREST);
    GLuint gameTexturesId = Loader::Texture::load2d(Loader::Texture::fromFile("snake-game-sprites.png"));
    GLuint programForMultipleDraws = Loader::Shader::fromFile("gameObjectInstanced.vs", "gameObjectInstanced.fs");
    GLuint programForOneDraw = Loader::Shader::fromFile("gameObject.vs", "gameObject.fs");
    
    player.entity = new Entity();
    player.entity->relatedShaderId = programForMultipleDraws;
    player.entity->relatedTexturesId[0] = gameTexturesId;
    player.hitbox = Hitbox{player.entity->getPosition().x, player.entity->getPosition().y};
    Mesh* playerModel = new Mesh;
    Creator::Quad::make(playerModel, objectSize, objectSize);
    Creator::Quad::addTexCoords(playerModel, 0.0f, 0.0f, TEX_PIXEL_POINT*8, TEX_PIXEL_POINT*8);
    playerModel->map["bodyInstance"] = ModelSubdata{};
    playerModel->map["bodyInstance"].attribDivisor = 1;
    playerModel->map["bodyInstance"].dimensions = 4;
    playerModel->map["bodyInstance"].locations = 4;
    playerModel->map["bodyInstance"].stride = sizeof(glm::mat4);
    playerModel->map["bodyInstance"].list.resize(256*16);
    
    playerModel->map["spriteLocations"] = ModelSubdata {};
    playerModel->map["spriteLocations"].attribDivisor = 1;
    playerModel->map["spriteLocations"].dimensions = 2;
    playerModel->map["spriteLocations"].locations = 1;
    playerModel->map["spriteLocations"].stride = 0;
    playerModel->map["spriteLocations"].list.resize(256*2);
    player.entity->buffer = Loader::Model::fromMesh(playerModel,
                                                    player.entity->relatedShaderId);
    
    player.matrices.insert(player.matrices.end(), {
        (glm::mat4)glm::translate(glm::vec3(11.0f, 11.0f, 0)),
        (glm::mat4)glm::translate(glm::vec3(11.0f, 12.0f, 0))
    });
    Loader::Model::updateBufferSubDataMatrix(player.entity->buffer,
                                             "bodyInstance",
                                             player.matrices);
    
    player.spriteLocations.insert(player.spriteLocations.end(), {
        spriteMap["snakeHead"].x, spriteMap["snakeHead"].y,
        spriteMap["snakeTail"].x, spriteMap["snakeTail"].y,
    });
    Loader::Model::updateBufferSubData(player.entity->buffer,
                                       "spriteLocations",
                                       player.spriteLocations);

    player.positionTargetsMat4.reserve(256);
    player.positionTargetsMat4.push_back(glm::translate(glm::vec3(11.0f, 11.0f, 0)));
    player.positionTargetsMat4.push_back(glm::translate(glm::vec3(11.0f, 12.0f, 0)));
    
    text = new Text();
    text->relatedTexturesId[0] = Loader::Texture::load2d(Loader::Texture::fromFile("fontmap.png"));
    text->relatedShaderId = Loader::Shader::fromFile("text.vs", "text.fs");
    text->setPosition(glm::vec3((width/2)-30, (height/2) ,1.0f));
    text->setScale(glm::vec3(20.0f));
    text->createModel();
    
    background = new Entity();
    background->setPosition(glm::vec3(-1, -1, 0));
    background->relatedTexturesId[0] = gameTexturesId;
    background->relatedShaderId = programForOneDraw;
    Mesh* bkgModel = new Mesh;
    float size = 23;
    Creator::Grid::addVertices2d(bkgModel, size, size);
    Creator::Grid::addTexCoords(bkgModel,
                                spriteMap["background"].x, spriteMap["background"].y,
                                0.25f, 0.25f);
    Creator::Grid::addCounterClockwiseIndices(bkgModel, size, size);
    background->buffer = Loader::Model::fromMesh(bkgModel, background->relatedShaderId);
    
    fruit.entity = new Entity();
    fruit.entity->setPosition(glm::vec3(11.0, 3.0,0));
    fruit.hitbox = Hitbox{0, 0};
    fruit.entity->relatedTexturesId[0] = gameTexturesId;
    fruit.entity->relatedShaderId = programForOneDraw;
    fruit.hitbox = Hitbox{fruit.entity->getPosition().x, fruit.entity->getPosition().y};
    Mesh* fruitModel = new Mesh;
    Creator::Quad::make(fruitModel, 1, 1);
    Creator::Quad::addTexCoords(fruitModel,
                                spriteMap["fruit"].x, spriteMap["fruit"].y,
                                0.25f, 0.25f);
    fruit.entity->buffer = Loader::Model::fromMesh(fruitModel, fruit.entity->relatedShaderId);
    
    barrier.north = new Entity();
    barrier.north->setPosition(glm::vec3(10.5, 21.0,0));
    barrier.north->relatedTexturesId[0] = gameTexturesId;
    barrier.north->relatedShaderId = programForOneDraw;
    Mesh* wallModel = new Mesh;
    Creator::Quad::make(wallModel, 20, 1);
    Creator::Quad::addTexCoords(wallModel,
                                spriteMap["wall"].x, spriteMap["wall"].y,
                                0.25f, 0.25f);
    barrier.north->buffer = Loader::Model::fromMesh(wallModel, barrier.north->relatedShaderId);
    
    barrier.northEast = new Entity();
    barrier.northEast->setPosition(glm::vec3(21.0, 21.0, 0));
    barrier.northEast->setOrientation(-90, glm::vec3(0,0,1));
    barrier.northEast->relatedTexturesId[0] = gameTexturesId;
    barrier.northEast->relatedShaderId = programForOneDraw;
    Mesh* wallCorner = new Mesh;
    Creator::Quad::make(wallCorner, 1.0f, 1.0f);
    Creator::Quad::addTexCoords(wallCorner,
                                spriteMap["wallCorner"].x, spriteMap["wallCorner"].y,
                                0.25f, 0.25f);
    barrier.northEast->buffer = Loader::Model::fromMesh(wallCorner, barrier.northEast->relatedShaderId);
    
    barrier.east = barrier.north->clone();
    barrier.east->setPosition(glm::vec3(21.0, 10.5, 0));
    barrier.east->setOrientation(-90, glm::vec3(0, 0, 1));
    
    barrier.southEast = barrier.northEast->clone();
    barrier.southEast->setPosition(glm::vec3(21.0, 0, 0));
    barrier.southEast->setOrientation(180, glm::vec3(0,0,1));
    
    barrier.south = barrier.north->clone();
    barrier.south->setPosition(glm::vec3(10.5, 0, 0));
    barrier.south->setOrientation(180, glm::vec3(0,0,1));
    
    barrier.southWest = barrier.northEast->clone();
    barrier.southWest->setPosition(glm::vec3(0, 0, 0));
    barrier.southWest->setOrientation(90, glm::vec3(0,0,1));
    
    barrier.west = barrier.north->clone();
    barrier.west->setPosition(glm::vec3(0, 10.5, 0));
    barrier.west->setOrientation(90, glm::vec3(0,0,1));
    
    barrier.northWest = barrier.northEast->clone();
    barrier.northWest->setPosition(glm::vec3(0, 21, 0));
    barrier.northWest->setOrientation(0, glm::vec3(0,0,1));
    
    camera = new Camera();
    camera->setPosition(glm::vec3(10.5, 10.5,11));
    
    projection = new Projection();
    projection->setPerspecProjection(90, Core::Window::getAspectRatio(), 0.1f, 100.0f);
    projection->setOrthographic(0, width, height, 0);
    
    prepareGameStart();
    
    Core::Renderer::enableFaceCulling();
    Core::Window::startLoop();
}

void prepareGameStart() {
    player.newDirection = glm::vec3(0, -updateRate, 0);
    player.direction = player.newDirection;
    startTime = Core::Window::getTimeElapsed();
    
    fillMap.resize(400);
    int c=0;
    for (int x=1; x<=20; x++) {
        for (int y=1; y<=20; y++) {
            fillMap[c] = glm::vec2(x,y);
            c++;
        }
    }
}

void startGameLoop() {
    Core::Shader::setActiveProgram(background->relatedShaderId);
    Core::Shader::setUniformTexture(background->relatedTexturesId[0], background->textureType, 0);
    Core::Shader::setUniformModelViewProjection(background, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElements(background);
    
    Core::Shader::setActiveProgram(text->relatedShaderId);
    sprintf(text->content, "Ready");
    text->processContents((uchar*) text->content);
    Loader::Model::updateBufferSubData(text->buffer, "charInfo", text->getStringInfo());
    Core::Shader::setUniformTexture(text->relatedTexturesId[0], text->textureType, 0);
    Core::Shader::setUniformModelProjection(text, projection->getOrthographic());
    Core::Renderer::drawElementsInstanced(text, text->getStringSize());
    
    if (Core::Window::getTimeElapsed() - startTime > 2000) {
        text->setPosition(glm::vec3((width/2)-128, 18 , 1));
        text->setScale(glm::vec3(16.0f));
        Core::Window::setLoopCallback(mainLoop);
    }
}
 
void mainLoop() {
    runGameLogic();
    renderGameScenery();
    
    Core::Shader::setActiveProgram(text->relatedShaderId);
    sprintf(text->content,
            "Points% d  |  Time Elapsed %d\n"
            "Hitbox: x: % 0.2f | y: % 0.2f",
            points, Core::Window::getTimeElapsed()/1000,
            player.hitbox.x, player.hitbox.y
    );
    text->processContents((uchar*) text->content);
    Loader::Model::updateBufferSubData(text->buffer, "charInfo", text->getStringInfo());
    Core::Shader::setUniformTexture(text->relatedTexturesId[0], text->textureType, 0);
    Core::Shader::setUniformModelProjection(text, projection->getOrthographic());
    Core::Renderer::drawElementsInstanced(text, text->getStringSize());
}

void prepareForGameOver() {
    player.spriteLocations[0] = spriteMap["snakeTongue"].x;
    player.spriteLocations[0+1] = spriteMap["snakeTongue"].y;
    player.newDirection = glm::vec4(0,0,1, 0);
    player.direction = player.newDirection;
    player.isControlEnabled = false;
    finishTime = Core::Window::getTimeElapsed()/1000;
    
    text->setPosition(glm::vec3((width/2)-75, (height/2) ,1.0f));
    text->setScale(glm::vec3(20.0f));
    
    Core::Window::setLoopCallback(gameOverLoop);
}

void gameOverLoop() {
    renderGameScenery();
    
    Core::Shader::setActiveProgram(text->relatedShaderId);
    sprintf(text->content,
            " Game Over\n"
            " Points: %d\n"
            " Time: %d ",
            points, finishTime
    );
    text->processContents((uchar*) text->content);
    Loader::Model::updateBufferSubData(text->buffer, "charInfo", text->getStringInfo());
    Core::Shader::setUniformTexture(text->relatedTexturesId[0], text->textureType, 0);
    Core::Shader::setUniformModelProjection(text, projection->getOrthographic());
    Core::Renderer::drawElementsInstanced(text, text->getStringSize());
}

void runGameLogic() {
    updateStep++;
    
    // Snake in action
    if (updateStep <= movementSteps) {
        for (int c=1; c<player.matrices.size(); c++) {
            if (player.positionTargetsMat4[c] == player.positionTargetsMat4[c-1]) {
                if (c < player.matrices.size()-1) {
                    player.spriteLocations[c*2] = spriteMap["snakeFruit"].x;
                    player.spriteLocations[c*2+1] = spriteMap["snakeFruit"].y;
                }
                continue;
            }
            float weight = updateRate*updateStep;
            glm::vec3 nextPositionStep = Util::Math::mix(player.positionTargetsMat4[c][3],
                                                         player.positionTargetsMat4[c-1][3],
                                                         weight);
            player.matrices[c] = player.positionTargetsMat4[c-1];
            player.matrices[c][3].x = nextPositionStep.x;
            player.matrices[c][3].y = nextPositionStep.y;
            if (c < player.matrices.size()-1) {
                player.spriteLocations[c*2] = spriteMap["snakeBody"].x;
                player.spriteLocations[c*2+1] = spriteMap["snakeBody"].y;
            }
        }
        player.hitbox.x = round(player.matrices[0][3].x);
        player.hitbox.y = round(player.matrices[0][3].y);
        player.matrices[0] = glm::translate(glm::vec3(player.matrices[0][3]) + player.direction) *
                             glm::rotate(glm::radians(player.orientation.w), glm::vec3(player.orientation));
        Loader::Model::updateBufferSubDataMatrix(player.entity->buffer, "bodyInstance", player.matrices);
        
        // Colision on walls
        if (player.hitbox.x > 20 || player.hitbox.x < 1 ||
            player.hitbox.y > 20 || player.hitbox.y < 1)
        {
            prepareForGameOver();
        }
    }
    
    // Snake "Sleep Mode"
    else if (updateStep < movementSteps*2) {
        
    }
    
    // Prepare for next movement
    else {
        updateStep = 0;
        points += pointsToMake;
        player.direction = player.newDirection;
        
        for (int c=0; c<player.matrices.size(); c++) {
            player.positionTargetsMat4[c] = player.matrices[c];
        }
        
        if (pointsToMake == 1) {
            glm::mat4 newSegment = player.matrices.front();
            player.matrices.insert(player.matrices.begin(), {newSegment});
            player.positionTargetsMat4.insert(player.positionTargetsMat4.begin(), {
                player.positionTargetsMat4.front()
            });
            player.spriteLocations.push_back(spriteMap["snakeTail"].x);
            player.spriteLocations.push_back(spriteMap["snakeTail"].y);
        }
        pointsToMake = 0;
        
        if ((player.hitbox.x == fruit.hitbox.x) && (player.hitbox.y == fruit.hitbox.y)) {
            int e=0;
            fillMap.resize(400);
            for (int y=1; y<=20; y++) {
                for (int x=1; x<=20; x++) {
                    fillMap[e] = glm::vec2(x,y);
                    e++;
                }
            }
            for (int d=0; d<player.matrices.size(); d++) {
                glm::vec2 segPos = glm::vec2(round(player.matrices[d][3].x), round(player.matrices[d][3].y));
                for (int c=0; c<fillMap.size(); c++) {
                    if (fillMap[c] == segPos) {
                        fillMap.erase(fillMap.begin() + c);
                    }
                }
            }
            int availablePosition = Util::Rand::intBetween(1, (int)fillMap.size());
            fruit.entity->setPosition(glm::vec3(fillMap[availablePosition].x,
                                                fillMap[availablePosition].y,
                                                0));
            fruit.hitbox.x = fruit.entity->getPosition().x;
            fruit.hitbox.y = fruit.entity->getPosition().y;
            pointsToMake = 1;
        }
        
        for (uint c=2; c<player.matrices.size(); c++) {
            float segmentColisionPointX = round(player.matrices[c][3].x);
            float segmentColisionPointY = round(player.matrices[c][3].y);
            if ((player.hitbox.x == segmentColisionPointX) &&
                (player.hitbox.y == segmentColisionPointY))
            {
                prepareForGameOver();
            }
        }
    }
    
    Loader::Model::updateBufferSubData(player.entity->buffer, "spriteLocations", player.spriteLocations);
}

void renderGameScenery() {
    Core::Shader::setActiveProgram(background->relatedShaderId);
    Core::Shader::setUniformTexture(background->relatedTexturesId[0], background->textureType, 0);
    Core::Shader::setUniformModelViewProjection(background, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElements(background);
    
    Core::Shader::setActiveProgram(barrier.north->relatedShaderId);
    Core::Shader::setUniformTexture(barrier.north->relatedTexturesId[0], barrier.north->textureType, 0);
    Core::Shader::setUniformMatrix("viewMatrix", camera->getMatrix());
    Core::Shader::setUniformMatrix("projectionMatrix", projection->getPerspective());
    Core::Shader::setUniformMatrix("modelMatrix", barrier.north->getMatrix());
    Core::Renderer::drawElements(barrier.north);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.northEast->getMatrix());
    Core::Renderer::drawElements(barrier.northEast);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.east->getMatrix());
    Core::Renderer::drawElements(barrier.east);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.southEast->getMatrix());
    Core::Renderer::drawElements(barrier.southEast);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.south->getMatrix());
    Core::Renderer::drawElements(barrier.south);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.southWest->getMatrix());
    Core::Renderer::drawElements(barrier.southWest);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.west->getMatrix());
    Core::Renderer::drawElements(barrier.west);
    Core::Shader::setUniformMatrix("modelMatrix", barrier.northWest->getMatrix());
    Core::Renderer::drawElements(barrier.northWest);
    
    Core::Shader::setActiveProgram(fruit.entity->relatedShaderId);
    Core::Shader::setUniformTexture(fruit.entity->relatedTexturesId[0], fruit.entity->textureType, 0);
    Core::Shader::setUniformModelViewProjection(fruit.entity, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElements(fruit.entity);
    
    Core::Shader::setActiveProgram(player.entity->relatedShaderId);
    Core::Shader::setUniformTexture(player.entity->relatedTexturesId[0], player.entity->textureType, 0);
    Core::Shader::setUniformMatrix("viewMatrix", camera->getMatrix());
    Core::Shader::setUniformMatrix("projectionMatrix", projection->getPerspective());
    Core::Renderer::drawElementsInstanced(player.entity, points);
}

void onKeyDown(int key) {
    if (player.isControlEnabled) {
        switch (key) {
            case IC_INPUT_ARROW_RIGHT:
                if (player.direction != direction.left) {
                    player.newDirection = direction.right;
                    player.orientation = glm::vec4(0,0,1, 90);
                }
                break;
            case IC_INPUT_ARROW_LEFT:
                if (player.direction != direction.right) {
                    player.newDirection = glm::vec3(-updateRate, 0, 0);
                    player.orientation = glm::vec4(0,0,1, -90);
                }
                break;
            case IC_INPUT_ARROW_DOWN:
                if (player.direction != direction.up) {
                    player.newDirection = direction.down;
                    player.orientation = glm::vec4(0,0,1, 360);
                }
                break;
            case IC_INPUT_ARROW_UP:
                if (player.direction != direction.down) {
                    player.newDirection = direction.up;
                    player.orientation = glm::vec4(0,0,1, 180);
                }
                break;
            default:
                break;
        }
    }
}

void onWindowResize(int newWidth, int newHeight) {
    glViewport(0, 0, newWidth, newWidth);
    projection->setPerspecProjection(90, (float) 1, 0.1f, 100.0f);
    projection->setOrthographic(0.0f, (float)newWidth, (float)newHeight, 0.0f);
}
