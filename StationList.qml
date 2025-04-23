
import QtQuick 2.15
import QtQuick.Controls 2.15


Item {
    id: stationListView
    anchors.fill: parent

        visible: true
        width: 400
        height: 500
        property var selectedStationId: null



        // Nagłówek
        Rectangle {
            id: header
            width: parent.width
            height: 50
            color: "#007acc"
            z: 2

            Text {
                anchors.centerIn: parent
                text: "Wybierz Stację Pomiarową"
                color: "black"
                font.pixelSize: 20
                font.bold: true

            }
        }
        Rectangle{
            anchors.top: header.bottom
            id: container
            z: 2
            color: "#007acc"
            height: 50
            width: parent.width

            TextField {
            id: latInput
            anchors.leftMargin: 20
            placeholderText: "Latitude"
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            }

            TextField {
            id: lonInput
            anchors.left: latInput.right
            anchors.top: latInput.top
            anchors.leftMargin: 6
            placeholderText: "Longitude"
            inputMethodHints: Qt.ImhFormattedNumbersOnly
            }

            Button {
            id: getClosestStation
            text: "Szukaj najbliższych stacji"
            anchors.left: lonInput.right
            anchors.leftMargin: 5
            onClicked: {
                let lat = parseFloat(latInput.text);
                let lon = parseFloat(lonInput.text);
                if (!isNaN(lat) && !isNaN(lon)) {
                    let result = mainWindow.findClosestStations(lat, lon);
                    console.log("Znalezione stacje:", JSON.stringify(result, null, 2));
                    // you can now display `result` in a ListView or similar
                }
                stationList.model= mainWindow.findClosestStations(lat, lon);
                statusText.text = "5 najbliższych stacji:";
            }
            }
            Text {
                id: statusText
                anchors.top: getClosestStation.bottom
                text: mainWindow.status // Pobieramy status z MainWindow
                font.pixelSize: 16
                font.bold: true
                color: "black"
            }
        }
        Column {
            anchors.top: container.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            spacing: 10

            // Station list

           ListView {
                id: stationList

                width: parent.width
                height: parent.height - statusText.height - 20
                spacing: 10
                model: mainWindow.stations
                z: 1
                delegate: ItemDelegate {
                    width: parent.width
                    height: 100
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

                        Text { text: "<b>Nazwa:</b> " + modelData.stationName; font.pixelSize: 14 }
                        Text { text: "<b>Adres:</b> " + (modelData.address ? modelData.address : "Brak danych"); font.pixelSize: 14 }
                        Text { text: "<b>ID:</b> " + modelData.stationId; font.pixelSize: 14 }
                        // { text: "<b>Współrzędne:</b> " + modelData.lat + ", " + modelData.lon; font.pixelSize: 14 }
                    }
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

                        const station = mainWindow.stations[stationList.currentIndex];
                        selectedStationId = station.stationId;
                        console.log("Selected stationId:", selectedStationId);
                        stack.push(["qrc:/StationInfo.qml", {station : selectedStationId}] )


                    }
                }
        Component.onCompleted: {
            mainWindow.fetchStations();
                console.log("Push URL:", Qt.resolvedUrl("StationInfo.qml"))
        }
    }


