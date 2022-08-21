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

    void closeEvent(QCloseEvent *) override;

private:
    EditorPreview* preview;

};

#endif // EDITORWINDOW_H
