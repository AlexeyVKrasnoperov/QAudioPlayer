#ifndef QAUDIODIALOG_H
#define QAUDIODIALOG_H

#include <QDialog>
#include <QAudioDeviceInfo>

namespace Ui {
class QAudioDialog;
}

class QComboBox;
class AudioFileLoader;
class AudioBuffer;
class Player;

class QAudioDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QAudioDialog(QWidget *parent = 0);
    ~QAudioDialog();

private slots:
    void on_comboBoxOutputDevice_currentIndexChanged(int index);

    void on_checkBoxPreferredFormat_clicked(bool checked);

    void on_pushButtonOpen_clicked();

    void bufferReadySlot(AudioBuffer * a);

    void on_pushButtonCloseFile_clicked();

protected:
    const QAudioDeviceInfo & getSelectedDevice(void);
    bool  selectComboBoxItem(QComboBox * b, const QString & txt);
    QString audioFileSuffixLoad;
    QString workingDir;
    AudioFileLoader *fileLoader;
    AudioBuffer *audioBuffer;
    Player *player;
private:
    Ui::QAudioDialog *ui;
};

#endif // QAUDIODIALOG_H
