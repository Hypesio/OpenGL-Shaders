#version 450

in vec3 position;
in vec3 normalFlat;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform vec3 cam_pos;
uniform vec4 clip_plane;

out vec3 out_color;
out vec3 frag_pos; 
out vec3 frag_normal;
out vec3 frag_light_pos;
out vec3 frag_cam_pos;

void main()
{
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
    vec3 normal = normalize(normalFlat);
    vec3 light_direction = normalize(light_pos - position);

    out_color = clamp(dot(normal, light_direction) * light_color, 0, 1) * color;

    frag_normal = normal;
    frag_pos = vec3(model_view_matrix * vec4(position, 1.0));
    frag_light_pos = light_pos;
    frag_cam_pos = cam_pos;

    vec4 world_pos = projection_matrix * vec4(position, 1);
    gl_ClipDistance[0] = dot(vec4(position, 1.0), clip_plane);
}