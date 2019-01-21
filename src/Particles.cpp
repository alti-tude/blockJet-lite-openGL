#include "Particles.h"
#include<iostream>
#include "main.h"
 
Particle::Particle(float x, float y, float width, float height, int life, color_t color) 
    :m_life(life), m_width(width), m_height(height), m_g(GRAVITY)
{
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    speedy = speedx = 0;
    static const GLfloat vertex_buffer_data[] = {
        -m_width/2, -m_height/2, 0,
         m_width/2, -m_height/2, 0,
         m_width/2,  m_height/2, 0,
        -m_width/2, -m_height/2, 0,
         m_width/2,  m_height/2, 0,
        -m_width/2,  m_height/2, 0,
    };

    this->object = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color, GL_FILL);
}

void Particle::Draw(glm::mat4 VP) {
    if(m_life<=0) return;
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Particle::Tick() {
    if(this->m_life<=0) {speedy = 0; return;}

    gravity();
    position.y += speedy;

    this->m_life --;
}