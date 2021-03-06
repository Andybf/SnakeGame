//
//  main.cpp
//  SnakeGame
//
//  Created by Anderson Bucchianico on 19/04/23.
//

#include "snakeGame.h"

Camera* camera;
Projection* projection;
Entity* background;
Text* gameInterface;
Text* centerText;
Text* creditsText;

struct Game game;
struct Player player;
struct Barrier barrier;
struct Fruit fruit;
struct Direction direction;

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

int main(int argc, const char * argv[]) {
    Loader::File::setExecutableSystemPath( (char*)argv[0] );
    
    game.startTime = 0;
    game.finishTime = 0;
    
    game.updateStep = 0;
    game.movementSteps = 12;
    game.updateRate = 1.0f/game.movementSteps;
    game.objectSize = 1;
    
    game.points = 0;
    game.pointsToMake = 0;
    game.record = 0;
    
    game.tilemapWidth = 20;
    game.tilemapHeight = 20;
    game.isGameplayInterrupted = true;
    game.windowWidth = 600;
    game.windowHeight = 630;
    
    direction.left = glm::vec3(-game.updateRate, 0, 0);
    direction.right = glm::vec3(+game.updateRate, 0, 0);
    direction.up = glm::vec3(0, game.updateRate, 0);
    direction.down = glm::vec3(0, -game.updateRate, 0);
    
    processProgramArguments(argc, argv);
    initializeSystem();
}

void processProgramArguments(int argc, const char * argv[]) {
    for (int c=0; c<argc; c++) {
        if (strstr(argv[c], ARG_WINDOW_WIDTH)) {
            game.windowWidth = atoi(argv[c] + strlen(ARG_WINDOW_WIDTH));
        }
        if (strstr(argv[c], ARG_WINDOW_HEIGHT)) {
            game.windowHeight = atoi(argv[c] + strlen(ARG_WINDOW_HEIGHT));
        }
        if (strstr(argv[c], ARG_TILEMAP_WIDTH)) {
            game.tilemapWidth = atoi(argv[c] + strlen(ARG_TILEMAP_WIDTH));
        }
        if (strstr(argv[c], ARG_TILEMAP_HEIGHT)) {
            game.tilemapHeight = atoi(argv[c] + strlen(ARG_TILEMAP_HEIGHT));
        }
    }
}

void initializeSystem() {
    srand((uint)time(NULL));
    
    Core::Window::enableWindowResizable();
    Core::Window::initialize(game.windowWidth, game.windowHeight, (char*)"snakeGame");
    Core::Renderer::initialize(Core::Window::getRenderContext());
    Core::Renderer::enableTransparency();
    
    Input::setKeyDownCallbackFunction(onKeyDown);
    Input::setMouseDownCallbackFunction(onMouseDown);
    Input::setTouchscreenDownCallbackFunction(onTouchDown);
    Input::setTouchscreenMoveCallbackFunction(onTouchMove);
    Input::setTouchscreenUpCallbackFunction(onTouchUp);
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
    Creator::Quad::make(playerModel, game.objectSize, game.objectSize);
    Creator::Quad::addTexCoords(playerModel, 0.0f, 0.0f, TEX_PIXEL_POINT*8, TEX_PIXEL_POINT*8);
    playerModel->map["bodyInstance"] = ModelSubdata{};
    playerModel->map["bodyInstance"].attribDivisor = 1;
    playerModel->map["bodyInstance"].dimensions = 4;
    playerModel->map["bodyInstance"].locations = 4;
    playerModel->map["bodyInstance"].stride = sizeof(glm::mat4);
    playerModel->map["bodyInstance"].list.resize(game.windowHeight*game.windowWidth*16);
    
    playerModel->map["spriteLocations"] = ModelSubdata {};
    playerModel->map["spriteLocations"].attribDivisor = 1;
    playerModel->map["spriteLocations"].dimensions = 2;
    playerModel->map["spriteLocations"].locations = 1;
    playerModel->map["spriteLocations"].stride = 0;
    playerModel->map["spriteLocations"].list.resize(game.windowHeight*game.windowWidth*2);
    player.entity->buffer = Loader::Model::fromMesh(playerModel, player.entity->relatedShaderId);
    
    gameInterface = new Text();
    gameInterface->relatedTexturesId[0] = Loader::Texture::load2d(Loader::Texture::fromFile("fontmap.png"));
    gameInterface->relatedShaderId = Loader::Shader::fromFile("text.vs", "text.fs");
    gameInterface->setScale(glm::vec3(0.58f));
    gameInterface->createModel();
    
    centerText = gameInterface->clone();
    centerText->setPosition(glm::vec3((game.tilemapWidth/2)-4.90, (game.tilemapHeight/2)+2.25, 0));
    centerText->setScale(glm::vec3(0.60f));
    
    creditsText = gameInterface->clone();
    creditsText->setPosition(glm::vec3((game.tilemapWidth/2)-5.15, 0, 0));
    creditsText->setScale(glm::vec3(0.58f));
    
    background = new Entity();
    background->setPosition(glm::vec3(-1, -1, 0));
    background->relatedTexturesId[0] = gameTexturesId;
    background->relatedShaderId = programForOneDraw;
    Mesh* bkgModel = new Mesh;
    glm::vec2 gridSize = glm::vec2(game.tilemapWidth+3, game.tilemapHeight+4);
    Creator::Grid::addVertices2d(bkgModel, gridSize.x, gridSize.y);
    Creator::Grid::addTexCoords(bkgModel, glm::vec2(spriteMap["background"]),glm::vec2(.25,.25),gridSize);
    Creator::Grid::addCounterClockwiseIndices(bkgModel, gridSize.x, gridSize.y);
    background->buffer = Loader::Model::fromMesh(bkgModel, background->relatedShaderId);
    
    fruit.entity = new Entity();
    fruit.sprite1 = {
        spriteMap["fruit"].x,       spriteMap["fruit"].y+0.25f,
        spriteMap["fruit"].x+0.25f, spriteMap["fruit"].y+0.25f,
        spriteMap["fruit"].x,       spriteMap["fruit"].y,
        spriteMap["fruit"].x+0.25f, spriteMap["fruit"].y,
    };
    fruit.sprite2 = {
        spriteMap["fruitShine"].x,       spriteMap["fruitShine"].y+0.25f,
        spriteMap["fruitShine"].x+0.25f, spriteMap["fruitShine"].y+0.25f,
        spriteMap["fruitShine"].x,       spriteMap["fruitShine"].y,
        spriteMap["fruitShine"].x+0.25f, spriteMap["fruitShine"].y,
    };
    fruit.entity->relatedTexturesId[0] = gameTexturesId;
    fruit.entity->relatedShaderId = programForOneDraw;
    Mesh* fruitModel = new Mesh;
    Creator::Quad::make(fruitModel, 1, 1);
    Creator::Quad::addTexCoords(fruitModel, spriteMap["fruit"].x, spriteMap["fruit"].y, 0.25f, 0.25f);
    fruit.entity->buffer = Loader::Model::fromMesh(fruitModel, fruit.entity->relatedShaderId);
    
    barrier.north = new Entity();
    barrier.north->setPosition(glm::vec3(game.tilemapWidth/2+0.5, game.tilemapHeight+1, 0));
    barrier.north->relatedTexturesId[0] = gameTexturesId;
    barrier.north->relatedShaderId = programForOneDraw;
    Mesh* wallModel = new Mesh;
    Creator::Quad::make(wallModel, game.tilemapWidth, 1);
    Creator::Quad::addTexCoords(wallModel, spriteMap["wall"].x, spriteMap["wall"].y, 0.25f, 0.25f);
    barrier.north->buffer = Loader::Model::fromMesh(wallModel, barrier.north->relatedShaderId);
    
    barrier.northEast = new Entity();
    barrier.northEast->setPosition(glm::vec3(game.tilemapWidth+1, game.tilemapHeight+1, 0));
    barrier.northEast->setOrientation(-90, glm::vec3(0,0,1));
    barrier.northEast->relatedTexturesId[0] = gameTexturesId;
    barrier.northEast->relatedShaderId = programForOneDraw;
    Mesh* wallCorner = new Mesh;
    Creator::Quad::make(wallCorner, 1.0f, 1.0f);
    Creator::Quad::addTexCoords(wallCorner, spriteMap["wallCorner"].x, spriteMap["wallCorner"].y, 0.25f, 0.25f);
    barrier.northEast->buffer = Loader::Model::fromMesh(wallCorner, barrier.northEast->relatedShaderId);
    
    barrier.east = new Entity();
    barrier.east->setPosition(glm::vec3(game.tilemapWidth+1, game.tilemapHeight/2+0.5f, 0));
    barrier.east->setOrientation(-90, glm::vec3(0, 0, 1));
    barrier.east->relatedTexturesId[0] = gameTexturesId;
    barrier.east->relatedShaderId = programForOneDraw;
    Mesh* wallModelVertical = new Mesh;
    Creator::Quad::make(wallModelVertical, game.tilemapHeight, 1);
    Creator::Quad::addTexCoords(wallModelVertical, spriteMap["wall"].x, spriteMap["wall"].y, 0.25f, 0.25f);
    barrier.east->buffer = Loader::Model::fromMesh(wallModelVertical, barrier.east->relatedShaderId);
    
    barrier.southEast = barrier.northEast->clone();
    barrier.southEast->setPosition(glm::vec3(game.tilemapWidth+1, 0, 0));
    barrier.southEast->setOrientation(180, glm::vec3(0,0,1));
    
    barrier.south = barrier.north->clone();
    barrier.south->setPosition(glm::vec3(game.tilemapWidth/2+0.5f, 0, 0));
    barrier.south->setOrientation(180, glm::vec3(0,0,1));
    
    barrier.southWest = barrier.northEast->clone();
    barrier.southWest->setPosition(glm::vec3(0, 0, 0));
    barrier.southWest->setOrientation(90, glm::vec3(0,0,1));
    
    barrier.west = barrier.east->clone();
    barrier.west->setPosition(glm::vec3(0, game.tilemapHeight/2+0.5f, 0));
    barrier.west->setOrientation(90, glm::vec3(0,0,1));
    
    barrier.northWest = barrier.northEast->clone();
    barrier.northWest->setPosition(glm::vec3(0, game.tilemapHeight+1, 0));
    barrier.northWest->setOrientation(0, glm::vec3(0,0,1));
    
    camera = new Camera();
    camera->setPosition(glm::vec3((game.tilemapWidth/2)+0.5f, (game.tilemapHeight/2)+1.0f, (game.tilemapHeight/2)+1.55f));
    
    projection = new Projection();
    projection->setPerspecProjection(90, Core::Window::getAspectRatio(), 0.1f, 100.0f);
    projection->setOrthographic(0, game.windowWidth, game.windowHeight, 0);
    
    Core::Window::setLoopCallback(titleScreenLoop);
    Core::Renderer::enableFaceCulling();
    Core::Window::startLoop();
}

void titleScreenLoop() {
    drawBackground();
    Core::Shader::setActiveProgram(centerText->relatedShaderId);
    sprintf(centerText->content,
            "                        \n"
            "        SnakeGame       \n"
            "                        \n"
            "   tap/press to start   \n"
            "                        \n"
    );
    centerText->processContents((uchar*) centerText->content);
    Loader::Model::updateBufferSubData(centerText->buffer, "charInfo", centerText->getStringInfo());
    Core::Shader::setUniformTexture(centerText->relatedTexturesId[0], centerText->textureType, 0);
    Core::Shader::setUniformModelViewProjection(centerText, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElementsInstanced(centerText, centerText->getStringSize());
    
    sprintf(creditsText->content, "2023 - Anderson Bucchianico");
    creditsText->processContents((uchar*) creditsText->content);
    Loader::Model::updateBufferSubData(creditsText->buffer, "charInfo", creditsText->getStringInfo());
    Core::Shader::setUniformModelViewProjection(creditsText, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElementsInstanced(creditsText, creditsText->getStringSize());
}

void prepareGameStart() {
    player.matrices.clear();
    player.matrices.insert(player.matrices.end(), {
        (glm::mat4)glm::translate(glm::vec3((game.tilemapWidth/2), (game.tilemapHeight/2)+1, 0)),
        (glm::mat4)glm::translate(glm::vec3((game.tilemapWidth/2), (game.tilemapHeight/2)+2, 0))
    });
    Loader::Model::updateBufferSubDataMatrix(player.entity->buffer, "bodyInstance", player.matrices);
    player.spriteLocations.clear();
    player.spriteLocations.insert(player.spriteLocations.end(), {
        spriteMap["snakeHead"].x, spriteMap["snakeHead"].y,
        spriteMap["snakeTail"].x, spriteMap["snakeTail"].y,
    });
    Loader::Model::updateBufferSubData(player.entity->buffer, "spriteLocations", player.spriteLocations);
    player.positionTargetsMat4.clear();
    player.positionTargetsMat4.push_back(glm::translate(glm::vec3((game.tilemapWidth/2), (game.tilemapHeight/2)+1, 0)));
    player.positionTargetsMat4.push_back(glm::translate(glm::vec3((game.tilemapWidth/2), (game.tilemapHeight/2)+2, 0)));
    player.entity->isEnabled = true;
    player.newDirection = glm::vec3(0, -game.updateRate, 0);
    player.orientation = glm::vec4(0,0,1, 0);
    player.direction = player.newDirection;
    
    fruit.entity->setPosition(glm::vec3((game.tilemapWidth/2), (int)(game.tilemapHeight/3), 0));
    fruit.hitbox = Hitbox{fruit.entity->getPosition().x, fruit.entity->getPosition().y};
    
    centerText->setPosition(glm::vec3((game.tilemapWidth/2)-4.90, (game.tilemapHeight/2)+1, 0));
    
    game.points = 0;
    game.updateStep = 0;
    game.isGameplayInterrupted = false;
    game.finishTime = 0;
    game.startTime = Core::Window::getTimeElapsed();
    
    Core::Window::setLoopCallback(startGameLoop);
}

void startGameLoop() {
    drawBackground();
    drawBarriers();
    drawGameInterface();
    Core::Shader::setActiveProgram(centerText->relatedShaderId);
    sprintf(centerText->content,
            "                        \n"
            "          Ready         \n"
            "                        \n"
    );
    centerText->processContents((uchar*) centerText->content);
    Loader::Model::updateBufferSubData(centerText->buffer, "charInfo", centerText->getStringInfo());
    Core::Shader::setUniformTexture(centerText->relatedTexturesId[0], centerText->textureType, 0);
    Core::Shader::setUniformModelViewProjection(centerText, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElementsInstanced(centerText, centerText->getStringSize());
    
    if (Core::Window::getTimeElapsed() - game.startTime > 2000) {
        Core::Window::setLoopCallback(mainLoop);
        game.startTime = Core::Window::getTimeElapsed();
    }
}
 
void mainLoop() {
    runGameLogic();
    renderGameScenery();
}

void runGameLogic() {
    game.updateStep++;
    
    // Snake in action
    if (game.updateStep <= game.movementSteps) {
        for (int c=1; c<player.matrices.size(); c++) {
            if (player.positionTargetsMat4[c] == player.positionTargetsMat4[c-1]) {
                if (c < player.matrices.size()-1) {
                    player.spriteLocations[c*2] = spriteMap["snakeFruit"].x;
                    player.spriteLocations[c*2+1] = spriteMap["snakeFruit"].y;
                }
                continue;
            }
            float weight = game.updateRate*game.updateStep;
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
        if (player.hitbox.x > game.tilemapWidth || player.hitbox.x < 1 ||
            player.hitbox.y > game.tilemapHeight || player.hitbox.y < 1)
        {
            prepareForGameOver();
        }
    }
    
    // Snake "Sleep Mode"
    else if (game.updateStep > game.movementSteps*2) {
        game.updateStep = 0;
        game.points += game.pointsToMake;
        player.direction = player.newDirection;
        
        for (int c=0; c<player.matrices.size(); c++) {
            player.positionTargetsMat4[c] = player.matrices[c];
        }
        
        if (game.pointsToMake == 1) {
            glm::mat4 newSegment = player.matrices.front();
            player.matrices.insert(player.matrices.begin(), {newSegment});
            Loader::Model::updateBufferSubDataMatrix(player.entity->buffer, "bodyInstance", player.matrices);
            player.positionTargetsMat4.insert(player.positionTargetsMat4.begin(), {
                player.positionTargetsMat4.front()
            });
            player.spriteLocations.push_back(spriteMap["snakeTail"].x);
            player.spriteLocations.push_back(spriteMap["snakeTail"].y);
        }
        game.pointsToMake = 0;
        
        float distance = sqrt(pow(fruit.hitbox.x-player.hitbox.x, 2) + pow(fruit.hitbox.y-player.hitbox.y, 2));
        if (distance <= 2) {
            player.spriteLocations[0] = spriteMap["snakeTongue"].x;
            player.spriteLocations[1] = spriteMap["snakeTongue"].y;
        } else {
            player.spriteLocations[0] = spriteMap["snakeHead"].x;
            player.spriteLocations[1] = spriteMap["snakeHead"].y;
        }
        
        if ((player.hitbox.x == fruit.hitbox.x) && (player.hitbox.y == fruit.hitbox.y)) {
            int e=0;
            std::vector<glm::vec2> fillMap;
            fillMap.resize(game.tilemapWidth*game.tilemapHeight);
            for (int y=1; y<=game.tilemapHeight; y++) {
                for (int x=1; x<=game.tilemapWidth; x++) {
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
            game.pointsToMake = 1;
        }
        
        for (uint c=2; c<player.matrices.size(); c++) {
            float segmentColisionPointX = round(player.matrices[c][3].x);
            float segmentColisionPointY = round(player.matrices[c][3].y);
            if ((player.hitbox.x == segmentColisionPointX) && (player.hitbox.y == segmentColisionPointY)) {
                prepareForGameOver();
            }
        }
        game.finishTime = (Core::Window::getTimeElapsed() - game.startTime)/1000;
        
        if (fruit.enabledSprite == 1) {
            Loader::Model::updateBufferSubData(fruit.entity->buffer, "texCoords", fruit.sprite2);
            fruit.enabledSprite = 2;
        } else {
            Loader::Model::updateBufferSubData(fruit.entity->buffer, "texCoords", fruit.sprite1);
            fruit.enabledSprite = 1;
        }
    }
    
    Loader::Model::updateBufferSubData(player.entity->buffer, "spriteLocations", player.spriteLocations);
}

void renderGameScenery() {
    drawBackground();
    drawBarriers();
    
    Core::Shader::setActiveProgram(fruit.entity->relatedShaderId);
    Core::Shader::setUniformTexture(fruit.entity->relatedTexturesId[0], fruit.entity->textureType, 0);
    Core::Shader::setUniformModelViewProjection(fruit.entity, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElements(fruit.entity);
    
    Core::Shader::setActiveProgram(player.entity->relatedShaderId);
    Core::Shader::setUniformTexture(player.entity->relatedTexturesId[0], player.entity->textureType, 0);
    Core::Shader::setUniformMatrix("viewMatrix", camera->getMatrix());
    Core::Shader::setUniformMatrix("projectionMatrix", projection->getPerspective());
    Core::Renderer::drawElementsInstanced(player.entity, (int)player.matrices.size());
    
    drawGameInterface();
}

void drawBackground() {
    glClearColor(0.45f, 0.73f, 0.46f, 1);
    Core::Shader::setActiveProgram(background->relatedShaderId);
    Core::Shader::setUniformTexture(background->relatedTexturesId[0], background->textureType, 0);
    Core::Shader::setUniformModelViewProjection(background, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElements(background);
}

void drawBarriers() {
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
}

void drawGameInterface() {
    Core::Shader::setActiveProgram(gameInterface->relatedShaderId);
    sprintf(gameInterface->content, " Points %03d ", game.points);
    gameInterface->processContents((uchar*) gameInterface->content);
    Loader::Model::updateBufferSubData(gameInterface->buffer, "charInfo", gameInterface->getStringInfo());
    Core::Shader::setUniformTexture(gameInterface->relatedTexturesId[0], gameInterface->textureType, 0);
    gameInterface->setPosition(glm::vec3(0, (game.tilemapHeight)+1.90, 0));
    Core::Shader::setUniformModelViewProjection(gameInterface, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElementsInstanced(gameInterface, gameInterface->getStringSize());
    
    sprintf(gameInterface->content, " Time %04d ", game.finishTime);
    gameInterface->processContents((uchar*) gameInterface->content);
    Loader::Model::updateBufferSubData(gameInterface->buffer, "charInfo", gameInterface->getStringInfo());
    gameInterface->setPosition(glm::vec3((game.tilemapWidth-3.35), (game.tilemapHeight)+1.90, 0));
    Core::Shader::setUniformModelViewProjection(gameInterface, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElementsInstanced(gameInterface, gameInterface->getStringSize());
}

void prepareForGameOver() {
    player.spriteLocations[0] = spriteMap["snakeDead"].x;
    player.spriteLocations[0+1] = spriteMap["snakeDead"].y;
    player.newDirection = glm::vec4(0,0,1, 0);
    player.direction = player.newDirection;
    player.entity->isEnabled = false;
    game.isGameplayInterrupted = true;
    game.finishTime = (Core::Window::getTimeElapsed() - game.startTime)/1000;
    if (game.points > game.record) {
        game.record = game.points;
    }
    centerText->setPosition(glm::vec3((game.tilemapWidth/2)-4.90, (game.tilemapHeight/2)+2, 0));
    Core::Window::setLoopCallback(gameOverLoop);
}

void gameOverLoop() {
    renderGameScenery();
    Core::Shader::setActiveProgram(centerText->relatedShaderId);
    sprintf(centerText->content,
            "                        \n"
            "        Game Over       \n"
            "       Record: %03d      \n"
            "                        \n"
            " Tap or press to restart\n"
            "                        \n",
            game.record
    );
    centerText->processContents((uchar*) centerText->content);
    Loader::Model::updateBufferSubData(centerText->buffer, "charInfo", centerText->getStringInfo());
    Core::Shader::setUniformTexture(centerText->relatedTexturesId[0], centerText->textureType, 0);
    Core::Shader::setUniformModelViewProjection(centerText, camera->getMatrix(), projection->getPerspective());
    Core::Renderer::drawElementsInstanced(centerText, centerText->getStringSize());
}

void arrowUp() {
    if (player.direction != direction.down) {
        player.newDirection = direction.up;
        player.orientation = glm::vec4(0,0,1, 180);
    }
}
void arrowDown() {
    if (player.direction != direction.up) {
        player.newDirection = direction.down;
        player.orientation = glm::vec4(0,0,1, 360);
    }
}
void arrowLeft() {
    if (player.direction != direction.right) {
        player.newDirection = glm::vec3(-game.updateRate, 0, 0);
        player.orientation = glm::vec4(0,0,1, -90);
    }
}
void arrowRight() {
    if (player.direction != direction.left) {
        player.newDirection = direction.right;
        player.orientation = glm::vec4(0,0,1, 90);
    }
}
