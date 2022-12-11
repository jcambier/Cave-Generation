#include "sceneparser.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

glm::vec3 SceneParser::getWorldCoord(glm::vec3 origin, glm::vec3 xyz, float radius) {
    int world_x = origin[0] - radius + xyz[0];
    int world_y = origin[1] - radius + xyz[1];
    int world_z = origin[2] - radius + xyz[2];
    return glm::vec3(world_x, world_y, world_z);
}

bool SceneParser::inSphere(glm::vec3 origin, float radius, glm::vec3 xyz) {
    if (glm::distance(origin, xyz) < radius - 1.0) {
        return true;
    } else return false;
}

RenderShapeData SceneParser::createCube(glm::vec3 location) {
    RenderShapeData cube = RenderShapeData();
    cube.position = location;
    return cube;
}



void SceneParser::createSphere(glm::vec3 origin, float radius) {
    for (int x = 0; x < radius*2 + 1; x++) {
        for (int y = 0; y < radius*2 + 1; y++) {
            for (int z = 0; z < radius*2 + 1; z++) {
                glm::vec3 index_coord = getWorldCoord(origin, glm::vec3(x,y,z), radius);
                std::vector<int> index_coord_std = std::vector<int>{int(index_coord[0]), int(index_coord[1]),int(index_coord[2])};
                if (!inSphere(origin, radius, index_coord)) {
                    if (glm::distance(origin, index_coord) < radius + 1 and !isBlock.contains(index_coord_std)) {
                        cube_data.push_back(createCube(index_coord));
                        isBlock.insert({index_coord_std, true});
                    }
                } else isCovered.insert({index_coord_std, true});
            }
        }
    }
}

void SceneParser::createLine(float radius, glm::vec3 point_1, glm::vec3 point_2) {
    int distance = glm::floor(glm::distance(point_1, point_2));
    glm::vec3 direction_line = glm::normalize(point_2 - point_1);
    glm::vec3 prev_point = point_1;
    for (int i = 0; i < distance; i++) {
//        float input_radius = radius + sin(i) + sin(float(i) / 2) + sin(9*float(i)) + sin(90*float(i));
        glm::vec3 sphere_center = point_1 + direction_line*float(i);
//        sphere_center[0] = sphere_center[0] + sin(i) + sin(10*float(i)) + sin(3*float(i)) + sin(float(i) / 2) + sin(9*float(i)) + sin(90*float(i)) + sin(8*float(i));
//        sphere_center[1] = sphere_center[1] - sin(i) + sin(10*float(i)) + sin(3*float(i)) + sin(float(i) / 2) + sin(9*float(i)) + sin(90*float(i)) + sin(8*float(i));
//        sphere_center[2] = sphere_center[2] + sin(i) + sin(10*float(i)) - sin(3*float(i)) - sin(float(i) / 2) + sin(9*float(i)) + sin(90*float(i)) + sin(8*float(i));



        float perlinRadius = getRadius(sphere_center, radius);
        glm::vec3 perlinPos = getPerlinPos(sphere_center);
        if(perlinRadius < 3) {
            perlinRadius = 3;
        }
        glm::vec3 perlinPosDir = glm::normalize(prev_point - perlinPos);
        for (int j = 0; j < glm::length(perlinPos - prev_point); j++) {
            createSphere(prev_point + float(j)*perlinPosDir, perlinRadius);
        }
        prev_point = perlinPos;
    }
}

std::vector<SceneLightData> SceneParser::getLights() {
    glm::vec3 direction_line = glm::normalize(point_2 - point_1);
    int distance = glm::floor(glm::distance(point_1, point_2));
    std::vector<SceneLightData> lights_vec;
    for (int i = 0; i < distance; i += 30) {
        SceneLightData light = SceneLightData();
        light.color = glm::vec4(1.f,1.f,1.f,1.f);
        light.function = glm::vec3(1.f, 0.f, 0.f);
        light.dir = glm::vec4(-3.f,-2.f,-1.f,0);
        light.pos = glm::vec4(point_1 + float(i)*direction_line, 1.0);
        light.type = LightType::LIGHT_POINT;
        lights_vec.push_back(light);
    }

    return lights_vec;
}

bool SceneParser::parse(RenderData &renderData, float radius) {
    SceneParser parser;
    glm::vec3 point_1 = glm::vec3(0.f,0.f,0.f);
    glm::vec3 point_2 = glm::vec3(90.f,-30.f,20.f);

    parser.m_lookupSize = 1024;
    parser.m_randVecLookup.reserve(parser.m_lookupSize);
    std::srand(1230);
    for (int i = 0; i < parser.m_lookupSize; i++) {
        parser.m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }

    parser.createLine(radius, point_1, point_2);
    std::vector<RenderShapeData> shape_data;
    for (RenderShapeData& cube: parser.cube_data) {
        glm::vec3 pos = cube.position;
        if (!parser.isCovered.contains(std::vector<int>{int(pos[0]), int(pos[1]), int(pos[2])})) {
            shape_data.push_back(cube);
        }
    }

    renderData.material.cDiffuse = glm::vec4(0.3, 0.3, 0.3, 1.0);
    renderData.material.cSpecular = glm::vec4(0.15, 0.15, 0.15, 1.0);
    renderData.material.cAmbient = glm::vec4(0.05, 0.05, 0.05, 1.0);
    renderData.material.shininess = 25;
    renderData.lights = parser.getLights();
    renderData.shapes = shape_data;
    renderData.cameraData.heightAngle = 0.52f;
    renderData.globalData.kd = 0.5f;
    renderData.globalData.ka = 0.5f;
    renderData.globalData.ks = 0.5f;
    return true;
}

float SceneParser::interpolate(float A, float B, float alpha) {
    float alpha2 = alpha*alpha;
    float alpha3 = alpha*alpha*alpha;
    float inter = A + (3*alpha2 - 2*alpha3)*(B-A);
    return inter;
}

glm::vec3 SceneParser::sampleRandomVector() {
//    std::hash<int> intHash;
//    int index = intHash(row*41 + col*43) % m_lookupSize;
//    return m_randVecLookup.at(index);
    return glm::vec3(std::rand() * 2.0 / RAND_MAX - 1.0,
                         std::rand() * 2.0 / RAND_MAX - 1.0,
                         std::rand() * 2.0 / RAND_MAX - 1.0);
}



float SceneParser::getRadius(glm::vec3 posVec, float radius) {
    float r = 1.15*(computePerlin3D(posVec))*radius;
    return r;
}

glm::vec3 SceneParser::getPerlinPos(glm::vec3 posVec) {
    float x = 4*computePerlin3D(glm::vec3(64*posVec[0], 64*posVec[1], 64*posVec[2])) + posVec.x;
    float y = 2*computePerlin3D(8.f*posVec) +posVec.y;
    float z = 2*computePerlin3D(3.f*posVec) +posVec.z;

    return glm::vec3(x, y, z);
}

float SceneParser::computePerlin3D(glm::vec3 posVec) {
    int x = posVec.x;
    int y = posVec.y;
    int z = posVec.z;
    int x_1 = floor(posVec.x);
    int y_1 = floor(posVec.y);
    int z_1 = floor(posVec.z);
    int x_2 = x_1 + 1;
    int y_2 = y_1 + 1;
    int z_2 = z_1 + 1;
    glm::vec3 p1 = glm::vec3(x_1 - x, y_1 - y, z_1 - z);
    glm::vec3 p2 = glm::vec3(x_1 - x, y_2 - y, z_1 - z);
    glm::vec3 p3 = glm::vec3(x_1 - x, y_1 - y, z_2 - z);
    glm::vec3 p4 = glm::vec3(x_1 - x, y_2 - y, z_2 - z);
    glm::vec3 p5 = glm::vec3(x_2 - x, y_1 - y, z_1 - z);
    glm::vec3 p6 = glm::vec3(x_1 - x, y_2 - y, z_1 - z);
    glm::vec3 p7 = glm::vec3(x_1 - x, y_1 - y, z_2 - z);
    glm::vec3 p8 = glm::vec3(x_1 - x, y_2 - y, z_2 - z);
    float dot1 = glm::dot(p1, sampleRandomVector());
    float dot2 = glm::dot(p1, sampleRandomVector());
    float dot3 = glm::dot(p1, sampleRandomVector());
    float dot4 = glm::dot(p1, sampleRandomVector());
    float dot5 = glm::dot(p1, sampleRandomVector());
    float dot6 = glm::dot(p1, sampleRandomVector());
    float dot7 = glm::dot(p1, sampleRandomVector());
    float dot8 = glm::dot(p1, sampleRandomVector());
    float merge1 = interpolate(dot1, dot2, posVec.y - y_1);
    float merge2 = interpolate(merge1, dot3, posVec.z - z_1);
    float merge3 = interpolate(merge2, dot4, posVec.y - y_1);
    float merge4 = interpolate(merge3, dot5, posVec.x - x_1);
    float merge5 = interpolate(merge4, dot6, posVec.y - y_1);
    float merge6 = interpolate(merge5, dot7, posVec.z - z_1);
    float perlin = interpolate(merge6, dot8, posVec.x - x_1);
    return perlin;
}
