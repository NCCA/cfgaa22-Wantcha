#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_scene = new NGLScene();

    m_ui->mainGridLayout->addWidget(m_scene);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
