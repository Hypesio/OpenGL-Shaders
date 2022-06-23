#version 450

in vec3 TexCoords;

uniform samplerCube skybox;

layout(location = 0) out vec4 output_color;

void main()
{    
    output_color = vec4(1,1,1,1);
    //FragColor = texture(skybox, TexCoords);
}