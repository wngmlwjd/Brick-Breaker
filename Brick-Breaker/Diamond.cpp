#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <cmath>

#include "Diamond.h"

void Diamond::InitColor(double r, double g, double b) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

void Diamond::InitSize(double width, double height) {
    this->width = width;
    this->height = height;
}

void Diamond::InitPoint(double width, double height) {
    // 상
    this->point[0].x = width / 2;
    this->point[0].y = height / 2 + this->height / 2;
    // 우
    this->point[1].x = width / 2 + this->width / 2;
    this->point[1].y = height / 2;
    // 하
    this->point[2].x = width / 2;
    this->point[2].y = height / 2 - this->height / 2;
    // 좌
    this->point[3].x = width / 2 - this->width / 2;
    this->point[3].y = height / 2;
}

void Diamond::Draw(double left, double bottom) {
    glColor3fv(this->color);
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    for(int i = 0;i < 4;i++) {
        glVertex2f(left + this->point[i].x, bottom + this->point[i].y);
    }
    glEnd();
}

