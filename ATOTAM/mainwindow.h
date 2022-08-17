#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPainter>
#include <windows.h>

#include "game.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication *app, std::string assetsPath);
    ~MainWindow();

    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void getInputs();
    void setupEditorWindow();

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

private:
    QApplication *m_qApp;

    Game* game = nullptr;
    int renderingMultiplier = 1; // Textures are rendered with their size being multiplied by this value
    QImage errorTexture;
    QImage emptyTexture;
    bool showHUD = true;
};
#endif // MAINWINDOW_H
