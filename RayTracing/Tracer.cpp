#include "Tracer.h"

Sphere::Sphere(vec3 i, GLfloat j) { c = i, r = j; }
bool Sphere::intersect(Ray ray, GLfloat &t) {
	return false;
}

Plane::Plane(vec3 i, vec3 j) { normal = i, point = j; }
bool Plane::intersect(Ray ray, GLfloat &t) {
	return false;

}

Triangle::Triangle(vec3 i, vec3 j, vec3 k) { a = i, b = j, c = k; }
bool Triangle::intersect(Ray ray, GLfloat & t) {
	return false;
}

int main(int argc, char *argv[]){
}

