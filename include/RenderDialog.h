#ifndef RENDER_DIALOG_H
#define RENDER_DIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;

class RenderDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RenderDialog(QWidget *parent = nullptr);

    static QStringList getStrings(QWidget *parent, bool *ok = nullptr);

private:
    QList<QLineEdit*> fields;
};

#endif