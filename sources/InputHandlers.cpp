//
//  InputHandlers.cpp
//  SnakeGame
//
//  Created by Anderson Bucchianico on 30/05/23.
//

#include "snakeGame.h"

struct Input::Finger fingers[4];

void onKeyDown(int key) {
    if (player.entity->isEnabled) {
        switch (key) {
            case IC_INPUT_ARROW_RIGHT:
                arrowRight();
                break;
            case IC_INPUT_ARROW_LEFT:
                arrowLeft();
                break;
            case IC_INPUT_ARROW_DOWN:
                arrowDown();
                break;
            case IC_INPUT_ARROW_UP:
                arrowUp();
                break;
            default:
                break;
        }
    }
    if (game.isGameplayInterrupted) {
        prepareGameStart();
    }
}

void onMouseDown(int button, int x, int y) {
    if (game.windowWidth > 480) {
        if (game.isGameplayInterrupted) {
            prepareGameStart();
        }
    }
}

void onTouchDown(float x, float y, uint fingerId) {
    fingers[fingerId].isActive = true;
    fingers[fingerId].positionOrigin = glm::vec2(x,y);
    if (game.isGameplayInterrupted) {
        prepareGameStart();
    }
}

void onTouchMove(float x, float y, float dx, float dy, uint fingerId) {
    fingers[fingerId].positionCurrent = glm::vec2(x,y);
    if ( ! fingers[0].isActive) {
        return;
    }
    float movement = Util::Math::distanceBetweenTwoPoints(fingers[fingerId].positionOrigin,
                                                          fingers[fingerId].positionCurrent);
    float oneFourthOfTheScreen = 0.20f;
    if (movement < oneFourthOfTheScreen) {
        return;
    }
    glm::vec2 diff = glm::vec2(fingers[fingerId].positionOrigin.x - fingers[fingerId].positionCurrent.x,
                               fingers[fingerId].positionOrigin.y - fingers[fingerId].positionCurrent.y);
    
    if (abs(diff.x) > abs(diff.y)) {
        (diff.x > 0) ? arrowLeft() :arrowRight();
    } else {
        (diff.y < 0) ? arrowDown() : arrowUp();
    }
    fingers[0].isActive = false;
}

void onTouchUp(float x, float y, uint fingerId) {
    fingers[fingerId].isActive = false;
}

void onWindowResize(int newWidth, int newHeight) {
    glViewport(0, 0, newWidth, newHeight);
    projection->setPerspecProjection(90, (float) Core::Window::getAspectRatio(), 0.1f, 100.0f);
    projection->setOrthographic(0.0f, (float)newWidth, (float)newHeight, 0.0f);
}
