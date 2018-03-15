#version 430

layout(location = 0) in vec2 VertexPosition;
out vec3 Colour;

struct Sphere {
	vec4 center;
};

layout(std140, binding = 1) uniform SphereData {
	vec4 sphere[2];
};

uniform vec3 origin;
/*
uniform vec4 spheres[2]; // xyz = center; w = radius
uniform mat3 triangles[2]; //each column will be a corner;
uniform mat3x2 planes[2]; //first column = norm; second column = point on plane
*/
float fov = 60;
const float pi = 3.1415926535897931;
//calculate a direction vector from which px on screen we are rendering
vec3 directionFromPixel(vec2 pixel);

//determine intersection point for ray, and selected shape in selected shape index
float intersectSphere(vec3, int);
float intersectPlane(vec3 ray, int plane);
float intersectTriangle(vec3 ray, int triangle);

void main(){

	gl_Position = vec4(VertexPosition, 0.0, 1.0);

	vec3 dir = directionFromPixel(VertexPosition);
	vec3 col;
	float min = -1;
	for (int i = 0; i < 2; i++){ //iterate over spheres
		float t = intersectSphere(dir, i);
		if (t < 0) continue;
		if (min < 0) min = t; //if min has not been set
		else if ( t < min) min = t; //if min has been set
	}

	if ( min > 0) col = vec3(1,1,1);
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
	vec3 center = sphere[i].xyz;
	vec3 oc = origin - center;
	float r = sphere[i].w;

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