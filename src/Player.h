#ifndef PLAYER_H
#define PLAYER_H
#include"main.h"
class Player {
public:
    double speedx, speedy;
    glm::vec3 position;
    float rotation;
    color_t reloadSpeed;
    color_t loadedColor;
    unsigned int swordTime;

private:
    VAO *object, *object2, *object3;
    double m_g, m_accel;
    float m_width, m_height, m_maxSpeedy, old_width;
    color_t m_color;
    GLfloat m_vertex_buffer_data[20];

public:
    Player() {}
    Player(float x, float y, float width, float height, float accely, float maxSpeedy, color_t color, color_t swordTop, color_t swordBottom);
    void Draw(glm::mat4 VP);
    void SetPosition(float x, float y);
    void Tick();

    void Up();
    void Down();
    void SetGravity(double g) { m_g = g; }
    void SetAccel(double accel) { m_accel = accel; }
    bounding_box_t GetBoundingBox(){ return {position.x, position.y, m_width, m_height}; }
    void gravity() { speedy = speedy - m_g; }
    void GenSword(unsigned int swordTime){ this->swordTime = swordTime, m_width = 7*m_width/8; }

    void ChangeColor(color_t color);
};

#endif