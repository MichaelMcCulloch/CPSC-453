#version 430


in vec3 Colour;

out vec4 FragmentColour;

void main(void)
{
    
    FragmentColour = vec4(Colour, 0);
}