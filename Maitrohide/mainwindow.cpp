#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Set size of the window
    setFixedSize(1000, 500);

    // Create and position the button
    m_button = new QPushButton(QObject::tr("Quit"), this);
    m_button->setGeometry(100, 100, 80, 30);

    // NEW : Do the connection
    connect(m_button, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_button;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawRect(10, 10, 20, 20);
}

