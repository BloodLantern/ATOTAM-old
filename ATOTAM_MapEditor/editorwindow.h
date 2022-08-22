#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "editorpreview.h"

#include <QMainWindow>

class EditorWindow : public QMainWindow
{
public:
    EditorWindow(EditorPreview* preview);

    void save();
    void saveValues();

    // QWidget interface
    void closeEvent(QCloseEvent *) override;
    void keyPressEvent(QKeyEvent *) override;

private:
    EditorPreview* preview;
    unsigned int lastSaveEditCount = 0;
};

#endif // EDITORWINDOW_H
