#include "Tracer.h"



int main(int argc, char *argv[]){
}

//Generate normalized Direction vectors
vector<vector<vec3>> generateRays(int d, float fov) {

	float z = d / (2 * tan(fov / 2.0));
	vec3 topLeft = vec3((float)d / -2.0 + 0.5f, (float)d/2.0 - 0.5f, z);

	vector<vector<vec3>> image = {};
	for (int i = 0; i < d; i++) {
		vector<vec3> row = {};
		for (int j = 0; j < d; j++) {
			row.push_back(normalize(topLeft + vec3(i, -j, 0)));
		}
		image.push_back(row);
	}
	return image;
}