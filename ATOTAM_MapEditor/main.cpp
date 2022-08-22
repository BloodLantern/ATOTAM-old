#include "editorpreview.h"
#include "editorwindow.h"

#include <QApplication>
#include <QLocale>
#include <QSplitter>
#include <QTranslator>
#include <fstream>

#define JSON_DIAGNOSTICS 1 // Json extended error messages
#include "../ATOTAM/nlohmann/json.hpp"
#include "../ATOTAM/map.h"

void setupEditorWindow(nlohmann::json editorJson, std::string assetsPath)
{
    // Map preview
    Map editedMap = Map::loadMap(editorJson["lastLaunch"]["map"]["id"], assetsPath);
    std::ifstream file(assetsPath + "/params.json");
    nlohmann::json params;
    file >> params;
    EditorPreview* preview = new EditorPreview(&editedMap, new QImage(QString::fromStdString(assetsPath + "/textures/error.png"))
                                               , new QImage(QString::fromStdString(assetsPath + "/textures/empty.png")), 2
                                               , editorJson, params["frameRate"]);
    preview->setAssetsPath(assetsPath);

    // Window
    EditorWindow* editorWindow = new EditorWindow(preview);
    editorWindow->setWindowTitle("ATOTAM Map Editor");
    editorWindow->setMinimumSize(QSize(editorJson["values"]["minimumWindowSize"][0], editorJson["values"]["minimumWindowSize"][1]));
    // Set window screen
    /*QList<QScreen*> screens = m_qApp->screens();
    if (screens.size() > 1) {
        editorWindow->setScreen(screens[1]);
        editorWindow->setGeometry(screens[1]->geometry().center().x() - static_cast<int>(editorJson["lastLaunch"]["window"]["size"][0])/2,
                screens[1]->geometry().center().y() - static_cast<int>(editorJson["lastLaunch"]["window"]["size"][1])/2, 800, 600);
    }*/

    // Widgets inside the window

    // Align widgets
    QSplitter* splitter = new QSplitter;
    splitter->addWidget(preview);

    editorWindow->setCentralWidget(splitter);
    if (editorJson["lastLaunch"]["window"]["maximized"])
        editorWindow->showMaximized();
    else
        editorWindow->show();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ATOTAM_MapEditor_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    std::string assetsPath = "../ATOTAM/assets";
    std::ifstream file(assetsPath + "/map_editor.json");
    nlohmann::json editorJson;
    file >> editorJson;
    Entity::values = Entity::loadValues(assetsPath);
    setupEditorWindow(editorJson, assetsPath);

    return a.exec();
}
