#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QApplication *app)
    : ui(new Ui::MainWindow)
    , m_qApp(app)
    , rendering({})
{
    ui->setupUi(this);
    // Set size of the window
    setFixedSize(1000, 500);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawRect(10,10,10,10);
    for(Renderable *renderable : rendering){
        painter.drawImage(QPoint(renderable->getX(), renderable->getY()), renderable->getTexture());
    }
    painter.end();
}

void MainWindow::addRenderable(Renderable *renderable)
{
    rendering.push_back(renderable);
}

void MainWindow::clearRendering()
{
    rendering = {};
}

