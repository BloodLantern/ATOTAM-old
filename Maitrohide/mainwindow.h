#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication *app = nullptr, QWidget *parent = nullptr);
    ~MainWindow();
    virtual void paintEvent(QPaintEvent*);

private:
    Ui::MainWindow *ui;
    QPushButton *m_button;
    QApplication *qApp;
};
#endif // MAINWINDOW_H
