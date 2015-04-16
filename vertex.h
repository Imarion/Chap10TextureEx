#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>
#include <QVector2D>

class Vertex
{
public:
    Vertex();
    Vertex(QVector3D pos, QVector2D tex);

    QVector3D getPos(), getNormal();
    QVector2D getTexCoord();

    void setNormal(QVector3D inNormal);
    void setPos(QVector3D pos);

private:
    QVector3D m_pos;
    QVector2D m_tex;
    QVector3D m_normal;
};

#endif // VERTEX_H
