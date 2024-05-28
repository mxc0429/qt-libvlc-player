#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<vlc/vlc.h>
#include<QTimer>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum MediaPlayStatus{
    Media_Status_Play,
    Media_Status_Pause,
    Media_Status_None
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void MediaPlayerdrawWindow(libvlc_media_player_t *player);
    void MediaPlay();
libvlc_instance_t * m_instance;
libvlc_media_player_t * m_VlcMedia;
MediaPlayStatus m_MediaPlayStatus;
QTimer * m_updateTime;
public slots:
    void OnclickedStartButton();
    void OnclickedEndButton();
    void UpdateUserInterface();
    void OnProgressValueChange(int value);
    void OnVolumeValueChange(int value);
    void OpenFile();
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
