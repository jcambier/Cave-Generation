#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    glm::vec3 normal = {0,0,1};
    glm::vec3 normalTopLeft = glm::normalize(glm::cross((topLeft - bottomLeft),(topLeft - topRight)));
    glm::vec3 normalTopRight = glm::normalize(glm::cross((topRight - topLeft), (topRight - bottomRight)));
    glm::vec3 normalBottomRight = glm::normalize(glm::cross((bottomRight - topRight), (bottomRight - bottomLeft)));
    glm::vec3 normalBottomLeft = glm::normalize(glm::cross((bottomLeft - bottomRight),(bottomLeft - topLeft)));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normalTopRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normalBottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normalBottomRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normalTopLeft);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            glm::vec3 nTopLeft = topLeft - float(j)*(topLeft - bottomLeft) / float(m_param1) + float(i)*(topRight - topLeft) / float(m_param1);
            glm::vec3 nTopRight = nTopLeft + ((topRight - topLeft) / float(m_param1));
            glm::vec3 nBottomLeft = nTopLeft - ((topLeft - bottomLeft) / float(m_param1));
            glm::vec3 nBottomRight = nBottomLeft + ((bottomRight - bottomLeft) / float(m_param1));
            makeTile(nTopLeft, nTopRight, nBottomLeft, nBottomRight);
        }
    }


}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

//     makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
//              glm::vec3( 0.5f,  0.5f, 0.5f),
//              glm::vec3(-0.5f, -0.5f, 0.5f),
//              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:

     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube

     makeFace(glm::vec3(0.5f,  0.5f, 0.5f),
              glm::vec3( -0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f, 0.5f, -0.5f),
              glm::vec3(-0.5f, 0.5f, -0.5f));

     makeFace(glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(0.5f,  0.5f, -0.5f),
              glm::vec3(0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f)
              );
     makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( -0.5f,  0.5f, 0.5f),
              glm::vec3( -0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f)
              );
     makeFace(glm::vec3( 0.5f,  -0.5f, -0.5f),
              glm::vec3(-0.5f,  -0.5f, -0.5f),
              glm::vec3(0.5f, -0.5f, 0.5f),
              glm::vec3( -0.5f, -0.5f, 0.5f)
              );
     makeFace(glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f)
              );
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
