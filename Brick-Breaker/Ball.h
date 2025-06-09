#ifdef BALL_H
#else
#define BALL_H

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include "Bar.h"
#include "Brick.h"

class Ball {
private:
    Point center;
    Vector velocity;
    GLfloat color[3];
    double radius;
    int polygonNum = 100;
    
public:
    void InitColor(double r, double g, double b);
    void InitSize(double radius);
    void InitPoint(double width, Bar bar);
    void InitVelocity(double x, double y);
    
    Point GetCenter() { return this->center; };
    Vector GetVelocity() { return this->velocity; };
    
    void Draw(double left, double bottom);
    void Moving(Bar bar, Diamond diamond, BrickBox& brickbox);
};

#endif
