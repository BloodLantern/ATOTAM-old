#include "editorwindow.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <windows.h>

EditorWindow::EditorWindow(EditorPreview* preview)
    : preview(preview)
{
    setFocusPolicy(Qt::ClickFocus);
    setupMenuBar();
}

void EditorWindow::setupMenuBar()
{
    QMenuBar* menuBar = new QMenuBar;


    // File menu
    QMenu* menu = new QMenu;
    menu->setTitle("File");

    // Setup QAction
    QAction* edit = new QAction("New");
    edit->setShortcut(QKeySequence::New);
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::newFile);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Open");
    edit->setShortcut(QKeySequence::Open);
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::openFile);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Save");
    edit->setShortcut(QKeySequence::Save);
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::saveFile);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Save as...");
    edit->setShortcut(QKeySequence("Ctrl+Shift+S"));
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &::EditorWindow::saveAsFile);
    menu->addAction(edit);

    menu->addSeparator();

    // Setup QAction
    edit = new QAction("Restart");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::restartFile);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Exit");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &::EditorWindow::exitFile);
    menu->addAction(edit);
    menuBar->addMenu(menu);


    // Edit menu
    menu = new QMenu;
    menu->setTitle("Edit");

    // Setup QAction
    edit = new QAction("Undo");
    edit->setShortcut(QKeySequence::Undo);
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::undoEdit);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Redo");
    edit->setShortcut(QKeySequence::Redo);
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::redoEdit);
    menu->addAction(edit);
    menuBar->addMenu(menu);


    // View menu
    menu = new QMenu;
    menu->setTitle("View");

    // Setup QAction
    edit = new QAction("Reset camera position");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::resetPositionView);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Reset zoom");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::resetZoomView);
    menu->addAction(edit);
    menuBar->addMenu(menu);


    // Object menu
    menu = new QMenu;
    menu->setTitle("Object");

    // Setup QAction
    edit = new QAction("Duplicate");
    edit->setShortcut(QKeySequence("Ctrl+D"));
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::duplicateObject);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Delete");
    edit->setShortcut(QKeySequence::Delete);
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::deleteObject);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Reset size");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::resetSizeObject);
    menu->addAction(edit);

    menu->addSeparator();

    // Setup QAction
    edit = new QAction("Move left");
    edit->setShortcut(QKeySequence("Left"));
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::moveLeftObject);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Move right");
    edit->setShortcut(QKeySequence("Right"));
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::moveRightObject);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Move up");
    edit->setShortcut(QKeySequence("Up"));
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::moveUpObject);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Move down");
    edit->setShortcut(QKeySequence("Down"));
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::moveDownObject);
    menu->addAction(edit);
    menuBar->addMenu(menu);


    // IO menu
    menu = new QMenu;
    menu->setTitle("IO");

    // Setup QAction
    edit = new QAction("Read map JSON");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::readMapJsonIO);
    menu->addAction(edit);

    // Setup QAction
    edit = new QAction("Read Entities JSON");
    // Connect the signal with the slot
    connect(edit, &QAction::triggered,
            this, &EditorWindow::readEntitiesJsonIO);
    menu->addAction(edit);
    menuBar->addMenu(menu);


    setMenuBar(menuBar);
}

void EditorWindow::saveMap(std::string name)
{
    // Save map
    lastSaveEditCount = preview->getEdits().size();
    preview->saveJson(*preview->getCurrentMap()->getJson(), "maps/" + name);
}

void EditorWindow::saveMap()
{
    saveMap(preview->getCurrentMap()->getName());
}

void EditorWindow::saveValues()
{
    // Save editor values
    nlohmann::json editorJson = preview->getEditorJson();
    editorJson["lastLaunch"]["map"]["camera"] = {preview->getCamera().x(), preview->getCamera().y()};
    editorJson["lastLaunch"]["map"]["zoomFactor"] = preview->getZoomFactor();
    editorJson["lastLaunch"]["map"]["id"] = preview->getCurrentMap()->getName();
    editorJson["lastLaunch"]["map"]["roomId"] = preview->getRoomId();

    editorJson["lastLaunch"]["window"]["maximized"] = isMaximized();
    if (isMaximized())
        editorJson["lastLaunch"]["window"]["size"] = {minimumWidth(), minimumHeight()};
    else
        editorJson["lastLaunch"]["window"]["size"] = {width(), height()};
    editorJson["lastLaunch"]["window"]["position"] = {x(), y()};

    preview->saveJson(editorJson, "map_editor");
}

bool EditorWindow::modified()
{
    if (preview->getEdits().size() > lastSaveEditCount)
        if (preview->getEdits()[lastSaveEditCount]->getMade())
            return true;
    return false;
}

int EditorWindow::saveDialog()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("The map has been modified.");
    msgBox.setText("Do you want to save your changes ?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    switch (msgBox.exec()) {
    case QMessageBox::Save:
        saveMap();
        saveValues();
        return QMessageBox::Save;
    case QMessageBox::Discard:
        saveValues();
        return QMessageBox::Discard;
    case QMessageBox::Cancel:
        return QMessageBox::Cancel;
    }
    // Can't happen
    return -1;
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
    if (preview->getEdits().size() > lastSaveEditCount) {
        if (preview->getEdits()[lastSaveEditCount]->getMade()) {
            // Was edited
            switch (saveDialog()) {
            case QMessageBox::Save:
                event->setAccepted(true);
                break;
            case QMessageBox::Discard:
                event->setAccepted(true);
                break;
            case QMessageBox::Cancel:
                event->setAccepted(false);
                break;
            }
            if (event->isAccepted()) {
                preview->clearEntities();
                for (auto edit = preview->getEdits().begin(); edit != preview->getEdits().end(); edit++)
                    delete *edit;
                preview->getEdits().clear();
            }
            return;
        }
        for (auto edit = preview->getEdits().begin(); edit != preview->getEdits().end(); edit++)
            delete *edit;
        preview->getEdits().clear();
    }

    // Wasn't edited
    saveValues();
    preview->clearEntities();
    event->setAccepted(true);
}

void EditorWindow::newFile()
{
    if (modified())
        switch (saveDialog()) {
        case QMessageBox::Cancel:
            // If the user clicked on Cancel, don't open the file
            return;
        }

    // Everything is OK, create the new empty Map
    Map newMap;

    // Reset EditorPreview values
    // Clear edits list
    for (auto edit = preview->getEdits().begin(); edit != preview->getEdits().end(); edit++)
        delete *edit;
    preview->getEdits().clear();
    // Deselect the Entity
    preview->setSelected(nullptr);
    // Remove old Entities
    preview->clearEntities();

    preview->setCamera(QPoint(0, 0));
    preview->setZoomFactor(1);
    preview->setRoomId(newMap.getJson()->at("/startingRoom"_json_pointer));

    // Eventually set the new map and Entities
    preview->setCurrentMap(newMap);
    // This is quite useless but it is good to have it there in case the default Map constructor changes
    preview->setEntities(newMap.loadRooms());
}

void EditorWindow::openFile()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter("JSON File (*.json)");
    fileDialog.setViewMode(QFileDialog::Detail);
    if (preview->getEditorJson()["lastLaunch"]["fileSelectorDirectory"] == "")
        fileDialog.setDirectory(std::filesystem::current_path());
    else
        fileDialog.setDirectory(
                    QString::fromStdString(preview->getEditorJson()["lastLaunch"]["fileSelectorDirectory"].get<std::string>()));

    if (fileDialog.exec()) {
        // If the user selected a file and clicked Open
        preview->getEditorJson()["lastLaunch"]["fileSelectorDirectory"] = fileDialog.directory().filesystemPath();
        if (modified())
            switch (saveDialog()) {
            case QMessageBox::Cancel:
                // If the user clicked on Cancel, don't open the file
                return;
            }

        // Everything is OK, open the file
        QStringList temp = fileDialog.selectedFiles();
        QString fileNameWithExtension = QFileInfo(temp[0]).fileName();
        Map newMap = Map::loadMap(fileNameWithExtension.mid(0, fileNameWithExtension.size() - 5).toStdString(),
                                  preview->getAssetsPath());

        // Reset EditorPreview values
        // Clear edits list
        for (auto edit = preview->getEdits().begin(); edit != preview->getEdits().end(); edit++)
            delete *edit;
        preview->getEdits().clear();
        // Deselect the Entity
        preview->setSelected(nullptr);
        // Remove old Entities
        preview->clearEntities();

        preview->setCamera(QPoint(0, 0));
        preview->setZoomFactor(1);
        preview->setRoomId(newMap.getJson()->at("/startingRoom"_json_pointer));

        // Eventually set the new map and Entities
        preview->setCurrentMap(newMap);
        preview->setEntities(newMap.loadRooms());
    }
}

void EditorWindow::saveFile()
{
    saveMap();
}

void EditorWindow::saveAsFile()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::AnyFile);
    fileDialog.setNameFilter("JSON File (*.json)");
    fileDialog.setViewMode(QFileDialog::Detail);
    if (preview->getEditorJson()["lastLaunch"]["fileSelectorDirectory"] == "")
        fileDialog.setDirectory(std::filesystem::current_path());
    else
        fileDialog.setDirectory(
                    QString::fromStdString(preview->getEditorJson()["lastLaunch"]["fileSelectorDirectory"].get<std::string>()));

    if (fileDialog.exec()) {
        // If the user selected a file and clicked Save
        preview->getEditorJson()["lastLaunch"]["fileSelectorDirectory"] = fileDialog.directory().filesystemPath();
        if (modified())
            switch (saveDialog()) {
            case QMessageBox::Cancel:
                // If the user clicked on Cancel, don't save the file
                return;
            }

        // Everything is OK, save the file
        QStringList temp = fileDialog.selectedFiles();
        QString fileNameWithExtension = QFileInfo(temp[0]).fileName();
        saveMap(fileNameWithExtension.mid(0, fileNameWithExtension.size() - 5).toStdString());
    }
}

void EditorWindow::restartFile()
{
    std::cout << "Coming soon! (I hope...)" << std::endl;
}

void EditorWindow::exitFile()
{
    close();
}

void EditorWindow::undoEdit()
{
    preview->undoEdit();
}

void EditorWindow::redoEdit()
{
    preview->redoEdit();
}

void EditorWindow::duplicateObject()
{
    preview->duplicateObject();
}

void EditorWindow::deleteObject()
{
    preview->deleteObject();
}

void EditorWindow::resetSizeObject()
{
    preview->resetSizeObject();
}

void EditorWindow::moveLeftObject()
{
    preview->moveObjectByOnePixel("left");
}

void EditorWindow::moveRightObject()
{
    preview->moveObjectByOnePixel("right");
}

void EditorWindow::moveUpObject()
{
    preview->moveObjectByOnePixel("up");
}

void EditorWindow::moveDownObject()
{
    preview->moveObjectByOnePixel("down");
}

void EditorWindow::readMapJsonIO()
{

}

void EditorWindow::readEntitiesJsonIO()
{
    Entity::values = Entity::loadValues(preview->getAssetsPath());
}

void EditorWindow::resetPositionView()
{

}

void EditorWindow::resetZoomView()
{

}
