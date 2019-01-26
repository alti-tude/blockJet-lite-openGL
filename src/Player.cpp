#include "Player.h"
#include "main.h"
#include<iostream>

Player::Player(float x, float y, float width, float height, float accely, float maxSpeed, color_t color, color_t swordTop, color_t swordBottom) 
    :m_g(GRAVITY), m_accel(accely), m_width(width), m_height(height), m_maxSpeedy(maxSpeed), m_color(color)
{
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    this->old_width = m_width;
    this->swordTime = 0;
    speedy = speedx = 0;
    {
        GLfloat vertex_buffer_data[] = {
            -m_width/2, -m_height/2, 0,
            m_width/2, -m_height/2, 0,
            m_width/2,  m_height/2, 0,
            -m_width/2, -m_height/2, 0,
            m_width/2,  m_height/2, 0,
            -m_width/2,  m_height/2, 0,
        };
        this->object = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color, GL_FILL);
    }
    {
        GLfloat vertex_buffer_data[] = {
            5*m_width/8, -m_height/4, 0,
            7*m_width/8, -m_height/4, 0,
            3*m_width/4, m_height/2, 0,
        };

        this->object2 = create3DObject(GL_TRIANGLES, 1*3, vertex_buffer_data, swordTop, GL_FILL);
    }
    {
        GLfloat vertex_buffer_data[] = {
            11*m_width/16, -m_height/4, 0,
            13*m_width/16, -m_height/4, 0,
            3*m_width/4, -m_height/2, 0,
        };

        this->object3 = create3DObject(GL_TRIANGLES, 1*3, vertex_buffer_data, swordBottom, GL_FILL);
    }
}

void Player::ChangeColor(color_t color){
    m_color = color;
}

void Player::Draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, -1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate*rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

    glUniform1i(shaderColorActivationId, 1);
    glUniform3f(shaderColorId, m_color.r/255.0, m_color.g/255.0, m_color.b/255.0);
    draw3DObject(this->object);
    glUniform1i(shaderColorActivationId, 0);

    if(swordTime>0) 
        draw3DObject(this->object2), draw3DObject(this->object3);
}

void Player::SetPosition(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Player::Tick() {
    position.y += speedy;
    position.x += speedx;
    speedx = 0;
    m_color.r += m_color.r+reloadSpeed.r > loadedColor.r? 0:reloadSpeed.r;
    m_color.g += m_color.g+reloadSpeed.g > loadedColor.g? 0:reloadSpeed.g;
    m_color.b += m_color.b+reloadSpeed.b > loadedColor.b? 0:reloadSpeed.b;

    if(swordTime > 0) swordTime--;
    else m_width = old_width;
    std::cout << swordTime << std::endl;
}

void Player::Up(){
    speedy = speedy + m_accel;
    speedy = speedy > m_maxSpeedy ? m_maxSpeedy : speedy;
}

void Player::Down(){
    speedy = speedy - m_accel/2;
}

