#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <cmath>

#include "Objects.h"
#include "Brick.h"

void BrickBox::InitColor(double r, double g, double b) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

void BrickBox::InitSize(Diamond diamond, int col, int row) {
    this->boxWidth = diamond.GetWidth() / 3;
    this->boxHeight = diamond.GetHeight() / 5;
    
    this->col = col;
    this->row = row;
    this->num = col * row;
    this->brokenNum = this->num;
    
    this->width = boxWidth / col * (col - 1) / col;
    this->height = boxHeight / row * (row - 1) / row;
    
    this->gapWidth = boxWidth / col / (col - 1);
    this->gapHeight = boxHeight / row / (row - 1);
}

void BrickBox::InitPoint(Diamond diamond, double x, double y) {
    // 벽돌 개별 좌표 계산
    x -= this->boxWidth / 2;
    y += this->boxHeight / 2;
    
    double temp = x;
    
    this->bricks.clear();
    for(int i = 0;i < row;i++) {
        this->bricks.push_back(vector<Brick>());
        
        for(int j = 0;j < col;j++) {
            this->bricks[i].push_back(Brick());
            
            // 좌측하단부터 반시계방향
            this->bricks[i][j].point[0].x = x;
            this->bricks[i][j].point[0].y = y;
            this->bricks[i][j].point[1].x = x + this->width;
            this->bricks[i][j].point[1].y = y;
            this->bricks[i][j].point[2].x = x + this->width;
            this->bricks[i][j].point[2].y = y + this->height;
            this->bricks[i][j].point[3].x = x;
            this->bricks[i][j].point[3].y = y + this->height;
            
            x += this->width + this->gapWidth;
        }
        
        x = temp;
        y += this->height + this->gapHeight;
    }
}

void BrickBox::Reset() {
    for(int i = 0;i < this->row;i++) {
        for(int j = 0;j < this->col;j++) {
            this->bricks[i][j].broken = false;
        }
    }
    
    this->brokenNum = this->num;
}

void BrickBox::Draw(double left, double bottom) {
    glColor3fv(this->color);
    for (int i = 0; i < this->row; i++) {
        for (int j = 0; j < this->col; j++) {
            if (this->bricks[i][j].broken) continue; // 깨진 벽돌은 그리지 않음
            
            glBegin(GL_POLYGON);
            for (int k = 0; k < 4; k++) {
                glVertex2f(left + this->bricks[i][j].point[k].x,
                           bottom + this->bricks[i][j].point[k].y);
            }
            glEnd();
        }
    }
}
