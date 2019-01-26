#include "main.h"
#include <utility> 
using namespace std;

#ifndef CURVE_H
#define CURVE_H

class Curve{
public:
public:
    glm::vec3 position;
    float rotation;
    float min_x, min_y, max_x, max_y;
    float angleLeft, angleRight, radius;
    unsigned int life;
    double speedx;

private:
    VAO *object;
    VAO *object2;
    float m_ratio;
    vector<bounding_box_t> v;
    float sintheta, costheta;
    float sizeCache;
        
public:
    Curve() {}
    Curve(float x, float y, float radius, float ratio, float speedx, float rot, color_t color);
    void CurveInside(float stroke, color_t color);
    ~Curve(){
        std::cout << "CURVE OBJ DESTRUCTOR\n" << std::endl;
    }
    void Draw(glm::mat4 VP);
    void SetPosition(float x, float y);
    void Tick();
private:
    pair<float, float> rotate(float angle, float x, float y){
	    return make_pair(x*cos(angle) - y*sin(angle), x*sin(angle) + y*cos(angle));
    }
};

#endif