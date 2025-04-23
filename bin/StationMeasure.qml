import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.15


Item {

    property var station
    property var date: new Date()
    width: 1000
    height: 800


    Component.onCompleted: {
        mainWindow.fetchStationMeasure(station);
        console.log("The data is with us");
        appWindow.width = 1180
        appWindow.height = 800
    }


    ChartView {
        id: chartView
        anchors.fill: parent
        legend.visible: false
        antialiasing: true
        title: mainWindow.param + " w powietrzu"

        DateTimeAxis {
            id: axisX
            tickCount: 10
            format: "dd-MM HH:mm"
            titleText: "Data"
            min: new Date()
            max: new Date()
        }

        ValuesAxis {
            id: axisY
            titleText: mainWindow.stationMeasure[0].key
            min: mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].min - mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].min * 2/10
            max: mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].max + mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].max * 1/10
            tickCount:  10
        }

        LineSeries {
            id: lineSeries
            //name: "Measurement"

            pointsVisible: true
            axisX: axisX
            axisY: axisY
            useOpenGL: false

            Connections {
                target: mainWindow
                function onStationMeasureChanged() {
                    lineSeries.clear();
                    console.log("MeasureLength:", mainWindow.stationMeasure.length);
                    for (var i = mainWindow.stationMeasure.length - 1; i >= 0; --i) {
                        let point = mainWindow.stationMeasure[i];

                        //console.log(point.date, " | " , point.value);
                        lineSeries.append(point.date, point.value);
                    }
                    if (mainWindow.stationMeasure.length > 1) {
                                            axisX.min = new Date(mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].date);
                                            axisX.max = new Date(mainWindow.stationMeasure[0].date);
                                        }
                    //console.log("max: ", mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].max);
                }
            }
        }
    }

    Rectangle{

        anchors.left: chartView.right
        anchors.verticalCenter: chartView.verticalCenter
        width: 160
        height: 250
        color: parent.color

        Column{
            id: significant
            anchors.fill: parent
            width: parent.width
            height: parent.height
            spacing: 10
            Text{ id: dane; text: "Dane:"; font.pixelSize: 20; font.bold: true }
            Text{ id: min; text: "Min: " + mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].min; font.pixelSize: 20}
            Text{ id: minTimestamp; text: mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].minTimestamp; font.pixelSize: 20}
            Text{ id: max; text: "Max: " + mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].max; font.pixelSize: 20 }
            Text{ id: maxTimestamp; text: mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].maxTimestamp; font.pixelSize: 20}
            Text{ id: avg; text: "Średnia: " + mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].avg; font.pixelSize: 20}
            Text{ id: trend; text: "Trend: " + mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].trend; font.pixelSize: 20}
        }
    }
    ComboBox{
        id: timeRangeSelector
        model: ["Pełne dane", "24 godziny", "3 dni", "tydzień"]
        onCurrentIndexChanged: filterData()
    }
    function filterData(){
        let now = new Date().getTime();
        let timeLimit;
        switch (timeRangeSelector.currentIndex) {
            case 0:
                timeLimit = new Date(mainWindow.stationMeasure[mainWindow.stationMeasure.length - 1].date);
                break;
            case 1:
                timeLimit = now - 24 * 60 * 60 * 1000;
                break;
            case 2:
                timeLimit = now - 3 * 24 * 60 * 60 * 1000;
                break;
            case 3:
                timeLimit = now - 7 * 24 * 60 * 60 * 1000;
                break;
            default:
                timeLimit = now - 24 * 60 * 60 * 1000;
        }
        axisX.min=new Date(timeLimit);
    }
    Button {
        text: "← Wróć"
        anchors.top: parent
        anchors.left: chartView.right
        anchors.topMargin: 10
        onClicked: {
            // Pop the current page from the stack
            stack.pop();
        }
    }
}
