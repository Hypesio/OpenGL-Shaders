#version 450

layout(location = 0) out vec4 output_color;

void main()
{    
    gl_FragDepth = gl_FragCoord.z;
    output_color = mix(vec4(1.0,1.0,1.0,1.0), vec4(0.0,0.0,0.0,1.0), gl_FragCoord.z);
}