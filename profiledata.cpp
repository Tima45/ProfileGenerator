#include "profiledata.h"
#include "gsl/gsl_spline.h"


ProfileData::ProfileData(QObject *parent) : QObject(parent)
{

}

ProfileData::ProfileData(QString inFileName, QObject *parent) : QObject(parent)
{
    fileName = inFileName;

    errors = false;
    readData(fileName);
    if(!errors){
        findBorders();
        loadFrames();
    }
}

ProfileData::~ProfileData()
{

}


void ProfileData::readData(QString fileName)
{
    if(!QFileInfo(fileName).isFile()){
        errors = true;
        return;
    }
    if(!fileName.endsWith(".csv")){
        errors = true;
        return;
    }
    QFile f(fileName);
    if(!f.open(QIODevice::ReadOnly | QIODevice::Text)){
        errors = true;
        return;
    }
    //---------------------------------------------------------------------------------------
    rawAmpers.clear();
    rawAngles.clear();
    //---------------------------------------------------------------------------------------
    bool findData = false;
    QRegExp dateTampl("Scan Start Time*");
    dateTampl.setPatternSyntax(QRegExp::Wildcard);
    QRegExp dataStartTempl("Scan Angle (degrees),Current (A)*");
    dataStartTempl.setPatternSyntax(QRegExp::Wildcard);
    if(!f.atEnd()){
        while(!findData || !f.atEnd()){
            QString rawLine = f.readLine();
            if(rawLine == "") break;
            if(dateTampl.exactMatch(rawLine)){
                rawLine.remove(0,17);
                rawLine.chop(2);
                scanDate = rawLine;
            }
            if(dataStartTempl.exactMatch(rawLine)){
                findData = true;
                break;
            }
        }
        //---------------------------------------------------------------------------------------
        if(findData){
            while(!f.atEnd()){
                QString rawLine = f.readLine();
                QStringList slpittedLine = rawLine.split(",");
                if(slpittedLine.count() == 2){
                    //---------------------------------------------------------------
                    if(slpittedLine[0] != "NaN"){
                        bool good = false;
                        rawAngles.append(slpittedLine[0].toDouble(&good));
                        if(!good){
                            f.close();
                            return;
                        }
                    }else{
                        if(!rawAngles.isEmpty()){
                            rawAngles.append(rawAngles.last());
                        }else{
                            rawAngles.append(-14);
                        }
                    }
                    //---------------------------------------------------------------
                    bool good = false;

                    rawAmpers.append(slpittedLine[1].toDouble(&good));
                    if(!good){
                        f.close();
                        errors = true;
                        return;
                    }
                } else{
                    f.close();
                    errors = true;
                    return;
                }
            }
        }else{
            errors = true;
        }
    }
    f.close();
}

void ProfileData::findBorders()
{
    borders.clear();
    if(!rawAngles.isEmpty()){
        QVector<double> smoothAngles = denoiseAngles(rawAngles);
        if(!smoothAngles.isEmpty()){
            for(int i = 1; i < smoothAngles.count()-1; i++){
                if( (smoothAngles.at(i-1) > smoothAngles.at(i) && smoothAngles.at(i+1) > smoothAngles.at(i)) || (smoothAngles.at(i-1) < smoothAngles.at(i) && smoothAngles.at(i+1) < smoothAngles.at(i))){
                    if(!borders.isEmpty()){
                        if(rawAngles.at(borders.last())*rawAngles.at(i) < 0){
                            borders.append(i);
                        }
                    }else{
                        borders.append(i);
                    }
                }
            }
        }
    }
}

void ProfileData::loadFrames()
{
    if(!rawAngles.isEmpty() && !rawAmpers.isEmpty() && borders.count() > 1){
        int start = borders.at(0);
        int stop = borders.at(1);

        QVector<double> angles;
        QVector<double> ampers;
        QVector<int> normAmpers;

        for(int i = start; i < stop; i++){
            angles.append(rawAngles.at(i));
            ampers.append(fabs(rawAmpers.at(i)));
        }


        int zeroAngleIndex = 0;

        double minAbs = fabs(angles.first());
        for(int i = 0; i < angles.count(); i++){
            if(fabs(angles.at(i)) < minAbs){
                minAbs = fabs(angles.at(i));
                zeroAngleIndex = i;
            }
        }

        double maxFabsvalue = 0;
        for(int i = 0; i < ampers.count(); i++){
            if(fabs(ampers.at(i)) > maxFabsvalue){
                maxFabsvalue = fabs(ampers.at(i));
            }
        }

        for(int i = 0; i < ampers.count(); i++){
            normAmpers.append((int)(ampers.at(i)/maxFabsvalue*5));
            if(normAmpers.last() != 0){
                if(i < zeroAngleIndex){
                    xProfile.append(ampers.at(i));
                }else{
                    yProfile.append(ampers.at(i));
                }
            }
        }

        minAbs = fabs(xProfile.first());
        for(int i = 1; i < xProfile.count(); i++){
            if(fabs(xProfile.at(i)) < minAbs){
                minAbs = fabs(xProfile.at(i));
            }
        }
        for(int i = 0; i < xProfile.count(); i++){
            xProfile[i] -= minAbs;
        }

        minAbs = fabs(yProfile.first());
        for(int i = 1; i < yProfile.count(); i++){
            if(fabs(yProfile.at(i)) < minAbs){
                minAbs = fabs(yProfile.at(i));
            }
        }
        for(int i = 0; i < yProfile.count(); i++){
            yProfile[i] -= minAbs;
        }



        qDebug() << xProfile.count();
        qDebug() << yProfile.count();

        resize(xProfile,124);
        resize(yProfile,124);

        xProfile.prepend(0);
        xProfile.prepend(0);
        xProfile.append(0);
        xProfile.append(0);

        yProfile.prepend(0);
        yProfile.prepend(0);
        yProfile.append(0);
        yProfile.append(0);

        qDebug() << xProfile.count();
        qDebug() << yProfile.count();

        xyProfile.append(xProfile);
        xyProfile.append(yProfile);
    }
}

void ProfileData::resize(QVector<double> &vector, int size)
{
    if(vector.count() > size){
        qDebug() << "lol!";
    }

    if(!vector.isEmpty()){
        int lastSize = vector.count();
        double *x = new double[lastSize];
        for(int i = 0; i < lastSize; i++){
            x[i] = i;
        }
        gsl_interp_accel *acc = gsl_interp_accel_alloc();
        gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline,lastSize);
        gsl_spline_init(spline,x,vector.constData(),lastSize);

        vector.clear();
        for(int i = 0; i < size; i++){
            double yi = gsl_spline_eval(spline,i*(lastSize/(double)size), acc);
            yi < 0? yi = 0 : yi;
            vector.append(yi);
        }

        gsl_spline_free(spline);
        gsl_interp_accel_free(acc);
    }
}

QVector<double> ProfileData::denoiseAngles(QVector<double> angles)
{
    WaveletSpectrum dwt(angles,WaveletSpectrum::BSPLINE_309);
    dwt.highFilter(0);
    return dwt.toData();
}

