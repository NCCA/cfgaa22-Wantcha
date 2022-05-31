#include "MainWindow.h"
#include <QFileDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QMessageBox>
#include <QLayout>
#include <QGridLayout>
#include <QLayoutItem>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include "SceneSerializer.h"
#include "RenderDialog.h"
#include "Lights.h"
#include <ngl/NGLInit.h>
//#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
    m_scene = new NGLScene();

    m_scene->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_scene->setMinimumSize(600, 350);
    m_ui->mainGridLayout->addWidget(m_scene, 0, 1, 1, 1);

    m_ui->SceneList->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(m_ui->m_posX)
    connect(m_ui->m_posX, SIGNAL(valueChanged(double)), m_scene, SLOT(setPosX(double)) );
    connect(m_ui->m_posY, SIGNAL(valueChanged(double)), m_scene, SLOT(setPosY(double)) );
    connect(m_ui->m_posZ, SIGNAL(valueChanged(double)), m_scene, SLOT(setPosZ(double)) );

    connect(m_ui->m_rotX, SIGNAL(valueChanged(double)), m_scene, SLOT(setRotX(double)) );
    connect(m_ui->m_rotY, SIGNAL(valueChanged(double)), m_scene, SLOT(setRotY(double)) );
    connect(m_ui->m_rotZ, SIGNAL(valueChanged(double)), m_scene, SLOT(setRotZ(double)) );

    connect(m_ui->m_scaleX, SIGNAL(valueChanged(double)), m_scene, SLOT(setScaleX(double)) );
    connect(m_ui->m_scaleY, SIGNAL(valueChanged(double)), m_scene, SLOT(setScaleY(double)) );
    connect(m_ui->m_scaleZ, SIGNAL(valueChanged(double)), m_scene, SLOT(setScaleZ(double)) );

    connect(m_ui->SceneList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(OnSceneListItemSelect(QListWidgetItem*)) );

    connect(m_ui->SceneList, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(OnShowSceneObjectContextMenu(QPoint)) );

    connect(this, SIGNAL(SelectSceneListItem(int)),
            m_scene, SLOT(OnSceneListItemSelected(int)) );

    connect(this, SIGNAL(DeleteSceneListItem(int)),
            m_scene, SLOT(OnSceneListItemDeleted(int)) );

    connect(this, SIGNAL(AddSceneListItemMesh(std::string)),
            m_scene, SLOT(OnAddMesh(std::string)) );

    connect(m_scene, SIGNAL(UpdateTransformUI(Transform)),
            this, SLOT(OnUpdateTransformWidget(Transform)));
    connect(m_scene, SIGNAL(UpdateSelectedIndex(int)),
            this, SLOT(OnSelectIndex(int)));
    connect(m_scene, SIGNAL(UpdateSceneListUI(const std::vector<std::shared_ptr<SceneObject>>&)),
            this, SLOT(OnUpdateSceneList(const std::vector<std::shared_ptr<SceneObject>>&)));

    connect(m_scene, SIGNAL(UpdatePropertiesBox(std::shared_ptr<SceneObject>)),
            this, SLOT(OnUpdatePropertiesBox(std::shared_ptr<SceneObject>)));

    connect(m_ui->actionSave_As, SIGNAL(triggered()), this, SLOT(OnSaveScene()));
    connect(m_ui->actionLoad, SIGNAL(triggered()), this, SLOT(OnLoadScene()));
    connect(m_ui->actionCurrent_View, SIGNAL(triggered()), this, SLOT(OnSaveView()));
    connect(m_ui->actionCustom_Resolution, SIGNAL(triggered()), this, SLOT(OnSaveViewCustom()));

    PrepareLayouts();
}

void MainWindow::PrepareLayouts()
{
    // "caching" layouts so we don't have to build them every time

    qDeleteAll(m_ui->PropertiesBox->children());
    delete m_ui->PropertiesBox->layout();

    m_propertiesLayout = new QStackedLayout(m_ui->PropertiesBox->layout());

    QWidget* defaultWidget = new QWidget;
    QGridLayout* defaultLayout = new QGridLayout;
    defaultLayout->addWidget( new QLabel("SCENE PROPERTIES"), 0, 0 );
    defaultLayout->addWidget( new QLabel("Environment Texture"), 1, 0 );
    m_envWidget = new TextureWidget(m_scene->getEnvironmentMap()->GetHDRMapPointer(), 100, 75, nullptr);

    QObject::connect(m_envWidget, qOverload<const std::string&>(&TextureWidget::selectedPath),
    [this](const std::string& path) { m_scene->makeCurrent(); m_scene->getEnvironmentMap()->SetTexture(path); m_scene->doneCurrent(); });

    defaultLayout->addWidget(m_envWidget, 1, 1, Qt::AlignLeft);

    QCheckBox* display = new QCheckBox();
    display->setText("Render Environment"); display->setChecked(m_scene->getRenderEnvironment());
    QObject::connect(display, qOverload<bool>(&QCheckBox::clicked),
    [this](bool arg) { m_scene->setRenderEnvironment(arg); m_scene->update(); });

    defaultLayout->addWidget( display, 2, 0, Qt::AlignLeft );

    defaultLayout->addWidget( new QLabel("Ambient Intensity"), 3, 0 );
    QDoubleSpinBox* intensity = new QDoubleSpinBox();
    intensity->setMaximum(10); intensity->setMinimum(0); intensity->setSingleStep(0.1f); intensity->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::PlusMinus);
    intensity->setValue(m_scene->getAmbientIntensity());

    QObject::connect(intensity, qOverload<double>(&QDoubleSpinBox::valueChanged),
    [this](double arg) { m_scene->setAmbientIntensity( arg ); m_scene->update(); });
    defaultLayout->addWidget( intensity, 3, 1, Qt::AlignLeft );

    defaultLayout->addWidget( new QLabel("Background Color"), 4, 0 );
    m_backgroundPicker = new ColorPicker(m_scene->GetBackgroundColor());
    connect(m_backgroundPicker, qOverload<const ngl::Vec3&>(&ColorPicker::PickedColor),
        [this](const ngl::Vec3& arg) { m_scene->SetBackgroundColor(arg); m_scene->update(); });

    defaultLayout->addWidget(m_backgroundPicker, 4, 1);

    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    defaultLayout->addItem(spacer, defaultLayout->rowCount(), 0);

    defaultWidget->setLayout(defaultLayout);
    m_propertiesLayout->addWidget(defaultWidget);


    QWidget* meshWidget = new QWidget;
    QGridLayout* meshLayout = new QGridLayout;
    meshLayout->addWidget( new QLabel("MATERIAL PROPERTIES"), 0, 0 );

    meshLayout->addWidget( new QLabel("Albedo"), 1, 0 );
    m_albedoWidget = new TextureWidget(nullptr, 50, 50, nullptr);
    meshLayout->addWidget(m_albedoWidget, 1, 1, Qt::AlignLeft);

    meshLayout->addWidget( new QLabel("Color"), 2, 0 );
    m_baseColorPicker = new ColorPicker( ngl::Vec3(1,1,1) );
    

    meshLayout->addWidget( m_baseColorPicker, 2, 1, Qt::AlignLeft );
    
    meshLayout->addWidget( new QLabel("Roughness"), 3, 0 );
    m_roughnessWidget = new TextureWidget(nullptr, 50, 50, nullptr);
    meshLayout->addWidget(m_roughnessWidget, 3, 1, Qt::AlignLeft);

    m_roughnessSlider = new QSlider(Qt::Orientation::Horizontal);
    m_roughnessSlider->setMinimum(0); m_roughnessSlider->setMaximum(100);
    meshLayout->addWidget(m_roughnessSlider, 3, 2, Qt::AlignLeft);

    meshLayout->addWidget( new QLabel("Normal"), 4, 0 );
    m_normalWidget = new TextureWidget(nullptr, 50, 50, nullptr);
    meshLayout->addWidget(m_normalWidget, 4, 1, Qt::AlignLeft);

    meshLayout->addWidget( new QLabel("Ambient Occlusion"), 5, 0 );
    m_aoWidget = new TextureWidget(nullptr, 50, 50, nullptr);
    meshLayout->addWidget(m_aoWidget, 5, 1, Qt::AlignLeft);

    meshLayout->addWidget( new QLabel("Metallic"), 6, 0 );
    m_metallicWidget = new TextureWidget(nullptr, 50, 50, nullptr);
    meshLayout->addWidget(m_metallicWidget, 6, 1, Qt::AlignLeft);
    m_metallicSlider = new QSlider(Qt::Orientation::Horizontal);
    m_metallicSlider->setMinimum(0); m_metallicSlider->setMaximum(100); 
    meshLayout->addWidget(m_metallicSlider, 6, 2, Qt::AlignLeft);

    QSpacerItem* spacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    meshLayout->addItem(spacer2, meshLayout->rowCount(), 0);

    meshWidget->setLayout(meshLayout);
    m_propertiesLayout->addWidget(meshWidget);


    QWidget* lightWidget = new QWidget;
    QGridLayout* lightLayout = new QGridLayout;

    lightLayout->addWidget( new QLabel("LIGHT PROPERTIES"), 0, 0 );
    lightLayout->addWidget( new QLabel("Color"), 1, 0 );
    m_lightColorPicker = new ColorPicker(ngl::Vec3(1,1,1));

    lightLayout->addWidget( m_lightColorPicker, 1, 1, Qt::AlignLeft );

    lightLayout->addWidget( new QLabel("Intensity"), 2, 0 );

    m_lightIntensity = new QDoubleSpinBox();
    m_lightIntensity->setMaximum(100); m_lightIntensity->setMinimum(0);

    lightLayout->addWidget( m_lightIntensity, 2, 1, Qt::AlignLeft );
    QSpacerItem* spacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    lightLayout->addItem(spacer3, meshLayout->rowCount(), 0);

    lightWidget->setLayout(lightLayout);
    m_propertiesLayout->addWidget(lightWidget);

    m_propertiesLayout->setCurrentIndex(1); // for some reason texture widgets don't load properly if I don't do this first
    m_propertiesLayout->setCurrentIndex(0);

    m_ui->PropertiesBox->setLayout(m_propertiesLayout);
}

void MainWindow::OnUpdatePropertiesBox(std::shared_ptr<SceneObject> sceneObject)
{
    if(sceneObject == nullptr)
    {
        m_backgroundPicker->SetColor(m_scene->GetBackgroundColor());
        m_envWidget->SetTexture(m_scene->getEnvironmentMap()->GetHDRMapPointer(), nullptr, false);
        m_propertiesLayout->setCurrentIndex(0);
    }
    else if(!sceneObject->IsLight())
    {
        m_albedoWidget->SetTexture(&sceneObject->GetMaterial().m_albedoID, &sceneObject->GetMaterial().m_albedoTexture);
        m_roughnessWidget->SetTexture(&sceneObject->GetMaterial().m_roughnessID, &sceneObject->GetMaterial().m_roughnessTexture);
        m_normalWidget->SetTexture(&sceneObject->GetMaterial().m_normalID, &sceneObject->GetMaterial().m_normalTexture);
        m_aoWidget->SetTexture(&sceneObject->GetMaterial().m_aoID, &sceneObject->GetMaterial().m_aoTexture);
        m_metallicWidget->SetTexture(&sceneObject->GetMaterial().m_metallicID, &sceneObject->GetMaterial().m_metallicTexture);

        m_metallicSlider->setValue(sceneObject->GetMaterial().m_metallic * 100);
        connect(m_metallicSlider, qOverload<int>(&QSlider::valueChanged),
            [this](int arg) { m_scene->GetSelectedObject()->GetMaterial().m_metallic= float(arg) / 100.0f; m_scene->update(); });

        m_roughnessSlider->setValue(sceneObject->GetMaterial().m_roughness * 100);
        connect(m_roughnessSlider, qOverload<int>(&QSlider::valueChanged),
            [this](int arg) { m_scene->GetSelectedObject()->GetMaterial().m_roughness= float(arg) / 100.0f; m_scene->update(); });

        m_baseColorPicker->SetColor(sceneObject->GetMaterial().m_baseColor);
        connect(m_baseColorPicker, qOverload<const ngl::Vec3&>(&ColorPicker::PickedColor),
            [this](const ngl::Vec3& arg) { m_scene->GetSelectedObject()->GetMaterial().m_baseColor = arg; m_scene->update(); });


        m_propertiesLayout->setCurrentIndex(1);
    }
    else
    {
        std::shared_ptr<Light> light = std::dynamic_pointer_cast<Light>(sceneObject);
        m_lightIntensity->setValue(light->GetIntensity());
        m_lightColorPicker->SetColor(light->GetColor());

        connect(m_lightColorPicker, qOverload<const ngl::Vec3&>(&ColorPicker::PickedColor),
            [this](const ngl::Vec3& arg) { std::dynamic_pointer_cast<Light>(m_scene->GetSelectedObject())->SetColor(arg);
            PBRShaderManager::RefreshCurrentLights(); m_lightColorPicker->update(); m_scene->update(); });

         connect(m_lightIntensity, qOverload<double>(&QDoubleSpinBox::valueChanged),
            [this](double arg) { std::dynamic_pointer_cast<Light>(m_scene->GetSelectedObject())->SetIntensity(arg);
            PBRShaderManager::RefreshCurrentLights(); m_scene->update();});

       m_propertiesLayout->setCurrentIndex(2);
    }
    
}

void MainWindow::OnSelectIndex(int index)
{
    m_ui->SceneList->setCurrentRow(index);
}

void MainWindow::OnUpdateTransformWidget(const Transform& transform)
{
    ngl::Vec3 pos = transform.getPosition();
    ngl::Vec3 rot = transform.getRotation();
    ngl::Vec3 scale = transform.getScale();

    m_ui->m_posX->setValue(pos.m_x);
    m_ui->m_posY->setValue(pos.m_y);
    m_ui->m_posZ->setValue(pos.m_z);

    m_ui->m_rotX->setValue(rot.m_x);
    m_ui->m_rotY->setValue(rot.m_y);
    m_ui->m_rotZ->setValue(rot.m_z);

    m_ui->m_scaleX->setValue(scale.m_x);
    m_ui->m_scaleY->setValue(scale.m_y);
    m_ui->m_scaleZ->setValue(scale.m_z);
}

void MainWindow::OnSaveViewCustom()
{
    bool ok;
    QStringList list = RenderDialog::getStrings(this, &ok);
    if (ok) {
        int width = list[0].toInt();
        int height = list[1].toInt();

        QString q_filepath = QFileDialog::getSaveFileName(this, "Render As PNG...", QString::fromStdString(m_lastFilepath), ".png");
        if(!q_filepath.endsWith(".png", Qt::CaseSensitivity::CaseInsensitive))
        {
            q_filepath.append(".png");
        }
        std::string filepath = q_filepath.toStdString();
        auto lastSlashPos = filepath.find_last_of('/');
        m_lastFilepath = filepath.substr(0, lastSlashPos);

        m_scene->OnSaveFramebufferSize(filepath, width, height);
    }
}

void MainWindow::OnSaveView()
{
    QString q_filepath = QFileDialog::getSaveFileName(this, "Render As PNG...", QString::fromStdString(m_lastFilepath), ".png");
    if(!q_filepath.endsWith(".png", Qt::CaseSensitivity::CaseInsensitive))
    {
        q_filepath.append(".png");
    }
    std::string filepath = q_filepath.toStdString();
    auto lastSlashPos = filepath.find_last_of('/');
    m_lastFilepath = filepath.substr(0, lastSlashPos);

    m_scene->OnSaveFramebuffer(filepath);
}

void MainWindow::OnSaveScene()
{
    QString q_filepath = QFileDialog::getSaveFileName(this, "Save Scene As...", QString::fromStdString(m_lastFilepath), ".lol");
    if(!q_filepath.endsWith(".lol", Qt::CaseSensitivity::CaseInsensitive))
    {
        q_filepath.append(".lol"); //Lightweight OpenGL Loader - L.O.L.
    }
    std::string filepath = q_filepath.toStdString();
    auto lastSlashPos = filepath.find_last_of('/');
    m_lastFilepath = filepath.substr(0, lastSlashPos);

    SceneSerializer::Serialize(filepath, *m_scene);
}

void MainWindow::OnLoadScene()
{
    QString q_filepath = QFileDialog::getOpenFileName(this, "Select Scene File", QString::fromStdString(m_lastFilepath));
    if(!q_filepath.endsWith(".lol", Qt::CaseSensitivity::CaseInsensitive))
    {
        QMessageBox::information(this, "Error!", "The selected file is not a .lol file!");
        return;
    }
    std::string filepath = q_filepath.toStdString();
    auto lastSlashPos = filepath.find_last_of('/');
    m_lastFilepath = filepath.substr(0, lastSlashPos);
    SceneSerializer::Deserialize(filepath, *m_scene);
}

void MainWindow::OnUpdateSceneList(const std::vector<std::shared_ptr<SceneObject>>& objects)
{
    m_ui->SceneList->clear();
    for(auto& obj : objects)
    {
        m_ui->SceneList->addItem(QString::fromStdString(obj->GetName()));
    }  
}

void MainWindow::OnSceneListItemSelect(QListWidgetItem* item)
{
    emit SelectSceneListItem(m_ui->SceneList->row(item));
}

// Source for this function taken from
// https://stackoverflow.com/questions/31383519/qt-rightclick-on-qlistwidget-opens-contextmenu-and-delete-item
void MainWindow::OnShowSceneObjectContextMenu(const QPoint& pos)
{
    // Handle global position
    QPoint globalPos = m_ui->SceneList->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;

    myMenu.addAction("Add Mesh",  this, SLOT(OnCreateMesh()));
    myMenu.addAction("Add Directional Light",  m_scene, SLOT(OnAddDirectionalLight()));
    myMenu.addAction("Add Point Light",  m_scene, SLOT(OnAddPointLight()));
    myMenu.addAction("Delete",  this, SLOT(OnEraseItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void MainWindow::OnEraseItem()
{
    // If multiple selection is on, we need to erase all selected items
    for (int i = 0; i < m_ui->SceneList->selectedItems().size(); ++i) {
        // Get curent item on selected row
        emit DeleteSceneListItem(m_ui->SceneList->currentRow());
        QListWidgetItem *item = m_ui->SceneList->takeItem(m_ui->SceneList->currentRow());
        // And remove it
        delete item;
    }
}

void MainWindow::OnCreateMesh()
{
    QString q_filepath = QFileDialog::getOpenFileName(this, "Select mesh file", QString::fromStdString(m_lastFilepath));
    if(!q_filepath.endsWith(".obj", Qt::CaseSensitivity::CaseInsensitive))
    {
        QMessageBox::information(this, "Error!", "The selected file is not an .obj!");
        return;
    }
    std::string filepath = q_filepath.toStdString();
    auto lastSlashPos = filepath.find_last_of('/');
    m_lastFilepath = filepath.substr(0, lastSlashPos);
    emit AddSceneListItemMesh(filepath);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
