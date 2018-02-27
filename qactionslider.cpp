#include "qactionslider.h"

QActionSlider::QActionSlider(QWidget *parent,int id):QWidgetAction(parent)
{
    control = new FormSliderControl(tr(""),tr(""),parent,id);
    connect(control,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged(int)));
    setDefaultWidget(control);
}
