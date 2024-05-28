#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QPushButton>
#include<QSlider>
#include<QString>
#include<QAction>
#include<QFileDialog>
#include<QTime>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_instance = libvlc_new(0,NULL);
    if(m_instance==NULL)
    {
        qDebug()<<"instance null";
    }
    else
    {
        qDebug()<<"istance is ok...";
    }
    m_MediaPlayStatus = Media_Status_None;
    connect(ui->startButton,&QPushButton::clicked,this,&MainWindow::OnclickedStartButton);
    connect(ui->endButton,&QPushButton::clicked,this,&MainWindow::OnclickedEndButton);
    ui->progressSlider->setMaximum(1000);
    ui->volumeSlider->setMaximum(100);
    connect(ui->progressSlider,&QSlider::valueChanged,this,&MainWindow::OnProgressValueChange);
    connect(ui->volumeSlider,&QSlider::valueChanged,this,&MainWindow::OnVolumeValueChange);
    connect(ui->actionopen,&QAction::triggered,this,&MainWindow::OpenFile);
    m_updateTime = new QTimer(this);
    connect(m_updateTime,&QTimer::timeout,this,&MainWindow::UpdateUserInterface);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::MediaPlayerdrawWindow(libvlc_media_player_t *player)
{
//将vlc的窗口嵌入到布局中
    libvlc_media_player_set_hwnd(player,(void*)ui->video_show->winId());
    if(m_updateTime)
    {
        m_updateTime->start(100);
    }
}

void MainWindow::MediaPlay()
{
    if(m_VlcMedia==NULL)
    {
        return;
    }
    if(m_MediaPlayStatus==Media_Status_Play)
    {
        libvlc_media_player_pause(m_VlcMedia);
        m_MediaPlayStatus=Media_Status_Pause;
    }
    else if(m_MediaPlayStatus==Media_Status_Pause)
    {
        libvlc_media_player_play(m_VlcMedia);
        m_MediaPlayStatus=Media_Status_Play;
    }
    else
    {
        libvlc_media_player_play(m_VlcMedia);
        m_MediaPlayStatus=Media_Status_Play;
    }

}

void MainWindow::OnclickedStartButton()
{
    MediaPlay();
}

void MainWindow::OnclickedEndButton()
{
    if(m_VlcMedia!=NULL)
    {
        libvlc_media_player_release(m_VlcMedia);
        m_VlcMedia=NULL;
        m_MediaPlayStatus=Media_Status_None;
    }
}

void MainWindow::UpdateUserInterface()
{
    if(m_VlcMedia==NULL)
    {
        return;
    }
    if(libvlc_media_player_is_playing(m_VlcMedia))
    {
        int progressPos = libvlc_media_player_get_position(m_VlcMedia)*ui->progressSlider->maximum();
        int volumePos = libvlc_audio_get_volume(m_VlcMedia);
        bool status= ui->progressSlider->blockSignals(true);
        ui->progressSlider->setValue(progressPos);
        ui->progressSlider->blockSignals(status);
        bool statusVolume= ui->volumeSlider->blockSignals(true);
        ui->volumeSlider->setValue(volumePos);
        ui->volumeSlider->blockSignals(statusVolume);
        //显示当前时间
        int currentTimeMs = libvlc_media_player_get_time(m_VlcMedia);
        QTime currentTime = QTime(0, 0).addMSecs(currentTimeMs);
        ui->startTime->setText(currentTime.toString("hh:mm:ss"));
        //显示剩余时间
        int totalTimeMs = libvlc_media_player_get_length(m_VlcMedia);
        int remainingTimeMs = totalTimeMs - currentTimeMs;
        QTime remainingTime = QTime(0, 0).addMSecs(remainingTimeMs);
        ui->endTime->setText(remainingTime.toString("hh:mm:ss"));

    }
}

void MainWindow::OnProgressValueChange(int value)
{
    if(m_VlcMedia==NULL)
    {
        return;
    }
    if(libvlc_media_player_is_playing(m_VlcMedia))
    {
        float pos = (float)value/(float)ui->progressSlider->maximum();
        libvlc_media_player_set_position(m_VlcMedia,pos);
    }

}

void MainWindow::OnVolumeValueChange(int value)
{
    if(m_VlcMedia==NULL)
    {
        return;
    }
    if(libvlc_media_player_is_playing(m_VlcMedia))
    {
        int volume=value;
       libvlc_audio_set_volume(m_VlcMedia,volume);
    }
}

void MainWindow::OpenFile()
{
    QString filePathQstring = QFileDialog::getOpenFileName(this,tr("open Media file"),".",tr("Media Files(*.avi *.mp3 *.mp4 *.wav *.mkv)"));
    if (filePathQstring.length()!=0)
    {
        qDebug()<<filePathQstring;
        filePathQstring= QDir::toNativeSeparators(filePathQstring);
        const char *filePath = filePathQstring.toUtf8().constData(); // 转换为const char*
        libvlc_media_t *media = libvlc_media_new_path(m_instance,filePath);
        m_VlcMedia = libvlc_media_player_new_from_media(media);
        libvlc_media_release(media);
        MediaPlayerdrawWindow(m_VlcMedia);
        MediaPlay();
    }
}

