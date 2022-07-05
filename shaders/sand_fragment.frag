#version 450

in vec3 out_color;
in vec3 view_dir;
in vec3 frag_light_dir;
in vec3 frag_normal;
in vec2 interpolated_uv;
in vec3 frag_tangent;

in float windward_coeff;
in float leeward_coeff;
in vec2 upwind_tex_coord;
in vec2 leeward_tex_coord;
in vec2 windspot_tex_coord;
in vec4 frag_pos_light_space;

uniform sampler2D normal_map;
uniform sampler2D dust_texture;
uniform sampler2D light_depth_texure;

layout(location = 0) out vec4 output_color;

vec3 CalcBumpedNormal()
{
    vec3 normal = normalize(frag_normal);
    vec3 tangent = normalize(frag_tangent);

    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(tangent, normal);

    vec3 bump_map_normal = texture(normal_map, interpolated_uv).xyz;

    bump_map_normal = bump_map_normal / 2.0 + vec3(0.5, 0.5, 0.5);

    vec3 result_normal;
    mat3 TBN = mat3(tangent, bitangent, normal);
    result_normal = TBN * bump_map_normal;
    result_normal = normalize(result_normal);

    return result_normal;
}

// Tell if actual fragment is in shadow
bool shadow()
{
    // For perspective light
    vec3 coords = (frag_pos_light_space.xyz / frag_pos_light_space.w) * 0.5 + 0.5;

    float depth_for_light = texture(light_depth_texure, coords.xy).r; 
    float real_depth = coords.z;

    if (real_depth > depth_for_light)
        return true;
    return false;
}  

void main()
{
    vec3 light = vec3(1.0, 1.0, 1.0); // Could be pass as uniform

    // Bump mapping
    vec3 normal = CalcBumpedNormal();

    float dust_opacity = 0.05;

    // Dust and wind
    vec4 windward = texture(dust_texture, upwind_tex_coord);
    vec4 leeward = texture(dust_texture, leeward_tex_coord);

    vec4 waves = windward * dust_opacity * windward_coeff;
    waves += leeward * dust_opacity * leeward_coeff;
    waves *= 1.0 - clamp(texture(dust_texture, windspot_tex_coord).r * 5.0, 0.0, 1.0);

    // Specular
    
    if (!shadow()) {
        float spec_strength = 0.3;
        vec3 reflect_dir = reflect(-frag_light_dir, normal);
        float spec = pow(abs(dot(view_dir, reflect_dir)), 25);
        vec3 specular = spec_strength * spec * light;

        // Diffuse
        vec3 diffuse = clamp(dot(normal, frag_light_dir), 0.0, 1.0) * light;
        vec3 final = (specular + diffuse) * out_color + waves.xyz * out_color;
        output_color = vec4(final, 1.0);
    }
    else {
        output_color = mix(vec4(out_color + waves.xyz * out_color, 1.0), vec4(0.0, 0.0, 0.0, 1.0), 0.7);
    }
    
}