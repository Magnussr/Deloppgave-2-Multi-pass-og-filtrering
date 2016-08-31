#include "VirtualTrackball.h"
#include <cmath>
#include <iostream>

VirtualTrackball::VirtualTrackball() {
	view_matrix_old.w = 1.0;
	view_matrix_old.x = 0.0;
	view_matrix_old.y = 0.0;
	view_matrix_old.z = 0.0;
	rotating = false;
}

VirtualTrackball::~VirtualTrackball() {}

void VirtualTrackball::rotateBegin(int x, int y) {
	rotating = true;
	point_on_sphere_begin = getClosestPointOnUnitSphere(x, y);
}

void VirtualTrackball::rotateEnd(int x, int y) {
	rotating = false;
	view_matrix_old = glm::normalize(view_matrix_new);
}

glm::mat4 VirtualTrackball::rotate(int x, int y) {
	//If not rotating, simply return the old rotation matrix
	if (!rotating) return glm::transpose(glm::toMat4(view_matrix_old));

	glm::vec3 point_on_sphere_end; //Current point on unit sphere
	glm::vec3 axis_of_rotation; //axis of rotation
	float theta; //angle of rotation

	point_on_sphere_end = getClosestPointOnUnitSphere(x, y);
	theta = acos(glm::dot(point_on_sphere_begin, point_on_sphere_end)) * 180.0f / 3.141592653f;

	axis_of_rotation = glm::normalize(glm::cross(point_on_sphere_end, point_on_sphere_begin));
	
	//std::cout << axis_of_rotation.x << " " << axis_of_rotation.y << " " << axis_of_rotation.z << std::endl;

	view_matrix_new = glm::rotate(view_matrix_old, theta, axis_of_rotation);
	
	glm::mat4 m = glm::transpose(glm::toMat4(view_matrix_new));

	return glm::transpose(glm::toMat4(view_matrix_new));
}

void VirtualTrackball::setWindowSize(int w, int h) {
	this->w = w;
	this->h = h;
}

glm::vec2 VirtualTrackball::getNormalizedWindowCoordinates(int x, int y) {
	glm::vec2 p;
	p[0] = x/static_cast<float>(w) - 0.5f;
	p[1] = 0.5f - y/static_cast<float>(h);
	//std::cout << p[0] << " " << p[1] << std::endl;
	return p;
}

glm::vec3 VirtualTrackball::getClosestPointOnUnitSphere(int x, int y) {
	glm::vec2 normalized_coords;
	glm::vec3 point_on_sphere;
	float r;

	normalized_coords = getNormalizedWindowCoordinates(x, y);
	r = glm::length(normalized_coords);

	if (r < 0.5) { //Ray hits unit sphere
		point_on_sphere[0] = 2*normalized_coords[0];
		point_on_sphere[1] = 2*normalized_coords[1];
		point_on_sphere[2] = sqrt(1 - 4*r*r);

		point_on_sphere = glm::normalize(point_on_sphere);
	}
	else { //Ray falls outside unit sphere
		point_on_sphere[0] = normalized_coords[0]/r;
		point_on_sphere[1] = normalized_coords[1]/r;
		point_on_sphere[2] = 0;            
	}

	return point_on_sphere;
}