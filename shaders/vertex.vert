#version 450

in vec3 position;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

out vec3 out_color;

void main()
{
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
    out_color = color;
}