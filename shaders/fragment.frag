#version 450

in vec3 out_color;
in vec3 frag_pos; 
in vec3 frag_normal;
in vec3 frag_light_dir;
in vec3 view_dir;
in vec2 interpolated_uv;

uniform sampler2D normalNoise;

layout(location = 0) out vec4 output_color;

void main()
{
    vec3 light = vec3(1.0, 1.0, 1.0); // Could be pass as uniform
    
    // Specular 
    float spec_strength = 0.3; 
    vec3 reflect_dir = reflect(-frag_light_dir, frag_normal);
    float spec = pow(abs(dot(view_dir, reflect_dir)), 32);
    vec3 specular = spec_strength * spec * light;

    // Sparkling sand
    vec4 sparkling = vec4(frag_normal, 1.0) * texture(normalNoise, interpolated_uv);
    sparkling.y *= 0.3;
    // Diffuse
    vec3 diffuse = clamp(dot(sparkling.xyz, frag_light_dir), 0.0, 1.0) * light;

    vec3 final = (diffuse + specular) * out_color;

    output_color = vec4(final, 1.0);
}