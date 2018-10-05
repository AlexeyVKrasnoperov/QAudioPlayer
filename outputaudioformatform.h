#ifndef OUTPUTAUDIOFORMATFORM_H
#define OUTPUTAUDIOFORMATFORM_H

#include <QWidget>

namespace Ui {
class OutputAudioFormatForm;
}

class OutputAudioFormatForm : public QWidget
{
    Q_OBJECT

public:
    explicit OutputAudioFormatForm(QWidget *parent = nullptr);
    ~OutputAudioFormatForm();

private:
    Ui::OutputAudioFormatForm *ui;
};

#endif // OUTPUTAUDIOFORMATFORM_H
