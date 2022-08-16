QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Entities/collisionbox.cpp \
    Entities/door.cpp \
    Entities/entity.cpp \
    Entities/living.cpp \
    Entities/monster.cpp \
    Entities/npc.cpp \
    Entities/projectile.cpp \
    Entities/samos.cpp \
    Entities/terrain.cpp \
    Entities/area.cpp \
    Easing/Back.cpp \
    Easing/Bounce.cpp \
    Easing/Circ.cpp \
    Easing/Cubic.cpp \
    Easing/Elastic.cpp \
    Easing/Expo.cpp \
    Easing/Linear.cpp \
    Easing/Quad.cpp \
    Easing/Quart.cpp \
    Easing/Quint.cpp \
    Easing/Sine.cpp \
    dialogue.cpp \
    Entities/dynamicobj.cpp \
    editorpreview.cpp \
    game.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    physics.cpp \
    save.cpp

HEADERS += \
    Entities/collisionbox.h \
    Entities/door.h \
    Entities/dynamicobj.h \
    Entities/entity.h \
    Entities/living.h \
    Entities/monster.h \
    Entities/npc.h \
    Entities/projectile.h \
    Entities/samos.h \
    Entities/terrain.h \
    Entities/area.h \
    Easing/Back.h \
    Easing/Bounce.h \
    Easing/Circ.h \
    Easing/Cubic.h \
    Easing/Elastic.h \
    Easing/Expo.h \
    Easing/Linear.h \
    Easing/Quad.h \
    Easing/Quart.h \
    Easing/Quint.h \
    Easing/Sine.h \
    dialogue.h \
    editorpreview.h \
    game.h \
    mainwindow.h \
    map.h \
    nlohmann/json.hpp \
    physics.h \
    save.h

FORMS +=

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
