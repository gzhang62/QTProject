#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QSizePolicy>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->lines, &QCheckBox::stateChanged, this, &MainWindow::setLines);
    resize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resize()
{
    QSize windowSize = this->size();
    int windowHeight = windowSize.height();
    int windowWidth = windowSize.width();
    int splitter1Factor = windowHeight * 0.7;
    int plainTextFactor = windowHeight * 0.3;
    int groupTextFactor = windowWidth * 0.2;
    int glWidgetFactor = windowWidth * 0.8;

    QList<int> splitter2List = {splitter1Factor, plainTextFactor};
    QList<int> splitter1List = {groupTextFactor, glWidgetFactor};

    ui->splitter_2->setSizes(splitter2List);
    ui->splitter->setSizes(splitter1List);
}

void MainWindow::setLines()
{
    ui->openGLWidget->setMode(ui->lines->isChecked());
}

void MainWindow::openFile()
{
    QFileDialog dialog(this);
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), "",
        tr("Images (*.ply)"));

    if (fileName.isEmpty())
           return;
    else{
        ui->openGLWidget->readPoly(fileName);
    }
}






