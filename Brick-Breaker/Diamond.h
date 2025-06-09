#ifdef DIAMOND_H
#else
#define DIAMOND_H

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>

#include "Objects.h"

class Diamond {
private:
    Point point[4];
    GLfloat color[3];
    
    double width;
    double height;

public:
    void InitColor(double r, double g, double b);
    void InitSize(double width, double height);
    void InitPoint(double width, double height);
    
    double GetWidth() { return this->width; };
    double GetHeight() { return this->height; };
    
    void Draw(double left, double bottom);
    
    Point GetPoint(int n) { return this->point[n]; };
};

#endif
