#include "glwidget.h"
#include "mainwindow.h"

#include <QMouseEvent>
#include <QtMath>

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), m_vertexBuffer(QOpenGLBuffer::VertexBuffer)
{

    m_initialized = false;
    linesChecked = false;
    //readPoly(":/monkey.ply");
    for(int i = 0; i < 3; i++)
    {
        m_min[i] = 1e20;
    }
    for(int i = 0; i < 3; i++)
    {
        m_max[i] = -1e20;
    }

    m_yaw = -90.0;
    m_pitch = 0.0;
    m_fov = 60;
    m_camDirection = QVector3D(0, 0, 1);
    m_updirection = QVector3D(0, 1, 0);

}

void GLWidget::initializeGL()
{
    if (!initializeOpenGLFunctions()) {
        qFatal("initializeOpenGLFunctions failed");
    }
        m_initialized = true;
        glClearColor(0, 0, 0, 0);

        m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/phong.vert");
        m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/phong.frag");
        m_shader.link();
        m_shader.bind();

        /*
        m_shader.setAttributeArray("Vertex", GL_FLOAT,
                                   &m_data[0].coords, 3, sizeof(ScenePoint));
        m_shader.enableAttributeArray("Vertex");

        m_shader.setAttributeArray("Normal", GL_FLOAT,
                                   &m_data[0].normal, 3, sizeof(ScenePoint));
        m_shader.enableAttributeArray("Normal");
*/
        m_shader.setUniformValue("mat.ka", QVector3D(0.1, 0, 0.0));
        m_shader.setUniformValue("mat.kd", QVector3D(0.7, 0.0, 0.0));
        m_shader.setUniformValue("mat.ks", QVector3D(1.0, 1.0, 1.0));
        m_shader.setUniformValue("mat.shininess", 128.0f);

        m_shader.setUniformValue("light.position", QVector3D(2, 1, 1));
        m_shader.setUniformValue("light.intensity", QVector3D(1, 1, 1));
}

 void GLWidget::paintGL(){

     if (m_data.size() == 0)
         return;

     m_projectionMatrix.setToIdentity();
     m_aspectRatio = qreal(window()->width()) / window()->height();
     m_projectionMatrix.perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);


     m_viewMatrix.setToIdentity();
     m_viewMatrix.lookAt(m_camPos, m_center, m_updirection);
     m_shader.bind();

     m_shader.setAttributeArray("Vertex", GL_FLOAT,
                                 &m_data[0].coords, 3, sizeof(ScenePoint));
     m_shader.enableAttributeArray("Vertex");

     m_shader.setAttributeArray("Normal", GL_FLOAT,
                                 &m_data[0].normal, 3, sizeof(ScenePoint));

     m_shader.enableAttributeArray("Normal");


//    m_projectionMatrix.setToIdentity();
//    float aspectRatio = qreal(window()->width()) / window()->height();
//    m_projectionMatrix.perspective(90, aspectRatio, 0.5, 40);

//    m_viewMatrix.setToIdentity();
//    QVector3D eye(0, 0, 2);
//    QVector3D center(0, 0, 0);
//    QVector3D up(0, 1, 0);
//    m_viewMatrix.lookAt(eye, center, up);

    zoomFit(60);



    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, window()->width(), window()->height());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    QMatrix4x4 modelViewMatrix = m_viewMatrix * m_modelMatrix;


    paintObject(modelViewMatrix);

}

void GLWidget::resizeGL(int w, int h)
{

}


void GLWidget::paintObject(const QMatrix4x4 &mvMatrix) {
    m_shader.bind();
    m_shader.setUniformValue("projectionMatrix", m_projectionMatrix);
    m_shader.setUniformValue("modelViewMatrix", mvMatrix);
    m_shader.setUniformValue("mvpMatrix", m_projectionMatrix*mvMatrix);
    m_shader.setUniformValue("normalMatrix", mvMatrix.normalMatrix());
    
    

    if(linesChecked == true)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glDrawArrays(GL_TRIANGLES, 0, m_data.size());
}


void GLWidget::readPoly(const QString &fname) {

    PlyReader reader(fname);
    if(reader.read()) {
        // reconstruct faces
        const QVector<PlyReader::ScenePoint> vertices = reader.vertices();
        const QVector<QVector<int>> faces = reader.faces();
        for(const QVector<int>& face: faces) {
            ScenePoint pt;
            for(int vertexId: face) {
                const PlyReader::ScenePoint &scenePoint = vertices.at(vertexId);
                pt.coords = scenePoint.coord;
                pt.normal = scenePoint.normal;
                m_data << pt;

            }
        }
        for(int i = 0; i < m_data.length(); ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                if(m_data[i].coords[j] < m_min[j])
                {
                    m_min[j] = m_data[i].coords[j];
                }
                if(m_data[i].coords[j] > m_max[j])
                {
                    m_max[j] = m_data[i].coords[j];
                }
            }

        }

    }
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_data.constData(),
                            m_data.count() * sizeof(ScenePoint));


}

void GLWidget::readOff(const QString &fname) {

    OffReader reader(fname);
//    for(int i = 0; i < 3; i++)
//    {
//        m_min[i] = 1e20;
//    }
//    for(int i = 0; i < 3; i++)
//    {
//        m_max[i] = -1e20;
//    }

    if(reader.read()) {
        // reconstruct faces
        const QVector<OffReader::ScenePoint> vertices = reader.vertices();
        const QVector<QVector<int>> faces = reader.faces();
        for(const QVector<int>& face: faces) {
            ScenePoint pt;
            for(int vertexId: face) {
                const OffReader::ScenePoint &scenePoint = vertices.at(vertexId);
                pt.coords = scenePoint.coord;
                pt.normal = scenePoint.normal;
                m_data << pt;

            }
        }

        for(int i = 0; i < m_data.length(); ++i)
        {
            for(int j = 0; j < 3; ++j)
            {
                if(m_data[i].coords[j] < m_min[j])
                {
                    m_min[j] = m_data[i].coords[j];
                }
                if(m_data[i].coords[j] > m_max[j])
                {
                    m_max[j] = m_data[i].coords[j];
                }
            }

        }

    }   
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_data.constData(),
                            m_data.count() * sizeof(ScenePoint));




}

void GLWidget::setMode(bool checked)
{
    linesChecked = checked;
    update();
}

void GLWidget::zoomFit(int angle)
{
    double half_angle_radians = angle * M_PI / 180 / 2;
    double radius = (m_max - m_min).length() / 2;
    m_distancetoCenter = radius / sin(half_angle_radians);


    m_nearPlane = m_distancetoCenter - radius * 1.2;
    m_farPlane = m_distancetoCenter + radius * 2;



//    m_projectionMatrix.setToIdentity();
//    m_aspectRatio = qreal(window()->width()) / window()->height();
//    m_projectionMatrix.perspective(angle, m_aspectRatio, m_nearPlane, m_farPlane);


//    m_viewMatrix.setToIdentity();
    m_center = (m_min + m_max) * 0.5;
    m_camPos = m_center + m_camDirection * m_distancetoCenter;
//    m_viewMatrix.lookAt(m_camPos, m_center, m_updirection);

    update();
}




