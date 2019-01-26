#include "StaticObj.h"
#include "main.h"

struct triplet{
	float x;
	float y;
	float z;
};

StaticObj::StaticObj(float x, float y, float width, float height, float e_speedx, float rot, color_t color) 
{
    this->position = glm::vec3(x, y, 0);
    this->rotation = rot;
    this->m_width = width;
    this->m_height = height;
    this->speedx = e_speedx;
    this->costheta = cos(this->rotation*M_PI/180.0);
    this->sintheta = sin(this->rotation*M_PI/180.0);
    this->sizeCache = 0;

    const GLfloat vertex_buffer_data[] = { 
        -m_width/2, -m_height/2, 0,
         m_width/2, -m_height/2, 0,
         m_width/2,  m_height/2, 0,
        -m_width/2, -m_height/2, 0,
         m_width/2,  m_height/2, 0,
        -m_width/2,  m_height/2, 0,
    };
    
    this->object = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color, GL_FILL);
}

StaticObj::StaticObj(float x, float y, float radius, int n, float speedx, color_t color) 
    :m_width(2*radius), m_height(2*radius), speedx(speedx)
{
    this->position = glm::vec3(x, y, 0);
    this->sizeCache = 0;

	static float vertex_buffer_data[900000];
    triplet point, center;
	point.x = radius; point.y = point.z = 0;
	center.x = center.y = center.z = 0;
	float theta = 2 * M_PI / (float)n;

	int c = 0;

	for(int t=0;t<=n;t++){
		vertex_buffer_data[c++] = point.x;
		vertex_buffer_data[c++] = point.y;
		vertex_buffer_data[c++] = point.z;
		
		triplet tmp = point;
		point = center;
		vertex_buffer_data[c++] = point.x;
		vertex_buffer_data[c++] = point.y;
		vertex_buffer_data[c++] = point.z;
		point = tmp;

		pair<float, float> tr = rotate(theta, point.x, point.y);
		point.x = tr.first;
		point.y = tr.second;

		vertex_buffer_data[c++] = point.x;
		vertex_buffer_data[c++] = point.y;
		vertex_buffer_data[c++] = point.z;
	}

    this->object = create3DObject(GL_TRIANGLES, n*3, vertex_buffer_data, color, GL_FILL);
}

void StaticObj::Draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, -1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void StaticObj::SetPosition(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void StaticObj::Tick() {
    position.x += speedx;
    for(int i=0;i<v.size();i++){
        v[i].x+=speedx;
    }
}

vector<bounding_box_t>& StaticObj::GetBoundingBox(float size){
    if(size != sizeCache){
        sizeCache = size;
        float x = position.x-m_height/2*sintheta + size*sintheta;
        float y = position.y-m_height/2*costheta + size*costheta;

        for(int i=0;i<(int)(m_height/size);i++){
            v.push_back({x,y,sintheta ,size*costheta });
            x += size*costheta;
            y += size*sintheta;
        }
    }   

    return v;
}

