#version 450

in vec3 TexCoords;
in vec4 frag_color;
in vec4 visual_space; 

in vec2 uv_coords;

layout(location = 0) out vec4 output_color;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;

void main()
{    
    vec2 projective_coords = (visual_space.xy/visual_space.w) / 2.0 + 0.5;
    vec2 reflect_coords = vec2(projective_coords.x, -projective_coords.y);
    vec2 refract_coords = vec2(projective_coords.x, projective_coords.y);

    vec4 reflect_color = texture(reflection_texture, reflect_coords);
    vec4 refract_color = texture(refraction_texture, refract_coords);
    
    //output_color = refract_color;//mix(frag_color, refract_color, 0.8);
    vec4 ref_color = mix(reflect_color, refract_color, 0.5);
    output_color = mix(ref_color, frag_color, 0.02);

}