#version 450

in vec3 position;
in vec2 uv;
in vec3 normal_flat;
in vec3 tangent;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 camera_pos;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform vec4 clip_plane;

out vec3 out_color;
out vec3 frag_light_dir;
out vec3 frag_normal;
out vec2 interpolated_uv;
out vec3 frag_tangent;
out vec3 view_dir;

void main()
{
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);

    vec4 world_pos = projection_matrix * vec4(position, 1.0);
    gl_ClipDistance[0] = dot(vec4(position, 1.0), clip_plane);

    out_color = color;
    frag_light_dir = normalize(light_pos - position);
    interpolated_uv = uv;
    frag_normal = normal_flat;
    frag_tangent = tangent;
    view_dir = normalize(camera_pos - position);
}