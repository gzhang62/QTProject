#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "plyreader.h"
#include "offreader.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_1_1>
#include <QOpenGLShaderProgram>
#include <QString>
#include <QOpenGLBuffer>
#include <QEvent>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_1_1
{
    Q_OBJECT


public:
    GLWidget(QWidget *parent = 0);
    void paintObject(const QMatrix4x4& mvMatrix);
    void setMode(bool checked);
    void readPoly(const QString &fname);
    void readOff(const QString &fname);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    bool linesChecked;

private:
    QString filename;
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

    QVector3D m_min;
    QVector3D m_max;

    QOpenGLBuffer m_vertexBuffer;

    void zoomFit(int angle);

    QVector3D m_center;
    QVector3D m_camPos;
    QVector3D m_camDirection;
    QPoint m_mousePosition;
    QVector3D m_updirection;
    double m_distanceToCenter;
    int m_fov;

    double m_nearPlane;
    double m_farPlane;
    float m_aspectRatio;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    float m_yaw;
    float m_pitch;
};

#endif // GLWIDGET_H
