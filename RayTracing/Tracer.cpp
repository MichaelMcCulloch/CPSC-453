#include "Tracer.h"



int main(int argc, char *argv[]){
}

//Generate normalized Direction vectors
vector<vector<vec3>> generateRays(int w, int h) {

	vector<vector<vec3>> image = {};
	for (int y = 0; y < h; y++) {
		vector<vec3> row = {};
		for (int x = 0; x < w; x++) {
			float xD = ((((float)x / (float)(w - 1)) * 2.0f) - 1.0f) * 1.0;
			float yD = ((((float)y / (float)(y - 1)) * 2.0f) - 1.0f) * -1.0f; //Invert the y axis: as we travel 'up' the range, we should be travelling down the image
			row.push_back(normalize(vec3(xD, yD, -1)));
		}
		image.push_back(row);
	}
	return image;
}