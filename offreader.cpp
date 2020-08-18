#include "offreader.h"
#include <QFile>

OffReader::OffReader(const QString &fileName) {
    m_fileName = fileName;
}

bool OffReader::read() {
    m_vertices.clear();
    m_faces.clear();
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream stream(&file);
    QString line = stream.readLine();
    if(line != "OFF") {
        return false;
    }
    line = stream.readLine();
    int vertexCount = 0, faceCount = 0, edgeCount = 0;
    QStringList counts = line.split(" ");
    vertexCount = counts[0].toInt();
    faceCount = counts[2].toInt();
    edgeCount = counts[4].toInt();
    readVertices(stream, vertexCount);
    readFaces(stream, faceCount);
    computeNormals();
    return true;
}

void OffReader::readVertices(QTextStream &stream, int vertexCount) {
    m_vertices.reserve(vertexCount);
    for(int i = 0; i < vertexCount; ++i) {
        m_vertices << readVertex(stream);
    }
}

void OffReader::readFaces(QTextStream &stream, int faceCount) {
    m_faces.reserve(faceCount);
    for(int i = 0; i < faceCount; ++i) {
        m_faces << readFace(stream);
        QVector<int>* pFace = &m_faces[i];
        for (int id : *pFace)
        {
            ScenePoint *pVertex = &m_vertices[id];
            m_mapVertexFaces[pVertex].insert(pFace);
        }
    }
}

OffReader::ScenePoint OffReader::readVertex(QTextStream &stream) {
    ScenePoint pt;
    float xpos, ypos, zpos;
    stream >> xpos >> ypos >> zpos;
    pt.coord = QVector3D(xpos, ypos, zpos);
    return pt;
}

QVector<int> OffReader::readFace(QTextStream &stream) {
    int cnt, pt;
    stream >> cnt;
    QVector<int> face;
    face.reserve(cnt);
    for(int v = 0; v < cnt; ++v) {
        stream >> pt;
        face << pt;
    }
    return face;
}

void OffReader::computeNormals()
{
    for (auto p : m_mapVertexFaces)
    {
        ScenePoint* pVertex = p.first;
        set<QVector<int> *>& setFaces = p.second;
        QVector<QVector3D> faceNormals;
        QVector3D vNormal;
        for (auto pFace : setFaces)
        {
            QVector3D v1 = m_vertices.at(pFace->at(1)).coord - m_vertices.at(pFace->at(0)).coord;
            QVector3D v2 = m_vertices.at(pFace->at(2)).coord - m_vertices.at(pFace->at(0)).coord;
            QVector3D normal = QVector3D::crossProduct(v1, v2);
            normal.normalize();
            faceNormals.append(normal);
        }

        for(auto normal: faceNormals)
        {
            vNormal += normal;
        }
        vNormal = vNormal / faceNormals.size();
        vNormal.normalize();
        pVertex->normal = vNormal;
    }
}
