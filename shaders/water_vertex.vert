#version 450
in vec3 position;

out vec3 TexCoords;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;

void main()
{
    TexCoords = position;
    gl_Position = projection_matrix * model_view_matrix * vec4(position, 1.0);
} 