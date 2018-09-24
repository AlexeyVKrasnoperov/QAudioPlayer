#ifndef QACTIONSLIDER_H
#define QACTIONSLIDER_H

#include <QWidgetAction>
#include <QObject>
#include "formslidercontrol.h"

class QActionSlider: public QWidgetAction
{
    Q_OBJECT
protected:
    FormSliderControl *control;
public:
    QActionSlider(QWidget *parent, int id = -1);
    virtual ~QActionSlider()
    {
        delete control;
    }
    FormSliderControl * getControl()
    {
        return control;
    }
signals:
    void valueChanged(int);
};

#endif // QACTIONSLIDER_H
