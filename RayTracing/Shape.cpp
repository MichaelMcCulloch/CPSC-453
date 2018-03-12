#include "Shape.h"

Sphere::Sphere(vec3 i, GLfloat j) { c = i, r = j; }
bool Sphere::intersect(const Ray ray, GLfloat &t) {
	vec3 o = ray.o, d = ray.d, oc = o - c;
	float a = dot(d, d);
	float b = 2 * dot(d, oc);
	float c = dot(oc, oc) - pow(r, 2);
	float disc = pow(b, 2) - 4 * c;
	if (disc < 1e-4) return false;
	
	disc = sqrt(disc);
	float t0 = -b - disc;
	float t1 = -b + disc;
	t = (t0 < t1) ? t0 : t1;
	return true;
}

Plane::Plane(vec3 i, vec3 j) { n = i, p = j; }
bool Plane::intersect(Ray ray, GLfloat &t) {
	vec3 o = ray.o, q = p, d = ray.d;
	float qn = dot(q, n),
		on = dot(o, n),
		dn = dot(d, n);
	if (dn == 0) return false;
	t = (qn - on) / dn;
	return true;

}

Triangle::Triangle(vec3 i, vec3 j, vec3 k) {
	a = i, b = j, c = k; 
	vec3 m = c - a, n = b - a;
	vec3 norm = cross(m, n);
	p = new Plane(norm, a);
}
bool Triangle::intersect(Ray ray, GLfloat & t) {
	mat3 corners = mat3(a, b, c);
	vec3 d = ray.d;
	mat3 inv = inverse(corners);
	vec3 bary = inv * d;
	if (bary.x < 0 || bary.y < 0 || bary.x < 0) return false;
	//find intersection point on plane

}