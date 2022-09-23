#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "editorpreview.h"

#include <QMainWindow>

class EditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    EditorWindow(EditorPreview* preview);

    void setupMenuBar();
    void saveMap(std::string name);
    void saveMap();
    void saveValues();
    bool modified();
    int saveDialog();

    // QWidget interface
    void closeEvent(QCloseEvent *) override;

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsFile();
    // Separator
    void restartFile();
    void exitFile();

    void undoEdit();
    void redoEdit();

    void duplicateObject();
    void deleteObject();
    void resetSizeObject();
    // Separator
    void moveLeftObject();
    void moveRightObject();
    void moveUpObject();
    void moveDownObject();

    void readMapJsonIO();
    void readEntitiesJsonIO();

    void resetPositionView();
    void resetZoomView();


private:
    EditorPreview* preview;
    unsigned int lastSaveEditCount = 0;
};

#endif // EDITORWINDOW_H
