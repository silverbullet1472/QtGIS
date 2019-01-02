#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mywidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myWidget = new MyWidget();
    this->setCentralWidget(myWidget);
    QObject::connect(this->ui->actionLoadE00File, SIGNAL(triggered()),this, SLOT(loadE00File()));
    QObject::connect(this->ui->actionGenerateRoute, SIGNAL(triggered()),this, SLOT(generateRoute()));
    QObject::connect(this->ui->actionLoadDemandShp, SIGNAL(triggered()),this, SLOT(loadDemandShp()));
    QObject::connect(this->ui->actionLoadSupplyShp, SIGNAL(triggered()),this, SLOT(loadSupplyShp()));
    QObject::connect(this->ui->actionCalculateAccessibility, SIGNAL(triggered()),this, SLOT(calculateAccessibility()));
    QObject::connect(this->ui->actionLoadPointShp, SIGNAL(triggered()),this, SLOT(loadPointShp()));
    QObject::connect(this->ui->actionCalculateKde, SIGNAL(triggered()),this, SLOT(calculateKde()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete myWidget;
}

void MainWindow::loadE00File()
{
    if(myWidget->map!=nullptr) delete myWidget->map;
    myWidget->map= new GeoMap();
    myWidget->map->loadE00File();
    myWidget->map->task=1;
    myWidget->update();
}

void MainWindow::generateRoute()
{
    bool startInputed;
    int startNode = QInputDialog::getInt(this, "Input Dialog","start node:", 0, 0, 100, 1, &startInputed);
    bool endInputed;
    int endNode = QInputDialog::getInt(this, "Input Dialog","end node:", 0, 0, 100, 1, &endInputed);
    if (startInputed&&endInputed)
    {
        myWidget->map->generateRoute(startNode,endNode);
        myWidget->map->task=1;
        myWidget->update();
    }
}

void MainWindow::loadDemandShp()
{
    if(myWidget->map!=nullptr) delete myWidget->map;
    myWidget->map= new GeoMap();
    myWidget->map->loadDemandShp();
    myWidget->map->task=2;
    myWidget->update();
}

void MainWindow::loadSupplyShp()
{
    myWidget->map->loadSupplyShp();
    myWidget->map->task=2;
    myWidget->update();
}

void MainWindow::calculateAccessibility()
{
    myWidget->map->calculateAccessibility();
    myWidget->map->task=2;
    myWidget->update();
}

void MainWindow::loadPointShp()
{
    if(myWidget->map!=nullptr) delete myWidget->map;
    myWidget->map= new GeoMap();
    myWidget->map->loadPointShp();
    myWidget->map->task=3;
    myWidget->update();
}

void MainWindow::calculateKde()
{
    myWidget->map->calculateKde();
    myWidget->map->task=3;
    myWidget->update();
}
