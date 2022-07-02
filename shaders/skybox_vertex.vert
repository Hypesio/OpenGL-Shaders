#version 450
in vec3 position;
in vec3 normalFlat;
in vec2 uv;

out vec3 TexCoords;
out vec3 vertex_position;
out vec2 uv_pos;
out vec2 cloud_coords;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

void main()
{
    TexCoords = position;
    vec4 pos = projection_matrix * model_view_matrix * vec4(position, 1.0);
    uv_pos = uv;
    vertex_position = position;
    cloud_coords = vec2(position.x / 2.0 + 0.5, position.z / 2.0 + 0.5) * 1;
    gl_Position = pos.xyww;
} 