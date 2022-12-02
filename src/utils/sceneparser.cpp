#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>
#include <algorithm>

glm::mat4 applyTransformations(std::vector<SceneTransformation*> transformations) {
    glm::mat4 result = glm::mat4(1.0f);
    for (int i = 0; i < transformations.size(); i++) {
        int index = transformations.size() - i - 1;
        glm::mat4 tranMat;
        SceneTransformation* transformation = transformations[index];
        if (transformation->type == TransformationType::TRANSFORMATION_ROTATE) {
            tranMat = glm::rotate(transformation->angle, transformation->rotate);
        } else if (transformation->type == TransformationType::TRANSFORMATION_TRANSLATE) {
            tranMat = glm::translate(transformation->translate);
        } else if (transformation->type == TransformationType::TRANSFORMATION_SCALE) {
            tranMat = glm::scale(transformation->scale);
        } else if (transformation->type == TransformationType::TRANSFORMATION_MATRIX) {
            tranMat = transformation->matrix;
        }
        result = tranMat*result;
    }
    return result;
}

std::vector<RenderShapeData> getData(SceneNode* root, glm::mat4 transformation) {
    std::vector<RenderShapeData> data;
    for (SceneNode* child: root->children) {
        glm::mat4 tranMat = applyTransformations(child->transformations);
        std::vector<RenderShapeData> childData = getData(child, transformation*tranMat);
        data.insert(data.end(), childData.begin(), childData.end());
    }
    for (ScenePrimitive* leaf: root->primitives) {
        data.resize(data.size() + 1);
        RenderShapeData leafData = RenderShapeData();
        leafData.ctm = transformation;
        leafData.primitive = ScenePrimitive();
        leafData.primitive.material = leaf->material;
        leafData.primitive.meshfile = leaf->meshfile;
        leafData.primitive.type = leaf->type;
        data.push_back(leafData);
    }
    return data;
}

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
    cube.ctm = glm::translate(location);
    cube.primitive.material.cDiffuse = glm::vec4(1.0, 0, 0, 1.0);
    cube.primitive.material.cSpecular = glm::vec4(1.0, 1.0, 1.0, 1.0);
    cube.primitive.material.cAmbient = glm::vec4(0.5, 0, 0, 1.0);
    cube.primitive.material.shininess = 25;
    cube.primitive.position = location;
    return cube;
}

void SceneParser::createSphere(glm::vec3 origin, int radius) {
    for (int x = 0; x < radius*2 + 1; x++) {
        for (int y = 0; y < radius*2 + 1; y++) {
            for (int z = 0; z < radius*2 + 1; z++) {
                glm::vec3 index_coord = getWorldCoord(origin, glm::vec3(x,y,z), radius);
                std::vector<int> index_coord_std = std::vector<int>{int(index_coord[0]), int(index_coord[1]),int(index_coord[2])};
                if (!inSphere(origin, radius, index_coord)) {
                    if (glm::distance(origin, index_coord) < radius + 1) {
                        cube_data.push_back(createCube(index_coord));
                    }
                } else isCovered.insert({index_coord_std, true});
            }
        }
    }
}

void SceneParser::createLine(glm::vec3 point_1, glm::vec3 point_2, int radius) {
    int distance = glm::floor(glm::distance(point_1, point_2));
    glm::vec3 direction_line = glm::normalize(point_2 - point_1);
    for (int i = 0; i < distance; i++) {
        glm::vec3 sphere_center = point_1 + direction_line*float(i);
        createSphere(sphere_center, radius);
    }
}

std::vector<SceneLightData> getLights() {
    SceneLightData light = SceneLightData();
    light.color = glm::vec4(1.f,1.f,1.f,1.f);
    light.function = glm::vec3(1.f, 0.f, 0.f);
    light.dir = glm::vec4(-3.f,-2.f,-1.f,0);
    light.pos = glm::vec4(0.f,0.f,0.f,1.f);
    light.type = LightType::LIGHT_POINT;
    return std::vector<SceneLightData>{light};
}

bool SceneParser::parse(RenderData &renderData, int radius) {
    SceneParser parser;
    glm::vec3 origin = glm::vec3(0.f,0.f,0.f);
    glm::vec3 point_2 = glm::vec3(30.f,-4.f,0.f);
    parser.createLine(origin, point_2, radius);
    std::vector<RenderShapeData> shape_data;
    for (RenderShapeData& cube: parser.cube_data) {
        glm::vec3 pos = cube.primitive.position;
        if (!parser.isCovered.contains(std::vector<int>{int(pos[0]), int(pos[1]), int(pos[2])})) {
            shape_data.push_back(cube);
        }
    }
    renderData.shapes = shape_data;
    renderData.lights = getLights();
    renderData.cameraData.pos = glm::vec4(3.f, 3.f, 3.f, 1.f);
    renderData.cameraData.look = glm::vec4(-3.f, -3.f, -3.f, 0.f);
    renderData.cameraData.up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    renderData.cameraData.heightAngle = 0.52f;
    renderData.globalData.kd = 0.5f;
    renderData.globalData.ka = 0.5f;
    renderData.globalData.ks = 0.5f;
    return true;
}
