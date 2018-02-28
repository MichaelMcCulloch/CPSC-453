#version 410
/**
* Tessellation Evaluation Shader
*	Determines positions of points in tessellated patches
*	Receives input from gl_in, tcs out variables and gl_TessCoord
*	Run once for every vertex in the output patch
*/

vec2 bezier2(vec2 a, vec2 b, float t) {
   return mix(a, b, t);
}
vec2 bezier3(vec2 a, vec2 b, vec2 c, float t) {
   return mix(bezier2(a, b, t), bezier2(b, c, t), t);
}
vec2 bezier4(vec2 a, vec2 b, vec2 c, vec2 d, float t) {
   return mix(bezier3(a, b, c, t), bezier3(b, c, d, t), t);
}

//Type of patch being output
layout(isolines) in;

in vec3 teColour[];
//in gl_in[];

//Information being sent out to fragment shader
//Will be interpolated as if sent from vertex shader
out vec3 Colour;

#define PI 3.14159265359




void main()
{
	//gl_TessCoord.x will parameterize the segments of the line from 0 to 1
	//gl_TessCoord.y will parameterize the number of lines from 0 to 1
	float u = gl_TessCoord.x;

	vec3 startColour = teColour[0];
	vec3 endColour = teColour[1];

	vec2 p0 = gl_in[0].gl_Position.xy;
	vec2 p1 = gl_in[1].gl_Position.xy;
	
    

	vec2 position = bezier2(p0, p1, u);

	gl_Position = vec4(position, 0, 1);

	Colour = mix(startColour, endColour, u);

}
