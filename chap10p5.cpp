#include "chap10p5.h"

#include <QDebug>
#include <QFile>
#include <QImage>

#include <QVector2D>
#include <QVector3D>
#include <QMatrix4x4>

#include <cmath>

#include "vertex.h"

#define NUM_VERTICES    24
#define NUM_INDICES     36

MyWindow::~MyWindow()
{
    if (Vertices != 0) delete[] Vertices;
    if (Indices  != 0) delete[] Indices;    
}

MyWindow::MyWindow() : currentTimeMs(0), currentTimeS(0)
{
    Vertices  = 0;
    Indices   = 0;

    setSurfaceType(QWindow::OpenGLSurface);
    setFlags(Qt::Window | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setSamples(4);
    format.setProfile(QSurfaceFormat::CoreProfile);
    setFormat(format);
    create();

    resize(800, 600);

    mContext = new QOpenGLContext(this);
    mContext->setFormat(format);
    mContext->create();

    mContext->makeCurrent( this );

    mFuncs = mContext->versionFunctions<QOpenGLFunctions_4_3_Core>();
    if ( !mFuncs )
    {
        qWarning( "Could not obtain OpenGL versions object" );
        exit( 1 );
    }
    if (mFuncs->initializeOpenGLFunctions() == GL_FALSE)
    {
        qWarning( "Could not initialize core open GL functions" );
        exit( 1 );
    }

    initializeOpenGLFunctions();

    QTimer *repaintTimer = new QTimer(this);
    connect(repaintTimer, &QTimer::timeout, this, &MyWindow::render);
    repaintTimer->start(1000/60);

    QTimer *elapsedTimer = new QTimer(this);
    connect(elapsedTimer, &QTimer::timeout, this, &MyWindow::modCurTime);
    elapsedTimer->start(1);
}

void MyWindow::modCurTime()
{
    currentTimeMs++;
    currentTimeS=currentTimeMs/1000.0f;
}


void MyWindow::initialize()
{
    mFuncs->glGenVertexArrays(1, &mVAO);
    mFuncs->glBindVertexArray(mVAO);

    CreateVertexBuffer();
    initShaders();

    gWVPLocation     = mProgram->uniformLocation("gWVP");
    gWorldLocation   = mProgram->uniformLocation("gWorld");
    gSamplerLocation = mProgram->uniformLocation("gSampler");
    glUniform1i(gSamplerLocation, 0);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    //PrepareTexture(GL_TEXTURE_2D, "F:/Download/Programmation/OpenGL/ogldev-source/ogldev-source/tutorial16/test.png");
    PrepareTexture(GL_TEXTURE_2D, "squaredtexturewitharrow.jpg");
}

void MyWindow::CreateVertexBuffer()
{
    // C++11 required
    /*
    // baseline
        Vertex(QVector3D( -0.8f, 0.88f, 0.0f), QVector2D(0.0f, 1.0f)), // 0 top left
        Vertex(QVector3D( -0.8f, 0.08f, 0.0f), QVector2D(0.0f, 0.0f)), // 1 bottom left
        Vertex(QVector3D( -0.4f, 0.08f, 0.0f), QVector2D(1.0f, 0.0f)), // 2 bottom right
        Vertex(QVector3D( -0.4f, 0.88f, 0.0f), QVector2D(1.0f, 1.0f)), // 3 top right

        Vertex(QVector3D( -1.0f, 1.0f, 0.0f), QVector2D(0.0f, 1.0f)), // 0 top left
        Vertex(QVector3D( -1.0f,-1.0f, 0.0f), QVector2D(0.0f, 0.0f)), // 1 bottom left
        Vertex(QVector3D( 1.0f, -1.0f, 0.0f), QVector2D(1.0f, 0.0f)), // 2 bottom right
        Vertex(QVector3D( 1.0f, 1.0f, 0.0f), QVector2D(1.0f, 1.0f)), // 3 top right

        Vertex(QVector3D( -0.8f, 0.88f, 0.0f), QVector2D(0.20f, -0.3f)), // 0 top left
        Vertex(QVector3D( -0.8f, 0.08f, 0.0f), QVector2D(1.3f, -0.3f)), // 1 bottom left
        Vertex(QVector3D( -0.4f, 0.08f, 0.0f), QVector2D(1.3f, 1.2f)), // 2 bottom right
        Vertex(QVector3D( -0.4f, 0.88f, 0.0f), QVector2D(0.2f, 1.2f)), // 3 top right
    */

    Vertices = new Vertex[NUM_VERTICES] {

        Vertex(QVector3D( -0.8f, 0.88f, 0.0f), QVector2D( 0.2f, -0.3f)), // 0 top left
        Vertex(QVector3D( -0.8f, 0.08f, 0.0f), QVector2D( 0.2f,  1.2f)), // 1 bottom left
        Vertex(QVector3D( -0.4f, 0.08f, 0.0f), QVector2D( 1.3f,  1.2f)), // 2 bottom right
        Vertex(QVector3D( -0.4f, 0.88f, 0.0f), QVector2D( 1.3f, -0.3f)), // 3 top right

        Vertex(QVector3D( -0.2f, 0.88f, 0.0f), QVector2D( 5.0f, -1.0f)), // 4 top left
        Vertex(QVector3D( -0.2f, 0.08f, 0.0f), QVector2D( 5.0f,  0.0f)), // 5 bottom left
        Vertex(QVector3D(  0.2f, 0.08f, 0.0f), QVector2D( 6.0f,  0.0f)), // 6 bottom right
        Vertex(QVector3D(  0.2f, 0.88f, 0.0f), QVector2D( 6.0f, -1.0f)), // 7 top right

        Vertex(QVector3D(  0.4f, 0.88f, 0.0f), QVector2D( 1.0f,   0.0f)),  // 8 top left
        Vertex(QVector3D(  0.4f, 0.08f, 0.0f), QVector2D( 1.24f,  1.77f)), // 9 bottom left
        Vertex(QVector3D(  0.8f, 0.08f, 0.0f), QVector2D( 0.0f,   1.0f)),  // 10 bottom right
        Vertex(QVector3D(  0.8f, 0.88f, 0.0f), QVector2D(-0.23f, -0.77f)), // 11 top right

        Vertex(QVector3D( -0.8f,  0.06f, 0.0f), QVector2D( 2.0f, 0.0f)), // 12 top left
        Vertex(QVector3D( -0.8f, -0.74f, 0.0f), QVector2D( 1.0f, 0.0f)), // 13 bottom left
        Vertex(QVector3D( -0.4f, -0.74f, 0.0f), QVector2D( 0.0f, 1.0f)), // 14 bottom right
        Vertex(QVector3D( -0.4f,  0.06f, 0.0f), QVector2D( 1.0f, 1.0f)), // 15 top right

        Vertex(QVector3D( -0.2f,  0.06f, 0.0f), QVector2D(-0.1f, 1.1f)), // 16 top left
        Vertex(QVector3D( -0.2f, -0.74f, 0.0f), QVector2D( 0.9f, 1.1f)), // 17 bottom left
        Vertex(QVector3D(  0.2f, -0.74f, 0.0f), QVector2D( 0.9f, 0.1f)), // 18 bottom right
        Vertex(QVector3D(  0.2f,  0.06f, 0.0f), QVector2D(-0.1f, 0.1f)), // 19 top right

        Vertex(QVector3D(  0.4f,  0.06f, 0.0f), QVector2D( 0.0f, -1.0f)),  // 20 top left
        Vertex(QVector3D(  0.4f, -0.74f, 0.0f), QVector2D(-2.36f, 0.94f)), // 21 bottom left
        Vertex(QVector3D(  0.8f, -0.74f, 0.0f), QVector2D( 1.0f,  2.0f)),  // 22 bottom right
        Vertex(QVector3D(  0.8f,  0.06f, 0.0f), QVector2D( 3.35f, 0.06f))  // 23 top right
    };

    Indices = new unsigned int[NUM_INDICES] {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices[0])*NUM_VERTICES, Vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0])*NUM_INDICES, Indices, GL_STATIC_DRAW);
}

void MyWindow::resizeEvent(QResizeEvent *)
{
    mUpdateSize = true;
}

void MyWindow::render()
{
    if(!isVisible() || !isExposed())
        return;

    if (!mContext->makeCurrent(this))
        return;

    static bool initialized = false;
    if (!initialized) {
        initialize();
        initialized = true;
    }

    if (mUpdateSize) {
        glViewport(0, 0, size().width(), size().height());
        mUpdateSize = false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)(sizeof(Vertices[0].getPos())));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)((sizeof(Vertices[0].getPos()))+(sizeof(Vertices[0].getTexCoord()))));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureObject);

    static float Scale = 0.0f;
    Scale += 0.1f; // tut 12


    mProgram->bind();
    {
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, NUM_INDICES, GL_UNSIGNED_INT, 0);
    }
    mProgram->release();

    mContext->swapBuffers(this);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void MyWindow::initShaders()
{
    QOpenGLShader vShader(QOpenGLShader::Vertex);
    QOpenGLShader fShader(QOpenGLShader::Fragment);
    QFile         shaderFile;
    QByteArray    shaderSource;

    //mProgram
    // Shader 1
    shaderFile.setFileName(":/vshader.txt");
    shaderFile.open(QIODevice::ReadOnly);
    shaderSource = shaderFile.readAll();
    shaderFile.close();
    qDebug() << "vertex 1 compile: " << vShader.compileSourceCode(shaderSource);

    shaderFile.setFileName(":/fshader.txt");
    shaderFile.open(QIODevice::ReadOnly);
    shaderSource = shaderFile.readAll();
    shaderFile.close();
    qDebug() << "frag   1 compile: " << fShader.compileSourceCode(shaderSource);

    mProgram = new (QOpenGLShaderProgram);
    mProgram->addShader(&vShader);
    mProgram->addShader(&fShader);
    qDebug() << "shader link: " << mProgram->link();
}

void MyWindow::PrepareTexture(GLenum TextureTarget, const QString& FileName)
{
    QImage TexImg;

    TexImg.load(FileName);
    glGenTextures(1, &mTextureObject);
    glBindTexture(TextureTarget, mTextureObject);
    glTexImage2D(TextureTarget, 0, GL_RGB, TexImg.width(), TexImg.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, TexImg.bits());
    glTexParameterf(TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void MyWindow::keyPressEvent(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
        case Qt::Key_P:
            break;
        case Qt::Key_Up:
            break;
        case Qt::Key_Down:
            break;
        case Qt::Key_Left:
            break;
        case Qt::Key_Right:
            break;
        case Qt::Key_Delete:
            break;
        case Qt::Key_PageDown:
            break;
        case Qt::Key_Home:
            break;
        case Qt::Key_Z:
            break;
        case Qt::Key_S:
            break;
        case Qt::Key_Q:
            break;
        case Qt::Key_D:
            break;
        case Qt::Key_A:
            break;
        case Qt::Key_E:
            break;
        default:
            break;
    }
}


void MyWindow::printMatrix(const QMatrix4x4& mat)
{
    const float *locMat = mat.transposed().constData();

    for (int i=0; i<4; i++)
    {
        qDebug() << locMat[i*4] << " " << locMat[i*4+1] << " " << locMat[i*4+2] << " " << locMat[i*4+3];
    }
}
