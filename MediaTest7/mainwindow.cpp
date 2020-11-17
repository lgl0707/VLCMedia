#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>
#include "vlc/vlc.h"
#include <QDebug>
#include <Windows.h>
#include <QPoint>
#include "mediactrl.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

//    setStyleSheet("border-image: url(:/Resources/media-png/background.png)");
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(192,253,123));
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/Resources/media-png/background.png")));
    this->setPalette(palette);

    mediactrl = new MediaCtrl(this);
    loadVlc("");

    connect(mediactrl, SIGNAL(onPlay()), this, SLOT(on_pushButton_clicked()));
    connect(mediactrl, SIGNAL(onPause()), this, SLOT(on_pushButton_2_clicked()));
    connect(mediactrl, SIGNAL(onStop()), this, SLOT(on_pushButton_3_clicked()));
    connect(mediactrl, SIGNAL(onMaxOrNormal()), this, SLOT(on_pushButton_4_clicked()));
    connect(mediactrl, SIGNAL(onclose()), this, SLOT(on_pushButton_5_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
    freeVlc();
}

//加载
bool MainWindow::load(const QString &filepath)
{
    if(filepath.isEmpty()){
        qDebug() << "filepath isEmpty";
        return false;
    }
    if(!QFile::exists(filepath)){
        qDebug() << "QFile no exists";
        return false;
    }
    bool bload = loadVlc(QDir::toNativeSeparators(filepath).toUtf8().constData());
    if(!bload){
        qDebug() << "bload no isEmpty";
        return false;
    }
    return true;
}

//播放
void MainWindow::play()
{
    if(libvlc_Ended == libvlc_media_get_state(m_media)){
        stop();//vlc这个设定太奇怪了，当自然播放结束，重新播放还得主动调用一次stop();
    }
    libvlc_media_player_play(m_mediaPlayer);
}

//暂停
void MainWindow::pause()
{
    if((bool)libvlc_media_player_is_playing(m_mediaPlayer)){
        m_bPlaying = false;
        libvlc_media_player_set_pause(m_mediaPlayer, 1);//暂停
    }
}

//停止
void MainWindow::stop()
{
    m_bPlaying = false;
    libvlc_media_player_stop(m_mediaPlayer);
}

//加载
bool MainWindow::loadVlc(const QString &filepath)
{
    m_vlcInstance = libvlc_new(0, NULL);
    //m_media = libvlc_media_new_path(m_vlcInstance, filepath.toUtf8().data());
    //m_media = libvlc_media_new_location (m_vlcInstance, "rtmp://192.168.4.31:1935/live/movie");rtmp://58.200.131.2:1935/livetv/hunantv
    m_media = libvlc_media_new_location (m_vlcInstance, "rtmp://58.200.131.2:1935/livetv/hunantv");
    //m_media = libvlc_media_new_location (m_vlcInstance, "rtmp://192.168.4.31:1935/live/movie");
    if(!m_media){
        freeVlc();
        return m_media;
    }
    qDebug() << "loadVlc" << qPrintable(filepath);
    m_mediaPlayer = libvlc_media_player_new_from_media(m_media);
    if(!m_mediaPlayer){
        freeVlc();
        return false;
    }
    libvlc_media_parse(m_media);
    libvlc_event_manager_t *em = libvlc_media_player_event_manager(m_mediaPlayer);
    libvlc_event_attach(em, libvlc_MediaPlayerTimeChanged, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerEndReached, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerStopped, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPlaying, vlcEvents, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPaused, vlcEvents, this);

    //libvlc_media_player_set_hwnd(m_mediaPlayer, (void *)ui->label->winId());
    libvlc_media_player_set_hwnd(m_mediaPlayer, (void *)ui->centralwidget->winId());
    return true;
}

//VLC事件
void MainWindow::vlcEvents(const libvlc_event_t *ev, void *param)
{
    Q_UNUSED(param);
    switch (ev->type) {
    case libvlc_MediaPlayerTimeChanged:
        break;
    case libvlc_MediaPlayerEndReached://不能在回调里调用stop();否则会卡死
        break;
    case libvlc_MediaPlayerStopped:
        qDebug() << "libvlc_MediaPlayerStopped";
        break;
    case libvlc_MediaPlayerPlaying:
        qDebug() << "libvlc_MediaPlayerPlay";
        break;
    case libvlc_MediaPlayerPaused:
        qDebug() << "libvlc_MediaPlayerPaused";
        break;
    }
}

//释放
void MainWindow::freeVlc()
{
    if(m_media){
        libvlc_media_release(m_media);
        m_media = Q_NULLPTR;
    }
    if(m_mediaPlayer){
        libvlc_media_player_stop(m_mediaPlayer);
        libvlc_media_player_release(m_mediaPlayer);
        m_mediaPlayer = Q_NULLPTR;
    }
    if(m_vlcInstance){
        libvlc_release(m_vlcInstance);
        m_vlcInstance = Q_NULLPTR;
    }
}

void MainWindow::on_pushButton_clicked()
{
    play();
}

void MainWindow::on_pushButton_3_clicked()
{
    pause();
}

void MainWindow::on_pushButton_2_clicked()
{
    stop();
}

void MainWindow::on_pushButton_4_clicked()
{
    QWidget *pWindow = this->window();

    if(pWindow->isMaximized())
    {
        pWindow->showNormal();
    }
    else
    {
        pWindow->showMaximized();
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    mediactrl->close();
    close();
}

void MainWindow::enterEvent(QEvent *)
{
    mediactrl->show();
}
