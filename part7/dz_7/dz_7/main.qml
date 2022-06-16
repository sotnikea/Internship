import QtQuick.Controls 2.2
import QtQuick.Window 2.15
import Qt.labs.platform
import QtQuick 2.2
import QtQuick.Dialogs
import Qt.labs.folderlistmodel 2.15

ApplicationWindow {

    width: 640
    height: 480
    visible: true
    title: qsTr("MyPictureViewer")

    //Make buttons for open folder and different type of view images
    header: ToolBar {
        Flow {
            anchors.fill: parent
            ToolButton {
                text: qsTr("OpenFolder")                
                onClicked: {
                    fileOpenDialog.open()    
                }
            }

            ToolButton {
                text: qsTr("ListView")
                onClicked: {
                    rootPath.visible = false;   //Disable pathView
                    rootGrid.visible = false;   //Disable tableView
                    rootColumn.visible = true;  //Enable listView
                }
            }

            ToolButton {
                text: qsTr("TableView")
                onClicked: {
                    rootPath.visible = false;       //Disable pathView
                    rootColumn.visible = false;     //Disable listView
                    rootGrid.visible = true;        //Enable tableView
                }

            }

            ToolButton {
                text: qsTr("PathView")
                onClicked: {
                    rootColumn.visible = false;     //Disable listView
                    rootGrid.visible = false;       //Disable tableView
                    rootPath.visible = true;        //Enable pathView
                }

            }
        }
    }

    //Work with file directory
    FileDialog {
        id: fileOpenDialog
        title: "Select an image file"
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)  //Make Documents base folder
        nameFilters: [
            "Image files (*.png *.jpeg *.jpg)", //Make mask just by image type of files
        ]
        onAccepted: {
            currentFolder: fileOpenDialog.fileUrl   //Save path to the folder with images
            rootGrid.visible = true;                //Make visible one of type image view
        }
    }

    //Save images from folder to models
    FolderListModel {
        id: folderModel        

        showDirs: false
        nameFilters: [
            "*.jpg",
            "*.png"
        ]
        folder: fileOpenDialog.currentFolder
    }

    //Create structure of table view
    Rectangle{
        id: rootGrid
        anchors.fill: parent
        visible: false;

        //Create scroll bar
        ScrollView {
            width: rootGrid.width
            height: rootGrid.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.width: 20

            //Create table
            Grid{
                width: rootGrid.width - 20
                height: rootGrid.height
                anchors.margins: 8
                spacing: 4                

                //Take all images from model
                Repeater{
                   model: folderModel

                   Image {
                       id: tmp
                       width: 150
                       fillMode: Image.PreserveAspectFit
                       source: fileOpenDialog.currentFolder + '/' + fileName

                       //Check mouse click to make image bigger
                       MouseArea{
                           width: parent.width
                           height: parent.height

                           onClicked:  {
                               showImage.source = fileOpenDialog.currentFolder + '/' + fileName
                               showImage.visible = true
                           }
                       }
                   }
                }
            }
        }        
    }

    //Create structure of table view
    Rectangle{
        id: rootColumn
        anchors.fill: parent
        visible: false

        //Create scrollbar
        ScrollView {
            width: rootColumn.width
            height: rootColumn.height
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.width: 20

            //Make view like list
            Column{
               width: rootColumn.width
               height: rootColumn.height
               spacing: 4

               //Get all images from model
               Repeater{
                  model: folderModel

                  Image {
                      anchors.horizontalCenter: parent.horizontalCenter
                      width: parent.width*0.5
                      //height: 100
                      fillMode: Image.PreserveAspectFit
                      source: fileOpenDialog.currentFolder + '/' + fileName

                      //Check for mouse click
                      MouseArea{
                          width: parent.width
                          height: parent.height

                          onClicked:  {
                              showImage.source = fileOpenDialog.currentFolder + '/' + fileName
                              showImage.visible = true
                          }
                      }
                  }
               }
           }
        }
    }

    //Create structure of path view
    Rectangle{
        id: rootPath
        anchors.fill: parent    //Растягиваем по размеру экрана
        visible: false;

        //Make path
        PathView {
            id: pathView
            anchors.fill: parent

            model: folderModel
            snapMode: PathView.SnapOneItem
            pathItemCount: 5    //Amount of elements on the screen

            path: Path {
                id: path
                startX: -50                     //some place out the screen
                startY: pathView.height / 2     //center of the screen

                //destination
                PathLine {
                    id: pathLine
                    x: pathView.width-50
                    y: pathView.height / 2
                    relativeX: x+100
                }
            }

            //Create structure for shof image
            delegate: Item {
                id: itemListInfo
                height: 100
                width: 100

                Rectangle {
                    id: rectItemListInfo
                    anchors.fill: parent
                    color: "transparent"

                    Image {
                        id: imgListItemReleased
                        anchors.fill: parent
                        source: fileOpenDialog.currentFolder + '/' + fileName
                    }

                    //Check for mouse click
                    MouseArea {
                        width: parent.width
                        height: parent.height

                        onClicked:  {
                            showImage.source = fileOpenDialog.currentFolder + '/' + fileName
                            showImage.visible = true
                        }
                    }
                }
            }
        }
    }

    //Create structure to show image in full size
    Image {
        id: showImage
        visible: false
        anchors.fill: parent
        MouseArea{
            width: parent.width
            height: parent.height

            onClicked:  {
                showImage.visible = false
            }
        }
    }
}
