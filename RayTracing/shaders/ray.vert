#version 430

layout(location = 0) in vec2 VertexPosition;
out vec3 Colour;

float fov = 60;
//calculate a direction vector from which px on screen we are rendering
vec3 directionFromPixel(vec2 pixel);

//determine intersection point for ray, and selected shape in selected shape index
float intersectSphere(vec3 ray, int sphere);
float intersectPlane(vec3 ray, int plane);
float intersectTriangle(vec3 ray, int triangle);

void main(){

	gl_Position = vec4(VertexPosition, 0.0, 1.0);

	vec3 dir = directionFromPixel(VertexPosition);
	Colour = dir;
}

vec3 directionFromPixel(vec2 pixel){
	float z = 1 / (2 * tan(fov/2.0));
	return(normalize(vec3(pixel.x, pixel.y, z)));
}