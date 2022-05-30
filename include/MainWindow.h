#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>
#include "NGLScene.h"
#include "ui_MainWindow.h"
#include "TextureWidget.h"
#include "ColorPicker.h"

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
        void OnUpdateTransformWidget(const Transform& transform);
        void OnUpdateSceneList(const std::vector<std::shared_ptr<SceneObject>>& objects);
        void OnSceneListItemSelect(QListWidgetItem* item);
        void OnShowSceneObjectContextMenu(const QPoint&);
        void OnEraseItem();
        void OnCreateMesh();
        void OnUpdatePropertiesBox(std::shared_ptr<SceneObject> sceneObject);
        void OnSaveScene();
        void OnLoadScene();
        void OnSaveView();
        void OnSaveViewCustom();

private:
    void PrepareLayouts();
    Ui::MainWindow *m_ui;
    NGLScene* m_scene;
    std::string m_lastFilepath = "C://";
    QStackedLayout* m_propertiesLayout;

    TextureWidget* m_envWidget;

    TextureWidget* m_albedoWidget;
    TextureWidget* m_roughnessWidget;
    TextureWidget* m_normalWidget;
    TextureWidget* m_aoWidget;
    TextureWidget* m_metallicWidget;
    QSlider* m_roughnessSlider;
    QSlider* m_metallicSlider;
    ColorPicker* m_baseColorPicker;
    
    ColorPicker* m_lightColorPicker;
    QDoubleSpinBox* m_lightIntensity;
};

#endif // MAINWINDOW_H
