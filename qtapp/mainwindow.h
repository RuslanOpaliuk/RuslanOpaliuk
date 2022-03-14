#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
Q_OBJECT
  
public:
    explicit MainWindow(QWidget* parent = 0, QVector<double>* _x = 0, QVector<double>* _y0 = 0, QVector<double>* _y1 = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
