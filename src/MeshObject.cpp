#include "MeshObject.h"

#include "Assets/AssetManager.h"
#include "ObjMesh.h"
#include <QGLWidget>
#include <QOpenGLTexture>
#include "TextureWidget.h"

//QGridLayout* MeshObject::s_layout = new QGridLayout();

MeshObject::MeshObject(const std::string& filepath) : SceneObject(filepath)
{

}

QGridLayout* MeshObject::GetLayout()
{
    QGridLayout* layout = new QGridLayout();
    layout->addWidget( new QLabel("MATERIAL PROPERTIES"), 0, 0 );

    layout->addWidget( new QLabel("Albedo"), 1, 0 );
    //QIcon albedoIcon;
    //albedoIcon.addFile(QString::fromUtf8("textures/checkerboard.jpg"));
    //QPushButton* albedoButton = new QPushButton();
    //QGLWidget* albedoWidget = new QGLWidget();
    TextureWidget* albedoWidget = new TextureWidget(m_mesh->GetMaterial().m_albedoID, 50, 50);
    albedoWidget->paintGL();
    //QPointF point(0, 0);

    //albedoWidget->drawTexture(point, m_mesh->GetMaterial().m_albedoID);

    //albedoButton->setIcon(albedoIcon);
    //albedoButton->setIconSize(QSize(50, 50));

    layout->addWidget(albedoWidget, 1, 1, Qt::AlignLeft);
    //albedoButton->setGeometry(QRect())

    //m_sceneObjects[1]->GetMesh()->GetMaterial().SetTexture(TextureType::ALBEDO, "textures/Cerberus/Cerberus_A.png");

    //layout->addWidget( new QLabel("Albedo"), 0, 0 );
    //layout->addWidget( new QGraphicsView() );

    return layout;
}