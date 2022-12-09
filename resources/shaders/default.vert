#version 330 core

// Task 4: declare a vec3 object-space position variable, using
//         the `layout` and `in` keywords.
layout(location = 0) in vec3 obj_pos;
layout(location = 1) in vec3 obj_norm;
layout(location = 2) in vec2 obj_uv; /* Note: added for texture */

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
layout(location = 0) out vec3 world_pos_out;
layout(location = 1) out vec3 world_norm_out;
out vec2 uvCoords; /* Note: added for texture */

// Task 6: declare a uniform mat4 to store model matrix
uniform mat4 model_matrix;
uniform mat3 model_trans;

// Task 7: declare uniform mat4's for the view and projection matrix
uniform mat4 view_matrix;
uniform mat4 proj_matrix;

void main() {
    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5
    vec4 obj_pos4 = vec4(obj_pos[0], obj_pos[1], obj_pos[2], 1);
    vec4 world_pos_4 = model_matrix*obj_pos4;
    world_pos_out = vec3(world_pos_4[0], world_pos_4[1], world_pos_4[2]);
    world_norm_out = model_trans*obj_norm;
    uvCoords = obj_uv;

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    // Task 9: set gl_Position to the object space position transformed to clip space
    vec4 pos_4 = proj_matrix*view_matrix*model_matrix*obj_pos4;
    gl_Position = pos_4;
}
