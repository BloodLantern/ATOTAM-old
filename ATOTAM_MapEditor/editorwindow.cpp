#include "editorwindow.h"
#include "moveedit.h"
#include <QCloseEvent>
#include <QMessageBox>

EditorWindow::EditorWindow(EditorPreview* preview)
    : preview(preview)
{
    setFocusPolicy(Qt::ClickFocus);
}

void EditorWindow::save()
{
    // Save map
    lastSaveEditCount = preview->getEdits().size();
    preview->saveJson(*preview->getCurrentMap()->getJson(), "maps/" + preview->getCurrentMap()->getName());
}

void EditorWindow::saveValues()
{
    // Save editor values
    nlohmann::json editorJson = preview->getEditorJson();
    editorJson["lastLaunch"]["map"]["camera"] = {preview->getCamera().x(), preview->getCamera().y()};
    editorJson["lastLaunch"]["map"]["id"] = preview->getCurrentMap()->getName();

    editorJson["lastLaunch"]["window"]["maximized"] = isMaximized();
    if (isMaximized())
        editorJson["lastLaunch"]["window"]["size"] = {minimumWidth(), minimumHeight()};
    else
        editorJson["lastLaunch"]["window"]["size"] = {width(), height()};
    editorJson["lastLaunch"]["window"]["position"] = {x(), y()};

    preview->saveJson(editorJson, "map_editor");
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
    if (preview->getEdits().size() > lastSaveEditCount) {
        if (preview->getEdits()[lastSaveEditCount]->getMade()) {
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

void EditorWindow::keyPressEvent(QKeyEvent *event)
{
    preview->getInputs();
    if (preview->getInputList()["control"]) {
        if (preview->getInputList()["save"])
            save();
        else if (preview->getInputList()["undoEdit"])
            preview->undoEdit();
        else if (preview->getInputList()["redoEdit"])
            preview->redoEdit();
        else if (preview->getInputList()["duplicateObject"])
            preview->duplicateObject();
        else
            event->ignore();
    } else if (preview->getInputList()["delete"])
        preview->deleteObject();
    else if (preview->getInputList()["left"] && preview->getSelected()) {
        MoveEdit* move = new MoveEdit(preview->getCurrentMap(), preview->getSelected(), QPoint(-1, 0));
        move->make();
        preview->getEdits().push_back(move);
        update();
    }
    else if (preview->getInputList()["right"] && preview->getSelected()) {
        MoveEdit* move = new MoveEdit(preview->getCurrentMap(), preview->getSelected(), QPoint(1, 0));
        move->make();
        preview->getEdits().push_back(move);
        update();
    }
    else if (preview->getInputList()["up"] && preview->getSelected()) {
        MoveEdit* move = new MoveEdit(preview->getCurrentMap(), preview->getSelected(), QPoint(0, -1));
        move->make();
        preview->getEdits().push_back(move);
        update();
    }
    else if (preview->getInputList()["down"] && preview->getSelected()) {
        MoveEdit* move = new MoveEdit(preview->getCurrentMap(), preview->getSelected(), QPoint(0, 1));
        move->make();
        preview->getEdits().push_back(move);
        update();
    }
    else
        event->ignore();
}
