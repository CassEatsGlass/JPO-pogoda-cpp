import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: root
    visible: true
    width: 1150
    height: 800
    title: "Pomiary Stacji"

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: StationList{}
    }
}
