#version 450
in vec3 position;
in vec2 uv;

out vec3 TexCoords;
out vec4 frag_color;
out vec2 uv_coords;
out vec4 visual_space;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform vec4 clip_plane; 

void main()
{
    gl_ClipDistance[0] = dot(vec4(position, 1.0), clip_plane);
    vec4 water_color = vec4(0.055, 0.97, 0.66, 1.0);
    frag_color = water_color;
    TexCoords = position;
    visual_space =  projection_matrix * model_view_matrix * vec4(position, 1.0);
    gl_Position = visual_space;
    uv_coords = uv;
} 