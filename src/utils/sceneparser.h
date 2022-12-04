#pragma once

#include "scenedata.h"
#include <vector>
#include <string>
#include <map>

// Struct which contains data for a single primitive, to be used for rendering
struct RenderShapeData {
    ScenePrimitive primitive;
    glm::mat4 ctm; // the cumulative transformation matrix
};

// Struct which contains all the data needed to render a scene
struct RenderData {
    SceneGlobalData globalData;
    SceneCameraData cameraData;

    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;
};

class SceneParser {
private:
    glm::vec3 getWorldCoord(glm::vec3 origin, glm::vec3 xyz, int radius);
    bool inSphere(glm::vec3 origin, int radius, glm::vec3 xyz);
    RenderShapeData createCube(glm::vec3 location);
    void createSphere(glm::vec3 origin, int radius);
    void createLine(glm::vec3 point_1, glm::vec3 point_2, int radius);
    std::vector<RenderShapeData> cube_data;
    std::map<std::vector<int>, bool> isCovered;
    std::map<std::vector<int>, bool> isBlock;
public:
    // Parse the scene and store the results in renderData.
    // @param filepath    The path of the scene file to load.
    // @param renderData  On return, this will contain the metadata of the loaded scene.
    // @return            A boolean value indicating whether the parse was successful.
    static bool parse(RenderData &renderData, int radius);
};

