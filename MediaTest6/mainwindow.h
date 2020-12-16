#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "vlc/vlc.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


namespace Ui {
class MainWindow;
}
struct libvlc_media_track_info_t;
struct libvlc_media_t;
struct libvlc_instance_t;
struct libvlc_media_player_t;
struct libvlc_event_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadStyleSheet();

    void attachEvents(libvlc_media_player_t *m_mediaPlayer);

    void play();//播放
    void pause();//暂停
    void stop();//停止
    bool loadVlc();//加载
    void freeVlc();//释放

    void getEndTime();//获取视频总时长

    void getVolume();

signals:
    void currentTime();
    void getMediaInfo();

protected:
    static void vlcEvents(const libvlc_event_t *ev, void *param);//VLC事件

    void enterEvent(QEvent *);
//    void leaveEvent(QEvent *);
//    void resizeEvent(QResizeEvent *event);//界面大小发生改变事件

    bool eventFilter(QObject * watched, QEvent * event);

private slots:
    void on_pushButton_play_clicked();//播放/暂停

    void on_pushButton_stop_clicked();//停止

    void on_pushButton_play2_clicked();

    void on_pushButton_max_clicked();//最大化

    void on_pushButton_close_clicked();//关闭

    void on_pushButton_volume_clicked();

    void currentTime2();//视频播放时间

    void getMediaInfo2();//视频的一些基础信息

    void on_horizontalSlider_sliderMoved(int position);

    void on_Slider_Volume_sliderMoved(int position);

    void on_Slider_Volume_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    libvlc_instance_t *m_vlcInstance = Q_NULLPTR;
    libvlc_media_t *m_media = Q_NULLPTR;
    libvlc_media_player_t *m_mediaPlayer = Q_NULLPTR;
    libvlc_event_manager_t *m_eventManager;

    bool m_bPlaying = false;
    bool m_bMute = false;
};
#endif // MAINWINDOW_H
