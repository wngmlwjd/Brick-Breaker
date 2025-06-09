#ifdef BRICK_H
#else
#define BRICK_H

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include <vector>
using namespace std;

#include "Objects.h"
#include "Diamond.h"

struct Brick {
    Point point[4];
    bool broken = false;
};

class BrickBox {
private:
    vector<vector<Brick>> bricks;
    GLfloat color[3];
    
    int col;
    int row;
    int num;
    int brokenNum;
    
    double width;
    double height;
    
    double gapWidth;
    double gapHeight;
    
    double boxWidth;
    double boxHeight;
    
public:
    void InitColor(double r, double g, double b);
    void InitSize(Diamond diamond, int col, int row);
    void InitPoint(Diamond diamond, double x, double y);
    
    void Reset();
    
    int GetCol() { return this->col; };
    int GetRow() { return this->row; };
    int GetBrokenNum() { return this->brokenNum; };
    Brick& GetBrick(int a, int b) { return this->bricks[a][b]; };
    
    void Draw(double left, double bottom);
    void Broken(int i, int j) { this->bricks[i][j].broken = true; this->brokenNum--; };
};


#endif
