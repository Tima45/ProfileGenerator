#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    generator = new ProfileGenerator(128);
    f = new QThread(this);
    generator->moveToThread(f);
    f->start();
    initPlot();
    dataset = new Dataset(this);

    qRegisterMetaType<QVector<double> >("QVector<double>");

    connect(generator,SIGNAL(csvEpochFinished(int)),this,SLOT(updateEpoch(int)));
    connect(this,SIGNAL(startTraining(double,int,double,Dataset*)),generator,SLOT(startTraining(double,int,double,Dataset*)),Qt::QueuedConnection);
    connect(generator,SIGNAL(showProfiles(QVector<double>,QVector<double>)),this,SLOT(showProfiles(QVector<double>,QVector<double>)));
    connect(this,SIGNAL(interaptTraining()),generator,SLOT(interapt()),Qt::DirectConnection);
    connect(generator,SIGNAL(epochFinished(ulong,double,int)),this,SLOT(updateTrainingProgress(ulong,double,int)),Qt::QueuedConnection);
    connect(generator,SIGNAL(trainingFinished()),this,SLOT(trainingFinished()));


}

MainWindow::~MainWindow()
{
    delete ui;
    f->terminate();
    generator->deleteLater();
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
    xyResultProfileGraph = ui->plot2->addGraph();
    xyResultProfileGraph->setPen(QPen(QColor(Qt::red)));
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
            ui->epochCount->setEnabled(false);
            ui->saveGeneratorButton->setEnabled(false);
            ui->loadDataSetButton->setEnabled(false);
            ui->loadGeneratorButton->setEnabled(false);
            ui->tryButton->setEnabled(false);
            ui->diffButton->setEnabled(false);

            emit startTraining(ui->speedEdit->value(),ui->epochCount->value(),1,dataset);
        }else{
            QMessageBox::critical(this,"Error","Dataset is epmty!");
        }
    }else{
        emit interaptTraining();
    }

}

void MainWindow::on_tryButton_clicked()
{
    lastPath = QFileDialog::getOpenFileName(this,"Try",lastPath,"Image or csv files (*.png *.csv)");

    if(lastPath.endsWith(".csv")){
        if(profile == nullptr){
            profile->deleteLater();
        }
        profile = new ProfileData(lastPath,this);
        if(!profile->xyProfile.isEmpty()){
            QVector<double> keys;
            for(int i = 0; i < profile->xyProfile.count(); i++){
                keys.append(i);
            }
            xyProfileGraph->clearData();
            xyProfileGraph->setData(keys,profile->xyProfile);

            Mat pic = generator->generateProfile(profile->xyProfile);
            imshow("Try",pic);

            QVector<double> xProfile;
            QVector<double> yProfile;
            for(int y = 0; y < pic.rows; y++){
                double summY = 0;
                for(int x = 0; x < pic.cols; x++){
                    uchar value = pic.at<uchar>(y,x);
                    summY += (value);
                }
                yProfile.append(summY);
            }
            for(int x = 0; x < pic.cols; x++){
                double summX = 0;
                for(int y = 0; y < pic.rows; y++){
                    uchar value = pic.at<uchar>(y,x);
                    summX += (value);
                }
                xProfile.append(summX);
            }



            QVector<double> xyProfile;
            xyProfile.append(xProfile);
            xyProfile.append(yProfile);

            double maxAbs = 0;
            for(int i = 0; i < xyProfile.count(); i++){
                if(fabs(xyProfile.at(i)) > maxAbs){
                    maxAbs = fabs(xyProfile.at(i));
                }
            }
            for(int i = 0; i < xyProfile.count(); i++){
                xyProfile[i] /= maxAbs;
            }

            xyResultProfileGraph->setData(keys,xyProfile);


            ui->plot2->rescaleAxes();
            ui->plot2->replot();
            qDebug() << profile->xyProfile.count();

        }else{
            qDebug() << "Empty";
        }
    }else if(lastPath.endsWith(".png")){


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


            double maxAbs = 0;
            for(int i = 0; i < xyProfile.count(); i++){
                if(fabs(xyProfile.at(i)) > maxAbs){
                    maxAbs = fabs(xyProfile.at(i));
                }
            }
            for(int i = 0; i < xyProfile.count(); i++){
                xyProfile[i] /= maxAbs;
            }


            Mat pic = generator->generateProfile(xyProfile);
            imshow("Try",pic);
        }else{
            qDebug() << "on_tryButton_clicked:: no pic data";
        }
    }


}

void MainWindow::on_loadDataSetButton_clicked()
{
    dataset->clear();
    QString path = QFileDialog::getExistingDirectory(this,"Select folder with pictures png or csv dataset",lastPath);
    dataset->load(path);
    ui->dataSetCount->setText(dataset->datasetType+" "+QString::number(dataset->elements.count()));
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
    if(!dataset->elements.isEmpty() && dataset->datasetType == "pic"){
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
        QMessageBox::critical(this,"Error","Dataset is epmty and must be pic!");
    }
}


void MainWindow::showProfiles(QVector<double> expected, QVector<double> real)
{
    QVector<double> keys;
    for(int i = 0; i < 256; i++){
        keys.append(i);
    }
    xyProfileGraph->clearData();
    xyProfileGraph->setData(keys,expected);
    xyResultProfileGraph->clearData();
    xyResultProfileGraph->setData(keys,real);
    ui->plot2->rescaleAxes();
    ui->plot2->replot();
}

void MainWindow::updateEpoch(int value)
{
    ui->currentEpochLabel->setText(QString::number(value));
}
