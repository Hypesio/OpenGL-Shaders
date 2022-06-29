#version 450

in vec3 TexCoords;
in vec4 frag_color;
in vec4 visual_space; 

in vec2 uv_coords;

layout(location = 0) out vec4 output_color;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform sampler2D refraction_depth_texture;

void main()
{    
    vec2 projective_coords = (visual_space.xy/visual_space.w) / 2.0 + 0.5;
    vec2 reflect_coords = vec2(projective_coords.x, -projective_coords.y);
    vec2 refract_coords = vec2(projective_coords.x, projective_coords.y);

    vec4 reflect_color = texture(reflection_texture, reflect_coords);
    vec4 refract_color = texture(refraction_texture, refract_coords);

    // Compute new depth 
    float far = 1000.0; 
    float near = 0.1;
    float depth_refract = texture(refraction_depth_texture, refract_coords).r;
    float depth_floor = (2.0 * near * far)/ (far + near - (depth_refract * 2.0 - 1.0) * far - near);

    float surface_distance = (2.0 * near * far)/ (far + near - (gl_FragCoord.z * 2.0 - 1.0) * far - near);
    float depth_inwater = depth_floor - surface_distance;
    float depth_strength = 2.0;
    float depth_value = depth_inwater * depth_strength;
    vec4 depth_color = vec4(0, 0, 0, 1);
    
    vec4 ref_color = mix(reflect_color, refract_color, 0.7);
    output_color = mix(ref_color, frag_color, 0.02);
    output_color = mix(output_color, depth_color, depth_value);
}