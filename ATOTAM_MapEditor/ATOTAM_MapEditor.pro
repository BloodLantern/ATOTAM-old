QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addedit.cpp \
    entitylist.cpp \
    main.cpp \
    edit.cpp \
    editorpreview.cpp \
    editorwindow.cpp \
    moveedit.cpp \
    propertiesedit.cpp \
    propertiesmodel.cpp \
    removeedit.cpp \
    resizeedit.cpp \
    multitypeedit.cpp \
    ../ATOTAM/map.cpp \
    ../ATOTAM/Entities/entity.cpp \
    ../ATOTAM/Entities/area.cpp \
    ../ATOTAM/Entities/collisionbox.cpp \
    ../ATOTAM/Entities/terrain.cpp \
    ../ATOTAM/Entities/door.cpp \
    ../ATOTAM/Entities/dynamicobj.cpp \
    ../ATOTAM/Entities/living.cpp \
    ../ATOTAM/Entities/monster.cpp  \
    ../ATOTAM/Entities/npc.cpp \
    ../ATOTAM/Entities/samos.cpp \
    ../ATOTAM/Entities/savepoint.cpp \
    ../ATOTAM/Entities/projectile.cpp

HEADERS += \
    propertiesmodel.h \
    addedit.h \
    edit.h \
    editorpreview.h \
    editorwindow.h \
    entitylist.h \
    moveedit.h \
    precompiledheaders.h \
    propertiesedit.h \
    removeedit.h \
    resizeedit.h \
    multitypeedit.h \
    ../ATOTAM/map.h \
    ../ATOTAM/Entities/entity.h \
    ../ATOTAM/Entities/area.h \
    ../ATOTAM/Entities/collisionbox.h \
    ../ATOTAM/Entities/terrain.h \
    ../ATOTAM/Entities/door.h \
    ../ATOTAM/Entities/dynamicobj.h \
    ../ATOTAM/Entities/living.h \
    ../ATOTAM/Entities/monster.h  \
    ../ATOTAM/Entities/npc.h \
    ../ATOTAM/Entities/samos.h \
    ../ATOTAM/Entities/savepoint.h \
    ../ATOTAM/Entities/projectile.h

PRECOMPILED_HEADER = precompiledheaders.h

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
