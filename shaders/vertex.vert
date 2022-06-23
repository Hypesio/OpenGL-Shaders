#version 450

in vec3 position;
in vec3 normalFlat;
in vec2 uv;

uniform vec3 color;
uniform vec3 light_pos;
uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform vec3 cam_pos;
uniform vec4 clip_plane;

out vec3 out_color;
out vec3 frag_pos; 
out vec3 frag_normal;
out vec3 frag_light_dir;
out vec3 view_dir;
out vec2 interpolated_uv;

void main()
{
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);

    vec4 world_pos = projection_matrix * vec4(position, 1.0);
    gl_ClipDistance[0] = dot(vec4(position, 1.0), clip_plane);
    
    out_color = color;
    view_dir = normalize(cam_pos - position);
    frag_normal = normalize(normalFlat);
    frag_light_dir = normalize(light_pos - position);
    interpolated_uv = uv;
}