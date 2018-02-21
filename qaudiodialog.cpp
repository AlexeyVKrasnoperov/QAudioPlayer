#include "qaudiodialog.h"
#include "ui_qaudiodialog.h"
#include <QAudioDeviceInfo>
#include <QMetaEnum>
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
    workingDir  = QDir::homePath();
    fileLoader  = 0;
    audioBuffer = 0;
    player      = 0;
    int cnt = 0;
    foreach(const QAudioDeviceInfo & info, QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    {
        if( ui->comboBoxOutputDevice->findText(info.deviceName()) == -1 )
            ui->comboBoxOutputDevice->addItem(info.deviceName(),cnt);
        cnt++;
    }
    ui->pushButtonPlay->setEnabled(false);
    ui->pushButtonGoEnd->setEnabled(false);
    ui->pushButtonLoop->setEnabled(false);
    ui->pushButtonCloseFile->setEnabled(false);
    ui->pushButtonGoBegin->setEnabled(false);
}

QAudioDialog::~QAudioDialog()
{
    on_pushButtonCloseFile_clicked();
    delete ui;
}

void QAudioDialog::on_comboBoxOutputDevice_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    ui->comboBoxSampleRates->clear();
    ui->comboBoxChannelCounts->clear();
    ui->comboBoxSampleSizes->clear();
    ui->comboBoxCodecs->clear();
    ui->comboBoxByteOrder->clear();
    ui->comboBoxSampleType->clear();
    const QAudioDeviceInfo & info = getSelectedDevice();
    if( info.isNull() )
        return;
    //
    foreach (int rate, info.supportedSampleRates())
        ui->comboBoxSampleRates->addItem(QString::number(rate),rate);
    foreach (int channels, info.supportedChannelCounts())
        ui->comboBoxChannelCounts->addItem(QString::number(channels),channels);
    foreach (int size, info.supportedSampleSizes())
        ui->comboBoxSampleSizes->addItem(QString::number(size),size);
    foreach (const QString & codec, info.supportedCodecs())
        ui->comboBoxCodecs->addItem(codec);
    foreach (QAudioFormat::Endian order, info.supportedByteOrders())
    {
        if( order == QAudioFormat::BigEndian )
            ui->comboBoxByteOrder->addItem("BigEndian",order);
        else if ( order == QAudioFormat::LittleEndian )
            ui->comboBoxByteOrder->addItem("LittleEndian",order);
    }
    foreach (QAudioFormat::SampleType type, info.supportedSampleTypes())
    {
        if( type == QAudioFormat::Unknown )
            ui->comboBoxSampleType->addItem("Unknown",type);
        else if ( type == QAudioFormat::SignedInt )
            ui->comboBoxSampleType->addItem("SignedInt",type);
        else if ( type == QAudioFormat::UnSignedInt )
            ui->comboBoxSampleType->addItem("UnSignedInt",type);
        else if ( type == QAudioFormat::Float )
            ui->comboBoxSampleType->addItem("Float",type);
    }
    on_checkBoxPreferredFormat_clicked(ui->checkBoxPreferredFormat->isChecked());
}

const QAudioDeviceInfo & QAudioDialog::getSelectedDevice(void)
{
    static QAudioDeviceInfo deviceInfo;
    deviceInfo = QAudioDeviceInfo();
    if( ui->comboBoxOutputDevice->count() != 0 )
    {
        int idx = ui->comboBoxOutputDevice->currentData().toInt();
        QList<QAudioDeviceInfo> deviceList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
        if( idx < deviceList.size() )
            deviceInfo = deviceList.at(idx);
    }
    return deviceInfo;
}

void QAudioDialog::on_checkBoxPreferredFormat_clicked(bool checked)
{
    ui->groupBoxFormat->setEnabled(!checked);
    if( !checked )
        return;
    const QAudioDeviceInfo & info = getSelectedDevice();
    if( info.isNull() )
        return;
    QAudioFormat preferredFormat = info.preferredFormat();
    selectComboBoxItem(ui->comboBoxSampleRates,QString::number(preferredFormat.sampleRate()));
    selectComboBoxItem(ui->comboBoxChannelCounts,QString::number(preferredFormat.channelCount()));
    selectComboBoxItem(ui->comboBoxSampleSizes,QString::number(preferredFormat.sampleSize()));
    selectComboBoxItem(ui->comboBoxCodecs,preferredFormat.codec());
    //
    if( preferredFormat.byteOrder() == QAudioFormat::BigEndian )
        selectComboBoxItem(ui->comboBoxByteOrder,"BigEndian");
    else if ( preferredFormat.byteOrder() == QAudioFormat::LittleEndian )
        selectComboBoxItem(ui->comboBoxByteOrder,"LittleEndian");
    //
    if( preferredFormat.sampleType() == QAudioFormat::Unknown )
        selectComboBoxItem(ui->comboBoxSampleType,"Unknown");
    else if( preferredFormat.sampleType() == QAudioFormat::SignedInt )
        selectComboBoxItem(ui->comboBoxSampleType,"SignedInt");
    else if( preferredFormat.sampleType() == QAudioFormat::UnSignedInt )
        selectComboBoxItem(ui->comboBoxSampleType,"UnSignedInt");
    else if( preferredFormat.sampleType() == QAudioFormat::Float )
        selectComboBoxItem(ui->comboBoxSampleType,"Float");
    //
    selectComboBoxItem(ui->comboBoxByteOrder,preferredFormat.codec());
}

bool QAudioDialog::selectComboBoxItem(QComboBox * b, const QString & txt)
{
    int idx = b->findText(txt);
    if( idx == -1 )
        return false;
    b->setCurrentIndex(idx);
    return true;
}


void QAudioDialog::on_pushButtonOpen_clicked()
{
    if( ! QDir(workingDir).exists() )
        workingDir = QDir::homePath();
    QString selectedFilter(audioFileSuffixLoad);
    QStringList filters = AudioFile::getAudioFileFilters(selectedFilter);
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open file"),workingDir,filters.join(";;"),
                                                    &selectedFilter,QFileDialog::DontUseNativeDialog);
    if( fileName.isEmpty() )
        return;
    QFileInfo fi(fileName);
    workingDir = fi.absolutePath();
    audioFileSuffixLoad = fi.suffix();
    if( fileLoader == 0 )
    {
        fileLoader = new AudioFileLoader();
        connect(fileLoader,SIGNAL(ready(AudioBuffer*)),this,SLOT(bufferReadySlot(AudioBuffer*)));
    }
    fileLoader->Load(fileName);
}

void QAudioDialog::bufferReadySlot(AudioBuffer * a)
{
    if( player != 0 )
        player->stop();
    if( audioBuffer != 0 )
        delete audioBuffer;
    audioBuffer = a;
    bool isOk =  (audioBuffer != 0);
    ui->pushButtonPlay->setEnabled(isOk);
    ui->pushButtonGoEnd->setEnabled(isOk);
    ui->pushButtonLoop->setEnabled(isOk);
    ui->pushButtonCloseFile->setEnabled(isOk);
    ui->pushButtonGoBegin->setEnabled(false);
    //
    if(isOk)
    {
        player = new Player(1);
        player->setAutoBuffer(audioBuffer);
        ui->groupBoxControl->setTitle(tr("Control: %1/%2").arg(0).arg(audioBuffer->duration()));
        ui->horizontalSliderProgress->setMinimum(0);
        ui->horizontalSliderProgress->setMaximum(audioBuffer->duration());
        ui->horizontalSliderProgress->setSliderPosition(0);
    }
}

void QAudioDialog::on_pushButtonCloseFile_clicked()
{
    ui->pushButtonPlay->setEnabled(false);
    ui->pushButtonGoEnd->setEnabled(false);
    ui->pushButtonLoop->setEnabled(false);
    ui->pushButtonCloseFile->setEnabled(false);
    ui->pushButtonGoBegin->setEnabled(false);
    ui->groupBoxControl->setTitle(tr("Control"));
    if( player != 0 )
    {
        delete player;
        player = 0;
    }
    if( audioBuffer != 0 )
    {
        delete audioBuffer;
        audioBuffer = 0;
    }
}
