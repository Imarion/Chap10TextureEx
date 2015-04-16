#include "vertex.h"

Vertex::Vertex()
{
    m_pos    = QVector3D(0.0f, 0.0f, 0.0f);
    m_tex    = QVector2D(0.0f, 0.0f);
    m_normal = QVector3D(0.0f, 0.0f, 0.0f);    
}

Vertex::Vertex(QVector3D pos, QVector2D tex)
{
    m_pos    = pos;
    m_tex    = tex;
    m_normal = QVector3D(0.0f, 0.0f, 0.0f);    
}

QVector3D Vertex::getPos()
{
    return m_pos;
}

QVector3D Vertex::getNormal()
{
    return m_normal;
}

QVector2D Vertex::getTexCoord()
{
    return m_tex;
}

void Vertex::setPos(QVector3D pos)
{
    m_pos = pos;
}

void Vertex::setNormal(QVector3D inNormal)
{
    m_normal = inNormal;
}
