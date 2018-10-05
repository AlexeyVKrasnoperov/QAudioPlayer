#include "qaudiodialog.h"
#include "ui_qaudiodialog.h"
#include <QAudioDeviceInfo>
#include <QIcon>
#include <QSettings>
#include <QFileDialog>
#include "audiofile.h"
#include "audiofilereader.h"
#include "player.h"
#include "qsliderbutton.h"

QAudioDialog::QAudioDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QAudioDialog)
{    
    ui->setupUi(this);
    player = nullptr;
    workingDir = QDir::homePath();
    //
    ui->labelPosition->setMinimumWidth(QFontMetrics(ui->labelPosition->font()).width(QStringLiteral("0000.000")));
    ui->labelPosition->setText(QStringLiteral("0.000 c"));
    ui->labelPosition->setEnabled(false);
    //
    audioVolume = new QSliderButton(tr("Volume"),QStringLiteral("%"),this);
    audioVolume->setIcon(QIcon(":/icons/audio-volume-medium.png"));
    audioVolume->init(0,50,100);
    ui->horizontalLayoutDevice->addWidget(audioVolume);
    //
    {
        int cnt = 0;
        QSignalBlocker blocker(ui->comboBoxOutputDevice);
        foreach(const QAudioDeviceInfo & info, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
        {
            if( ui->comboBoxOutputDevice->findText(info.deviceName()) == -1 )
                ui->comboBoxOutputDevice->addItem(info.deviceName(),cnt);
            cnt++;
        }
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
    connect(player,&Player::playerReady,this,&QAudioDialog::playerReadySlot);
    connect(player,&Player::started,this,&QAudioDialog::playerStarted);
    connect(player,&Player::stoped,this,&QAudioDialog::playerStoped);
    connect(player,&Player::currentTimeChanged,this,&QAudioDialog::playerPositionChanged);
    connect(audioVolume,&QSliderButton::valueChanged,player,&Player::setVolume);
    //
    adjustSize();
    setFixedSize(size());
    readSettings();
    //
    player->setVolume(audioVolume->value());
    player->setAutoRestart(ui->pushButtonLoop->isChecked());
    //
}

QAudioDialog::~QAudioDialog()
{
    delete ui;
}


void QAudioDialog::on_pushButtonOpen_clicked()
{
    if( ! QDir(workingDir).exists() )
        workingDir = QDir::homePath();
    QString selectedFilter(audioFileSuffixLoad);
    QStringList filters = AudioFile::getAudioFileFilters(selectedFilter);
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open file"),workingDir,filters.join(QStringLiteral(";;")),
                                                    &selectedFilter,QFileDialog::DontUseNativeDialog);
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
    setWindowTitle(tr("QtAudioPlayer"));
    //
    if( (player != nullptr) && ready )
    {
        qint64 d = player->getDuration();
        if( d > 0 )
        {
            ui->horizontalSliderProgress->setMaximum(int(d));
            ui->groupBoxControl->setTitle(tr("Duration %1 c").arg(0.001*d,0,'f',3));
            setWindowTitle(tr("QtAudioPlayer : \"%1\"").arg(QFileInfo(player->getFileName()).baseName()));
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

void QAudioDialog::playerStarted()
{
    ui->pushButtonPlay->setIcon(QIcon(":/icons/media-playback-stop-7.png"));
    ui->pushButtonPlay->setChecked(true);
}

void QAudioDialog::playerStoped()
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
    if( player != nullptr )
        player->setAudioDevice(getSelectedDevice());
}

QAudioDeviceInfo QAudioDialog::getSelectedDevice()
{
    int idx = ui->comboBoxOutputDevice->currentData().toInt();
    QList<QAudioDeviceInfo> deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    if( idx < deviceList.size() )
        return deviceList.at(idx);
    return QAudioDeviceInfo::defaultOutputDevice();
}

void QAudioDialog::readSettings()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("QAudioDialog"));
    restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    workingDir = settings.value(QStringLiteral("workingDir"),QDir::homePath()).toString();
    audioFileSuffixLoad = settings.value(QStringLiteral("audioFileSuffixLoad")).toString();
    ui->pushButtonLoop->setChecked(settings.value(QStringLiteral("loop"),true).toBool());
    audioVolume->setValue(settings.value(QStringLiteral("volume"),50).toInt());
    int idx = ui->comboBoxOutputDevice->findText(settings.value(QStringLiteral("device")).toString());
    if( idx != -1 )
        ui->comboBoxOutputDevice->setCurrentIndex(idx);
    settings.endGroup();
}

void QAudioDialog::writeSettings()
{
    QSettings settings;
    settings.beginGroup(QStringLiteral("QAudioDialog"));
    settings.setValue(QStringLiteral("geometry"),saveGeometry());
    settings.setValue(QStringLiteral("workingDir"),workingDir);
    settings.setValue(QStringLiteral("audioFileSuffixLoad"),audioFileSuffixLoad);
    settings.setValue(QStringLiteral("loop"),ui->pushButtonLoop->isChecked());
    settings.setValue(QStringLiteral("volume"),audioVolume->value());
    settings.setValue(QStringLiteral("device"),ui->comboBoxOutputDevice->currentText());
    settings.endGroup();
}

void QAudioDialog::closeEvent(QCloseEvent *e)
{
    writeSettings();
    on_pushButtonCloseFile_clicked();
    QDialog::closeEvent(e);
}

void QAudioDialog::on_horizontalSliderProgress_sliderPressed()
{
    player->blockSignals(true);
}

void QAudioDialog::on_horizontalSliderProgress_sliderReleased()
{
    player->blockSignals(false);
    player->seek(ui->horizontalSliderProgress->value());
}
