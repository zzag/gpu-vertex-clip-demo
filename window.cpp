#include "window.h"

#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>

static const char *vertexShaderCode =
        "#version 330\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec4 clipRect;\n"
        "uniform mat4 projectionMatrix;\n"
        "void main()\n"
        "{\n"
        "    gl_ClipDistance[0] = position.x - clipRect[0];\n"
        "    gl_ClipDistance[1] = clipRect[1] - position.x;\n"
        "    gl_ClipDistance[2] = position.y - clipRect[2];\n"
        "    gl_ClipDistance[3] = clipRect[3] - position.y;\n"
        "\n"
        "    gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);\n"
        "}\n";

static const char *vertexShaderCodeGLES =
        "#version 300 es\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec4 clipRect;\n"
        "uniform mat4 projectionMatrix;\n"
        "void main()\n"
        "{\n"
        "    gl_ClipDistance[0] = position.x - clipRect[0];\n"
        "    gl_ClipDistance[1] = clipRect[1] - position.x;\n"
        "    gl_ClipDistance[2] = position.y - clipRect[2];\n"
        "    gl_ClipDistance[3] = clipRect[3] - position.y;\n"
        "\n"
        "    gl_Position = projectionMatrix * vec4(position, 0.0, 1.0);\n"
        "}\n";

static const char *fragmentShaderCode =
        "#version 330\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

static const char *fragmentShaderCodeGLES =
        "#version 300 es\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";

Window::Window(QWidget *parent)
    : QOpenGLWidget(parent)
{
    resize(800, 600);
}

Window::~Window()
{
    makeCurrent();

    m_clipVbo.destroy();
    m_geometryVbo.destroy();

    delete m_texture;
    delete m_program;
    delete m_vao;
}

void Window::initializeGL()
{
    initializeOpenGLFunctions();

    const bool gles = QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGLES;
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                       gles ? vertexShaderCodeGLES : vertexShaderCode);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                       gles ? fragmentShaderCodeGLES : fragmentShaderCode);
    m_program->link();

    m_vao = new QOpenGLVertexArrayObject;
    m_vao->create();

    const QRect geometry(0, 0, 800, 600);
    const QVector<QVector2D> vertexData {
        // First triangle.
        QVector2D(geometry.x(), geometry.y()),
        QVector2D(geometry.x() + geometry.width(), geometry.y()),
        QVector2D(geometry.x() + geometry.width(), geometry.y() + geometry.height()),

        // Second triangle
        QVector2D(geometry.x(), geometry.y()),
        QVector2D(geometry.x() + geometry.width(), geometry.y() + geometry.height()),
        QVector2D(geometry.x(), geometry.y() + geometry.height()),
    };

    m_geometryVbo.create();
    m_geometryVbo.bind();
    m_geometryVbo.allocate(vertexData.constData(), vertexData.size() * sizeof(QVector2D));

    m_clipRegion += QRect(0, 0, 100, 100);
    m_clipRegion += QRect(300, 350, 100, 250);
    m_clipRegion += QRect(0, 400, 30, 150);

    QVector<QVector2D> clipData;
    clipData.reserve(m_clipRegion.rectCount() * 4);
    for (const QRect &rect : m_clipRegion)
        clipData << QVector2D(rect.x(), rect.x() + rect.width())
                 << QVector2D(rect.y(), rect.y() + rect.height());

    m_clipVbo.create();
    m_clipVbo.bind();
    m_clipVbo.allocate(clipData.constData(), clipData.size() * sizeof(QVector2D));
}

void Window::paintGL()
{
    QOpenGLExtraFunctions *f = QOpenGLContext::currentContext()->extraFunctions();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    m_vao->bind();
    m_program->bind();

    QMatrix4x4 projectionMatrix;
    projectionMatrix.ortho(rect());
    m_program->setUniformValue("projectionMatrix", projectionMatrix);

    // Setup the vertex buffer object.
    const int positionLocation = 0;
    m_geometryVbo.bind();
    glEnableVertexAttribArray(positionLocation);
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    f->glVertexAttribDivisor(positionLocation, 0);

    // Setup the clip buffer object.
    const int clipRectLocation = 1;
    m_clipVbo.bind();
    glEnableVertexAttribArray(clipRectLocation);
    glVertexAttribPointer(clipRectLocation, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
    f->glVertexAttribDivisor(clipRectLocation, 1);

    // GL_CLIP_DISTANCEi must be enabled before using it.
    glEnable(GL_CLIP_DISTANCE0);
    glEnable(GL_CLIP_DISTANCE1);
    glEnable(GL_CLIP_DISTANCE2);
    glEnable(GL_CLIP_DISTANCE3);

    // Use instanced rendering to clip the geometry on the GPU. The instance count
    // matches the total number of clip rects.
    f->glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_clipRegion.rectCount());

    // Cleanup the opengl state.
    glDisable(GL_CLIP_DISTANCE0);
    glDisable(GL_CLIP_DISTANCE1);
    glDisable(GL_CLIP_DISTANCE2);
    glDisable(GL_CLIP_DISTANCE3);
    glDisableVertexAttribArray(positionLocation);
    glDisableVertexAttribArray(clipRectLocation);

    m_program->release();
    m_vao->release();
}

void Window::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}
