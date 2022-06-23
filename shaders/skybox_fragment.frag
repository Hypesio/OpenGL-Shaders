#version 450

in vec3 TexCoords;

uniform samplerCube skybox;

layout(location = 0) out vec4 output_color;

void main()
{    
    output_color = texture(skybox, TexCoords);
}