#version 450

in vec3 TexCoords;
in vec4 frag_color;

in vec2 uv_coords;

layout(location = 0) out vec4 output_color;

uniform sampler2D renderedTexture;

void main()
{    
    output_color = texture(renderedTexture, uv_coords);
}