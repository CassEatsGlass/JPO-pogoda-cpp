#include "mainwindow.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <float.h>
#include <cmath>
#include <algorithm>
#include <QDir>


MainWindow::MainWindow(QObject *parent) : QObject(parent), m_networkManager(new QNetworkAccessManager(this)) {
    // Connecting the finished signal to our slot
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onNetworkReply);
    m_status = "Ładowanie danych...";
    emit statusChanged();

}


MainWindow::~MainWindow() {
    delete m_networkManager;
}


void MainWindow::fetchStations() {
    requestType = 0;
    QUrl url("https://api.gios.gov.pl/pjp-api/rest/station/findAll");
    QNetworkRequest request(url);
    m_networkManager->get(request);
}
void MainWindow::fetchStationDetails(int stationId) {
    requestType = 1;
    current_id = QString::number(stationId);
    QString url = QString("https://api.gios.gov.pl/pjp-api/rest/station/sensors/%1").arg(stationId);
    QNetworkRequest request(url);
    m_networkManager->get(request);
}
void MainWindow::fetchStationMeasure(int stationId){
    requestType = 2;
    current_id = QString::number(stationId);
    QString url = QString("https://api.gios.gov.pl/pjp-api/rest/data/getData/%1").arg(stationId);
    QNetworkRequest request(url);
    m_networkManager->get(request);
}


void MainWindow::readStations(){

    QFile stationFile("data/stations.json");

    if (stationFile.open(QIODevice::ReadOnly)){ //opening local file

        QByteArray fileData = stationFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        QJsonArray stationsArray = doc.array();

        for (const QJsonValue &value : stationsArray) { //loading to cache

            QJsonObject station = value.toObject();
            QVariantMap stationData;

            stationData["stationId"] = station["id"].toInt();
            stationData["stationName"] = station["stationName"].toString();
            stationData["lat"] = station["gegrLat"].toString();
            stationData["lon"] = station["gegrLon"].toString();
            stationData["address"] = station["addressStreet"].toString();

            m_stations.append(stationData);
        }
    }

    m_status = "Stacje pomiarowe: ";
    //sending singals to our qml files
    emit statusChanged();
    emit stationsChanged();
}


void MainWindow::readStationDetails(){
    QFile stationFile("data/station_details-"+ current_id +".json");
    if(stationFile.open(QIODevice::ReadOnly)){

        QByteArray fileData = stationFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(fileData);
        QJsonArray stationsArray = doc.array();
        //qDebug() << "Details:" <<stationsArray;

        for (const QJsonValue &value : stationsArray) {  //appending the data to our map

                QJsonObject station = value.toObject();
                QJsonObject param = station["param"].toObject();

                QVariantMap stationDetails;
                stationDetails["id"] = station["id"].toInt();
                stationDetails["paramName"] = param["paramName"].toString();
                stationDetails["paramFormula"] = param["paramFormula"].toString();
                m_stationDetails.append(stationDetails);
        }
    }
    qDebug() <<  m_stationDetails;
    //sending the singal to our qml files
    emit stationDetailsChanged();
}


void MainWindow::readStationMeasure(){
    QFile stationMeasureFile("data/station_measure-"+ current_id +".json");
    QJsonArray existingValues;

    if (stationMeasureFile.open(QIODevice::ReadOnly)) {  //appending the data to our map
        qDebug() << "Opening file";
        QJsonDocument existingDoc = QJsonDocument::fromJson(stationMeasureFile.readAll());
        existingValues = existingDoc.array();
        stationMeasureFile.close();
    }

    //qDebug() << "existing:" << existingMeasure;
    QVariantMap stationMeasure;
    stationMeasure["key"] = key;

    double max = 0.0;
    double min = DBL_MAX;
    double sum = 0;
    double i = 0;
    double extraSum = 0;
    double extraSum2 = 0;
    QJsonValue extraValue;
    for (const QJsonValue &value : existingValues) {

        QDateTime dateTime = QDateTime::fromString(value["date"].toString(), "yyyy-MM-dd HH:mm:ss");
        qint64 timestamp = dateTime.toMSecsSinceEpoch();
        if(value["value"].isNull()) continue; // in case a measurement was not taken
        double val = value["value"].toDouble();
        //qDebug() << val;
        stationMeasure["date"] = timestamp;
        stationMeasure["value"] = val;

        sum += val;
        i++;
        if(val > max){
            max = val;
            qDebug() << max;
        }
        if(val < min){
            min = val;
            qDebug() << min;
        }
        m_stationMeasure.append(stationMeasure);
        //qDebug() << "StationMeasure count:" << m_stationMeasure.count();
    }
    // Very stupid way to get the trend (might not be accurate) :3
    for(int j = 0;j<10;j++){
        extraValue = existingValues[j];
        extraSum += extraValue["value"].toDouble();
        if(j>5){
            extraSum2 += extraValue["value"].toDouble();
        }
    }
    //adding all the extra data to the last measurement in our map, it could be done any other way but this works fine
    extraSum = extraSum/10;
    extraSum2 = extraSum2/5;
    stationMeasure["max"] = max;
    stationMeasure["min"] = min;
    stationMeasure["avg"] = round(sum/i * 100.0) / 100.0;
    stationMeasure["trend"] = extraSum >= extraSum2 ? "rosnący" : "malejący";
    m_stationMeasure.append(stationMeasure);

    //sending singals to our qml files

    emit stationMeasureChanged();
    stationMeasureFile.close();
}

// Used by findClosestStations
static double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth radius in km
    double dLat = qDegreesToRadians(lat2 - lat1);
    double dLon = qDegreesToRadians(lon2 - lon1);

    lat1 = qDegreesToRadians(lat1);
    lat2 = qDegreesToRadians(lat2);

    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::sin(dLon / 2) * std::sin(dLon / 2) * std::cos(lat1) * std::cos(lat2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return R * c;
}

QVariantList MainWindow::findClosestStations(double lat, double lon, int count) {
    struct StationDistance {
        QVariantMap station;
        double distance;
    };

    QList<StationDistance> distances;

    for (const QVariant &entry : m_stations) {
        QVariantMap station = entry.toMap();
        double stationLat = station["lat"].toString().toDouble();
        double stationLon = station["lon"].toString().toDouble();
        double dist = haversine(lat, lon, stationLat, stationLon);
        distances.append({station, dist});
    }

    std::sort(distances.begin(), distances.end(), [](const StationDistance &a, const StationDistance &b) {
        return a.distance < b.distance;
    });

    QVariantList closest;
    for (int i = 0; i < qMin(count, distances.size()); ++i) {
        QVariantMap s = distances[i].station;
        s["distance"] = QString::number(distances[i].distance, 'f', 2) + " km";
        closest.append(s);
    }

    return closest;
}

void MainWindow::onNetworkReply(QNetworkReply *reply) {

    //Check for errors and internet connection
    if (reply->error() != QNetworkReply::NoError) {
        m_status = "Brak połączenia z internetem";
        emit statusChanged();
        reply->deleteLater();
        switch(requestType){
            case 0:
                readStations();
                break;
            case 1:
                readStationDetails();
                break;
            case 2:
                readStationMeasure();
                break;
            default:
                m_status = "Critical error";
                emit statusChanged();
        }

        return;
    }

    //Creating directory
    QString path = "data";
    QDir dir(path);
    if (!dir.exists()) {
        if (QDir().mkdir(path)) {
            qDebug() << "Directory created:" << path;
        } else {
            qDebug() << "Failed to create directory:" << path;
        }
    } else {
        qDebug() << "Directory already exists:" << path;
    }

    //Saving data
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());

    if (doc.isArray()) {
        QJsonArray stationsArray = doc.array();

        if (stationsArray.size() > 0 && stationsArray[0].toObject().contains("stationName")) {  //getting list of stations
            m_stations.clear();
            QFile stationFile("data/stations.json");
            QJsonArray existingArray;

            if (stationFile.open(QIODevice::ReadOnly)) {
                QJsonDocument existingDoc = QJsonDocument::fromJson(stationFile.readAll());
                existingArray = existingDoc.array();
                stationFile.close();
                //qDebug() << "existing:" << existingArray;
            }

            for (const QJsonValue &value : stationsArray) {

                if(!existingArray.contains(value)){
                    existingArray.append(value);
                }

                if (stationFile.open(QIODevice::WriteOnly)) {
                    stationFile.write(QJsonDocument(existingArray).toJson(QJsonDocument::Indented));
                    stationFile.close();
                }
            }

            readStations();

        } else if (stationsArray[0].toObject().contains("param")) { //getting details of a station
            m_stationDetails.clear();
            QString stationId = QString::number(stationsArray[0].toObject()["stationId"].toInt());
            QFile stationDetailsFile("data/station_details-"+ stationId +".json");
            QJsonArray existingArray;

            if (stationDetailsFile.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(stationDetailsFile.readAll());
                existingArray = doc.array();
                stationDetailsFile.close();
                //qDebug() << "existing:" << existingArray;
            }

                for (const QJsonValue &value : stationsArray) {
                    if(!existingArray.contains(value)){
                        existingArray.append(value);
                    }
                }

            if (stationDetailsFile.open(QIODevice::WriteOnly)) {
                stationDetailsFile.write(QJsonDocument(existingArray).toJson(QJsonDocument::Indented));
                stationDetailsFile.close();
            }

            qDebug() << "Wywołuje detale";
            readStationDetails();

        }
    }
        else if (doc.isObject()){  //If you ever want more API request, this line will almost surely need to be changed
            m_stationMeasure.clear();
            QFile stationMeasureFile("data/station_measure-"+ current_id +".json");
            QJsonArray existingArray;
            QJsonObject existingMeasure;

            if (stationMeasureFile.open(QIODevice::ReadOnly)) {
                QJsonDocument existingDoc = QJsonDocument::fromJson(stationMeasureFile.readAll());
                existingMeasure = existingDoc.object();
                stationMeasureFile.close();
                //qDebug() << "existing:" << existingArray;
            }

            QJsonArray existingValues = existingMeasure["values"].toArray();
            //qDebug() << "Existing onreply:" << existingValues[0];
            QJsonObject Data = doc.object();
            key = Data["key"].toString();

            for (const QJsonValue &value : Data["values"].toArray()) {

                if(!existingValues.contains(value)){
                    //qDebug() << existingValues;
                    existingValues.append(value);
                }

            }

            if (stationMeasureFile.open(QIODevice::WriteOnly)) {
                stationMeasureFile.write(QJsonDocument(existingValues).toJson(QJsonDocument::Indented));
                stationMeasureFile.close();
            }

            readStationMeasure();
        }

}
