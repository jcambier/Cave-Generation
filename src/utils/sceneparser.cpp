#include "sceneparser.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

glm::vec3 SceneParser::getWorldCoord(glm::vec3 origin, glm::vec3 xyz, int radius) {
    int world_x = origin[0] - radius + xyz[0];
    int world_y = origin[1] - radius + xyz[1];
    int world_z = origin[2] - radius + xyz[2];
    return glm::vec3(world_x, world_y, world_z);
}

bool SceneParser::inSphere(glm::vec3 origin, int radius, glm::vec3 xyz) {
    if (glm::distance(origin, xyz) < radius - 1.0) {
        return true;
    } else return false;
}

RenderShapeData SceneParser::createCube(glm::vec3 location) {
    RenderShapeData cube = RenderShapeData();
    cube.position = location;
    return cube;
}



void SceneParser::createSphere(glm::vec3 origin, int radius) {
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

void SceneParser::createLine(int radius) {
    int distance = glm::floor(glm::distance(point_1, point_2));
    glm::vec3 direction_line = glm::normalize(point_2 - point_1);
    for (int i = 0; i < distance; i++) {
        glm::vec3 sphere_center = point_1 + direction_line*float(i);
        createSphere(sphere_center, radius);
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

bool SceneParser::parse(RenderData &renderData, int radius) {
    SceneParser parser;
    parser.point_1 = glm::vec3(0.f,0.f,0.f);
    parser.point_2 = glm::vec3(90.f,-30.f,20.f);
    parser.createLine(radius);
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
    renderData.cameraData.pos = glm::vec4(3.f, 3.f, 3.f, 1.f);
    renderData.cameraData.look = glm::vec4(-3.f, -3.f, -3.f, 0.f);
    renderData.cameraData.up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    renderData.cameraData.heightAngle = 0.52f;
    renderData.globalData.kd = 0.5f;
    renderData.globalData.ka = 0.5f;
    renderData.globalData.ks = 0.5f;
    return true;
}
