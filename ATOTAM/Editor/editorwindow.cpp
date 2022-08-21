#include "editorwindow.h"
#include <QCloseEvent>
#include <QMessageBox>

EditorWindow::EditorWindow(EditorPreview* preview)
    : preview(preview)
{

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
    if (!preview->getEdits().empty()) {
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
        for (auto edit = preview->getEdits().begin(); edit != preview->getEdits().end(); edit++)
            delete *edit;
        preview->getEdits().clear();
    }

    // Wasn't edited
    saveValues();
    event->setAccepted(true);
}
