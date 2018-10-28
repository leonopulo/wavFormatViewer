#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->actionOpen_Wav_File->setShortcut(QKeySequence::Open);
    ui->actionOpen_Wav_File->setStatusTip(tr("Open an existing WAV file"));
    ui->treeWidget->setColumnCount(1);
    riffChunkNames = new QList<QTableWidgetItem *>();
    riffChunkNames->append(new QTableWidgetItem("Chunk Name"));
    riffChunkNames->append(new QTableWidgetItem("Chunk Size"));
    riffChunkNames->append(new QTableWidgetItem("Formst"));
    riffChunkNames->append(new QTableWidgetItem("subChunk ID"));
    riffChunkNames->append(new QTableWidgetItem("subChunk 1 Size"));
    riffChunkNames->append(new QTableWidgetItem("Audio Format"));
    riffChunkNames->append(new QTableWidgetItem("Numbers of Channels"));
    riffChunkNames->append(new QTableWidgetItem("Sample Rate"));
    riffChunkNames->append(new QTableWidgetItem("Byte Rate"));
    riffChunkNames->append(new QTableWidgetItem("Block Align"));
    riffChunkNames->append(new QTableWidgetItem("Bits Per Sample"));
    riffChunkNames->append(new QTableWidgetItem("subChunk 2 ID"));
    riffChunkNames->append(new QTableWidgetItem("subChunk 2 Size"));
    riffChunk = new QList<QTableWidgetItem *>();
    riffChunk->clear();
    infoChunkNames = new QList<QTableWidgetItem *>();
    infoChunkNames->clear();
    infoChunk = new QList<QTableWidgetItem *>();
    infoChunk->clear();
    list.size = 0;
    list.info = nullptr;
    list.exif = nullptr;
    connect(ui->actionOpen_Wav_File, &QAction::triggered, this, &MainWindow::OpenWavFile);
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &MainWindow::viewChunkInfo);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenWavFile(){
    QString filename=QFileDialog::getOpenFileName(this, tr("Open WAV vile"), qApp->applicationDirPath(),tr("WAV files (*.wav)"));
    if (filename.isEmpty())
        return;
    file = new QFile(filename);
    if (!file->open(QIODevice::ReadOnly))
        return;
    QStringList fname = filename.split("/");
    QString name = fname.at(fname.size() - 1);
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(static_cast<QTreeWidget*>(nullptr), QStringList(name));
    ui->treeWidget->insertTopLevelItem(0, rootItem);
    while (!file->atEnd()){
        QString chunkid = readChunk(file);
        if (chunkid.isEmpty())
            break;
        if (chunkid == "RIFF"){
            QTreeWidgetItem *riff = new QTreeWidgetItem(rootItem, QStringList(QString("RIFF")));
            rootItem->addChild(riff);
            file->seek(file->pos() + this->riff.subchunk2Size);
        }
        if (chunkid == "LIST(INFO)"){
            QTreeWidgetItem *list = new QTreeWidgetItem(rootItem, QStringList(QString("LIST(INFO)")));
            rootItem->addChild(list);
        }
    }
    file->close();
}

QString MainWindow::readChunk(QFile *file){
    char chunkid[4];
    char tmp[4];
    file->read(&chunkid[0], 4);
    if (!strncmp(&chunkid[0], "RIFF", 4)){
        riffChunk->append(new QTableWidgetItem("RIFF"));
        file->read(&tmp[0], 4);
        riff.chunkSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.chunkSize)));
        file->read(&riff.format[0], 4);
        riffChunk->append(new QTableWidgetItem(QString("%1%2%3%4").arg(riff.format[0]).arg(riff.format[1]).arg(riff.format[2]).arg(riff.format[3])));
        file->read(&riff.subchunk1Id[0], 4);
        riffChunk->append(new QTableWidgetItem(QString("%1%2%3%4").arg(riff.subchunk1Id[0]).arg(riff.subchunk1Id[1]).arg(riff.subchunk1Id[2]).arg(riff.subchunk1Id[3])));
        file->read(&tmp[0], 4);
        riff.subchunk1Size =  static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.subchunk1Size)));
        file->read(&tmp[0], 2);
        riff.audioFormat =  static_cast<unsigned short>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8);
        switch (riff.audioFormat){
        case WAVE_FORMAT_UNKNOWN: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UNKNOWN"));
            break;
        case WAVE_FORMAT_PCM: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_PCM"));
            break;
        case WAVE_FORMAT_ADPCM: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ADPCM"));
            break;
        case WAVE_FORMAT_IEEE_FLOAT: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_IEEE_FLOAT"));
            break;
        case WAVE_FORMAT_VSELP: /* Compaq Computer Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VSELP"));
            break;
        case WAVE_FORMAT_IBM_CVSD: /* IBM Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_IBM_CVSD"));
            break;
        case WAVE_FORMAT_ALAW: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ALAW"));
            break;
        case WAVE_FORMAT_MULAW: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MULAW"));
            break;
        case WAVE_FORMAT_DTS: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DTS"));
            break;
        case WAVE_FORMAT_DRM: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DRM"));
            break;
        case WAVE_FORMAT_OKI_ADPCM: /* OKI */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_OKI_ADPCM"));
            break;
        case WAVE_FORMAT_DVI_ADPCM: /* Intel Corporation */
        //case WAVE_FORMAT_IMA_ADPCM: /* Intel Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DVI_ADPCM"));
            break;
        case WAVE_FORMAT_MEDIASPACE_ADPCM: /* Videologic */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MEDIASPACE_ADPCM"));
            break;
        case WAVE_FORMAT_SIERRA_ADPCM: /* Sierra Semiconductor Corp */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIERRA_ADPCM"));
            break;
        case WAVE_FORMAT_G723_ADPCM: /* Antex Electronics Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G723_ADPCM"));
            break;
        case WAVE_FORMAT_DIGISTD: /* DSP Solutions, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DIGISTD"));
            break;
        case WAVE_FORMAT_DIGIFIX: /* DSP Solutions, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DIGIFIX"));
            break;
        case WAVE_FORMAT_DIALOGIC_OKI_ADPCM: /* Dialogic Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DIALOGIC_OKI_ADPCM"));
            break;
        case WAVE_FORMAT_MEDIAVISION_ADPCM: /* Media Vision, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MEDIAVISION_ADPCM"));
            break;
        case WAVE_FORMAT_CU_CODEC: /* Hewlett-Packard Company */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CU_CODEC"));
            break;
        case WAVE_FORMAT_YAMAHA_ADPCM: /* Yamaha Corporation of America */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_YAMAHA_ADPCM"));
            break;
        case WAVE_FORMAT_SONARC: /* Speech Compression */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SONARC"));
            break;
        case WAVE_FORMAT_DSPGROUP_TRUESPEECH: /* DSP Group, Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DSPGROUP_TRUESPEECH"));
            break;
        case WAVE_FORMAT_ECHOSC1: /* Echo Speech Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ECHOSC1"));
            break;
        case WAVE_FORMAT_AUDIOFILE_AF36: /* Virtual Music, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_AUDIOFILE_AF36"));
            break;
        case WAVE_FORMAT_APTX: /* Audio Processing Technology */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_APTX"));
            break;
        case WAVE_FORMAT_AUDIOFILE_AF10: /* Virtual Music, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_AUDIOFILE_AF10"));
            break;
        case WAVE_FORMAT_PROSODY_1612: /* Aculab plc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_PROSODY_1612"));
            break;
        case WAVE_FORMAT_LRC: /* Merging Technologies S.A. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_LRC"));
            break;
        case WAVE_FORMAT_DOLBY_AC2: /* Dolby Laboratories */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DOLBY_AC2"));
            break;
        case WAVE_FORMAT_GSM610: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_GSM610"));
            break;
        case WAVE_FORMAT_MSNAUDIO: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MSNAUDIO"));
            break;
        case WAVE_FORMAT_ANTEX_ADPCME: /* Antex Electronics Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ANTEX_ADPCME"));
            break;
        case WAVE_FORMAT_CONTROL_RES_VQLPC: /* Control Resources Limited */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CONTROL_RES_VQLPC"));
            break;
        case WAVE_FORMAT_DIGIREAL: /* DSP Solutions, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DIGIREAL"));
            break;
        case WAVE_FORMAT_DIGIADPCM: /* DSP Solutions, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DIGIADPCM"));
            break;
        case WAVE_FORMAT_CONTROL_RES_CR10: /* Control Resources Limited */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CONTROL_RES_CR10"));
            break;
        case WAVE_FORMAT_NMS_VBXADPCM: /* Natural MicroSystems */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_NMS_VBXADPCM"));
            break;
        case WAVE_FORMAT_CS_IMAADPCM: /* Crystal Semiconductor IMA ADPCM */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CS_IMAADPCM"));
            break;
        case WAVE_FORMAT_ECHOSC3: /* Echo Speech Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ECHOSC3"));
            break;
        case WAVE_FORMAT_ROCKWELL_ADPCM: /* Rockwell International */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ROCKWELL_ADPCM"));
            break;
        case WAVE_FORMAT_ROCKWELL_DIGITALK: /* Rockwell International */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ROCKWELL_DIGITALK"));
            break;
        case WAVE_FORMAT_XEBEC: /* Xebec Multimedia Solutions Limited */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_XEBEC"));
            break;
        case WAVE_FORMAT_G721_ADPCM: /* Antex Electronics Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G721_ADPCM"));
            break;
        case WAVE_FORMAT_G728_CELP: /* Antex Electronics Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G728_CELP"));
            break;
        case WAVE_FORMAT_MSG723: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MSG723"));
            break;
        case WAVE_FORMAT_MPEG: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MPEG"));
            break;
        case WAVE_FORMAT_RT24: /* InSoft, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_RT24"));
            break;
        case WAVE_FORMAT_PAC: /* InSoft, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_PAC"));
            break;
        case WAVE_FORMAT_MPEGLAYER3: /* ISO/MPEG Layer3 Format Tag */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MPEGLAYER3"));
            break;
        case WAVE_FORMAT_LUCENT_G723: /* Lucent Technologies */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_LUCENT_G723"));
            break;
        case WAVE_FORMAT_CIRRUS: /* Cirrus Logic */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CIRRUS"));
            break;
        case WAVE_FORMAT_ESPCM: /* ESS Technology */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ESPCM"));
            break;
        case WAVE_FORMAT_VOXWARE: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE"));
            break;
        case WAVE_FORMAT_CANOPUS_ATRAC: /* Canopus, co., Ltd. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CANOPUS_ATRAC"));
            break;
        case WAVE_FORMAT_G726_ADPCM: /* APICOM */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G726_ADPCM"));
            break;
        case WAVE_FORMAT_G722_ADPCM: /* APICOM */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G722_ADPCM"));
            break;
        case WAVE_FORMAT_DSAT_DISPLAY: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DSAT_DISPLAY"));
            break;
        case WAVE_FORMAT_VOXWARE_BYTE_ALIGNED: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_BYTE_ALIGNED"));
            break;
        case WAVE_FORMAT_VOXWARE_AC8: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_AC8"));
            break;
        case WAVE_FORMAT_VOXWARE_AC10: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_AC10"));
            break;
        case WAVE_FORMAT_VOXWARE_AC16: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_AC16"));
            break;
        case WAVE_FORMAT_VOXWARE_AC20: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_AC20"));
            break;
        case WAVE_FORMAT_VOXWARE_RT24: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_RT24"));
            break;
        case WAVE_FORMAT_VOXWARE_RT29: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_RT29"));
            break;
        case WAVE_FORMAT_VOXWARE_RT29HW: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_RT29HW"));
            break;
        case WAVE_FORMAT_VOXWARE_VR12: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_VR12"));
            break;
        case WAVE_FORMAT_VOXWARE_VR18: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_VR18"));
            break;
        case WAVE_FORMAT_VOXWARE_TQ40: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_TQ40"));
            break;
        case WAVE_FORMAT_SOFTSOUND: /* Softsound, Ltd. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SOFTSOUND"));
            break;
        case WAVE_FORMAT_VOXWARE_TQ60: /* Voxware Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VOXWARE_TQ60"));
            break;
        case WAVE_FORMAT_MSRT24: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MSRT24"));
            break;
        case WAVE_FORMAT_G729A: /* AT&amp;T Labs, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G729A"));
            break;
        case WAVE_FORMAT_MVI_MVI2: /* Motion Pixels */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MVI_MVI2"));
            break;
        case WAVE_FORMAT_DF_G726: /* DataFusion Systems (Pty) (Ltd) */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DF_G726"));
            break;
        case WAVE_FORMAT_DF_GSM610: /* DataFusion Systems (Pty) (Ltd) */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DF_GSM610"));
            break;
        case WAVE_FORMAT_ISIAUDIO: /* Iterated Systems, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ISIAUDIO"));
            break;
        case WAVE_FORMAT_ONLIVE: /* OnLive! Technologies, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ONLIVE"));
            break;
        case WAVE_FORMAT_SBC24: /* Siemens Business Communications Sys */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SBC24"));
            break;
        case WAVE_FORMAT_DOLBY_AC3_SPDIF: /* Sonic Foundry */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DOLBY_AC3_SPDIF"));
            break;
        case WAVE_FORMAT_MEDIASONIC_G723: /* MediaSonic */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MEDIASONIC_G723"));
            break;
        case WAVE_FORMAT_PROSODY_8KBPS: /* Aculab plc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_PROSODY_8KBPS"));
            break;
        case WAVE_FORMAT_ZYXEL_ADPCM: /* ZyXEL Communications, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ZYXEL_ADPCM"));
            break;
        case WAVE_FORMAT_PHILIPS_LPCBB: /* Philips Speech Processing */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_PHILIPS_LPCBB"));
            break;
        case WAVE_FORMAT_PACKED: /* Studer Professional Audio AG */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_PACKED"));
            break;
        case WAVE_FORMAT_MALDEN_PHONYTALK: /* Malden Electronics Ltd. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MALDEN_PHONYTALK"));
            break;
        case WAVE_FORMAT_RHETOREX_ADPCM: /* Rhetorex Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_RHETOREX_ADPCM"));
            break;
        case WAVE_FORMAT_IRAT: /* BeCubed Software Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_IRAT"));
            break;
        case WAVE_FORMAT_VIVO_G723: /* Vivo Software */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VIVO_G723"));
            break;
        case WAVE_FORMAT_VIVO_SIREN: /* Vivo Software */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VIVO_SIREN"));
            break;
        case WAVE_FORMAT_DIGITAL_G723: /* Digital Equipment Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DIGITAL_G723"));
            break;
        case WAVE_FORMAT_SANYO_LD_ADPCM: /* Sanyo Electric Co., Ltd. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SANYO_LD_ADPCM"));
            break;
        case WAVE_FORMAT_SIPROLAB_ACEPLNET: /* Sipro Lab Telecom Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIPROLAB_ACEPLNET"));
            break;
        case WAVE_FORMAT_SIPROLAB_ACELP4800: /* Sipro Lab Telecom Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIPROLAB_ACELP4800"));
            break;
        case WAVE_FORMAT_SIPROLAB_ACELP8V3: /* Sipro Lab Telecom Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIPROLAB_ACELP8V3"));
            break;
        case WAVE_FORMAT_SIPROLAB_G729: /* Sipro Lab Telecom Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIPROLAB_G729"));
            break;
        case WAVE_FORMAT_SIPROLAB_G729A: /* Sipro Lab Telecom Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIPROLAB_G729A"));
            break;
        case WAVE_FORMAT_SIPROLAB_KELVIN: /* Sipro Lab Telecom Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SIPROLAB_KELVIN"));
            break;
        case WAVE_FORMAT_G726ADPCM: /* Dictaphone Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_G726ADPCM"));
            break;
        case WAVE_FORMAT_QUALCOMM_PUREVOICE: /* Qualcomm, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_QUALCOMM_PUREVOICE"));
            break;
        case WAVE_FORMAT_QUALCOMM_HALFRATE: /* Qualcomm, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_QUALCOMM_HALFRATE"));
            break;
        case WAVE_FORMAT_TUBGSM: /* Ring Zero Systems, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_TUBGSM"));
            break;
        case WAVE_FORMAT_MSAUDIO1: /* Microsoft Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_MSAUDIO1"));
            break;
        case WAVE_FORMAT_UNISYS_NAP_ADPCM: /* Unisys Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UNISYS_NAP_ADPCM"));
            break;
        case WAVE_FORMAT_UNISYS_NAP_ULAW: /* Unisys Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UNISYS_NAP_ULAW"));
            break;
        case WAVE_FORMAT_UNISYS_NAP_ALAW: /* Unisys Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UNISYS_NAP_ALAW"));
            break;
        case WAVE_FORMAT_UNISYS_NAP_16K: /* Unisys Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UNISYS_NAP_16K"));
            break;
        case WAVE_FORMAT_CREATIVE_ADPCM: /* Creative Labs, Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CREATIVE_ADPCM"));
            break;
        case WAVE_FORMAT_CREATIVE_FASTSPEECH8: /* Creative Labs, Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CREATIVE_FASTSPEECH8"));
            break;
        case WAVE_FORMAT_CREATIVE_FASTSPEECH10: /* Creative Labs, Inc */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CREATIVE_FASTSPEECH10"));
            break;
        case WAVE_FORMAT_UHER_ADPCM: /* UHER informatic GmbH */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UHER_ADPCM"));
            break;
        case WAVE_FORMAT_QUARTERDECK: /* Quarterdeck Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_QUARTERDECK"));
            break;
        case WAVE_FORMAT_ILINK_VC: /* I-link Worldwide */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ILINK_VC"));
            break;
        case WAVE_FORMAT_RAW_SPORT: /* Aureal Semiconductor */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_RAW_SPORT"));
            break;
        case WAVE_FORMAT_ESST_AC3: /* ESS Technology, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_ESST_AC3"));
            break;
        case WAVE_FORMAT_IPI_HSX: /* Interactive Products, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_IPI_HSX"));
            break;
        case WAVE_FORMAT_IPI_RPELP: /* Interactive Products, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_IPI_RPELP"));
            break;
        case WAVE_FORMAT_CS2: /* Consistent Software */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_CS2"));
            break;
        case WAVE_FORMAT_SONY_SCX: /* Sony Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SONY_SCX"));
            break;
        case WAVE_FORMAT_FM_TOWNS_SND: /* Fujitsu Corp. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_FM_TOWNS_SND"));
            break;
        case WAVE_FORMAT_BTV_DIGITAL: /* Brooktree Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_BTV_DIGITAL"));
            break;
        case WAVE_FORMAT_QDESIGN_MUSIC: /* QDesign Corporation */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_QDESIGN_MUSIC"));
            break;
        case WAVE_FORMAT_VME_VMPCM: /* AT&amp;T Labs, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_VME_VMPCM"));
            break;
        case WAVE_FORMAT_TPC: /* AT&amp;T Labs, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_TPC"));
            break;
        case WAVE_FORMAT_OLIGSM: /* Ing C. Olivetti &amp; C., S.p.A. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_OLIGSM"));
            break;
        case WAVE_FORMAT_OLIADPCM: /* Ing C. Olivetti &amp; C., S.p.A. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_OLIADPCM"));
            break;
        case WAVE_FORMAT_OLICELP: /* Ing C. Olivetti &amp; C., S.p.A. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_OLICELP"));
            break;
        case WAVE_FORMAT_OLISBC: /* Ing C. Olivetti &amp; C., S.p.A. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_OLISBC"));
            break;
        case WAVE_FORMAT_OLIOPR: /* Ing C. Olivetti &amp; C., S.p.A. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_OLIOPR"));
            break;
        case WAVE_FORMAT_LH_CODEC: /* Lernout &amp; Hauspie */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_LH_CODEC"));
            break;
        case WAVE_FORMAT_NORRIS: /* Norris Communications, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_NORRIS"));
            break;
        case WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS: /* AT&amp;T Labs, Inc. */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_SOUNDSPACE_MUSICOMPRESS"));
            break;
        case WAVE_FORMAT_DVM: /* FAST Multimedia AG */
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_DVM"));
            break;
        default:
            riffChunk->append(new QTableWidgetItem("WAVE_FORMAT_UNKNOWN"));
            break;
        }
        file->read(&tmp[0], 2);
        riff.numChannels = static_cast<unsigned short>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.numChannels)));
        file->read(&tmp[0], 4);
        riff.sampleRate =  static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.sampleRate)));
        file->read(&tmp[0], 4);
        riff.byteRate =  static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.byteRate)));
        file->read(&tmp[0], 2);
        riff.blockAlign = static_cast<unsigned short>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.blockAlign)));
        file->read(&tmp[0], 2);
        riff.bitsPerSample = static_cast<unsigned short>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.bitsPerSample)));
        file->read(&riff.subchunk2Id[0], 4);
        riffChunk->append(new QTableWidgetItem(QString("%1%2%3%4").arg(riff.subchunk2Id[0]).arg(riff.subchunk2Id[1]).arg(riff.subchunk2Id[2]).arg(riff.subchunk2Id[3])));
        file->read(&tmp[0], 4);
        riff.subchunk2Size =  static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
        riffChunk->append(new QTableWidgetItem(QString::number(riff.subchunk2Size)));
        return ("RIFF");
    }
    if (!strncmp(&chunkid[0], "LIST", 4)){
        char subchunkid[4];
        file->read(&tmp[0], 4);
        unsigned long size = 0;
        list.size = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
        file->read(&subchunkid[0],4);
        if (!strncmp(&subchunkid[0], "INFO", 4)){
            infoChunkNames->clear();
            infoChunk->clear();
            list.info = static_cast<struct info_ts *>(malloc (sizeof(struct info_ts)));
            while (size < list.size){
                file->read(&tmp[0], 4);
                size += 4;
                if (!strncmp(&tmp[0], "IARL", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->iarlSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->iarl = static_cast<unsigned char *>(malloc (list.info->iarlSize));
                    file->read(reinterpret_cast<char *>(list.info->iarl), list.info->iarlSize);
                    size += list.info->iarlSize;
                    infoChunkNames->append(new QTableWidgetItem("IARL"));
                    QString tmpstr;
                    for (int i = 0; list.info->iarl[i]!=0; i++){
                        tmpstr.append(list.info->iarl[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
/********************************************************************/
                if (!strncmp(&tmp[0], "IART", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->iartSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->iart = static_cast<unsigned char *>(malloc (list.info->iartSize));
                    file->read(reinterpret_cast<char *>(list.info->iart), list.info->iartSize);
                    size += list.info->iartSize;
                    infoChunkNames->append(new QTableWidgetItem("IART"));
                    QString tmpstr;
                    for (int i = 0; list.info->iart[i]!=0; i++){
                        tmpstr.append(list.info->iart[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ICMS", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->icmsSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->icms = static_cast<unsigned char *>(malloc (list.info->icmsSize));
                    file->read(reinterpret_cast<char *>(list.info->icms), list.info->icmsSize);
                    size += list.info->icmsSize;
                    infoChunkNames->append(new QTableWidgetItem("ICMS"));
                    QString tmpstr;
                    for (int i = 0; list.info->icms[i]!=0; i++){
                        tmpstr.append(list.info->icms[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ICMT", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->icmtSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->icmt = static_cast<unsigned char *>(malloc (list.info->icmtSize));
                    file->read(reinterpret_cast<char *>(list.info->icmt), list.info->icmtSize);
                    size += list.info->icmtSize;
                    infoChunkNames->append(new QTableWidgetItem("ICMT"));
                    QString tmpstr;
                    for (int i = 0; list.info->icmt[i]!=0; i++){
                        tmpstr.append(list.info->icmt[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ICOP", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->icopSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->icop = static_cast<unsigned char *>(malloc (list.info->icopSize));
                    file->read(reinterpret_cast<char *>(list.info->icop), list.info->icopSize);
                    size += list.info->icopSize;
                    infoChunkNames->append(new QTableWidgetItem("ICOP"));
                    QString tmpstr;
                    for (int i = 0; list.info->icop[i]!=0; i++){
                        tmpstr.append(list.info->icop[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ICRD", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->icrdSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->icrd = static_cast<unsigned char *>(malloc (list.info->icrdSize));
                    file->read(reinterpret_cast<char *>(list.info->icrd), list.info->icrdSize);
                    size += list.info->icrdSize;
                    infoChunkNames->append(new QTableWidgetItem("ICRD"));
                    QString tmpstr;
                    for (int i = 0; list.info->icrd[i]!=0; i++){
                        tmpstr.append(list.info->icrd[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ICRP", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->icrpSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->icrp = static_cast<unsigned char *>(malloc (list.info->icrpSize));
                    file->read(reinterpret_cast<char *>(list.info->icrp), list.info->icrpSize);
                    size += list.info->icrpSize;
                    infoChunkNames->append(new QTableWidgetItem("ICRP"));
                    QString tmpstr;
                    for (int i = 0; list.info->icrp[i]!=0; i++){
                        tmpstr.append(list.info->icrp[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IDIM", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->idimSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->idim = static_cast<unsigned char *>(malloc (list.info->idimSize));
                    file->read(reinterpret_cast<char *>(list.info->idim), list.info->idimSize);
                    size += list.info->idimSize;
                    infoChunkNames->append(new QTableWidgetItem("IDIM"));
                    QString tmpstr;
                    for (int i = 0; list.info->idim[i]!=0; i++){
                        tmpstr.append(list.info->idim[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IDPI", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->idpiSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->idpi = static_cast<unsigned char *>(malloc (list.info->idpiSize));
                    file->read(reinterpret_cast<char *>(list.info->idpi), list.info->idpiSize);
                    size += list.info->idpiSize;
                    infoChunkNames->append(new QTableWidgetItem("IDPI"));
                    QString tmpstr;
                    for (int i = 0; list.info->idpi[i]!=0; i++){
                        tmpstr.append(list.info->idpi[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IENG", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->iengSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->ieng = static_cast<unsigned char *>(malloc (list.info->iengSize));
                    file->read(reinterpret_cast<char *>(list.info->ieng), list.info->iengSize);
                    size += list.info->iengSize;
                    infoChunkNames->append(new QTableWidgetItem("IENG"));
                    QString tmpstr;
                    for (int i = 0; list.info->ieng[i]!=0; i++){
                        tmpstr.append(list.info->ieng[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IGNR", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->ignrSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->ignr = static_cast<unsigned char *>(malloc (list.info->ignrSize));
                    file->read(reinterpret_cast<char *>(list.info->ignr), list.info->ignrSize);
                    size += list.info->ignrSize;
                    infoChunkNames->append(new QTableWidgetItem("IGNR"));
                    QString tmpstr;
                    for (int i = 0; list.info->ignr[i]!=0; i++){
                        tmpstr.append(list.info->ignr[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IKEY", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->ikeySize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->ikey = static_cast<unsigned char *>(malloc (list.info->ikeySize));
                    file->read(reinterpret_cast<char *>(list.info->ikey), list.info->ikeySize);
                    size += list.info->ikeySize;
                    infoChunkNames->append(new QTableWidgetItem("IKEY"));
                    QString tmpstr;
                    for (int i = 0; list.info->ikey[i]!=0; i++){
                        tmpstr.append(list.info->ikey[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ILGT", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->ilgtSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->ilgt = static_cast<unsigned char *>(malloc (list.info->ilgtSize));
                    file->read(reinterpret_cast<char *>(list.info->ilgt), list.info->ilgtSize);
                    size += list.info->ilgtSize;
                    infoChunkNames->append(new QTableWidgetItem("ILGT"));
                    QString tmpstr;
                    for (int i = 0; list.info->ilgt[i]!=0; i++){
                        tmpstr.append(list.info->ilgt[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IMED", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->imedSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->imed = static_cast<unsigned char *>(malloc (list.info->imedSize));
                    file->read(reinterpret_cast<char *>(list.info->imed), list.info->imedSize);
                    size += list.info->imedSize;
                    infoChunkNames->append(new QTableWidgetItem("IMED"));
                    QString tmpstr;
                    for (int i = 0; list.info->imed[i]!=0; i++){
                        tmpstr.append(list.info->imed[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "INAM", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->inamSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->inam = static_cast<unsigned char *>(malloc (list.info->inamSize));
                    file->read(reinterpret_cast<char *>(list.info->iarl), list.info->inamSize);
                    size += list.info->inamSize;
                    infoChunkNames->append(new QTableWidgetItem("INAM"));
                    QString tmpstr;
                    for (int i = 0; list.info->inam[i]!=0; i++){
                        tmpstr.append(list.info->inam[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IPLT", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->ipltSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->iplt = static_cast<unsigned char *>(malloc (list.info->ipltSize));
                    file->read(reinterpret_cast<char *>(list.info->iplt), list.info->ipltSize);
                    size += list.info->ipltSize;
                    infoChunkNames->append(new QTableWidgetItem("IPLT"));
                    QString tmpstr;
                    for (int i = 0; list.info->iplt[i]!=0; i++){
                        tmpstr.append(list.info->iplt[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "IPRD", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->iprdSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->iprd = static_cast<unsigned char *>(malloc (list.info->iprdSize));
                    file->read(reinterpret_cast<char *>(list.info->iprd), list.info->iprdSize);
                    size += list.info->iprdSize;
                    infoChunkNames->append(new QTableWidgetItem("IPRD"));
                    QString tmpstr;
                    for (int i = 0; list.info->iprd[i]!=0; i++){
                        tmpstr.append(list.info->iprd[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ISBJ", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->isbjSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->isbj = static_cast<unsigned char *>(malloc (list.info->isbjSize));
                    file->read(reinterpret_cast<char *>(list.info->isbj), list.info->isbjSize);
                    size += list.info->isbjSize;
                    infoChunkNames->append(new QTableWidgetItem("ISBJ"));
                    QString tmpstr;
                    for (int i = 0; list.info->isbj[i]!=0; i++){
                        tmpstr.append(list.info->isbj[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ISFT", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->isftSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->isft = static_cast<unsigned char *>(malloc (list.info->isftSize));
                    file->read(reinterpret_cast<char *>(list.info->isft), list.info->isftSize);
                    size += list.info->isftSize;
                    infoChunkNames->append(new QTableWidgetItem("ISFT"));
                    QString tmpstr;
                    for (int i = 0; list.info->isft[i]!=0; i++){
                        tmpstr.append(list.info->isft[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ISHP", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->ishpSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->ishp = static_cast<unsigned char *>(malloc (list.info->ishpSize));
                    file->read(reinterpret_cast<char *>(list.info->ishp), list.info->ishpSize);
                    size += list.info->ishpSize;
                    infoChunkNames->append(new QTableWidgetItem("ISHP"));
                    QString tmpstr;
                    for (int i = 0; list.info->ishp[i]!=0; i++){
                        tmpstr.append(list.info->ishp[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ISRC", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->isrcSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->isrc = static_cast<unsigned char *>(malloc (list.info->isrcSize));
                    file->read(reinterpret_cast<char *>(list.info->isrc), list.info->isrcSize);
                    size += list.info->isrcSize;
                    infoChunkNames->append(new QTableWidgetItem("ISRC"));
                    QString tmpstr;
                    for (int i = 0; list.info->isrc[i]!=0; i++){
                        tmpstr.append(list.info->isrc[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ISRF", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->isrfSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->isrf = static_cast<unsigned char *>(malloc (list.info->isrfSize));
                    file->read(reinterpret_cast<char *>(list.info->isrf), list.info->isrfSize);
                    size += list.info->isrfSize;
                    infoChunkNames->append(new QTableWidgetItem("ISRF"));
                    QString tmpstr;
                    for (int i = 0; list.info->isrf[i]!=0; i++){
                        tmpstr.append(list.info->isrf[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
                if (!strncmp(&tmp[0], "ITCH", 4)){
                    file->read(&tmp[0], 4);
                    size += 4;
                    list.info->itchSize = static_cast<unsigned long>(static_cast<unsigned char>(tmp[0]) | static_cast<unsigned char>(tmp[1]) << 8 | static_cast<unsigned char>(tmp[2]) <<16 | static_cast<unsigned char>(tmp[3]) << 24);
                    list.info->itch = static_cast<unsigned char *>(malloc (list.info->itchSize));
                    file->read(reinterpret_cast<char *>(list.info->itch), list.info->itchSize);
                    size += list.info->itchSize;
                    infoChunkNames->append(new QTableWidgetItem("ITCH"));
                    QString tmpstr;
                    for (int i = 0; list.info->itch[i]!=0; i++){
                        tmpstr.append(list.info->itch[i]);
                    }
                    infoChunk->append(new QTableWidgetItem(tmpstr));
                    continue;
                }
            }
            return ("LIST(INFO)");
        }
        if (!strncmp(&subchunkid[0], "EXIF", 4)){
            return ("LIST(EXIF)");
        }
    }
    return nullptr;
}

void MainWindow::viewChunkInfo(QTreeWidgetItem *item, int column){
    ui->tableWidget->clear();
    if(item->text(column) == "RIFF"){
        ui->tableWidget->setColumnCount(2);
        ui->tableWidget->setRowCount(13);
        ui->tableWidget->setItem(0, 0, riffChunkNames->at(0));
        ui->tableWidget->setItem(0, 1, riffChunk->at(0));

        ui->tableWidget->setItem(1, 0, riffChunkNames->at(1));
        ui->tableWidget->setItem(1, 1, riffChunk->at(1));

        ui->tableWidget->setItem(2, 0, riffChunkNames->at(2));
        ui->tableWidget->setItem(2, 1, riffChunk->at(2));

        ui->tableWidget->setItem(3, 0, riffChunkNames->at(3));
        ui->tableWidget->setItem(3, 1, riffChunk->at(3));

        ui->tableWidget->setItem(4, 0, riffChunkNames->at(4));
        ui->tableWidget->setItem(4, 1, riffChunk->at(4));

        ui->tableWidget->setItem(5, 0, riffChunkNames->at(5));
        ui->tableWidget->setItem(5, 1, riffChunk->at(5));

        ui->tableWidget->setItem(6, 0, riffChunkNames->at(6));
        ui->tableWidget->setItem(6, 1, riffChunk->at(6));

        ui->tableWidget->setItem(7, 0, riffChunkNames->at(7));
        ui->tableWidget->setItem(7, 1, riffChunk->at(7));

        ui->tableWidget->setItem(8, 0, riffChunkNames->at(8));
        ui->tableWidget->setItem(8, 1, riffChunk->at(8));

        ui->tableWidget->setItem(9, 0, riffChunkNames->at(9));
        ui->tableWidget->setItem(9, 1, riffChunk->at(9));

        ui->tableWidget->setItem(10, 0, riffChunkNames->at(10));
        ui->tableWidget->setItem(10, 1, riffChunk->at(10));

        ui->tableWidget->setItem(11, 0, riffChunkNames->at(11));
        ui->tableWidget->setItem(11, 1, riffChunk->at(11));

        ui->tableWidget->setItem(12, 0, riffChunkNames->at(12));
        ui->tableWidget->setItem(12, 1, riffChunk->at(12));
    }
    if(item->text(column) == "LIST(INFO)"){
        ui->tableWidget->setColumnCount(2);
        ui->tableWidget->setRowCount(infoChunkNames->size());
        for (int i = 0; i < infoChunkNames->size(); i++){
            ui->tableWidget->setItem(i, 0, infoChunkNames->at(i));
            ui->tableWidget->setItem(i, 1, infoChunk->at(i));
        }
    }
}

