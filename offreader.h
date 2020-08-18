#ifndef OFFREADER_H
#define OFFREADER_H


#include <QObject>
#include <QVector3D>
#include <QTextStream>
#include <QVector>

#include <map>
#include <set>
using namespace std;

class QIODevice;

class OffReader : public QObject {
public:
    struct ScenePoint {
        QVector3D coord;
        QVector3D normal;

    };

    OffReader(const QString &fileName);
    bool read();

    inline const QVector<ScenePoint> vertices() const { return m_vertices; }
    inline const QVector<QVector<int>> faces() const { return m_faces; }
    void computeNormals();

protected:
    void readVertices(QTextStream &stream, int vertexCount);
    void readFaces(QTextStream &stream, int faceCount);
    ScenePoint readVertex(QTextStream &stream);
    QVector<int> readFace(QTextStream &stream);

private:
    QString m_fileName;
    QVector<ScenePoint> m_vertices;
    QVector<QVector<int>> m_faces;

    map<ScenePoint*, set< QVector<int>* > > m_mapVertexFaces;
};
#endif // OFFREADER_H
