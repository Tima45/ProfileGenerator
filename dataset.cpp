#include "dataset.h"

Dataset::Dataset(QObject *parent) : QObject(parent)
{

}

Dataset::~Dataset()
{
    for(int i = 0; i < elements.count(); i++){
        delete elements[i];
    }
}

void Dataset::load(QString path)
{
    QDir dir(path);
    QFileInfoList list = dir.entryInfoList();
    for(int i = 0; i < list.count(); i++){
        if(list.at(i).fileName().endsWith(".png")){
            Mat pic = imread(list.at(i).filePath().toStdString(),IMREAD_GRAYSCALE);
            if(pic.data){

                DatasetElement *element = new DatasetElement();

                double maxAbs = 0;

                for(int x = 0; x < pic.cols; x++){
                    double summX = 0;
                    for(int y = 0; y < pic.rows; y++){
                        uchar value = pic.at<uchar>(y,x);
                        summX += (value);
                    }
                    element->xyProfile.append(summX);
                    if(fabs(summX) > maxAbs){
                        maxAbs = fabs(summX);
                    }
                }

                for(int y = 0; y < pic.rows; y++){
                    double summY = 0;
                    element->pic.append(QVector<double>());
                    for(int x = 0; x < pic.cols; x++){
                        uchar value = pic.at<uchar>(y,x);
                        element->pic.last().append((double)value/(double)256);
                        summY += value;
                    }
                    element->xyProfile.append(summY);
                    if(fabs(summY) > maxAbs){
                        maxAbs = fabs(summY);
                    }
                }
                for(int j = 0; j < element->xyProfile.count(); j++){
                    element->xyProfile[j] /= maxAbs;
                }

                QString newFileName = list.at(i).fileName();
                newFileName.chop(4);
                QStringList lists = newFileName.split(" ");
                bool ok = false;
                element->takeToAccount = lists.at(1).toDouble(&ok);
                if(!ok){
                    element->takeToAccount = 1;
                    qDebug() << "Dataset::load !ok";
                }
                elements.append(element);
            }else{
                qDebug() << "Dataset::load no pic data";
            }
        }
    }
}

void Dataset::clear()
{
    for(int i = 0; i < elements.count(); i++){
        delete elements[i];
    }
    elements.clear();
}
