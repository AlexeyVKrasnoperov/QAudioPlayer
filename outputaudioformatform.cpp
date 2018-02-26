#include "outputaudioformatform.h"
#include "ui_outputaudioformatform.h"

OutputAudioFormatForm::OutputAudioFormatForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputAudioFormatForm)
{
    ui->setupUi(this);
}

OutputAudioFormatForm::~OutputAudioFormatForm()
{
    delete ui;
}

//void QAudioDialog::on_comboBoxOutputDevice_currentIndexChanged(int index)
//{
//    Q_UNUSED(index);
//    ui->comboBoxSampleRates->clear();
//    ui->comboBoxChannelCounts->clear();
//    ui->comboBoxSampleSizes->clear();
//    ui->comboBoxCodecs->clear();
//    ui->comboBoxByteOrder->clear();
//    ui->comboBoxSampleType->clear();
//    const QAudioDeviceInfo & info = getSelectedDevice();
//    if( info.isNull() )
//        return;
//    //
//    foreach (int rate, info.supportedSampleRates())
//        ui->comboBoxSampleRates->addItem(QString::number(rate),rate);
//    foreach (int channels, info.supportedChannelCounts())
//        ui->comboBoxChannelCounts->addItem(QString::number(channels),channels);
//    foreach (int size, info.supportedSampleSizes())
//        ui->comboBoxSampleSizes->addItem(QString::number(size),size);
//    foreach (const QString & codec, info.supportedCodecs())
//        ui->comboBoxCodecs->addItem(codec);
//    foreach (QAudioFormat::Endian order, info.supportedByteOrders())
//    {
//        if( order == QAudioFormat::BigEndian )
//            ui->comboBoxByteOrder->addItem("BigEndian",order);
//        else if ( order == QAudioFormat::LittleEndian )
//            ui->comboBoxByteOrder->addItem("LittleEndian",order);
//    }
//    foreach (QAudioFormat::SampleType type, info.supportedSampleTypes())
//    {
//        if( type == QAudioFormat::Unknown )
//            ui->comboBoxSampleType->addItem("Unknown",type);
//        else if ( type == QAudioFormat::SignedInt )
//            ui->comboBoxSampleType->addItem("SignedInt",type);
//        else if ( type == QAudioFormat::UnSignedInt )
//            ui->comboBoxSampleType->addItem("UnSignedInt",type);
//        else if ( type == QAudioFormat::Float )
//            ui->comboBoxSampleType->addItem("Float",type);
//    }
//    on_checkBoxPreferredFormat_clicked(ui->checkBoxPreferredFormat->isChecked());
//}



//void QAudioDialog::on_checkBoxPreferredFormat_clicked(bool checked)
//{
//    ui->groupBoxFormat->setEnabled(!checked);
//    if( !checked )
//        return;
//    const QAudioDeviceInfo & info = getSelectedDevice();
//    if( info.isNull() )
//        return;
//    QAudioFormat preferredFormat = info.preferredFormat();
//    selectComboBoxItem(ui->comboBoxSampleRates,QString::number(preferredFormat.sampleRate()));
//    selectComboBoxItem(ui->comboBoxChannelCounts,QString::number(preferredFormat.channelCount()));
//    selectComboBoxItem(ui->comboBoxSampleSizes,QString::number(preferredFormat.sampleSize()));
//    selectComboBoxItem(ui->comboBoxCodecs,preferredFormat.codec());
//    //
//    if( preferredFormat.byteOrder() == QAudioFormat::BigEndian )
//        selectComboBoxItem(ui->comboBoxByteOrder,"BigEndian");
//    else if ( preferredFormat.byteOrder() == QAudioFormat::LittleEndian )
//        selectComboBoxItem(ui->comboBoxByteOrder,"LittleEndian");
//    //
//    if( preferredFormat.sampleType() == QAudioFormat::Unknown )
//        selectComboBoxItem(ui->comboBoxSampleType,"Unknown");
//    else if( preferredFormat.sampleType() == QAudioFormat::SignedInt )
//        selectComboBoxItem(ui->comboBoxSampleType,"SignedInt");
//    else if( preferredFormat.sampleType() == QAudioFormat::UnSignedInt )
//        selectComboBoxItem(ui->comboBoxSampleType,"UnSignedInt");
//    else if( preferredFormat.sampleType() == QAudioFormat::Float )
//        selectComboBoxItem(ui->comboBoxSampleType,"Float");
//    //
//    selectComboBoxItem(ui->comboBoxByteOrder,preferredFormat.codec());
//}

//bool QAudioDialog::selectComboBoxItem(QComboBox * b, const QString & txt)
//{
//    int idx = b->findText(txt);
//    if( idx == -1 )
//        return false;
//    b->setCurrentIndex(idx);
//    return true;
//}

