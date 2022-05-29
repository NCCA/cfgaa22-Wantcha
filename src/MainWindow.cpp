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
#include "TextureWidget.h"
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
    connect(m_scene, SIGNAL(UpdateSceneListUI(const std::vector<std::shared_ptr<SceneObject>>&)),
            this, SLOT(OnUpdateSceneList(const std::vector<std::shared_ptr<SceneObject>>&)));

    connect(m_scene, SIGNAL(UpdatePropertiesBox(QGridLayout*)),
            this, SLOT(OnUpdatePropertiesBox(QGridLayout*)));

    //m_ui->SceneList->addItem(QString::fromStdString("obj.GetName()"));
    //QCheckBox box = QCheckBox("Enable Cringe 2");
    //QGridLayout* layout = m_ui->PropertiesBox->layout()->findChild<QGridLayout*>();
    //QCheckBox* box = new QCheckBox(QString("Enable Cringe 2"));
    //static_cast<QGridLayout*>(m_ui->PropertiesBox->layout() )->addWidget(box, 0, 1);
    //m_ui->PropertiesBox->
}

void MainWindow::OnUpdatePropertiesBox(QGridLayout* newLayout)
{

    qDeleteAll(m_ui->PropertiesBox->children());
    delete m_ui->PropertiesBox->layout();

    if(newLayout->count() == 0)
    {
        newLayout->addWidget( new QLabel("SCENE PROPERTIES"), 0, 0 );
        newLayout->addWidget( new QLabel("Environment Texture"), 1, 0 );
        TextureWidget* envWidget = new TextureWidget(m_scene->getEnvironmentMap()->GetHDRMapPointer(), 100, 75, nullptr);

        QObject::connect(envWidget, qOverload<const std::string&>(&TextureWidget::selectedPath),
        [this](const std::string& path) { m_scene->makeCurrent(); m_scene->getEnvironmentMap()->SetTexture(path); m_scene->doneCurrent(); });
        newLayout->addWidget(envWidget, 1, 1, Qt::AlignLeft);

        QCheckBox* display = new QCheckBox();
        display->setText("Render Environment"); display->setChecked(m_scene->getRenderEnvironment());
        QObject::connect(display, qOverload<bool>(&QCheckBox::clicked),
        [this](bool arg) { m_scene->setRenderEnvironment(arg); m_scene->update(); });

        newLayout->addWidget( display, 2, 0, Qt::AlignLeft );

        newLayout->addWidget( new QLabel("Ambient Intensity"), 3, 0 );
        QDoubleSpinBox* intensity = new QDoubleSpinBox();
        intensity->setMaximum(10); intensity->setMinimum(0); intensity->setSingleStep(0.1f); intensity->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::PlusMinus);
        intensity->setValue(m_scene->getAmbientIntensity());

        QObject::connect(intensity, qOverload<double>(&QDoubleSpinBox::valueChanged),
        [this](double arg) { m_scene->setAmbientIntensity( arg ); m_scene->update(); });

        newLayout->addWidget( intensity, 3, 1, Qt::AlignLeft );


        //newLayout->addWidget( new QLabel("No object selected"), 1, 0 );
    }
    QSpacerItem* spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    newLayout->addItem(spacer, newLayout->rowCount(), 0);
    
    m_ui->PropertiesBox->setLayout(newLayout);
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
    //std::cout<<m_ui->SceneList->row(item)<<"OOOOOOOOOOOOOO\n";
    emit SelectSceneListItem(m_ui->SceneList->row(item));
}

// https://stackoverflow.com/questions/31383519/qt-rightclick-on-qlistwidget-opens-contextmenu-and-delete-item
void MainWindow::OnShowSceneObjectContextMenu(const QPoint& pos)
{
    // Handle global position
    QPoint globalPos = m_ui->SceneList->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    //myMenu.addAction("Insert", this, SLOT(addItem()));
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
