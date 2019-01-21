#include "main.h"
#include <utility> 
using namespace std;

#ifndef STATIC_OBJ_H
#define STATIC_OBJ_H

class StaticObj{
public:
public:
    glm::vec3 position;
    float rotation;

private:
    VAO *object;
    double speedx;
    float m_width, m_height;
    vector<bounding_box_t> v;
    float sintheta, costheta;
    float sizeCache;
        
public:
    StaticObj() {}
    StaticObj(float x, float y, float width, float height, float speedx, float rot, color_t color);
    StaticObj(float x, float y, float radius, int n, float speedx, color_t color);
    ~StaticObj(){
        cout << "STATIC OBJ DESTRUCTOR\n" << endl;
    }
    void Draw(glm::mat4 VP);
    void SetPosition(float x, float y);
    bounding_box_t GetBoundingBox(){
        return {position.x, position.y, m_width, m_height};
    }
    vector<bounding_box_t>& GetBoundingBox(float size);
    void Tick();
private:
    pair<float, float> rotate(float angle, float x, float y){
	    return make_pair(x*cos(angle) - y*sin(angle), x*sin(angle) + y*cos(angle));
    }
};

#endif