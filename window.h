#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>

class Window : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window() override;

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

private:
    QOpenGLTexture *m_texture = nullptr;
    QOpenGLShaderProgram *m_program = nullptr;
    QOpenGLBuffer m_geometryVbo;
    QOpenGLBuffer m_clipVbo;
    QOpenGLVertexArrayObject *m_vao = nullptr;
    QRegion m_clipRegion;
};
