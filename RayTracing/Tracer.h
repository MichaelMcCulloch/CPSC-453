#pragma once

#include <vector>

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class Ray {
	vec3 o, d;
	Ray(vec3 i, vec3 j);
};

class Shape {
public:
	virtual ~Shape() {};
	virtual bool intersect(Ray ray, GLfloat &t) = 0;
};

class Sphere : public Shape{
	vec3 c;		//center
	GLfloat r;	//radius
	Sphere(vec3 i, float j);

	bool intersect(Ray ray, GLfloat & t);

};
class Plane : public Shape {
	vec3 normal;
	vec3 point;
	Plane(vec3 i, vec3 j);

	bool intersect(Ray ray, GLfloat & t);

};
class Triangle : public Shape {
	vec3 a, b, c; //corners in counter clockwise order
	Triangle(vec3 i, vec3 j, vec3 k);
	bool intersect(Ray ray, GLfloat & t);

};