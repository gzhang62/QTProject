#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "plyreader.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_1_1>
#include <QOpenGLShaderProgram>
#include <QString>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_1_1
{
    Q_OBJECT


public:
    GLWidget(QWidget *parent = 0);
    void paintObject(const QMatrix4x4& mvMatrix);
    void setMode(bool checked);



protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void initializeCubeData();
    
    bool linesChecked;

private:

    bool m_initialized;
    struct ScenePoint {
        QVector3D coords;
        QVector3D normal;

        ScenePoint(const QVector3D &c = QVector3D(),
                   const QVector3D &n = QVector3D()) :
            coords(c), normal(n)
        {

        }
    };
    QVector<ScenePoint> m_data;
    QOpenGLShaderProgram m_shader;

    QMatrix4x4 m_modelMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_projectionMatrix;
};

#endif // GLWIDGET_H
