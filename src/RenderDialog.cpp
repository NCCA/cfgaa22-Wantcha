#include "RenderDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QIntValidator>

// Source taken from https://stackoverflow.com/questions/17512542/getting-multiple-inputs-from-qinputdialog-in-qt

RenderDialog::RenderDialog(QWidget *parent) : QDialog(parent)
{
    QFormLayout *lytMain = new QFormLayout(this);

    QLabel *tLabel1 = new QLabel(QString("Width:"), this);
    QLineEdit *tLine1 = new QLineEdit(this);
    tLine1->setValidator( new QIntValidator(128, 4096, this) );
    tLine1->setText("1920");
    lytMain->addRow(tLabel1, tLine1);

    QLabel *tLabel2 = new QLabel(QString("Height:"), this);
    QLineEdit *tLine2 = new QLineEdit(this);
    tLine2->setValidator( new QIntValidator(128, 4096, this) );
    tLine2->setText("1080");
    lytMain->addRow(tLabel2, tLine2);

    fields << tLine1;
    fields << tLine2;

    QDialogButtonBox *buttonBox = new QDialogButtonBox
            ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
              Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                   this, &RenderDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &RenderDialog::reject);
    Q_ASSERT(conn);

    setLayout(lytMain);
}

QStringList RenderDialog::getStrings(QWidget *parent, bool *ok)
{
    RenderDialog *dialog = new RenderDialog(parent);

    QStringList list;

    const int ret = dialog->exec();
    if (ok)
        *ok = !!ret;

    if (ret) {
        foreach (auto field, dialog->fields) {
            list << field->text();
        }
    }

    dialog->deleteLater();

    return list;
}