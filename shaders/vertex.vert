#version 450

in vec3 position;
in vec3 normalFlat;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

out vec3 out_color;

void main()
{
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
    vec3 normal = normalize(normalFlat);
    vec3 light_direction = normalize(light_pos - position);

    out_color = clamp(dot(normal, light_direction) * light_color, 0, 1) * color;
}