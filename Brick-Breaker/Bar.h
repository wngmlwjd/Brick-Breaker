#ifdef BAR_H
#else
#define BAR_H

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include "Diamond.h"

class Bar {
private:
    Point point[4];
    GLfloat color[3];
    
    double width;
    double height;
    
public:
    void InitColor(double r, double g, double b);
    void InitSize(Diamond diamond);
    void InitPoint(double width, double diamondHeightScale, Diamond diamond);
    
    Point GetPoint(int n) { return this->point[n]; };
    
    void Draw(double left, double bottom);
    void HorizonMove(double velocity, Diamond diamond);
    void VerticalMove(double velocity, Diamond diamond);
};

#endif

