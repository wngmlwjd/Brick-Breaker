#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <cmath>
#include <cstring>
#include <iostream>
using namespace std;

#include "Brick.h"
#include "Ball.h"

#define PI 3.1415926
#define LEFT 0
#define BOTTOM 0

// 화면 크기
double monitorWidth, monitorHeight;
double windowWidth, windowHeight;

// 게임 상태
enum GameState {
    TEST,
    STATE_START,
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_PAUSE
};

GameState currentState = STATE_START;
bool START = false;
bool GAMECLEAR = false;

int pauseMenuSelected = 0;
int pausedTime = 0;       // 일시정지 누적 시간
int pauseStartTime = 0;   // 일시정지 시작 시점

// 게임 모드
enum GameMode {
    CLASSIC,
    TIME_ATTACK
};

GameMode currentMode = CLASSIC;
const int TIME_ATTACK_LIMIT = 30000; // 30초 제한 (밀리초 단위)

// 게임 요소
Diamond diamond;
double diamondWidthScale = 10, diamondHeightScale = 12;

BrickBox brickbox;
Bar bar;
Ball ball;

int playStartTime = 0;
int playTime = 0;

// 키 입력 상태 저장
bool specialKeyStates[256] = { false };

// 초기화 함수
void init() {
    monitorWidth = glutGet(GLUT_SCREEN_WIDTH);
    monitorHeight = glutGet(GLUT_SCREEN_HEIGHT);

    windowWidth = monitorWidth;
    windowHeight = monitorHeight;

    // 다이아몬드 설정
    diamond.InitColor(1.0, 1.0, 1.0);
    diamond.InitSize(windowWidth / diamondWidthScale * (diamondWidthScale - 2), windowHeight / diamondHeightScale * (diamondHeightScale - 2));
    diamond.InitPoint(windowWidth, windowHeight);

    // 벽돌 설정
    brickbox.InitColor(1.0, 1.0, 1.0);
    brickbox.InitSize(diamond, 4, 4);
    brickbox.InitPoint(diamond, windowWidth / 2, windowHeight / 2);

    // 바 설정
    bar.InitColor(1.0, 1.0, 1.0);
    bar.InitSize(diamond);
    bar.InitPoint(windowWidth, windowHeight, diamond);

    // 공 설정
    ball.InitColor(1.0, 1.0, 0.0);
    ball.InitSize(13.0);
    ball.InitPoint(windowWidth, bar);
    ball.InitVelocity(0, 0);
}

void DrawStringCenter(const char *str, int w, int h, int y0, int color = 0) {
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    if(color == 0) {
        glColor3f(1.0, 1.0, 1.0);
    }
    else if(color == 1) {
        glColor3f(0.0, 1.0, 0.0);
    }

    int size = (int)strlen(str);
    int charWidth = 9;  // GLUT_BITMAP_9_BY_15 기준 글자 너비
    int totalWidth = size * charWidth;

    int x0 = (w / 2) - (totalWidth / 2);

    glRasterPos2f(x0, y0);

    for (int i = 0; i < size; ++i) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, str[i]);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

int GetPlayTime() {
    if (currentState == STATE_PLAYING) {
        return glutGet(GLUT_ELAPSED_TIME) - playStartTime - pausedTime;
    }
    return 0;
}

void MyReshape(int w, int h) {
    glViewport(0, 0, windowWidth, windowHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(LEFT, LEFT + windowWidth, BOTTOM, BOTTOM + windowHeight);
}

// 시작 화면
void DrawStart() {
    DrawStringCenter("Welcome to Brick-Breaker!!", windowWidth, windowHeight, windowHeight / 2);

    int time = glutGet(GLUT_ELAPSED_TIME) % 1000;  // 1초 주기 (1000ms)

    // 표시 시간: 600ms, 숨김 시간: 400ms
    if (time < 600) {
        DrawStringCenter("Press 'SPACE' to continue...", windowWidth, windowHeight, windowHeight - 100);
    }
}

// 게임 모드 선택 화면
void DrawMenu() {
    switch (currentMode) {
        case CLASSIC:
            DrawStringCenter("> Classic Mode <", windowWidth, windowHeight, windowHeight / 2 - 15, 1);
            DrawStringCenter("Time Attack Mode", windowWidth, windowHeight, windowHeight / 2 + 15);
            break;
        case TIME_ATTACK:
            DrawStringCenter("Classic Mode", windowWidth, windowHeight, windowHeight / 2 - 15);
            DrawStringCenter("> Time Attack Mode <", windowWidth, windowHeight, windowHeight / 2 + 15, 1);
            break;
            
        default:
            break;
    }
    
    int time = glutGet(GLUT_ELAPSED_TIME) % 1000;
    
    if (time < 600) {
        DrawStringCenter("Press 'ENTER' to play...", windowWidth, windowHeight, windowHeight - 100);
    }
}

// 게임 시작 전, 게임 플레이 화면
void DrawBeforeGameStart() {
    int time = glutGet(GLUT_ELAPSED_TIME) % 1000;
    
    if (time < 600) {
        DrawStringCenter("Click anywhere to launch the ball towards that point!!", windowWidth, windowHeight, windowHeight / 2);
    }
}

// 일시정지 화면
void DrawPauseMenu() {
    DrawStringCenter("PAUSED", windowWidth, windowHeight, windowHeight / 2 - 60);

    if (pauseMenuSelected == 0) {
        DrawStringCenter("> Resume <", windowWidth, windowHeight, windowHeight / 2, 1);
        DrawStringCenter("Quit", windowWidth, windowHeight, windowHeight / 2 + 30);
    }
    else {
        DrawStringCenter("Resume", windowWidth, windowHeight, windowHeight / 2);
        DrawStringCenter("> Quit <", windowWidth, windowHeight, windowHeight / 2 + 30, 1);
    }
}


// 게임오버 화면
void DrawGameOver() {
    char recordMsg[100];
    
    switch (currentMode) {
        case CLASSIC:
            sprintf(recordMsg, "You smashed it! Your score: %.1f sec", playTime / 1000.0);
            
            DrawStringCenter(recordMsg, windowWidth, windowHeight, windowHeight / 2);
            break;
        case TIME_ATTACK:
            if(GAMECLEAR) {
                DrawStringCenter("GAME CLEAR!!", windowWidth, windowHeight, windowHeight / 2 - 15);
                
                sprintf(recordMsg, "Your score: %.1f sec", playTime / 1000.0);
                
                DrawStringCenter(recordMsg, windowWidth, windowHeight, windowHeight / 2 + 15);
            }
            else {
                sprintf(recordMsg, "Time's up! You broke %d bricks!", brickbox.GetCol() * brickbox.GetRow() - brickbox.GetBrokenNum());
                
                DrawStringCenter(recordMsg, windowWidth, windowHeight, windowHeight / 2 + 15);
            }
            break;
            
        default:
            break;
    }
    
    int time = glutGet(GLUT_ELAPSED_TIME) % 1000;

    if (time < 600) {
        DrawStringCenter("Press 'SPACE' to continue...", windowWidth, windowHeight, windowHeight - 100);
    }
}

// 키 입력 처리
void ProcessKeyInput() {
    switch (currentState) {
        case STATE_PLAYING:
            if(START) {
                if (specialKeyStates[GLUT_KEY_LEFT]) {
                    bar.HorizonMove(-10, diamond);
                }
                if (specialKeyStates[GLUT_KEY_RIGHT]) {
                    bar.HorizonMove(10, diamond);
                }
                if (specialKeyStates[GLUT_KEY_UP]) {
                    bar.VerticalMove(5, diamond);
                }
                if (specialKeyStates[GLUT_KEY_DOWN]) {
                    bar.VerticalMove(-5, diamond);
                }
            }
            break;
        case STATE_MENU:
            if(specialKeyStates[GLUT_KEY_UP]) {
                currentMode = CLASSIC;
                glutPostRedisplay();
            }
            if(specialKeyStates[GLUT_KEY_DOWN]) {
                currentMode = TIME_ATTACK;
                glutPostRedisplay();
            }
//            cout << currentMode << endl;
            break;
        case STATE_PAUSE:
            if(specialKeyStates[GLUT_KEY_UP]) {
                pauseMenuSelected = 0;
                glutPostRedisplay();
            }
            if(specialKeyStates[GLUT_KEY_DOWN]) {
                pauseMenuSelected = 1;
                glutPostRedisplay();
            }
//            cout << currentMode << endl;
            break;
            
        default:
            break;
    }
}

void RenderScene() {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    switch (currentState) {
        case TEST:
            currentMode = TIME_ATTACK;
            GAMECLEAR = true;
            playTime = 22400;
            DrawGameOver();
            break;
        case STATE_START:
            DrawStart();
            break;
        case STATE_MENU:
            ProcessKeyInput();
            DrawMenu();
            break;
        case STATE_PLAYING:
            ProcessKeyInput();
            
            if(!START) {
                DrawBeforeGameStart();
            }
            else {
                char timeStr[64];
                char brickNumStr[64];
                
                switch (currentMode) {
                    case CLASSIC:
                        playTime = GetPlayTime();

                        sprintf(timeStr, "Time: %.1f sec", playTime / 1000.0);
                        DrawStringCenter(timeStr, windowWidth, windowHeight, windowHeight / 2 - 15);

                        sprintf(brickNumStr, "Bricks Left: %d", brickbox.GetBrokenNum());
                        DrawStringCenter(brickNumStr, windowWidth, windowHeight, windowHeight / 2 + 15);

                        if (brickbox.GetBrokenNum() == 0) {
                            currentState = STATE_GAMEOVER;
                        }
                        break;

                    case TIME_ATTACK: {
                        playTime = TIME_ATTACK_LIMIT - GetPlayTime();
                        
                        sprintf(timeStr, "Time: %.1f sec", playTime / 1000.0);
                        DrawStringCenter(timeStr, windowWidth, windowHeight, windowHeight / 2 - 15);
                        
                        int brokenBricks = brickbox.GetCol() * brickbox.GetRow() - brickbox.GetBrokenNum();
                        sprintf(brickNumStr, "Broken Bricks: %d", brokenBricks);
                        DrawStringCenter(brickNumStr, windowWidth, windowHeight, windowHeight / 2 + 15);
                        
                        if (playTime <= 0) {
                            currentState = STATE_GAMEOVER;
                        }
                        else if(brickbox.GetBrokenNum() == 0) {
                            currentState = STATE_GAMEOVER;
                            GAMECLEAR = true;
                        }
                        break;
                    }

                    default:
                        break;
                }
            }

            diamond.Draw(LEFT, BOTTOM);
            brickbox.Draw(LEFT, BOTTOM);
            bar.Draw(LEFT, BOTTOM);
            ball.Draw(LEFT, BOTTOM);
            ball.Moving(bar, diamond, brickbox);
            break;
        case STATE_PAUSE:
            ProcessKeyInput();
            DrawPauseMenu();
            break;
        case STATE_GAMEOVER:
            DrawGameOver();
            break;
    }

    glutSwapBuffers();
}

// 특수키 처리
void MySpecialKeyDown(int key, int x, int y) {
    specialKeyStates[key] = true;
}

void MySpecialKeyUp(int key, int x, int y) {
    specialKeyStates[key] = false;
}

// 일반 키 입력 처리
void MyKeyboardDown(unsigned char key, int x, int y) {
    if (key == 27) {  // ESC
        if (currentState == STATE_PLAYING) {
            currentState = STATE_PAUSE;
            pauseMenuSelected = 0;  // 기본 선택은 Resume
            pauseStartTime = glutGet(GLUT_ELAPSED_TIME);  // 일시정지 시작 시점 기록
        }
        else if (currentState == STATE_PAUSE) {
            currentState = STATE_PLAYING;
            
            if(START) {
                int pauseEndTime = glutGet(GLUT_ELAPSED_TIME); // 일시정지 끝난 시점
                pausedTime += pauseEndTime - pauseStartTime;   // 일시정지 누적 시간 증가
            }
        }
        else if (currentState == STATE_MENU) {
            currentState = STATE_START;
        }
    }
    else if (key == 32) { // Spacebar
        switch (currentState) {
            case STATE_START:
                currentState = STATE_MENU;
                currentMode = CLASSIC;
                break;
            case STATE_GAMEOVER:
                currentState = STATE_MENU;
                
                // 게임 상태 재설정
                diamond.InitPoint(windowWidth, windowHeight);
                brickbox.InitPoint(diamond, windowWidth / 2, windowHeight / 2);
                bar.InitPoint(windowWidth, windowHeight, diamond);
                ball.InitPoint(windowWidth, bar);
                ball.InitVelocity(0, 0);
                brickbox.Reset();
                START = false;
                GAMECLEAR = false;
                currentMode = CLASSIC;
                
            default:
                break;
        }
    }
    else if (key == 13) { // Enter
        if (currentState == STATE_MENU) {
            currentState = STATE_PLAYING;
            ball.InitVelocity(0, 0);
        }
        else if (currentState == STATE_PAUSE) {
            if (pauseMenuSelected == 0) { // Resume
                // 게임 요소 초기화
                diamond.InitPoint(windowWidth, windowHeight);
                brickbox.InitPoint(diamond, windowWidth / 2, windowHeight / 2);
                bar.InitPoint(windowWidth, windowHeight, diamond);
                ball.InitPoint(windowWidth, bar);
                ball.InitVelocity(0, 0);
                brickbox.Reset();

                START = false;
                GAMECLEAR = false;

                currentState = STATE_PLAYING;

                // 일시정지 시간 초기화
                pausedTime = 0;
                pauseStartTime = 0;

                // 게임 시작 시간 초기화
                playStartTime = glutGet(GLUT_ELAPSED_TIME);
            }
            else if (pauseMenuSelected == 1) { // Quit
                currentState = STATE_START;

                diamond.InitPoint(windowWidth, windowHeight);
                brickbox.InitPoint(diamond, windowWidth / 2, windowHeight / 2);
                bar.InitPoint(windowWidth, windowHeight, diamond);
                ball.InitPoint(windowWidth, bar);
                ball.InitVelocity(0, 0);
                brickbox.Reset();

                START = false;
                GAMECLEAR = false;

                pausedTime = 0;
            }
            glutPostRedisplay();
        }
    }
}

// 마우스 클릭 처리
void mouse(int button, int state, int x, int y) {
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
        return;

    int n = 70;
    double adjustedY = windowHeight - y;

    switch (currentState) {
        case STATE_PLAYING:
            if (!START) {
                ball.InitVelocity((x - ball.GetCenter().x) / n, (adjustedY - ball.GetCenter().y) / n);
                
                playStartTime = glutGet(GLUT_ELAPSED_TIME);
                
                START = true;
            }
            break;
            
        default:
            break;
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    init();

    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Brick-Breaker");

    glutDisplayFunc(RenderScene);
    glutIdleFunc(RenderScene);
    glutReshapeFunc(MyReshape);

    glutSpecialFunc(MySpecialKeyDown);
    glutSpecialUpFunc(MySpecialKeyUp);
    glutKeyboardFunc(MyKeyboardDown);

    glutMouseFunc(mouse);

    glutMainLoop();

    return 0;
}
