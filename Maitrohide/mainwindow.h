#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPainter>
#include <vector>
#include "Rendering/renderable.h"

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
    void addRenderable(Renderable *renderable);
    void clearRendering();

private:
    Ui::MainWindow *ui;
    QApplication *m_qApp;
    std::vector<Renderable*> rendering;
};
#endif // MAINWINDOW_H
