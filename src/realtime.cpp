#include "realtime.h"
#include "utils/sceneparser.h"
#include "shapes/Cube.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "utils/shaderloader.h"
#include <string>
#include <glm/gtx/string_cast.hpp>
#include <unistd.h>

// ================== Project 5: Lights, Camera

RenderData renderData;

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteBuffers(1,&m_vbo);
    glDeleteVertexArrays(1,&m_vao);
    this->doneCurrent();
}

// Self-made function
void Realtime::updateShapes() {
    m_cube = Cube();
    m_cube.updateParams(2);
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert",
                                                 ":/resources/shaders/default.frag");
    glUseProgram(0);

    // Task 11: Fix this "fullscreen" quad's vertex data

    // Task 12: Play around with different values!
    // Task 13: Add UV coordinates
    std::vector<GLfloat> fullscreen_quad_data =
    { //     POSITIONS    //
        -1.f, 1.f, 0.0f,
         0.f, 1.f, 0.f,
        -1.f, -1.f, 0.0f,
         0.f, 0.f, 0.f,
         1.f, -1.f, 0.0f,
         1.f, 0.f, 0.f,
         1.f,  1.f, 0.0f,
         1.f, 1.f, 0.f,
        -1.f,  1.f, 0.0f,
         0.f, 1.f, 0.f,
         1.f, -1.f, 0.0f,
         1.f, 0.f, 0.f
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    // Task 14: modify the code below to add a second attribute to the vertex attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(sizeof(GLfloat)*3));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
    m_bezier.setCameraData(&renderData.cameraData);
}

void Realtime::handleObjects() {
    std::vector<float>* vbo_sphere;
    std::vector<float>* vbo_cube;
    std::vector<float>* vbo_cone;
    std::vector<float>* vbo_cylinder;
    //Create 4 vbos -- one for each shape
    vbo_cube = m_cube.generateShape();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,vbo_cube->size() * sizeof(GLfloat),vbo_cube->data(), GL_STATIC_DRAW);
        glBindVertexArray(m_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void*>(sizeof(GLfloat)*3));
}

void Realtime::loadLights() {
    int i = 0;
    glUniform1i(glGetUniformLocation(m_shader,"total_lights"), renderData.lights.size());
    for (SceneLightData light: renderData.lights) {
        std::string arg_1 = "c_light[" + std::to_string(i) + "]";
        GLint c_light_loc = glGetUniformLocation(m_shader,arg_1.data());
        glUniform4f(c_light_loc, light.color[0], light.color[1], light.color[2], light.color[3]);
        std::string arg_2 = "m_lightDir[" + std::to_string(i) + "]";
        GLint light_loc = glGetUniformLocation(m_shader, arg_2.data());
        glUniform4f(light_loc, light.dir[0],light.dir[1],light.dir[2], light.dir[3]);
        std::string arg_3 = "light_type[" + std::to_string(i) + "]";
        GLint light_type_loc = glGetUniformLocation(m_shader,arg_3.data());
        std::string arg_4 = "atten[" + std::to_string(i) + "]";
        GLint atten_loc = glGetUniformLocation(m_shader,arg_4.data());
        std::string arg_5 = "m_lightPos[" + std::to_string(i) + "]";
        GLint light_pos_loc = glGetUniformLocation(m_shader,arg_5.data());
        std::string arg_6 = "m_angle[" + std::to_string(i) + "]";
        GLint light_angle_loc = glGetUniformLocation(m_shader,arg_6.data());
        std::string arg_7 = "m_penumbra[" + std::to_string(i) + "]";
        GLint light_penumbra_loc = glGetUniformLocation(m_shader,arg_7.data());
        if (light.type == LightType::LIGHT_DIRECTIONAL) {
            glUniform1i(light_type_loc, 0);
        } else if (light.type == LightType::LIGHT_POINT) {
            glUniform1i(light_type_loc, 1);
            glUniform3f(atten_loc, light.function[0], light.function[1], light.function[2]);
            glUniform4f(light_pos_loc, light.pos[0], light.pos[1], light.pos[2], light.pos[3]);
        } else if (light.type == LightType::LIGHT_SPOT) {
            glUniform1i(light_type_loc, 2);
            glUniform3f(atten_loc, light.function[0], light.function[1], light.function[2]);
            glUniform4f(light_pos_loc, light.pos[0], light.pos[1], light.pos[2], light.pos[3]);
            glUniform4f(light_loc,light.dir[0],light.dir[1],light.dir[2], light.dir[3]);
            glUniform1f(light_angle_loc, light.angle);
            glUniform1f(light_penumbra_loc, light.penumbra);
        }
        i = i + 1;
    }
}

void Realtime::renderShapes() {
    // Initialize ctm to identity matrix
    glm::mat4 ctm = glm::mat4(1,0,0,0,
                              0,1,0,0,
                              0,0,1,0,
                              0,0,0,1);
    for(RenderShapeData shape: renderData.shapes) {
        glBindVertexArray(m_vao);
        ctm = shape.ctm;
        // Pass model matrix to shader program
        GLint uni_loc = glGetUniformLocation(m_shader, "model_matrix");
        glUniformMatrix4fv(uni_loc, 1, GL_FALSE, &ctm[0][0]);
        glm::mat4 mt_4 = glm::transpose(ctm);
        glm::mat3 model_trans = glm::inverse(glm::mat3(mt_4));
        GLint m_trans_loc = glGetUniformLocation(m_shader, "model_trans");
        glUniformMatrix3fv(m_trans_loc, 1, GL_FALSE, &model_trans[0][0]);
        // Pass view and projection matrices
        GLint view_loc = glGetUniformLocation(m_shader, "view_matrix");
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, &renderData.cameraData.m_view[0][0]);
        GLint proj_loc = glGetUniformLocation(m_shader, "proj_matrix");
        glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &m_proj[0][0]);
        // Pass ambient parameters
        GLint m_ka_loc = glGetUniformLocation(m_shader, "m_ka");
        glUniform1f(m_ka_loc, renderData.globalData.ka);
        GLint c_amb_loc = glGetUniformLocation(m_shader,"c_ambient");
        glUniform3fv(c_amb_loc, 1, &shape.primitive.material.cAmbient[0]);
        // Pass diffuse parameters
        GLint m_kd_loc = glGetUniformLocation(m_shader, "m_kd");
        glUniform1f(m_kd_loc, renderData.globalData.kd);
        GLint c_diff_loc = glGetUniformLocation(m_shader,"c_diffuse");
        glUniform3fv(c_diff_loc, 1, &shape.primitive.material.cDiffuse[0]);
        // Pass specular parameters
        GLint m_ks_loc = glGetUniformLocation(m_shader, "m_ks");
        glUniform1f(m_ks_loc, renderData.globalData.ks);
        GLint c_specular_loc = glGetUniformLocation(m_shader, "c_specular");
        glUniform3fv(c_specular_loc, 1, &shape.primitive.material.cSpecular[0]);
        GLint m_shin_loc = glGetUniformLocation(m_shader, "m_shin");
        glUniform1f(m_shin_loc, renderData.shapes[1].primitive.material.shininess);
        glm::vec4 world_cam = renderData.cameraData.pos;
        GLint m_cam_loc = glGetUniformLocation(m_shader, "cam_pos");
        glUniform4fv(m_cam_loc, 1, &world_cam[0]);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, m_cube.generateShape()->size() / 3);

        // Unbind VBO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Realtime::paintGL() {
    m_cube.updateParams(2);
    // Generate VAOs and VBOs
    glGenBuffers(4, &m_vbo);
    glGenVertexArrays(4, &m_vao);
    handleObjects();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Create shader program
    glUseProgram(m_shader);
    loadLights();
    renderShapes();
    // Unbind Vertex Array
    glBindVertexArray(0);
    // Deactivate shader program
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
    // Students: anything requiring OpenGL calls when the program starts should be done here
    renderData.cameraData.aspectRatio = float(w) / float(h);
    m_asp_ratio = float(w) / float(h);;
    renderData.cameraData.updateProjMatrix(settings.nearPlane, settings.farPlane);
    m_proj = renderData.cameraData.m_proj;

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
}

void Realtime::sceneChanged() {
    SceneParser parser;
    parser.parse(renderData, settings.shapeParameter1);
    renderData.cameraData.aspectRatio = m_asp_ratio;
    renderData.cameraData.updateProjMatrix(settings.nearPlane, settings.farPlane);
    m_proj = renderData.cameraData.m_proj;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up, renderData.cameraData.look, renderData.cameraData.pos);
    updateShapes();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    SceneParser parser;
    parser.parse(renderData, settings.shapeParameter1);
    renderData.cameraData.updateProjMatrix(settings.nearPlane, settings.farPlane);
    m_proj = renderData.cameraData.m_proj;
    m_cube.updateParams(2);

    /* Testing Bezier (can delete later) */
    if (settings.extraCredit3) {
        for (float t = 0; t < 1.0; t += 0.01) {
            m_bezier.updatePos(glm::vec4(0.0, 0.0, 0.0, 1.0), glm::vec4(10.0, 20.0, 0.0, 1.0), glm::vec4(40.0, 20.0, 0.0, 1.0), glm::vec4(50.0, 0.0, 0.0, 1.0), t);
            update();
        }
    }
    /* Testing Bezier (can delete later) */

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

glm::mat4 rotation_matrix(float theta, glm::vec3 axis) {
    float sin_t = glm::sin(theta);
    float cos_t = glm::cos(theta);
    axis = glm::normalize(axis);
    float ux = axis[0];
    float uy = axis[1];
    float uz = axis[2];
    return glm::mat4(cos_t + ux*ux*(1-cos_t), ux*uy*(1-cos_t) - uz*sin_t, ux*uz*(1-cos_t) + uy*sin_t, 0,
                     ux*uy*(1-cos_t)+uz*sin_t, cos_t + uy*uy*(1-cos_t), uy*uz*(1-cos_t) - ux*sin_t, 0,
                     ux*uz*(1-cos_t)-uy*sin_t, uy*uz*(1-cos_t)+ux*sin_t, cos_t+ux*ux*(1-cos_t), 0,
                     0,0,0,1);
}

void rotate_x(float x) {
    float theta = 1.f*x / 100.f;
    glm::vec3 axis = glm::vec3(0,1,0);
    glm::mat4 r_mat = rotation_matrix(theta, axis);
    renderData.cameraData.look = r_mat*renderData.cameraData.look;
    renderData.cameraData.up = r_mat*renderData.cameraData.up;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void rotate_y(float y) {
    float theta = 1.f*y / 100.f;
    glm::vec3 axis = glm::cross(glm::vec3(glm::normalize(renderData.cameraData.look)),
                                     glm::vec3(glm::normalize(renderData.cameraData.up)));
    glm::mat4 r_mat = rotation_matrix(theta, axis);
    renderData.cameraData.look = r_mat*renderData.cameraData.look;
    renderData.cameraData.up = r_mat*renderData.cameraData.up;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        if (m_mouseDown) {
            rotate_x(deltaX);
            rotate_y(deltaY);
        }

        update(); // asks for a PaintGL() call to occur
    }
}

void press_w(float time) {
    glm::vec4 trans = 1.f*glm::normalize(renderData.cameraData.look)*5.f*time;
    glm::mat4 m_trans = glm::mat4(1.f,0.f,0.f,0.f,
                                  0.f,1.f,0.f,0.f,
                                  0.f,0.f,1.f,0.f,
                                  trans[0], trans[1], trans[2], 1.f);
    renderData.cameraData.pos = m_trans*renderData.cameraData.pos;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void press_s(float time) {
    glm::vec4 trans = -1.f*glm::normalize(renderData.cameraData.look)*5.f*time;
    glm::mat4 m_trans = glm::mat4(1.f,0.f,0.f,0.f,
                                  0.f,1.f,0.f,0.f,
                                  0.f,0.f,1.f,0.f,
                                  trans[0], trans[1], trans[2], 1.f);
    renderData.cameraData.pos = m_trans*renderData.cameraData.pos;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void press_a(float time) {
    glm::vec4 trans = -1.f*glm::vec4(glm::cross(glm::vec3(glm::normalize(renderData.cameraData.look)),
                                 glm::vec3(glm::normalize(renderData.cameraData.up))), 1.f)*5.f*time;
    glm::mat4 m_trans = glm::mat4(1.f,0.f,0.f,0.f,
                                  0.f,1.f,0.f,0.f,
                                  0.f,0.f,1.f,0.f,
                                  trans[0], trans[1], trans[2], 1.f);
    renderData.cameraData.pos = m_trans*renderData.cameraData.pos;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void press_d(float time) {
    glm::vec4 trans = glm::vec4(glm::cross(glm::vec3(glm::normalize(renderData.cameraData.look)),
                                 glm::vec3(glm::normalize(renderData.cameraData.up))), 1.f)*5.f*time;
    glm::mat4 m_trans = glm::mat4(1.f,0.f,0.f,0.f,
                                  0.f,1.f,0.f,0.f,
                                  0.f,0.f,1.f,0.f,
                                  trans[0], trans[1], trans[2], 1.f);
    renderData.cameraData.pos = m_trans*renderData.cameraData.pos;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void press_space(float time) {
    glm::vec4 trans = glm::vec4(0.f,1.f,0.f,0.f)*5.f*time;
    glm::mat4 m_trans = glm::mat4(1.f,0.f,0.f,0.f,
                                  0.f,1.f,0.f,0.f,
                                  0.f,0.f,1.f,0.f,
                                  trans[0], trans[1], trans[2], 1.f);
    renderData.cameraData.pos = m_trans*renderData.cameraData.pos;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void press_ctrl(float time) {
    glm::vec4 trans = -1.f*glm::vec4(0.f,1.f,0.f,0.f)*5.f*time;
    glm::mat4 m_trans = glm::mat4(1.f,0.f,0.f,0.f,
                                  0.f,1.f,0.f,0.f,
                                  0.f,0.f,1.f,0.f,
                                  trans[0], trans[1], trans[2], 1.f);
    renderData.cameraData.pos = m_trans*renderData.cameraData.pos;
    renderData.cameraData.updateViewMatrix(renderData.cameraData.up,
            renderData.cameraData.look, renderData.cameraData.pos);
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_W]) {
        press_w(deltaTime);
    }
    if (m_keyMap[Qt::Key_S]) {
        press_s(deltaTime);
    }
    if (m_keyMap[Qt::Key_A]) {
        press_a(deltaTime);
    }
    if (m_keyMap[Qt::Key_D]) {
        press_d(deltaTime);
    }
    if (m_keyMap[Qt::Key_Space]) {
        press_space(deltaTime);
    }
    if (m_keyMap[Qt::Key_Control]) {
        press_ctrl(deltaTime);
    }
    /* Testing Bezier (can delete later) */
    else if (m_keyMap[Qt::Key_F]) {
        m_bezier.updatePos(glm::vec4(-6.0, 4.0, 4.0, 1.0), glm::vec4(-4.0, 5.0, 4.0, 1.0), glm::vec4(-2.0, 5.0, 4.0, 1.0), glm::vec4(0.0, 1.0, 0.0, 1.0), m_bezierInc);
        m_bezierInc += 0.01;
    }
    /* Testing Bezier (can delete later) */

    update(); // asks for a PaintGL() call to occur
}
