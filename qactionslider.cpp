#include "qactionslider.h"

QActionSlider::QActionSlider(QWidget *parent,int id):QWidgetAction(parent)
{
    control = new FormSliderControl(tr(""),tr(""),parent,id);
    connect(control,static_cast<void (FormSliderControl::*)(int)>(&FormSliderControl::valueChanged),this,&QActionSlider::valueChanged);
    setDefaultWidget(control);
}
