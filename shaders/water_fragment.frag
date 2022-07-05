#version 450

in vec4 frag_color;
in vec4 visual_space; 
in float reflection_force; 
in vec3 frag_normal;
in vec3 frag_light_dir;
in vec3 frag_cam_pos;
in vec3 frag_pos;
in vec2 normal_map_coords1;
in vec2 normal_map_coords2;
in vec3 vertex_position;
in vec4 frag_pos_light_space;

in vec2 uv_coords;

layout(location = 0) out vec4 output_color;

uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform sampler2D refraction_depth_texture;
uniform sampler2D normal_map;
uniform sampler2D light_depth_texure;

float distortion_strength = 0.01;

vec4 lighting(vec3 distort_normal) {
    vec3 light_color = vec3(1.0,1.0,1.0); // Could be pass as uniform
    vec3 light_dir = frag_light_dir;

    // Specular 
    float spec_strength = 0.2; 
    vec3 view_dir = normalize(frag_cam_pos - vertex_position);
    vec3 reflect_dir = reflect(-light_dir, distort_normal);
    float spec = pow(max(dot(reflect_dir, view_dir), 0.0), 20);
    vec3 specular = spec_strength * spec * light_color;

    return vec4(specular, 0.0);
}

// Tell if actual fragment is in shadow
bool shadow()
{
    // For perspective light
    vec3 coords = (frag_pos_light_space.xyz / frag_pos_light_space.w) * 0.5 + 0.5;

    float depth_for_light = texture(light_depth_texure, coords.xy).r; 
    float real_depth = coords.z;

    if (real_depth > depth_for_light)//real_depth > depth_for_light)
        return true;
    return false;
}  

void main()
{    
    vec2 distort_1 = (texture(normal_map, normal_map_coords1).rg * 2.0 - 1.0) * distortion_strength;
    vec2 distort_2 = (texture(normal_map, normal_map_coords2).rg * 2.0 - 1.0) * distortion_strength;
    vec2 distort_coords = (distort_1 + distort_2) / 2.0f;
    vec4 normal_pixel = (texture(normal_map, normal_map_coords1) + texture(normal_map, vec2(normal_map_coords2.y, normal_map_coords2.x))) / 2.0;
    vec3 distort_normal = normalize(vec3(normal_pixel.r * 2.0 - 1.0, normal_pixel.b, normal_pixel.g * 2.0 - 1.0));

    vec2 projective_coords = (visual_space.xy/visual_space.w) / 2.0 + 0.5;
    vec2 reflect_coords = vec2(projective_coords.x, -projective_coords.y) + distort_coords;
    
    reflect_coords.x = clamp(reflect_coords.x, 0.01, 0.99); // Avoid glitch at screen borders
    reflect_coords.y = clamp(reflect_coords.y, -0.99, 0.01);

    vec4 reflect_color = texture(reflection_texture, reflect_coords);

    vec2 refract_coords = vec2(projective_coords.x, projective_coords.y) + distort_coords;
    refract_coords = clamp(refract_coords, 0.01, 0.99);
    vec4 refract_color = texture(refraction_texture, refract_coords);

    // Compute new depth 
    float far = 1000.0; 
    float near = 0.1;
    float depth_refract = texture(refraction_depth_texture, projective_coords).r;
    float depth_floor = (2.0 * near * far)/ (far + near - (depth_refract * 2.0 - 1.0) * far - near);

    float surface_distance = (2.0 * near * far)/ (far + near - (gl_FragCoord.z * 2.0 - 1.0) * far - near);
    float max_depth = 20.0;
    float depth_inwater = (depth_floor - surface_distance) / max_depth;
    float depth_strength = 1.0;
    float depth_value = clamp(depth_inwater * depth_strength, 0.0, 1.0);
    vec4 depth_color = frag_color;
    
    // Avoid visual artefact at the border of the lac
    if (texture(refraction_depth_texture, refract_coords).r >= 0.999) { 
        refract_color = texture(refraction_texture, refract_coords - distort_coords);
        reflect_color = texture(reflection_texture, reflect_coords - distort_coords);
    }

    // Compute final color
    //refract_color = vec4(1.0, 1.0, 1.0, 1.0);
    output_color = mix(refract_color, depth_color, depth_value);
    output_color = mix(reflect_color, output_color, 1 - reflection_force);

    if (shadow()) {
        output_color = output_color * 0.5;
    }
    else {
        output_color = lighting(distort_normal) + output_color;
    }
}