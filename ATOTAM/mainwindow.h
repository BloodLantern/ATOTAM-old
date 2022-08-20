#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPainter>
#include <windows.h>

#include "Editor/editorwindow.h"
#include "game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:
    MainWindow(QApplication *app, std::string assetsPath);
    ~MainWindow();

    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void getInputs();
    void setupEditorWindow(nlohmann::json editorJson);

    int getRenderingMultiplier() const;
    void setRenderingMultiplier(int newRenderingMultiplier);
    const QImage &getErrorTexture() const;
    void setErrorTexture(const QImage &newErrorTexture);
    const QImage &getEmptyTexture() const;
    void setEmptyTexture(const QImage &newEmptyTexture);
    bool getShowHUD() const;
    void setShowHUD(bool newShowHUD);
    Game *getGame() const;
    void setGame(Game *newGame);
    EditorWindow *getEditorWindow() const;
    void setEditorWindow(EditorWindow *newEditorWindow);

    bool getRender() const;
    void setRender(bool newRender);

    const Game &getTempG() const;
    void setTempG(const Game &newTempG);

    bool getRenderDone() const;
    void setRenderDone(bool newRenderDone);

private:
    QApplication *m_qApp;
    bool eventFilter(QObject *object, QEvent *event);

    EditorWindow* editorWindow = nullptr;
    Game* game = nullptr;
    Game tempG;
    int renderingMultiplier = 1; // Textures are rendered with their size being multiplied by this value
    QImage errorTexture;
    QImage emptyTexture;
    bool showHUD = true;
    bool render = false;
    bool renderDone = true;
};
#endif // MAINWINDOW_H
