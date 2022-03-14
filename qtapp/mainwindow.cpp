#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent, QVector<double>* x, QVector<double>* y0, QVector<double>* y1) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setGeometry(100, 100, 1000, 600);
    ui->customPlot->addGraph();
    ui->customPlot->setBackground(Qt::black);
    ui->customPlot->axisRect()->setBackground(Qt::black);

    ui->customPlot->graph(0)->setPen(QPen(Qt::red, 1));
    ui->customPlot->addGraph();
    ui->customPlot->graph(1)->setPen(QPen(Qt::cyan, 1));

    ui->customPlot->xAxis->setTickLabelColor(QColor(255, 255, 255));
    ui->customPlot->yAxis->setTickLabelColor(QColor(255, 255, 255));

    QSharedPointer<QCPAxisTickerFixed> yfixedTicker(new QCPAxisTickerFixed);
    ui->customPlot->yAxis->setTicker(yfixedTicker);
    yfixedTicker->setTickStep(10.0);
    yfixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);

    QSharedPointer<QCPAxisTickerFixed> xfixedTicker(new QCPAxisTickerFixed);
    ui->customPlot->xAxis->setTicker(xfixedTicker);
    xfixedTicker->setTickStep(20.0);
    xfixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);

    ui->customPlot->xAxis->grid()->setPen(QPen(Qt::gray, 0.125));
//    ui->customPlot->xAxis->grid()->setVisible(false);
    ui->customPlot->yAxis->grid()->setPen(QPen(Qt::gray, 0.125));

    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->customPlot->graph(0)->setData(*x, *y0);
    ui->customPlot->graph(1)->setData(*x, *y1);
    ui->customPlot->graph(0)->rescaleAxes();
    ui->customPlot->graph(1)->rescaleAxes(true);
    ui->customPlot->replot();
    showFullScreen();
}

MainWindow::~MainWindow()
{
    delete ui;
}





































