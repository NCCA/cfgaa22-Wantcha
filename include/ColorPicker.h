#ifndef COLOR_PICKER_H
#define COLOR_PICKER_H

#include <ngl/Vec3.h>
#include <QWidget>
#include <QLabel>
#include <QColorDialog>

class ColorPicker : public QLabel
{
Q_OBJECT

public:
    ColorPicker(const ngl::Vec3& color);
    void SetColor(const ngl::Vec3& color);
signals:
    void PickedColor(const ngl::Vec3& color);

protected:
    void mousePressEvent(QMouseEvent* event);

private:
    QColor m_color;
};

#endif