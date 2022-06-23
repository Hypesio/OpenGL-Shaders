#version 450
in vec3 position;
in vec3 normalFlat;

out vec3 TexCoords;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

void main()
{
    TexCoords = position;
    vec4 pos = projection_matrix * model_view_matrix * vec4(position, 1.0);
    gl_Position = pos.xyww;
} 