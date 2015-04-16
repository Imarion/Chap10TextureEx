#QT += gui-private core-private
QT += gui core

HEADERS += chap10p5.h \
    vertex.h

SOURCES += chap10p5.cpp main.cpp \
    vertex.cpp \


OTHER_FILES += \
    vshader.txt \
    fshader.txt

RESOURCES += \
    shaders.qrc

