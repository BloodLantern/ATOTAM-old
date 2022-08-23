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
public:
    MainWindow(QApplication *app, std::string assetsPath);
    ~MainWindow();

    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void getInputs();
    void getSpecialInputs();
    void setupToDraw();

    int getRenderingMultiplier() const;
    void setRenderingMultiplier(int newRenderingMultiplier);
    const QImage &getErrorTexture() const;
    void setErrorTexture(const QImage &newErrorTexture);
    const QImage &getEmptyTexture() const;
    void setEmptyTexture(const QImage &newEmptyTexture);
    Game *getGame() const;
    void setGame(Game *newGame);
    bool getRender() const;
    void setRender(bool newRender);
    const nlohmann::json &getToDraw() const;
    void setToDraw(const nlohmann::json &newToDraw);
    const std::map<int, QImage> &getToDrawTextures() const;
    void setToDrawTextures(const std::map<int, QImage> &newToDrawTextures);
    bool getCopyingToDraw() const;
    void setCopyingToDraw(bool newCopyingToDraw);

private:
    QApplication *m_qApp;
    bool eventFilter(QObject *object, QEvent *event);

    Game* game = nullptr;
    int renderingMultiplier = 1; // Textures are rendered with their size being multiplied by this value
    QImage errorTexture;
    QImage emptyTexture;
    nlohmann::json toDraw;
    std::map<int, QImage> toDrawTextures;
    bool copyingToDraw = false;
    bool render;
};
#endif // MAINWINDOW_H
