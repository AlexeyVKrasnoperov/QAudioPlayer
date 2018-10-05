#ifndef QSliderButton_h
#define QSliderButton_h

#include <QToolButton>

class FormSliderControl;

class QSliderButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QSliderButton(const QString & title, const QString & unit = QStringLiteral("%"), QWidget *parent = nullptr);
    virtual ~QSliderButton();
    int value() const;
    void init(int vmin, int v, int vmax, int singleStep = -1, int pageStep = -1);
    virtual void setIcon(const QIcon &icon);
protected:
    FormSliderControl * control;
public slots:
    void setValue(int v);
    void updateToolTip(int v);
signals:
    void valueChanged(int v);
};

#endif // QSliderButton_h
