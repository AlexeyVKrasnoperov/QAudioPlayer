#include "qaudiodialog.h"
#include "ui_qaudiodialog.h"
#include <QAudioDeviceInfo>
#include <QIcon>
#include <QDebug>
#include <QFileDialog>
#include "audiofile.h"
#include "audiofilereader.h"
#include "player.h"

QAudioDialog::QAudioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAudioDialog)
{    
    ui->setupUi(this);
    player = 0;
    ui->labelPosition->setText(tr("%1 c").arg(0.,0,'f',3));
    ui->labelPosition->setEnabled(false);
    workingDir = QDir::homePath();
    workingDir = "C:/Work/AudioSamples";
    //
    int cnt = 0;
    QSignalBlocker blocker(ui->comboBoxOutputDevice);
    foreach(const QAudioDeviceInfo & info, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        if( ui->comboBoxOutputDevice->findText(info.deviceName()) == -1 )
            ui->comboBoxOutputDevice->addItem(info.deviceName(),cnt);
        cnt++;
    }
    //
    ui->pushButtonPlay->setEnabled(false);
    ui->pushButtonGoEnd->setEnabled(false);
    ui->pushButtonLoop->setEnabled(false);
    ui->pushButtonCloseFile->setEnabled(false);
    ui->pushButtonGoBegin->setEnabled(false);
    //
    player = new Player();
    player->setNotifyInterval(10);
    player->setAudioDevice(getSelectedDevice());
    //
    connect(player,SIGNAL(playerReady(bool)),this,SLOT(playerReadySlot(bool)));
    connect(player,SIGNAL(started()),this,SLOT(playerStarted()));
    connect(player,SIGNAL(stoped()),this,SLOT(playerStoped()));
    connect(player,SIGNAL(currentTimeChanged(qint32)),this,SLOT(playerPositionChanged(qint32)));
    adjustSize();
    setFixedSize(size());
}

QAudioDialog::~QAudioDialog()
{
    on_pushButtonCloseFile_clicked();
    delete ui;
}


void QAudioDialog::on_pushButtonOpen_clicked()
{
    if( ! QDir(workingDir).exists() )
        workingDir = QDir::homePath();
    QString selectedFilter(audioFileSuffixLoad);
    QStringList filters = AudioFile::getAudioFileFilters(selectedFilter);
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open file"),workingDir,filters.join(";;"),
                                                    &selectedFilter);//,QFileDialog::DontUseNativeDialog);
    if( fileName.isEmpty() )
        return;
    QFileInfo fi(fileName);
    workingDir = fi.absolutePath();
    audioFileSuffixLoad = fi.suffix();
    player->open(fileName);
}

void QAudioDialog::playerReadySlot(bool ready)
{
    ui->pushButtonPlay->setEnabled(ready);
    ui->pushButtonGoEnd->setEnabled(ready);
    ui->pushButtonLoop->setEnabled(ready);
    ui->pushButtonCloseFile->setEnabled(ready);
    ui->pushButtonGoBegin->setEnabled(false);
    if( ! ready &&  ui->pushButtonPlay->isChecked() )
        ui->pushButtonPlay->setChecked(false);
    ui->labelPosition->setEnabled(ready);
    //
    ui->horizontalSliderProgress->setEnabled(ready);
    ui->horizontalSliderProgress->setMinimum(0);
    ui->horizontalSliderProgress->setMaximum(100);
    ui->horizontalSliderProgress->setSliderPosition(0);
    ui->groupBoxControl->setTitle(tr("Control"));
    ui->labelPosition->setText(tr("%1 c").arg(0.,0,'f',3));
    //
    if( player != 0 )
    {
        qint64 d = player->getDuration();
        if( d > 0 )
        {
            ui->horizontalSliderProgress->setMaximum(d);
            ui->groupBoxControl->setTitle(tr("Duration %1 c").arg(0.001*d,0,'f',3));
        }
        ui->labelPosition->setText(tr("%1 c").arg(0.001*player->currentTime(),0,'f',3));
    }
}

void QAudioDialog::on_pushButtonCloseFile_clicked()
{
    player->close();
}

void QAudioDialog::on_pushButtonPlay_clicked(bool checked)
{
    if( checked )
        player->start(ui->horizontalSliderProgress->value());
    else
        player->stop();
}

void QAudioDialog::playerStarted(void)
{
    ui->pushButtonPlay->setIcon(QIcon(":/icons/media-playback-stop-7.png"));
    ui->pushButtonPlay->setChecked(true);
}

void QAudioDialog::playerStoped(void)
{
    ui->pushButtonPlay->setIcon(QIcon(":/icons/media-playback-start-7.png"));
    ui->pushButtonPlay->setChecked(false);
}


void QAudioDialog::on_pushButtonLoop_clicked(bool checked)
{
    player->setAutoRestart(checked);
}

void QAudioDialog::playerPositionChanged(qint32 pos)
{
    ui->horizontalSliderProgress->setSliderPosition(pos);
}

void QAudioDialog::on_horizontalSliderProgress_valueChanged(int value)
{
    ui->pushButtonGoBegin->setEnabled(value > 0);
    ui->pushButtonGoEnd->setEnabled(value < ui->horizontalSliderProgress->maximum());
    ui->labelPosition->setText(tr("%1 c").arg(0.001*value,0,'f',3));
}

void QAudioDialog::on_horizontalSliderProgress_sliderReleased()
{
    player->seek(ui->horizontalSliderProgress->value());
}

void QAudioDialog::on_pushButtonGoBegin_clicked()
{
    player->seek(0);
    ui->horizontalSliderProgress->setValue(0);
}

void QAudioDialog::on_pushButtonGoEnd_clicked()
{
    player->seek(ui->horizontalSliderProgress->maximum());
    ui->horizontalSliderProgress->setValue(ui->horizontalSliderProgress->maximum());
}

void QAudioDialog::on_comboBoxOutputDevice_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if( player != 0 )
        player->setAudioDevice(getSelectedDevice());
}

QAudioDeviceInfo QAudioDialog::getSelectedDevice(void)
{
    int idx = ui->comboBoxOutputDevice->currentData().toInt();
    QList<QAudioDeviceInfo> deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if( idx < deviceList.size() )
        return deviceList.at(idx);
    return QAudioDeviceInfo::defaultOutputDevice();
}
