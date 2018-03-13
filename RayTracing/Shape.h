#pragma once

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

class Ray {
public:
	vec3 o, d;
private:
	Ray(vec3 i, vec3 j);
};

class Shape {
public:
	virtual ~Shape() {};
	virtual bool intersect(Ray ray, float &t, vec3 &col) = 0;
};

class Sphere : public Shape {
	vec3 c;		//center
	GLfloat r;	//radius
public:
	Sphere(vec3 i, float j);
	bool intersect(const Ray ray, float & t, vec3 &col);

};
class Plane : public Shape {
	vec3 n;		//normal
	vec3 p;		//point on the plane
public:
	Plane(vec3 i, vec3 j);
	bool intersect(Ray ray, float & t, vec3 &col);

};
class Triangle : public Shape {
private:
	vec3 a, b, c; //corners in counter clockwise order
	vec3 n;//normal of triangle
	Plane *p;
public:
	Triangle(vec3 i, vec3 j, vec3 k);
	bool intersect(Ray ray, float & t, vec3 &col);

};