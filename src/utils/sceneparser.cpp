#include "sceneparser.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

//float x_coords[] = {0.0,           0.55959957,   1.21911198,   1.97503153,   2.82385249,
//        3.76206915,   4.7861758,    5.89266673,   7.07803622,   8.33877855,
//        9.67138802,  11.07235891,  12.5381855,   14.06536209,  15.65038295,
//       17.28974238,  18.97993466,  20.71745407,  22.49879491,  24.32045146,
//       26.178918,  28.07068882,  29.99225822,  31.94012046,  33.91076985,
//       35.90070066,  37.90640718,  39.9243837,   41.95112451,  43.98312388,
//       46.01687612,  48.04887549,  50.0756163,   52.09359282,  54.09929934,
//       56.08923015,  58.05987954,  60.00774178,  61.92931118,  63.821082,
//       65.67954854,  67.50120509,  69.28254593,  71.02006534,  72.71025762,
//       74.34961705,  75.93463791,  77.4618145,   78.92764109,  80.32861198,
//       81.66122145,  82.92196378,  84.10733327,  85.2138242,   86.23793085,
//       87.17614751,  88.02496847,  88.78088802,  89.44040043,  90.};

//float[] y_coords = {  0.          -2.50790003  -4.94685435  -7.31686297  -9.61792588
//        -11.85004309 -14.01321459 -16.10744039 -18.13272048 -20.08905487
//        -21.97644355 -23.79488653 -25.5443838  -27.22493536 -28.83654122
//        -30.37920138 -31.85291583 -33.25768457 -34.59350761 -35.86038495
//        -37.05831658 -38.1873025  -39.24734272 -40.23843723 -41.16058604
//        -42.01378914 -42.79804654 -43.51335823 -44.15972422 -44.7371445
//        -45.24561907 -45.68514795 -46.05573111 -46.35736857 -46.59006033
//        -46.75380638 -46.84860672 -46.87446136 -46.8313703  -46.71933352
//        -46.53835105 -46.28842287 -45.96954898 -45.58172939 -45.12496409
//        -44.59925309 -44.00459638 -43.34099397 -42.60844585 -41.80695203
//        -40.9365125  -39.99712726 -38.98879632 -37.91151968 -36.76529733
//        -35.55012927 -34.26601551 -32.91295605 -31.49095088 -30.
//}

//        float[] z_coords = {0.           1.48865269   2.90491238   4.25050273   5.52714737
//        6.73656995   7.88049411   8.96064349   9.97874174  10.9365125
//       11.8356794   12.6779661   13.46509624  14.19879345  14.88078138
//       15.51278368  16.09652399  16.63372594  17.12611319  17.57540937
//       17.98333812  18.3516231   18.68198793  18.97615628  19.23585177
//       19.46279805  19.65871876  19.82533755  19.96437805  20.07756392
//       20.16661879  20.2332663   20.27923011  20.30623384  20.31600115
//       20.31025567  20.29072106  20.25912094  20.21717897  20.16661879
//       20.10916403  20.04653835  19.98046538  19.91266877  19.84487216
//       19.7787992   19.71617351  19.65871876  19.60815858  19.5662166
//       19.53461649  19.51508187  19.5093364   19.51910371  19.54610744
//       19.59207124  19.65871876  19.74777363  19.86095949  20.
//}

float x_coords[] = {0.0,          4.7861758,  12.5381855,  22.49879491, 33.91076985, 46.01687612,
        58.05987954, 69.28254593, 78.92764109, 86.23793085, 90.0};

float y_coords[] = {0.,         -14.01321459, -25.5443838,  -34.59350761, -41.16058604,
                    -45.24561907, -46.84860672, -45.96954898, -42.60844585, -36.76529733, -30.
};

float z_coords[] = {0.,          7.88049411, 13.46509624, 17.12611319, 19.23585177, 20.16661879,
 20.29072106, 19.98046538, 19.60815858, 19.54610744, 20.0};






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
        light.pos = glm::vec4(point_1 + float(i)*direction_line, 1.0);
        light.type = LightType::LIGHT_POINT;
        lights_vec.push_back(light);
    }

    return lights_vec;
}

bool SceneParser::parse(RenderData &renderData, float radius, float seed) {
    SceneParser parser;
    glm::vec3 point_1 = glm::vec3(0.f,0.f,0.f);
    glm::vec3 point_2 = glm::vec3(10.f,-30.f,20.f);
    glm::vec3 point_3 = glm::vec3(50.f,-50.f,25.f);

    parser.m_lookupSize = 1024;
    parser.m_randVecLookup.reserve(parser.m_lookupSize);
    std::srand(seed);
    for (int i = 0; i < parser.m_lookupSize; i++) {
        parser.m_randVecLookup.push_back(glm::vec2(std::rand() * 2.0 / RAND_MAX - 1.0,
                                            std::rand() * 2.0 / RAND_MAX - 1.0));
    }

    for(int i = 0; i < 10; i++) {
        glm::vec3 start = glm::vec3{x_coords[i], y_coords[i], z_coords[i]};
        glm::vec3 end = glm::vec3{x_coords[i + 1], y_coords[i + 1], z_coords[i + 1]};
        parser.createLine(radius, start, end);
    }
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
