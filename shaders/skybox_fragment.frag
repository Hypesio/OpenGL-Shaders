#version 450

in vec3 TexCoords;
in vec3 vertex_position;
in vec2 uv_pos;
in vec2 cloud_coords;

uniform samplerCube skybox;
uniform sampler2D cloud_tiny; 
uniform float time_passed; 

layout(location = 0) out vec4 output_color;

vec4 HSVtoRGB(vec4 color) {
    float c = color.y * color.z; 
    float a = mod((color.x / 60),  2);
    float x = c * (1 - abs(a - 1));
    float m = color.z - c; 

    float r = 0;
    float g = 0;
    float b = 0;
    if (color.x < 60) {
        r = c; 
        g = x;
    }
    else if (color.x <= 120) {
        r = x;
        g = c;
    } 
    else if (color.x <= 180) {
        g = c; 
        b = x;
    }
    else if (color.x <= 240) {
        g = x;
        b = c;
    }
    else if (color.x <= 300) {
        r = x;
        b = c;
    }
    else {
        r = c;
        b = x;
    }
    return vec4((r + m), (g + m), (b + m), 1.0);
}

void main()
{    
    vec4 cloud_color = vec4(0.0, 0.0, 0.0, 1.0); 

    //output_color = texture(skybox, TexCoords);

    vec4 bottom_color = vec4(38.5, 0.155, 0.988, 1.0); 
    vec4 top_color = vec4(191.9, 0.671, 1.0, 1.0);
    //vec4 top_color = vec4(0.376, 0.659, 1.0, 1.0); 
    //vec4 bottom_color = vec4(1.0, 0.98, 0.37, 1.0);
    vec4 background_color = vec4(bottom_color);

    float lerp_val = clamp(clamp(vertex_position.y, -0.5, 1), 0.0, 1.0); 
    background_color = mix(HSVtoRGB(bottom_color), HSVtoRGB(top_color), lerp_val);
    //background_color = HSVtoRGB(background_color);

    output_color = background_color + cloud_color;
}