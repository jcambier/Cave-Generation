#version 330 core

// Task 5: declare "in" variables for the world-space position and normal,
//         received post-interpolation from the vertex shader
layout(location = 0) in vec3 world_pos_in;
layout(location = 1) in vec3 world_norm_in;

// Task 10: declare an out vec4 for your output color
layout(location = 0) out vec4 fragColor;

// Task 12: declare relevant uniform(s) here, for ambient lighting
uniform float m_ka;
uniform vec3 c_ambient;
// Task 13: declare relevant uniform(s) here, for diffuse lighting
uniform float m_kd;
// Specifies the kind of light -- 0 -> directional, 1 -> point, 2 -> spotlight
uniform int light_type[8];
uniform vec4 m_lightDir[8];
uniform vec4 c_light[8];
uniform vec3 atten[8];
uniform vec4 m_lightPos[8];
uniform vec3 m_lightDirSpot[8];
uniform float m_angle[8];
uniform float m_penumbra[8];
uniform vec3 c_diffuse;
uniform int total_lights;

// Task 14: declare relevant uniform(s) here, for specular lighting
uniform float m_ks;
uniform float m_shin;
uniform vec4 cam_pos;
uniform vec3 c_specular;

vec4 directional(int index, vec4 norm) {
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    vec4 dir_to_light = -1.f*normalize(m_lightDir[index]);
            if(dot(norm, dir_to_light) > 0) {
                color = color + c_light[index]*(m_kd*dot(norm, dir_to_light)*vec4(c_diffuse,0.0));
            }
            vec3 light_to_pos = normalize(vec3(m_lightDir[index][0], m_lightDir[index][1], m_lightDir[index][2]));
            // Task 14: add specular component to output color
            vec3 r = normalize(reflect(light_to_pos, normalize(world_norm_in)));
            vec3 e = normalize(vec3(cam_pos[0],cam_pos[1],cam_pos[2]) - world_pos_in);
            if (dot(r,e) > 0) {
                color = color + c_light[index]*m_ks*pow(dot(r,e),m_shin);
            }
    return color;
}

vec4 point(int index, vec4 norm) {
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    vec3 dir_to_light = normalize(vec3(m_lightPos[index][0], m_lightPos[index][1], m_lightPos[index][2]) - world_pos_in);
    vec3 light_to_pos = normalize(-1.f*dir_to_light);
    float distance = distance(vec3(m_lightPos[index][0], m_lightPos[index][1], m_lightPos[index][2]), world_pos_in);
    float f_att = min(1.f,1.f / (atten[index][0]+distance*atten[index][1] + distance*distance*atten[index][2]));
    if(dot(vec3(norm[0], norm[1], norm[2]), dir_to_light) > 0) {
        color = color + f_att*c_light[index]*(m_kd*dot(vec3(norm[0], norm[1], norm[2]), dir_to_light)*vec4(c_diffuse,0.0));
    }
    vec3 v = normalize(vec3(cam_pos[0],cam_pos[1],cam_pos[2]) - world_pos_in);
    vec3 r = normalize(reflect(light_to_pos, normalize(world_norm_in)));
    if (dot(dir_to_light,vec3(norm[0], norm[1], norm[2])) >= 0) {
        color = color + f_att*c_light[index]*m_ks*vec4(c_specular, 1.0)*pow(dot(r,v), m_shin);
    }
    return color;
}

vec4 spot(int index, vec4 norm) {
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    vec3 dir_to_light = normalize(vec3(m_lightPos[index][0], m_lightPos[index][1], m_lightPos[index][2]) - world_pos_in);
    vec3 light_to_pos = normalize(-1.f*dir_to_light);
    float distance = distance(vec3(m_lightPos[index][0], m_lightPos[index][1], m_lightPos[index][2]), world_pos_in);
    float f_att = min(1.f,1.f / (atten[index][0]+distance*atten[index][1] + distance*distance*atten[index][2]));
    vec3 light_dir = vec3(m_lightDir[index][0], m_lightDir[index][1], m_lightDir[index][2]);
    float light_pos_angle = acos(dot(light_dir, light_to_pos) / (length(light_dir)*length(light_to_pos)));
    vec3 li = normalize(vec3(m_lightPos[index][0], m_lightPos[index][1], m_lightPos[index][2]) - world_pos_in);
    vec4 light_color;
    float theta_inner = m_angle[index] - m_penumbra[index];
    float theta_outer = m_angle[index];
    if (light_pos_angle <= theta_inner) {
        light_color = c_light[index];
    } else if (light_pos_angle >= theta_inner && light_pos_angle <= theta_outer) {
        float inner_term = ((light_pos_angle - theta_inner) / (theta_outer - theta_inner));
        float falloff = -2*pow(inner_term, 3) + 3*pow(inner_term,2);
        light_color = c_light[index]*(1.f-falloff);
    } else {
        light_color = vec4(0,0,0,1);
    }
    if (dot(vec3(norm[0], norm[1], norm[2]), li) >= 0) {
        vec3 diffuse =  f_att*vec3(light_color[0], light_color[1], light_color[2])*m_kd*(c_diffuse*dot(vec3(norm[0], norm[1], norm[2]), li));
        color = color + vec4(diffuse, 0.f);
    }
    vec3 v = normalize(vec3(cam_pos[0],cam_pos[1],cam_pos[2]) - world_pos_in);
    vec3 r = normalize(reflect(light_to_pos, normalize(world_norm_in)));
    if (dot(vec3(norm[0], norm[1], norm[2]),li) >=0) {
        vec3 specular =  f_att*vec3(light_color[0], light_color[1], light_color[2])*m_ks*c_specular*pow(dot(r,v), m_shin);
        color = color + vec4(specular, 0.f);
    }
    return color;
}

void main() {
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    color = color + m_ka*vec4(c_ambient,0.0);
    vec4 norm_4 = vec4(normalize(world_norm_in),0.0);
    for (int i = 0; i<total_lights; i++) {
        if (light_type[i] == 0) {
            color = color + directional(i, norm_4);
        } else if (light_type[i] == 1) {
            color = color + point(i, norm_4);
        } else if (light_type[i] == 2) {
            color = color + spot(i, norm_4);
        }
    }
    fragColor = color;
}
