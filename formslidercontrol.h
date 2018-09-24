#ifndef FORMSLIDERCONTROL_H
#define FORMSLIDERCONTROL_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class FormSliderControl;
}

class QSlider;

class FormSliderControl : public QWidget
{
    Q_OBJECT
public:
    explicit FormSliderControl(const QString & title, const QString & unit, QWidget *parent = 0, int id = -1);
    ~FormSliderControl();
    void init(int min, int v, int max, int step = 1, int pageStep = 10);
    void initLabels(const QString & title, const QString & unit = QString());
    void initLabels(const QIcon & icon, const QString & unit = QString());
    QLabel  * getTitle();
    QLabel  * getUnit();
    void setValue(int v);
    int  value();
    void setValide(bool valide);
    void setColor(const QColor & color);
    void setLayoutDirection(Qt::LayoutDirection direction);
    void setRange(int vmin, int vmax);
    QSlider * getSlider();
private slots:
    void on_horizontalSlider_valueChanged(int value);
    void sliderPressedSlot()
    {
        sliderPressed = true;
    }
    void sliderReleasedSlot();
    void valueChangedSlot(int v);
private:
    Ui::FormSliderControl *ui;
    int Id;
    bool sliderPressed;
    int  roundValue(int v);
signals:
    void valueChanged(int v);
    void valueChanged(int id, int v);
};

#endif // FORMSLIDERCONTROL_H
