#include "Dragon.h"
#include "main.h"

struct triplet{
	float x;
	float y;
	float z;
};

Dragon::Dragon(float x, float y, float width, float height, float speedy, color_t color, color_t colorInside) 
    :m_width(width), m_height(height), speedx(speedx), speedy(speedy)
{
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    {
        GLfloat vertex_buffer_data[] = {
            -m_width/2, m_height/3, 0,
            0, m_height/3, 0,
            0,0, 0,
            -m_width/2, -m_height/3, 0,
            0, -m_height/3, 0,
            0,0, 0,
            
            m_width, m_height/3, 0,
            m_width, -m_height/3, 0,
            3*m_width/2, 0, 0
        };
        this->object = create3DObject(GL_TRIANGLES, 3*3, vertex_buffer_data, color, GL_FILL);
    }
    {

        GLfloat vertex_buffer_data[] = {
            0,m_height/2, 0,
            0,-m_height/2, 0,
            m_width, m_height/2, 0,
            0,-m_height/2, 0,
            m_width,-m_height/2, 0,
            m_width, m_height/2, 0,
        };
        this->object2 = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, colorInside, GL_FILL);
    }
}

void Dragon::Draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, -1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate*rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
    draw3DObject(this->object2);
}

void Dragon::SetPosition(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}
 
void Dragon::Tick() {
    position.x += speedx;
    position.y += speedy;
    life --;
}

void Dragon::Gravity() {
    speedy -= GRAVITY;
}

