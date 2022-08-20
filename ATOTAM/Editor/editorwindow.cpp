#include "editorwindow.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <windows.h>

EditorWindow::EditorWindow(EditorPreview* preview, double physicsFrameRate)
    : preview(preview)
    , physicsFrameRate(physicsFrameRate)
{

}

void EditorWindow::getInputs()
{
    for (std::map<std::string, bool>::iterator i = preview->getInputList()->begin(); i != preview->getInputList()->end(); i ++) {
        if (i->second)
            (*preview->getInputTime())[i->first] += 1 / physicsFrameRate;
        else
            (*preview->getInputTime())[i->first] = 0;
    }

    //Only listen for inputs if the window is currently selected
    if (isActiveWindow()) {
        //Check every key
        for (nlohmann::json::iterator i = preview->getEditorJson()["inputs"].begin(); i != preview->getEditorJson()["inputs"].end(); i++) {
            (*preview->getInputList())[i.key()] = GetKeyState(i.value()) & 0x8000;
        }
    } else
        //Reset the keys if the window is not selected
        for (nlohmann::json::iterator i = preview->getEditorJson()["inputs"].begin(); i != preview->getEditorJson()["inputs"].end(); i++) {
            (*preview->getInputList())[i.key()] = 0;
        }
}

void EditorWindow::save()
{
    // Save map
    preview->saveJson(preview->getCurrentMap().getJson(), "maps/" + preview->getCurrentMap().getName());
}

void EditorWindow::saveValues()
{
    // Save editor values
    nlohmann::json editorJson = preview->getEditorJson()["lastLaunch"];
    editorJson["map"]["camera"] = {preview->getCamera().x(), preview->getCamera().y()};
    editorJson["map"]["id"] = preview->getCurrentMap().getName();

    preview->saveJson(preview->getEditorJson(), "map_editor");
}


void EditorWindow::closeEvent(QCloseEvent *event)
{
    if (!preview->getEdits().empty())
        if (preview->getEdits()[0]->getMade()) {
            // Was edited
            QMessageBox msgBox;
            msgBox.setWindowTitle("The map has been modified.");
            msgBox.setText("Do you want to save your changes ?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            switch (msgBox.exec()) {
            case QMessageBox::Save:
                event->setAccepted(true);
                save();
                saveValues();
                break;
            case QMessageBox::Discard:
                event->setAccepted(true);
                saveValues();
                break;
            case QMessageBox::Cancel:
                event->setAccepted(false);
                break;
            }
            return;
        }

    // Wasn't edited
    saveValues();
    event->setAccepted(true);
}
