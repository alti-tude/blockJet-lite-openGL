#include "main.h"
#include <utility> 
using namespace std;

#ifndef BOOMERANG_H
#define BOOMERANG_H

class Boomerang{
public:
public:
    glm::vec3 position;
    float rotation;
    double speedx, speedy;
    unsigned int life;

private:
    VAO *object, *object2;
    float m_width, m_height;
    
public:
    Boomerang() {}
    Boomerang(float x, float y, float width, float height, float speedx, float speedy, float stroke, color_t color, color_t colorInside);

    void Draw(glm::mat4 VP);
    void SetPosition(float x, float y);
    bounding_box_t GetBoundingBox(){
        return {position.x, position.y, m_width, m_height};
    }

    void Tick();
    void Gravity();
private:
    pair<float, float> rotate(float angle, float x, float y){
	    return make_pair(x*cos(angle) - y*sin(angle), x*sin(angle) + y*cos(angle));
    }
};

#endif