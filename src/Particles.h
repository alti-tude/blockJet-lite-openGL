#include"main.h"

#ifndef PARTICLES_H
#define PARTICLES_H
class Particle{
public:
    double speedx, speedy;
    glm::vec3 position;
    float rotation;

private:
    VAO *object;
    float m_width, m_height, m_g;
    int m_life;
    
public:
    Particle() {}
    Particle(float x, float y, float width ,float height, int life, color_t color);
    void Draw(glm::mat4 VP);

    void Tick();
    void SetPosition(float x, float y){
        this->position = glm::vec3(x, y, 0);
    };
    bounding_box_t GetBoundingBox(){
        return {position.x, position.y, m_width, m_height};
    }

    void GiveLife(unsigned int life) {m_life = life;}
    int GetLife(){return m_life;}
    void gravity() { speedy = speedy - m_g; }

};
#endif