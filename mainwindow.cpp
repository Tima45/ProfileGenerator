#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->acceptableErrorEdit->setMinimum(0.00000000001);
    ui->acceptableErrorEdit->setValue(0.00000000001);
    generator = new ProfileGenerator(128,this);
    initPlot();
    dataset = new Dataset(this);
    connect(this,SIGNAL(startTraining(double,int,double,Dataset*)),generator,SLOT(startTraining(double,int,double,Dataset*)));
    connect(this,SIGNAL(interaptTraining()),generator,SLOT(interapt()));
    connect(generator,SIGNAL(epochFinished(ulong,double,int)),this,SLOT(updateTrainingProgress(ulong,double,int)),Qt::QueuedConnection);
    connect(generator,SIGNAL(trainingFinished()),this,SLOT(trainingFinished()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initPlot()
{
    ui->plot->setNoAntialiasingOnDrag(true);
    ui->plot->setNotAntialiasedElement(QCP::AntialiasedElement::aeAll,true);
    for(int i = 0; i < generator->trainersCount;i++){
        errorGraphs.append(ui->plot->addGraph());
        errorGraphs.last()->addData(0,0);
        errorGraphs.last()->setPen(QPen(QColor(Qt::red)));
        QCPItemTracer *tr = new QCPItemTracer(ui->plot);
        tr->setStyle(QCPItemTracer::TracerStyle::tsCircle);
        tr->setPen(QPen(QColor(Qt::blue)));
        tr->setSize(4);
        tr->setGraph(errorGraphs.last());
        tracers.append(tr);

        QCPItemTracer *tr2 = new QCPItemTracer(ui->plot);
        QPen p;
        p.setStyle(Qt::DashLine);
        p.setColor(QColor(0,0,255,128));
        tr2->setPen(p);
        tr2->setGraph(errorGraphs.last());
        tracers2.append(tr2);
    }

    ui->plot2->setInteraction(QCP::iRangeDrag, true);
    ui->plot2->setInteraction(QCP::iRangeZoom, true);
    ui->plot2->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->plot2->axisRect()->setRangeZoom(Qt::Horizontal);
    xyProfileGraph = ui->plot2->addGraph();
    //tempGraph = ui->plot2->addGraph();



    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    ui->plot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    connect(ui->plot,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(changeAutoScaleMode()));
}

void MainWindow::changeAutoScaleMode()
{
    autoScale = !autoScale;
}

void MainWindow::on_startStopButton_clicked()
{
    if(!isTraining){
        if(!dataset->elements.isEmpty()){
            isTraining = true;
            ui->startStopButton->setText("Stop");
            ui->speedEdit->setEnabled(false);
            ui->acceptableErrorEdit->setEnabled(false);
            ui->epochCount->setEnabled(false);
            ui->saveGeneratorButton->setEnabled(false);
            ui->loadDataSetButton->setEnabled(false);
            ui->loadGeneratorButton->setEnabled(false);
            ui->tryButton->setEnabled(false);
            ui->diffButton->setEnabled(false);

            emit startTraining(ui->speedEdit->value(),ui->epochCount->value(),ui->acceptableErrorEdit->value(),dataset);
        }else{
            QMessageBox::critical(this,"Error","Dataset is epmty!");
        }
    }else{
        emit interaptTraining();
    }

}

void MainWindow::on_tryButton_clicked()
{
    lastPath = QFileDialog::getOpenFileName(this,"Try",lastPath,"Image files (*.png)");
    Mat sourcePic;
    sourcePic = imread(lastPath.toStdString(),IMREAD_GRAYSCALE);

    if(sourcePic.data){
        QVector<double> xProfile;
        QVector<double> yProfile;
        for(int y = 0; y < sourcePic.rows; y++){
            double summY = 0;
            for(int x = 0; x < sourcePic.cols; x++){
                uchar value = sourcePic.at<uchar>(y,x);
                summY += (value);
            }
            yProfile.append(summY);
        }
        for(int x = 0; x < sourcePic.cols; x++){
            double summX = 0;
            for(int y = 0; y < sourcePic.rows; y++){
                uchar value = sourcePic.at<uchar>(y,x);
                summX += (value);
            }
            xProfile.append(summX);
        }
        QVector<double> xyProfile;

        xyProfile.append(xProfile);
        xyProfile.append(yProfile);

        Mat pic = generator->generateProfile(xyProfile);
        imshow("Try",pic);
    }else{
        qDebug() << "on_tryButton_clicked:: no pic data";
    }


}

void MainWindow::on_loadDataSetButton_clicked()
{
    dataset->clear();
    QString path = QFileDialog::getExistingDirectory(this,"Select folder with pictures png dataset",lastPath);
    dataset->load(path);
    ui->dataSetCount->setText(QString::number(dataset->elements.count()));
}

void MainWindow::updateTrainingProgress(ulong currentEpoch, double error,int id)
{


    errorGraphs.at(id)->addData(currentEpoch,error);
    tracers.at(id)->setGraphKey(currentEpoch);
    tracers2.at(id)->setGraphKey(currentEpoch);


    int64 diff = getTickCount() - lastUpdate;
    if(diff > 50){
        if(id == 0){
            ui->currentEpochLabel->setText(QString::number(currentEpoch));
            ui->errorLabel->setText(QString::number(error));
        }

        if(autoScale){
            ui->plot->rescaleAxes();
        }
        ui->plot->replot();
        lastUpdate = getTickCount();
    }
}

void MainWindow::trainingFinished()
{
    ui->currentEpochLabel->setText(QString::number(generator->networks[0][0]->epochCount));

    isTraining = false;
    ui->startStopButton->setText("Start");
    ui->speedEdit->setEnabled(true);
    ui->acceptableErrorEdit->setEnabled(true);
    ui->epochCount->setEnabled(true);
    ui->saveGeneratorButton->setEnabled(true);
    ui->loadDataSetButton->setEnabled(true);
    ui->loadGeneratorButton->setEnabled(true);
    ui->tryButton->setEnabled(true);
    ui->diffButton->setEnabled(true);
}


void MainWindow::on_saveGeneratorButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this,"Save generator",lastPath,"Profile generator (*.pg)");
    generator->save(path);
}

void MainWindow::on_loadGeneratorButton_clicked()
{




    QString path = QFileDialog::getOpenFileName(this,"Load generator",lastPath,"Profile generator (*.pg)");
    generator->load(path);

    ui->currentEpochLabel->setText(QString::number(generator->networks[0][0]->epochCount));

    for(int i = 0; i < generator->trainersCount;i++){
        errorGraphs.at(i)->clearData();
        //errorGraphs.at(i)->addData(generator->networks[0][0]->epochCount,0);
        //tracers.at(i)->setGraphKey(generator->networks[0][0]->epochCount);
        //tracers2.at(i)->setGraphKey(generator->networks[0][0]->epochCount);
    }
}

void MainWindow::on_diffButton_clicked()
{
    if(!dataset->elements.isEmpty()){
        QVector<QVector<double>> resultPic;
        for(unsigned int y = 0; y < generator->profileSize; y++){
            resultPic.append(QVector<double>());
            for(unsigned int x = 0; x < generator->profileSize; x++){
                resultPic[y].append(0);
            }
        }

        for(int i = 0; i < dataset->elements.count(); i++){
            for(unsigned int y = 0; y < generator->profileSize; y++){
                for(unsigned int x = 0; x < generator->profileSize; x++){
                    double pixelValue = generator->networks.at(y).at(x)->work(dataset->elements.at(i)->xyProfile.constData(),dataset->elements.at(i)->xyProfile.count());
                    resultPic[y][x] += fabs(dataset->elements.at(i)->pic[y][x] - pixelValue)/dataset->elements.count();

                }
            }
        }
        Mat newPic(generator->profileSize,generator->profileSize,CV_8UC1);
        for(unsigned int y = 0; y < generator->profileSize; y++){
            for(unsigned int x = 0; x < generator->profileSize; x++){
                double pixelValue = resultPic[y][x];
                newPic.at<uchar>(y,x) = (uchar)(pixelValue*255);
            }
        }
        imshow("Diff",newPic);
    }else{
        QMessageBox::critical(this,"Error","Dataset is epmty!");
    }
}

void MainWindow::on_tryRealProfileButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,"Select .csv file",lastPath,"Profile data (*.csv)");
    if(!filePath.isEmpty()){
        if(profile == nullptr){
            profile->deleteLater();
        }
        profile = new ProfileData(filePath,this);
        if(!profile->xyProfile.isEmpty()){
            QVector<double> keys;
            for(int i = 0; i < profile->xyProfile.count(); i++){
                keys.append(i);
            }
            xyProfileGraph->clearData();
            xyProfileGraph->setData(keys,profile->xyProfile);
            ui->plot2->rescaleAxes();
            ui->plot2->replot();


            Mat pic = generator->generateProfile(profile->xyProfile);
            imshow("Try",pic);

            qDebug() << profile->xyProfile.count();
        }else{
            qDebug() << "Empty";
        }
    }
}
