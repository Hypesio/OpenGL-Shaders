#version 450
in vec3 position;
in vec3 normal_flat;
in vec2 uv;

out vec4 frag_color;
out vec2 uv_coords;
out vec4 visual_space;
out float reflection_force; 
out vec3 frag_pos; 
out vec3 frag_normal;
out vec3 frag_light_pos;
out vec3 frag_cam_pos;
out vec2 normal_map_coords1;
out vec2 normal_map_coords2;
out vec3 vertex_position;

uniform mat4 projection_matrix;
uniform mat4 model_view_matrix;
uniform vec4 clip_plane; 
uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform float time_passed;

float normal_map_tilling1 = 0.3;
float normal_map_tilling2 = 0.44;
float normal_map_speed1 = -0.2;
float normal_map_speed2 = -0.5;

void main()
{
    gl_ClipDistance[0] = dot(vec4(position, 1.0), clip_plane);
    vec4 water_color = vec4(0.008, 0.231, 0.270, 1.0);
    frag_color = water_color;

    visual_space =  projection_matrix * model_view_matrix * vec4(position, 1.0);
    gl_Position = visual_space;

    frag_pos = vec3(model_view_matrix * vec4(position, 1.0));
    frag_normal = normalize(normal_flat);
    frag_pos = vec3(model_view_matrix * vec4(position, 1.0));
    frag_light_pos = light_pos;
    frag_cam_pos = cam_pos;
    vertex_position = position;

    // Normal map 
    float speed1 = time_passed * normal_map_speed1;
    float speed2 = time_passed * normal_map_speed2;
    normal_map_coords1 = vec2((position.z + speed1) / 2.0 + 0.5, (-position.x + speed1) / 2.0 + 0.5) * normal_map_tilling1;
    normal_map_coords2 = vec2((position.z + speed2) / 2.0 + 0.5, (-position.x + speed2) / 2.0 + 0.5) * normal_map_tilling2;

    // Reflection force - Schilck approximation
    float reflect_alpha = 0.25;
    reflection_force = 1 - pow(dot(normal_flat, normalize(cam_pos - position)), reflect_alpha);

    uv_coords = uv;
} 