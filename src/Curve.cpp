#include "Curve.h"
#include "main.h"

struct triplet{
	float x;
	float y;
	float z;
};

Curve::Curve(float x, float y, float radius, float ratio, float speedx, float rot, color_t color) 
    :radius(radius), speedx(speedx), m_ratio(ratio), angleLeft( 360-ratio*360+rot ), angleRight( 360+rot )
{
    this->position = glm::vec3(x, y, 0);
    this->min_y = 1000, this->min_x = 1000;
    this->max_y = -1000, this->max_x = -1000;
    this->rotation = rot; 
	float vertex_buffer_data[900000];
    angleLeft = angleLeft > 360? angleLeft - 360:angleLeft;
    angleRight = angleRight > 360? angleRight - 360:angleRight;
    triplet point, center;
	point.x = radius; point.y = 0; point.z = 0;
	center.x = center.y = center.z = 0;
	float theta = 2 * M_PI / (float)(1000.0);

	int c = 0;

	for(int t=0;t<=(int)(1000.0*ratio);t++){
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

    std::cout << angleLeft << " " << angleRight << std::endl;
    this->object = create3DObject(GL_TRIANGLES, c, vertex_buffer_data, color, GL_FILL);
}

void Curve::CurveInside(float stroke, color_t color)
{
	float vertex_buffer_data[900000];
    triplet point, center;
	point.x = radius - stroke; point.y = 0; point.z = 0;
	center.x = center.y = center.z = 0;
	float theta = 2 * M_PI / (float)(1000.0);

	int c = 0;

	for(int t=0;t<=(int)(1000.0*m_ratio);t++){
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

    this->object2 = create3DObject(GL_TRIANGLES, c, vertex_buffer_data, color, GL_FILL);
}


void Curve::Draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, -1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
    draw3DObject(this->object2);
}

void Curve::SetPosition(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Curve::Tick() {
    position.x += speedx;
    for(int i=0;i<v.size();i++){
        v[i].x+=speedx;
    }
    life--;
}


