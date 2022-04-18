#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"
#include "ui_MainWindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    signals:
        void SelectSceneListItem(int index);
        void DeleteSceneListItem(int index);
        void AddSceneListItemMesh(const std::string& filepath);

    public slots:
        void OnUpdateTransformWidget(const ngl::Transformation& transform);
        void OnUpdateSceneList(const std::vector<std::shared_ptr<SceneObject>>& objects);
        void OnSceneListItemSelect(QListWidgetItem* item);
        void OnShowSceneObjectContextMenu(const QPoint&);
        void OnEraseItem();
        void OnCreateMesh();

private:
    Ui::MainWindow *m_ui;
    NGLScene* m_scene;
    std::string m_lastFilepath = "C://";
};

#endif // MAINWINDOW_H
