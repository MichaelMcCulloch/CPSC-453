#version 430

layout(location = 0) in vec2 VertexPosition;
out vec2 vPos;


void main()
{

    vPos = VertexPosition;
    gl_Position = vec4(VertexPosition, 0.0, 1.0);
}
