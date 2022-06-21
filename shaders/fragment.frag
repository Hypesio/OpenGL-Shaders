#version 450

in vec3 out_color;
in vec3 frag_pos; 
in vec3 frag_normal;
in vec3 frag_light_pos;
in vec3 frag_cam_pos;

layout(location = 0) out vec4 output_color;

void main()
{
    vec3 light_color = vec3(1.0,1.0,1.0); // Could be pass as uniform
    vec3 light_dir = normalize(frag_light_pos - frag_pos);
    
    // Diffuse
    float diff = max(dot(frag_normal, light_dir), 0.0);
    vec3 diffuse = diff * light_color; 

    // Specular 
    float spec_strength = 0.3; 
    vec3 view_dir = normalize(frag_cam_pos - frag_pos);
    vec3 reflect_dir = reflect(-light_dir, frag_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = spec_strength * spec * light_color;

    vec3 final = (diffuse + specular) * out_color;
    output_color = vec4(final, 1.0);
}