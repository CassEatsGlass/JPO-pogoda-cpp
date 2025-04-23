#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QVariant>
#include <QtCharts>

class MainWindow : public QObject {
    Q_OBJECT
    // Properties for storing data and exposing it to our qml
    Q_PROPERTY(QVariantList stations READ stations NOTIFY stationsChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariantList stationDetails READ stationDetails NOTIFY stationDetailsChanged)
    Q_PROPERTY(QVariantList stationMeasure READ stationMeasure NOTIFY stationMeasureChanged)

public:
    explicit MainWindow(QObject *parent = nullptr);
    ~MainWindow();

    // Getters
    QVariantList stations() const { return m_stations; }
    QString status() const { return m_status; }
    QVariantList stationDetails() const { return m_stationDetails; }
    QVariantList stationMeasure() const { return m_stationMeasure; }
    //Helpful for getting name of parameter
    QString  param;

public slots:
    // Fetching data from the API
    void fetchStations();
    void fetchStationDetails(int stationId);
    void fetchStationMeasure(int stationId);

    QVariantList findClosestStations(double lat, double lon, int count = 5);

signals:
    // Signals about changes for the qml files
    void stationsChanged();
    void statusChanged();
    void stationDetailsChanged();
    void stationMeasureChanged();

private slots:
    // Slot for API replies
    void onNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    // Maps for data
    QVariantList m_stations;
    QString m_status;
    QVariantList m_stationDetails;
    QVariantList m_stationMeasure;

    QString current_id; //mostly used for file creation
    //Standardizsed functions that work on local files, no matter the internet connection
    void readStations();
    void readStationDetails();
    void readStationMeasure();

    int requestType; // for handling offline users

    QString key; // same as param, but only used locally in the class

};

#endif // MAINWINDOW_H
