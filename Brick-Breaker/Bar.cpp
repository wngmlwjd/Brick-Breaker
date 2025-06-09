#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

#include "Bar.h"

#define PI 3.1415926

void Bar::InitColor(double r, double g, double b) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

void Bar::InitSize(Diamond diamond) {
    this->width = diamond.GetWidth() / 7;
    this->height = diamond.GetHeight() / 70;
}

void Bar::InitPoint(double width, double height, Diamond diamond) {
    // 좌측하단부터 반시계방향
    this->point[0].x = width / 2 - this->width / 2;
    this->point[0].y = (height - diamond.GetHeight()) / 2 + diamond.GetHeight() / 5;
    this->point[1].x = width / 2 + this->width / 2;
    this->point[1].y = (height - diamond.GetHeight()) / 2 + diamond.GetHeight() / 5;
    this->point[2].x = width / 2 + this->width / 2;
    this->point[2].y = (height - diamond.GetHeight()) / 2 + diamond.GetHeight() / 5 - this->height;
    this->point[3].x = width / 2 - this->width / 2;
    this->point[3].y = (height - diamond.GetHeight()) / 2 + diamond.GetHeight() / 5 - this->height;
}

void Bar::Draw(double left, double bottom) {
    glColor3fv(this->color);
    glBegin(GL_POLYGON);
    for(int i = 0;i < 4;i++) {
        glVertex2f(left + this->point[i].x, bottom + this->point[i].y);
    }
    glEnd();
}

void Bar::HorizonMove(double velocity, Diamond diamond) {
    int a = 0, b = 0;
    
    if(velocity < 0) {
        a = 2;
        b = 3;
    }
    else if(velocity > 0) {
        a = 1;
        b = 2;
    }
    
    Point p1 = diamond.GetPoint(a);
    Point p2 = diamond.GetPoint(b);
    
    // 직선의 방정식 계산: Ax + By + C = 0
    double A = p2.y - p1.y;
    double B = p1.x - p2.x;
    double C = p2.x * p1.y - p1.x * p2.y;
    
    // 거리
    if(abs(A * this->point[b].x + B * this->point[b].y + C) / sqrt(A * A + B * B) < abs(velocity)) {
        return;
    }
    
    for(int i = 0;i < 4;i++) {
        this->point[i].x += velocity;
    }
}

void Bar::VerticalMove(double velocity, Diamond diamond) {
    if(velocity < 0) {
        for(int i = 1;i < 3;i++) {
            int a = i;
            int b = i + 1;
            Point p1 = diamond.GetPoint(a);
            Point p2 = diamond.GetPoint(b);
            
            // 직선의 방정식 계산: Ax + By + C = 0
            double A = p2.y - p1.y;
            double B = p1.x - p2.x;
            double C = p2.x * p1.y - p1.x * p2.y;
            
            // 거리
            if(abs(A * this->point[b].x + B * this->point[b].y + C) / sqrt(A * A + B * B) < abs(velocity)) {
                return;
            }
        }
    }
    else if(velocity > 0) {
        if(abs((diamond.GetHeight() / 5 * 2 + diamond.GetPoint(2).y) - this->point[3].y) < 5) {
            return;
        }
    }
    
    for(int i = 0;i < 4;i++) {
        this->point[i].y += velocity;
    }
}
