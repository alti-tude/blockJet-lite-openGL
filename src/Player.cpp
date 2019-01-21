#include "Player.h"
#include "main.h"
#include<iostream>

Player::Player(float x, float y, float width, float height, float accely, float maxSpeed, color_t color) 
    :m_g(GRAVITY), m_accel(accely), m_width(width), m_height(height), m_maxSpeedy(maxSpeed), m_color(color)
{
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    speedy = speedx = 0;
    GLfloat vertex_buffer_data[] = {
        -m_width/2, -m_height/2, 0,
         m_width/2, -m_height/2, 0,
         m_width/2,  m_height/2, 0,
        -m_width/2, -m_height/2, 0,
         m_width/2,  m_height/2, 0,
        -m_width/2,  m_height/2, 0,
    };

    this->object = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color, GL_FILL);

    for(int i=0;i<sizeof(vertex_buffer_data)/sizeof(float);i++) 
        m_vertex_buffer_data[i] = vertex_buffer_data[i]; 
}

void Player::ChangeColor(color_t color){
    this->object = create3DObject(GL_TRIANGLES, 2*3, m_vertex_buffer_data, color, GL_FILL);
}

void Player::Draw(glm::mat4 VP) {
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

void Player::SetPosition(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Player::Tick() {
    position.y += speedy;
    position.x += speedx;
    speedx = 0;
    m_color.r += reloadSpeed, m_color.r%=256;
}

void Player::Up(){
    speedy = speedy + m_accel;
    speedy = speedy > m_maxSpeedy ? m_maxSpeedy : speedy;
}

void Player::Down(){
    speedy = speedy - m_accel/2;
}

