#include <ColorPicker.h>

ColorPicker::ColorPicker(const ngl::Vec3& color)
    :QLabel(Q_NULLPTR)
{
    SetColor(color);
}

void ColorPicker::SetColor(const ngl::Vec3& color)
{
    m_color = QColor::fromRgb(color.m_r * 255, color.m_g * 255, color.m_b * 255);
    setFixedSize(40, 40);
    QString r, g, b;
    r = QString::number( m_color.red() );
    g = QString::number( m_color.green() );
    b = QString::number( m_color.blue() );

    setStyleSheet("QLabel{"
                    "background-color:rgb("+r+", "+g+", "+b+");"
                    "}");
}

void ColorPicker::mousePressEvent(QMouseEvent* event)
{
    QColor col = QColorDialog::getColor(m_color);
    if(col.isValid())
    {
        m_color = col;
        QString r, g, b;
        r = QString::number( m_color.red() );
        g = QString::number( m_color.green() );
        b = QString::number( m_color.blue() );

        setStyleSheet("QLabel{"
                        "background-color:rgb("+r+", "+g+", "+b+");"
                        "}");
        emit PickedColor(ngl::Vec3( col.red() / 255.0f,  col.green() / 255.0f, col.blue() / 255.0f));
    }
}