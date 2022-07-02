#version 450

in vec3 TexCoords;
in vec3 vertex_position;
in vec2 uv_pos;
in vec2 cloud_coords;

uniform samplerCube skybox;
uniform sampler2D cloud_tiny; 
uniform float time_passed; 

layout(location = 0) out vec4 output_color;

void main()
{    
    vec4 cloud_color = vec4(0.0, 0.0, 0.0, 1.0); 

    //output_color = texture(skybox, TexCoords);

    vec4 top_color = vec4(0.376, 0.659, 1.0, 1.0); 
    vec4 bottom_color = vec4(1.0, 0.98, 0.37, 1.0);

    float lerp_val = clamp(vertex_position.y, -0.8, 0.3) * 3.33; /// 10.0; 
    vec4 background_color = mix(bottom_color, top_color, lerp_val);

    output_color = background_color + cloud_color;
}