#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPainter>
#include <vector>
#include "Entities/entity.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication *app);
    ~MainWindow();
    virtual void paintEvent(QPaintEvent*);
    void addRenderable(Entity *entity);
    void clearRendering();
    void updatePhysics(double framerate);
    void updateAnimations();

    const std::vector<Entity *> &getRendering() const;
    void setRendering(const std::vector<Entity *> &newRendering);

private:
    Ui::MainWindow *ui;
    QApplication *m_qApp;
    std::vector<Entity*> rendering;
};
#endif // MAINWINDOW_H
