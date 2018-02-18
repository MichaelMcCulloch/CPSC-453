#version 410
out vec4 FragColor;

in vec2 TexCoord;

uniform mat3 kernel;
uniform vec3 greyscale;
uniform int usingKernel; //-1 no kernel, 0 kernel, 1 greyscale
uniform int kernelSize; //3 expected

const mat3 lowPassFilter = {
    {0.111111, 0.111111, 0.111111},
    {0.111111, 0.111111, 0.111111},
    {0.111111, 0.111111, 0.111111},
    
};

uniform sampler2DRect ourTexture;

void main()
{
    vec4 color = {0.0,0.0,0.0,1.0};
    if (usingKernel == 0){
        for(int j = 0; j < 3; j++){
            for (int k = 0; k < 3; k++){
                color += texelFetch(ourTexture, ivec2(TexCoord + vec2(j-1, k-1))) * kernel[j][k];
            }
        }
    } else if (usingKernel == 1) {
        vec4 tColor = texture(ourTexture, TexCoord);
        float gray = (greyscale.r * tColor.r + greyscale.g * tColor.g + greyscale.b * tColor.b);
        vec3 grayscaleConv = vec3(gray);
        color = vec4(grayscaleConv, tColor.a) ;
    } else {
		color = texture(ourTexture, TexCoord);
	}
	 
	
    
    FragColor = color;
    
} 