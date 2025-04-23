import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    property var station

    Connections {
        target: mainWindow
        function onStationDetailsChanged() {
            console.log("Sensor data:", mainWindow.stationDetails)
        }
    }

    Component.onCompleted: {
        mainWindow.fetchStationDetails(station);
    }

    Text {
        id: statusText
        text: mainWindow.status
        font.pixelSize: 16
        color: "#333"
    }


    ListView {
         id: stationList
         anchors.top: statusText.bottom
         anchors.topMargin: 5
         width: parent.width * 0.8
         height: parent.height - statusText.height - 20
         spacing: 10
         model: mainWindow.stationDetails
         delegate: ItemDelegate {
             width: parent.width
             height: 120
             text: modelData
             highlighted: stationList.currentIndex === index
             checked: stationList.currentIndex === index
             onClicked: stationList.currentIndex = index
             background: Rectangle {
                         color: highlighted ? "#d0d0d0" : "transparent"
                         radius: 4
                     }
              Column {
                 anchors.fill: parent
                 anchors.margins: 10
                 spacing: 5

                 Text { text: "<b>id</b> " + modelData.id; font.pixelSize: 14 }
                 Text { text: "<b>Parametr</b> " + modelData.paramName; font.pixelSize: 14 }
                 Text { text: "<b>Cząsteczka</b> " + modelData.paramFormula; font.pixelSize: 14 }

             }
         }

    }
    Button {
                text: "Zatwierdź"
                enabled: stationList.currentIndex >= 0
                anchors {
                           right: parent.right
                           bottom: parent.bottom
                           margins: 16
                       }
                background: Rectangle {
                        color: enabled ? "#007acc" : "#cccccc"
                        radius: 8
                    }
                onClicked: {
                    console.log(mainWindow.stationDetails[stationList.currentIndex].id);
                    const selectedStationMeasure = mainWindow.stationDetails[stationList.currentIndex].id;
                    mainWindow.param = mainWindow.stationDetails[stationList.currentIndex].paramName;

                    console.log("Selected stationId for measurement: ", selectedStationMeasure);
                    //mainWindow.current_id = selectedStationMeasure.toString();
                    //console.log("mainWindow.current_id:", mainWindow.current_id);
                    stack.push(["qrc:/StationMeasure.qml", {station : selectedStationMeasure}]);
                }
    }

    Button {
        text: "← Wróć"
        anchors.left: stationList.right
        onClicked: {
            // Pop the current page from the stack
            stack.pop();
        }
    }

}

