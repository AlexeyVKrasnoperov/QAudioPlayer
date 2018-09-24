#include "formslidercontrol.h"
#include "ui_formslidercontrol.h"
#include <QFontMetrics>
#include <QIcon>

FormSliderControl::FormSliderControl(const QString & title, const QString & unit, QWidget *parent, int id):
    QWidget(parent), ui(new Ui::FormSliderControl),  Id(id)
{
    ui->setupUi(this);
    sliderPressed = false;
    ui->labelTitle->setText(title);
    ui->labelUnit->setText(unit);
    connect(ui->horizontalSlider,SIGNAL(sliderPressed()),this,SLOT(sliderPressedSlot()));
    connect(ui->horizontalSlider,SIGNAL(sliderReleased()),this,SLOT(sliderReleasedSlot()));
    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(valueChangedSlot(int)));
}

FormSliderControl::~FormSliderControl()
{
    delete ui;
}

void FormSliderControl::setColor(const QColor & color)
{
    QPalette palette(ui->labelTitle->palette());
    palette.setColor(ui->labelTitle->foregroundRole(),color);
    ui->labelTitle->setPalette(palette);
    ui->labelUnit->setPalette(palette);
    ui->labelValue->setPalette(palette);
}

void FormSliderControl::initLabels(const QString & title, const QString & unit)
{
    ui->labelTitle->setText(title);
    ui->labelUnit->setText(unit);
}

void FormSliderControl::initLabels(const QIcon & icon, const QString & unit)
{
    ui->labelTitle->setPixmap(icon.pixmap(QSize(16,16)));
    ui->labelUnit->setText(unit);
}


void FormSliderControl::init(int min, int v, int max, int step, int pageStep)
{
    QSignalBlocker b(ui->horizontalSlider);
    ui->horizontalSlider->setMinimum(min);
    ui->horizontalSlider->setMaximum(max);
    ui->horizontalSlider->setValue(v);
    ui->horizontalSlider->setSingleStep(step);
    ui->horizontalSlider->setPageStep(pageStep);
    ui->labelValue->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
    int l = QString::number(max).length()+1;
    ui->labelValue->setMinimumWidth(QFontMetrics(ui->labelValue->font()).width("0")*l);
    ui->labelValue->setText(QString::number(v));
}

void FormSliderControl::setValide(bool valide)
{
    ui->labelTitle->setEnabled(valide);
    ui->labelUnit->setEnabled(valide);
    ui->labelValue->setEnabled(valide);
}


void FormSliderControl::setValue(int v)
{
    QSignalBlocker b(ui->horizontalSlider);
    ui->horizontalSlider->setValue(v);
    ui->labelValue->setText(QString::number(v));
}

void FormSliderControl::setRange(int vmin, int vmax)
{
    QSignalBlocker b(ui->horizontalSlider);
    ui->horizontalSlider->setRange(vmin,vmax);
    ui->labelValue->setText(QString::number(ui->horizontalSlider->value()));
}

int FormSliderControl::value()
{
    return ui->horizontalSlider->value();
}


void FormSliderControl::on_horizontalSlider_valueChanged(int value)
{
    ui->labelValue->setText(QString::number(value));
}

QLabel * FormSliderControl::getTitle()
{
    return ui->labelTitle;
}

QLabel * FormSliderControl::getUnit()
{
    return ui->labelUnit;
}

QSlider * FormSliderControl::getSlider()
{
    return ui->horizontalSlider;
}

void FormSliderControl::sliderReleasedSlot()
{
    int v = roundValue(ui->horizontalSlider->value());
    sliderPressed = false;
    if( Id == -1 )
        emit valueChanged(v);
    else
        emit valueChanged(Id,v);

}

void FormSliderControl::setLayoutDirection(Qt::LayoutDirection direction)
{
    ui->horizontalSlider->setLayoutDirection(direction);
}


void FormSliderControl::valueChangedSlot(int v)
{
    if( ! sliderPressed )
    {
        v = roundValue(v);
        if( Id == -1 )
            emit valueChanged(v);
        else
            emit valueChanged(Id,v);
    }
}

int FormSliderControl::roundValue(int v)
{
    int ss = ui->horizontalSlider->singleStep();
    if( ss == 1 )
        return v;
    if( (v%ss) != 0 )
    {
        v = (v/ss)*ss;
        setValue(v);
    }
    return v;
}

