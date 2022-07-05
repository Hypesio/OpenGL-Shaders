#version 450

in vec3 position;
in vec3 normal_flat;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform vec4 kd_color;
uniform vec3 light_pos;

out vec3 color;
out vec3 frag_normal;
out vec3 light_dir;

void main()
{
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
    frag_normal = normalize(normal_flat);
    light_dir = normalize(light_pos - position);
    color = kd_color.xyz;
}