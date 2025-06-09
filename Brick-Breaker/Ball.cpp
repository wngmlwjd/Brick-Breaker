#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

#include "Ball.h"

#define PI 3.1415926
#define GRAVITY 0.05
#define FRICTION 0.98

double CalculationDistance(Point p1, Point p2, Point point) {
    // 직선의 벡터
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    
    // 바의 길이 계산
    double length = sqrt(dx * dx + dy * dy);
    
    // 벡터 정규화
    dx /= length;
    dy /= length;
    
    // p1에서 point로 향하는 벡터의 투영 계산 (스칼라 값)
    double projection = (point.x - p1.x) * dx + (point.y - p1.y) * dy;

    // 공이 바의 양 끝점 밖에 있다면, 가장 가까운 끝점까지의 거리로 계산
    if (projection < 0) {
        return sqrt(pow(point.x - p1.x, 2) + pow(point.y - p1.y, 2));
    } else if (projection > length) {
        return sqrt(pow(point.x - p2.x, 2) + pow(point.y - p2.y, 2));
    }

    // 바의 중간에 위치하는 경우, 직선 거리를 계산
    double A = p2.y - p1.y;
    double B = p1.x - p2.x;
    double C = p2.x * p1.y - p1.x * p2.y;

    return abs(A * point.x + B * point.y + C) / sqrt(A * A + B * B);
}

void Ball::InitColor(double r, double g, double b) {
    this->color[0] = r;
    this->color[1] = g;
    this->color[2] = b;
}

void Ball::InitSize(double radius) {
    this->radius = radius;
}

void Ball::InitPoint(double width, Bar bar) {
    this->center.x = width / 2;
    this->center.y = bar.GetPoint(3).y + this->radius;
}

void Ball::InitVelocity(double x, double y) {
    this->velocity.x = x;
    this->velocity.y = y;
}

void Ball::Draw(double left, double bottom) {
    float delta = 2 * PI / polygonNum;
    
    glBegin(GL_POLYGON);
    for(int i = 0;i < polygonNum;i++) {
        glVertex2f(left + this->center.x + this->radius * cos(delta * i), bottom + this->center.y + this->radius * sin(delta * i));
    }
    glEnd();
}

void Ball::Moving(Bar bar, Diamond diamond, BrickBox& brickbox) {
    // --- 1. 중력 적용 ---
    this->velocity.y -= GRAVITY;

    // --- 2. 속도에 따른 위치 업데이트 ---
    // --- 속도 제한 ---
    const double MAX_SPEED = 10.0;

    double speed = sqrt(this->velocity.x * this->velocity.x + this->velocity.y * this->velocity.y);
    if (speed > MAX_SPEED) {
        this->velocity.x = (this->velocity.x / speed) * MAX_SPEED;
        this->velocity.y = (this->velocity.y / speed) * MAX_SPEED;
    }
    
    this->center.x += this->velocity.x;
    this->center.y += this->velocity.y;

    // --- 3. Bar 충돌 검사 ---
    for (int i = 0; i < 4; i++) {
        Point p1 = bar.GetPoint(i);
        Point p2 = bar.GetPoint((i + 1) % 4);

        double dist = CalculationDistance(p1, p2, this->center);
        if (dist <= this->radius) {
//            cout << "bar" << endl;
            // 법선 벡터 계산
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;

            double nx = -dy;
            double ny = dx;
            double length = sqrt(nx * nx + ny * ny);
            nx /= length;
            ny /= length;

            // 속도 벡터 반사
            double dot = this->velocity.x * nx + this->velocity.y * ny;
            this->velocity.x = this->velocity.x - 2 * dot * nx;
            this->velocity.y = this->velocity.y - 2 * dot * ny;
            
            // 마찰 효과 및 반발력 강화
            double bounceStrength = 1.1;  // 원하는 반발력 (1.0은 기본, 1.2는 약간 강하게)
            this->velocity.x *= FRICTION * bounceStrength;
            this->velocity.y *= FRICTION * bounceStrength;


            // 위치 보정
            this->center.x += nx * (this->radius - dist);
            this->center.y += ny * (this->radius - dist);

            break;
        }
    }

    // --- 4. Diamond 충돌 검사 ---
    for (int i = 0; i < 4; i++) {
        Point p1 = diamond.GetPoint(i);
        Point p2 = diamond.GetPoint((i + 1) % 4);

        double dist = CalculationDistance(p1, p2, this->center);

        if (dist <= this->radius + 10) {
//            cout << "diamond" << endl;
            // 법선 벡터 계산
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;

            double nx = -dy;
            double ny = dx;
            double length = sqrt(nx * nx + ny * ny);
            nx /= length;
            ny /= length;

            // 속도 벡터 반사
            double dot = this->velocity.x * nx + this->velocity.y * ny;
            this->velocity.x = this->velocity.x - 2 * dot * nx;
            this->velocity.y = this->velocity.y - 2 * dot * ny;

            // 위치 보정
            this->center.x += nx * (this->radius - dist);
            this->center.y += ny * (this->radius - dist);

            break;
        }
    }
    
    // 벽돌 충돌 감지
    for(int i = 0; i < brickbox.GetRow(); ++i) {
        for(int j = 0; j < brickbox.GetCol(); ++j) {
            if(!brickbox.GetBrick(i, j).broken) {
                Brick& brick = brickbox.GetBrick(i, j);

                double min_dist = 1e9;
                Point closest_p1, closest_p2;

                for(int k = 0; k < 4; ++k) {
                    Point p1 = brick.point[k];
                    Point p2 = brick.point[(k + 1) % 4];

                    double dist = CalculationDistance(p1, p2, this->center);
                    if (dist < min_dist) {
                        min_dist = dist;
                        closest_p1 = p1;
                        closest_p2 = p2;
                    }
                }

                if (min_dist <= this->radius) {
//                    cout << "brick" << endl;
                    brickbox.Broken(i, j);

                    // 법선 벡터 계산
                    double dx = closest_p2.x - closest_p1.x;
                    double dy = closest_p2.y - closest_p1.y;

                    double nx = -dy;
                    double ny = dx;
                    double length = sqrt(nx * nx + ny * ny);
                    nx /= length;
                    ny /= length;

                    // 속도 벡터 반사
                    double dot = this->velocity.x * nx + this->velocity.y * ny;
                    this->velocity.x = this->velocity.x - 2 * dot * nx;
                    this->velocity.y = this->velocity.y - 2 * dot * ny;

                    // 위치 보정
                    this->center.x += nx * (this->radius - min_dist);
                    this->center.y += ny * (this->radius - min_dist);
                }
            }
        }
    }
}
