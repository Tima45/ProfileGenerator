#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QTime>
#include <cv.hpp>
#include "plot/qcustomplot.h"
#include "profilegenerator.h"
#include "dataset.h"
#include "profiledata.h"

namespace Ui {
class MainWindow;
}

using namespace cv;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initPlot();

    QVector<QCPGraph*> errorGraphs;
    QVector<QCPItemTracer*> tracers;
    QVector<QCPItemTracer*> tracers2;

    QCPGraph *xyProfileGraph;

    ProfileGenerator *generator;
    Dataset *dataset;

    int64 lastUpdate = 0;
    bool isTraining = false;


    ProfileData *profile = nullptr;

signals:
    void startTraining(double speed, int epochCount, double acceptableError,Dataset *dataset);
    void interaptTraining();
private slots:
    void changeAutoScaleMode();
    void on_startStopButton_clicked();

    void on_tryButton_clicked();

    void on_loadDataSetButton_clicked();
    void updateTrainingProgress(ulong currentEpoch, double error, int id);
    void trainingFinished();


    void on_saveGeneratorButton_clicked();

    void on_loadGeneratorButton_clicked();

    void on_diffButton_clicked();

    void on_tryRealProfileButton_clicked();

private:
    Ui::MainWindow *ui;
    QString lastPath = "";
    bool autoScale = true;
};

#endif // MAINWINDOW_H
