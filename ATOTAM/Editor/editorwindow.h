#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "editorpreview.h"

#include <QMainWindow>

class EditorWindow : public QMainWindow
{
public:
    EditorWindow(EditorPreview* preview, double physicsFrameRate);

    void getInputs();
    void save();
    void saveValues();

    void closeEvent(QCloseEvent *) override;

private:
    EditorPreview* preview;
    double physicsFrameRate;

    // QWidget interface
};

#endif // EDITORWINDOW_H
