#include <QMenu>
#include <QSlider>
#include "qsliderbutton.h"
#include "qactionslider.h"

QSliderButton::QSliderButton(const QString & title, const QString & unit, QWidget *parent):QToolButton(parent)
{
    setWindowTitle(title);
    setToolTip(title);
    setStatusTip(title);
    setPopupMode(QToolButton::InstantPopup);
    auto *a = new QActionSlider(this);
    control = a->getControl();
    control->initLabels(title,unit);
    control->init(0,100,100,1,10);
    connect(control,static_cast<void (FormSliderControl::*)(int)>(&FormSliderControl::valueChanged),this,&QSliderButton::updateToolTip);
    connect(control,static_cast<void (FormSliderControl::*)(int)>(&FormSliderControl::valueChanged),this,&QSliderButton::valueChanged);
    setMenu(new QMenu(this));
    menu()->addAction(a);
}

QSliderButton::~QSliderButton()
{
    foreach(QAction *a, menu()->actions())
        delete a;
    delete menu();
}

void QSliderButton::setIcon(const QIcon &icon)
{
    QToolButton::setIcon(icon);
    QLabel *l = control->getTitle();
    if( l != nullptr )
        l->setPixmap(icon.pixmap(QSize(16,16)));
}

void QSliderButton::init(int vmin, int v, int vmax, int singleStep, int pageStep)
{
    control->setRange(vmin,vmax);
    setValue(v);
    if( singleStep > 0 )
        control->getSlider()->setSingleStep(singleStep);
    if( pageStep > 0 )
        control->getSlider()->setPageStep(singleStep);
}

int QSliderButton::value() const
{
    return control->value();
}

void QSliderButton::setValue(int v)
{
    control->setValue(v);
    updateToolTip(v);
}

void QSliderButton::updateToolTip(int v)
{
    QString str(windowTitle());
    str += QStringLiteral(", ");
    str += QString::number(v);
    str += QStringLiteral(" ");
    str += control->getUnit()->text();
    control->setToolTip(str);
    control->setStatusTip(str);
    setToolTip(str);
    setStatusTip(str);
}
