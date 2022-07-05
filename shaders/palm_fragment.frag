#version 450

in vec3 color;
in vec3 frag_normal;
in vec3 light_dir;

layout(location = 0) out vec4 output_color;

void main()
{
    vec3 light_intensity = vec3(1.0, 1.0, 1.0);
    vec3 diff = clamp(abs(dot(frag_normal, light_dir)) * light_intensity * color, 0.0, 1.0);
    output_color = vec4(diff, 1.0);
}