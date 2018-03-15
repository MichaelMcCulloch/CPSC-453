#include "Tracer.h"



int main(int argc, char *argv[]){
}

//Generate normalized Direction vectors
vector<vec3> generateRays(int d, float fov) {

	float z = d / (2 * tan(fov / 2.0));
	vec3 topLeft = vec3((float)d / -2.0 + 0.5f, (float)d/2.0 - 0.5f, z);


	vector<vec3> dirs = {};
	for (int i = 0; i < d; i++) {
		for (int j = 0; j < d; j++) {
			dirs.push_back(normalize(topLeft + vec3(i, -j, 0)));
		}
	}
	return dirs;
}