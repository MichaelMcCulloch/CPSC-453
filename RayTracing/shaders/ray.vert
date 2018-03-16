#version 430

layout(location = 0) in vec2 VertexPosition;
out vec3 Colour;

struct Sphere {
	vec4 center;
	vec4 color;
	float radius;
	
};

struct Plane {
	vec4 norm;
	vec4 point;
	vec4 color;
};

struct Triangle{
	vec4 A;
	vec4 B;
	vec4 C;
	vec4 color;
};

layout(std140, binding = 1) uniform SphereData {
	Sphere sphere[5];
};
layout(std140, binding = 2) uniform TriangleData {
	Triangle triangle[5];
};
layout(std140, binding = 3) uniform PlaneData {
	Plane plane[5];
};

//for iterating over
uniform int numSpheres; 
uniform int numTriangles;
uniform int numPlanes;

uniform vec3 origin;

float fov = 60;
const float pi = 3.1415926535897931;
//calculate a direction vector from which px on screen we are rendering
vec3 directionFromPixel(vec2 pixel);

//determine intersection point for ray, and selected shape in selected shape index
float intersectSphere(vec3, int);
float intersectPlane(vec3, int );
float intersectTriangle(vec3, int);

void main(){

	gl_Position = vec4(VertexPosition, 0.0, 1.0);

	vec3 dir = directionFromPixel(VertexPosition);
	vec3 col;
	float min = -1;
	vec3 chosenColor = vec3(0,0,0);
	for (int i = 0; i < numSpheres; i++){ //iterate over spheres
		float t = intersectSphere(dir, i);
		if (t < 0) continue;
		if (min < 0 || t < min){ 
			min = t; //if min has not been set
			chosenColor = sphere[i].color.rgb;
		}
	}

	for (int i = 0; i < numPlanes; i++){ //iterate over planes
		float t = intersectPlane(dir, i);
		if (t < 0) continue;
		if (min < 0 || t < min){ 
			min = t; //if min has not been set
			chosenColor = plane[i].color.rgb;
		}
	}

	for (int i = 0; i < numTriangles; i++){ //iterate over triangles
		float t = intersectTriangle(dir, i);
		if (t < 0) continue;
		if (min < 0 || t < min){ 
			min = t; //if min has not been set
			chosenColor = triangle[i].color.rgb;
		}
	}

	if ( min > 0) col = chosenColor;
	else col = vec3(0,0,0);
	
	Colour = col;
}

vec3 directionFromPixel(vec2 pixel){
	float rads = fov * pi / 180;
	float z = 1 / (2 * tan(rads/2.0));
	vec3 dir = vec3(pixel.x, pixel.y, -z);
	return normalize(dir);
}

float intersectSphere(vec3 ray, int i){
	
	vec3 d = ray;
	vec3 center = sphere[i].center.xyz;
	vec3 oc = origin - center;
	float r = sphere[i].radius;

	float a = dot(d,d);
	float b = 2 * dot(d, oc);
	float c = dot(oc, oc) - pow(r, 2);
	
	float disc = pow(b, 2) - 4 * c;
	if (disc < 0) return -1;

	disc = sqrt(disc);
	float t0 = -b - disc;
	float t1 = -b + disc;
	if (t0 < t1) {
		return t0;
	} else {
		return t1;
	}
}

float intersectPlane(vec3 ray, int i){
	vec3 o = origin;
	vec3 d = ray;
	vec3 q = plane[i].point.xyz;
	vec3 n = plane[i].norm.xyz;

	float qn = dot(q,n);
	float on = dot(o,n);
    float dn = dot(d,n);
	float t = (qn - on) / dn;
	return t;
}

float intersectTriangle(vec3 ray, int i){

	
	vec3 A = triangle[i].A.xyz;
	vec3 B = triangle[i].B.xyz;
	vec3 C = triangle[i].C.xyz;

	mat3 corn = mat3(A,B,C);

	mat3 inv = inverse(corn);
	vec3 bary = inv * ray;

	//check that ray is in triangle
	if (bary.x < 0 || bary.y < 0 || bary.x < 0) return -1;

	vec3 M = C - A;
	vec3 N = B - A;

	vec3 norm = cross(M, N);

	//same plane calc
	vec3 o = origin;
	vec3 d = ray;
	vec3 q = A;
	vec3 n = norm;

	float qn = dot(q,n);
	float on = dot(o,n);
    float dn = dot(d,n);
	float t = (qn - on) / dn;
	return t;

}