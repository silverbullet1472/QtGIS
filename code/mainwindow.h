#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "mywidget.h"
#include "geomap.h"
#include <qinputdialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    MyWidget *myWidget;

private slots:
    //task1
    void loadE00File();
    void generateRoute();
    //task2
    void loadDemandShp();
    void loadSupplyShp();
    void calculateAccessibility();
    //task3
    void loadPointShp();
    void calculateKde();
};

#endif // MAINWINDOW_H
