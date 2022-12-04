#pragma once

#include "utils/scenedata.h"

class Bezier {
public:
    Bezier(SceneCameraData *cameraData);
    SceneCameraData *m_cameraData;
};
