#version 450
in vec3 position;
in vec2 uv;

out vec3 TexCoords;
out vec4 frag_color;
out vec2 uv_coords;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;


void main()
{
    vec4 water_color = vec4(0.055, 0.97, 0.66, 1.0);
    frag_color = water_color;
    TexCoords = position;
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
    uv_coords = uv;
} 