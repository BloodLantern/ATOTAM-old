#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "editorpreview.h"

#include <QMainWindow>

class EditorWindow : public QMainWindow
{
public:
    EditorWindow(EditorPreview* preview);

private:
    EditorPreview* preview;
};

#endif // EDITORWINDOW_H
