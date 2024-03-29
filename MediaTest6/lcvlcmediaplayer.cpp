﻿#include "lcvlcmediaplayer.h"
#include "ui_lcvlcmediaplayer.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QMouseEvent>
#include "configmanager.h"

LCVLCMediaPlayer::LCVLCMediaPlayer(QString url, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LCVLCMediaPlayer)
{
    ui->setupUi(this);
    setWindowTitle("低空通航");
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->horizontalSlider->installEventFilter(this);
    ui->Slider_Volume->installEventFilter(this);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0,0,0,0);
    vLayout->setSpacing(0);

    vLayout->addWidget(ui->verticalWidget);
    centralWidget()->setLayout(vLayout);

    //设置为窗口模式
    ui->verticalWidget->setWindowFlags(Qt::FramelessWindowHint|Qt::Window);
    //设置背景透明
    ui->verticalWidget->setAttribute(Qt::WA_TranslucentBackground,true);
    ui->verticalWidget->show();

    QPoint globalPos = this->mapToGlobal(QPoint(0,0));//父窗口绝对坐标
    int x = globalPos.x() + (this->width() - ui->verticalWidget->width()) / 2;//x坐标
    int y = globalPos.y() + (this->height() - ui->verticalWidget->height()) / 2;//y坐标
    ui->verticalWidget->move(x, y);

    loadStyleSheet();
    loadVlc();

    ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/volume1.png);} \
                                        QToolTip {border-image: url(:/Resources/media-png/white.png);}");
    ui->pushButton_play->setToolTip("播放");
    ui->pushButton_max->setToolTip("全屏");
    ui->pushButton_stop->setToolTip("停止");
    ui->pushButton_close->setToolTip("关闭");
    ui->pushButton_volume->setToolTip("静音");

    ui->label_timing->setText("00:00:00");
    ui->label_end->setText("00:00:00");

    ui->Slider_Volume->setMinimum(0);
    ui->Slider_Volume->setMaximum(100);
    ui->Slider_Volume->setSingleStep(1);

    ui->Slider_Volume->setSliderPosition(100);
    ui->label_volume->setText("100%");


//    connect(this, SIGNAL(setEndTime()), this, SLOT(setEndTime2()));
    connect(this, SIGNAL(getMediaInfo()), this, SLOT(getMediaInfo2()));
    connect(this, SIGNAL(currentTime()), this, SLOT(currentTime2()));

    on_pushButton_play_clicked();
}

LCVLCMediaPlayer::~LCVLCMediaPlayer()
{
    delete ui;
    freeVlc();
}

void LCVLCMediaPlayer::loadStyleSheet()
{
    QFile file(":/Resources/media-ui.css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }

    QFont font( "Microsoft YaHei", 13, 75);
    ui->label_volume->setFont(font);
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::white);
    ui->label_volume->setPalette(pa);
    ui->label_end->setPalette(pa);
    ui->label_timing->setPalette(pa);
}

//加载
bool LCVLCMediaPlayer::loadVlc()
{
    m_vlcInstance = libvlc_new(0, nullptr);

    QString mediaaddr = ConfigManager::getValueConfig(MEDIAADDR);
    //m_media = libvlc_media_new_location (m_vlcInstance, "rtmp://58.200.131.2:1935/livetv/hunantv");
    m_media = libvlc_media_new_path(m_vlcInstance, "C:\\Users\\liugl\\Desktop\\shipin1.flv");
    //m_media = libvlc_media_new_location (m_vlcInstance, mediaaddr.toStdString().c_str());

    if(!m_media){
        freeVlc();
        return m_media;
    }

    m_mediaPlayer = libvlc_media_player_new_from_media(m_media);
    if(!m_mediaPlayer){
        freeVlc();
        return false;
    }

    // 忽略事件处理鼠标和键盘
    libvlc_video_set_mouse_input(m_mediaPlayer, false);
    libvlc_video_set_key_input(m_mediaPlayer, false);

    libvlc_media_parse(m_media);

    //订阅事件
    attachEvents(m_mediaPlayer);

    libvlc_media_player_set_hwnd(m_mediaPlayer, (void *)ui->centralwidget->winId());
    return true;
}

//释放
void LCVLCMediaPlayer::freeVlc()
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

void LCVLCMediaPlayer::attachEvents(libvlc_media_player_t *m_mediaPlayer)
{
    m_eventManager = libvlc_media_player_event_manager(m_mediaPlayer);
    // 事件列表
    QList<libvlc_event_e> events;
    events << libvlc_MediaPlayerOpening
           << libvlc_MediaPlayerBuffering
           << libvlc_MediaPlayerPlaying
           << libvlc_MediaPlayerPaused
           << libvlc_MediaPlayerStopped
           << libvlc_MediaPlayerEncounteredError
           << libvlc_MediaPlayerLengthChanged
           << libvlc_MediaPlayerTimeChanged
           << libvlc_MediaPlayerPositionChanged
           << libvlc_MediaPlayerEndReached;

    // 订阅事件
    foreach (const libvlc_event_e &e, events) {
        libvlc_event_attach(m_eventManager, e, vlcEvents, this);
    }
}

//VLC事件
void LCVLCMediaPlayer::vlcEvents(const libvlc_event_t *ev, void *param)
{
    LCVLCMediaPlayer *pwindow = (LCVLCMediaPlayer *)param;
    Q_UNUSED(param);
    switch (ev->type) {
    case libvlc_MediaPlayerTimeChanged:
        emit pwindow->currentTime();
        break;
    case libvlc_MediaPlayerEndReached://不能在回调里调用stop();否则会卡死
        break;
    case libvlc_MediaPlayerStopped:
        qDebug() << "libvlc_MediaPlayerStopped";
        break;
    case libvlc_MediaPlayerPlaying:
        qDebug() << "libvlc_MediaPlayerPlay";
        emit pwindow->getMediaInfo();
        break;
    case libvlc_MediaPlayerPaused:
        qDebug() << "libvlc_MediaPlayerPaused";
        break;
    // 时长改变
    case libvlc_MediaPlayerLengthChanged: {
        break;
       }
    }
}

//播放
void LCVLCMediaPlayer::play()
{
    if(libvlc_Ended == libvlc_media_get_state(m_media)){
        stop();//vlc这个设定太奇怪了，当自然播放结束，重新播放还得主动调用一次stop();
    }
    libvlc_media_player_play(m_mediaPlayer);
}

//暂停
void LCVLCMediaPlayer::pause()
{
    if((bool)libvlc_media_player_is_playing(m_mediaPlayer)){
        m_bPlaying = false;
        libvlc_media_player_set_pause(m_mediaPlayer, 1);//暂停
    }
}

//停止
void LCVLCMediaPlayer::stop()
{
    m_bPlaying = false;
    libvlc_media_player_stop(m_mediaPlayer);
}

void LCVLCMediaPlayer::getMediaInfo2()
{
    getEndTime();

    getVolume();

    ui->centralwidget->setStyleSheet("background-color: rgb(0, 0, 0, 0)");
    loadStyleSheet();
}

void LCVLCMediaPlayer::getVolume()
{
//    if (m_mediaPlayer != nullptr)
//    {
//        int mute = libvlc_audio_get_mute(m_mediaPlayer);
//        if(mute == 1)
//        {
//             ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/mute1.png);} \
//                                                 QToolTip {border-image: url(:/Resources/media-png/white.png);}");
//        }else
//        {
//             ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/volume1.png);} \
//                                                 QToolTip {border-image: url(:/Resources/media-png/white.png);}");
//        }

//首次获取音量是有的时候获取不到，因此设置为实时获取
//        int volume = libvlc_audio_get_volume(m_mediaPlayer);
//        ui->Slider_Volume->setValue(volume);

//        QString strvolume = QString::number(volume);
//        strvolume = "%" + strvolume;

//        QPalette pa;
//        pa.setColor(QPalette::WindowText,Qt::white);
//        ui->label_volume->setPalette(pa);
//        QFont font( "Microsoft YaHei", 13, 75);
//        ui->label_volume->setFont(font);
//        ui->label_volume->setText(strvolume);

//        QFont font( "Microsoft YaHei", 13, 75);
//        ui->label_volume->setFont(font);
//    }
}

void LCVLCMediaPlayer::getEndTime()
{
    //获取的为ms
    libvlc_time_t play_time = libvlc_media_player_get_length(m_mediaPlayer);
    play_time /= 1000;
    int iHour = int(play_time)/3600;
    int iMin = (int(play_time)-iHour*3600)/60;
    int iSen = int(play_time)-iHour*3600-iMin*60;

    QString strhour, strmin, strsen;
    if(iHour < 10)
    {
        strhour = "0"+QString::number(iHour);
    } else {
        strhour = QString::number(iHour);
    }
    if(iMin < 10)
    {
        strmin = ":0"+QString::number(iMin);
    } else {
        strmin = ":"+QString::number(iMin);
    }
    if(iSen < 10)
    {
        strsen = ":0"+QString::number(iSen);
    } else {
        strsen = ":"+QString::number(iSen);
    }

    QString endtime = strhour + strmin + strsen;

    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(int(play_time));
    ui->horizontalSlider->setSingleStep(1); // 步长

    ui->label_end->setText(endtime);
}

void LCVLCMediaPlayer::currentTime2()
{
    getEndTime();

    libvlc_time_t play_current_time = libvlc_media_player_get_time(m_mediaPlayer);
    play_current_time /= 1000;
    int iHour = int(play_current_time)/3600;
    int iMin = (int(play_current_time)-iHour*3600)/60;
    int iSen = int(play_current_time)-iHour*3600-iMin*60;

    //设置进度条的值
//    ui->horizontalSlider->setValue(int(play_current_time));
    ui->horizontalSlider->setSliderPosition(int(play_current_time));


    //当前播放时间拼接：格式00:00:00
    QString strhour, strmin, strsen;
    if(iHour < 10)
    {
        strhour = "0"+QString::number(iHour);
    } else {
        strhour = QString::number(iHour);
    }
    if(iMin < 10)
    {
        strmin = ":0"+QString::number(iMin);
    } else {
        strmin = ":"+QString::number(iMin);
    }
    if(iSen < 10)
    {
        strsen = ":0"+QString::number(iSen);
    } else {
        strsen = ":"+QString::number(iSen);
    }

    QString currtime = strhour + strmin + strsen;

    ui->label_timing->setText(currtime);

    //首次获取音量是有的时候获取不到，因此设置为实时获取
//    int volume = libvlc_audio_get_volume(m_mediaPlayer);
//    ui->Slider_Volume->setValue(volume);

    int volume = ui->Slider_Volume->value();
    if(m_bMute)
    {
        libvlc_audio_set_mute(m_mediaPlayer, 1);
        ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/mute1.png);} \
                                            QToolTip {border-image: url(:/Resources/media-png/white.png);}");
        ui->Slider_Volume->setStyleSheet("QSlider::handle:horizontal{width:10px;background-color:rgb(255,255,255);margin:-11px 0px -11px 0px;border-radius:12px;}\
                                         QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
                                         QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
                                         QSlider::sub-page:horizontal{background-color:rgb(150,150,150);}");
    }else {
        libvlc_audio_set_mute(m_mediaPlayer, 0);
        ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/volume1.png);} \
                                            QToolTip {border-image: url(:/Resources/media-png/white.png);}");
        ui->Slider_Volume->setStyleSheet("QSlider::handle:horizontal{width:10px;background-color:rgb(255,255,255);margin:-11px 0px -11px 0px;border-radius:12px;}\
                                         QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
                                         QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
                                         QSlider::sub-page:horizontal{background-color:rgb(26,217,110);}");

        //设置播放音量
        libvlc_audio_set_volume(m_mediaPlayer, volume);
    }

    QString strvolume = QString::number(volume);
    strvolume =  strvolume + "%";
    ui->label_volume->setText(strvolume);
}

bool LCVLCMediaPlayer::eventFilter(QObject * obj, QEvent * event)
{
    if(obj== ui->horizontalSlider)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (event->type()==QEvent::MouseButtonPress)           //判断类型
        {
            if (mouseEvent->button() == Qt::LeftButton) //判断左键
            {
                int dur = ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum();
                int value = ui->horizontalSlider->minimum() + dur * ((double)mouseEvent->x() / ui->horizontalSlider->width());
//                if(value != ui->horizontalSlider->sliderPosition())
//                {
//                    ui->horizontalSlider->setValue(value);
//                }

                libvlc_media_t *curMedia = libvlc_media_player_get_media(m_mediaPlayer);
                if (nullptr == curMedia)
                    return false;
                libvlc_media_player_set_time(m_mediaPlayer, libvlc_time_t(value*1000));

                ui->horizontalSlider->setSliderPosition(value);
            }
        }
        else if(event->type()  == QEvent::MouseMove)
        {
            int dur = ui->horizontalSlider->maximum() - ui->horizontalSlider->minimum();
            int value = ui->horizontalSlider->minimum() + dur * ((double)mouseEvent->x() / ui->horizontalSlider->width());

            libvlc_media_player_set_time(m_mediaPlayer, libvlc_time_t(value*1000));
            ui->horizontalSlider->setSliderPosition(value);
        }
    }

    if(obj == ui->Slider_Volume)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (event->type()==QEvent::MouseButtonPress)
        {
            if (mouseEvent->button() == Qt::LeftButton) //判断左键
            {
                int dur = ui->Slider_Volume->maximum() - ui->Slider_Volume->minimum();
                int value = ui->Slider_Volume->minimum() + dur * ((double)mouseEvent->x() / ui->Slider_Volume->width());

                if(value > ui->Slider_Volume->maximum())
                {
                    value = ui->Slider_Volume->maximum();
                }
                if(value < ui->Slider_Volume->minimum())
                {
                    value = ui->Slider_Volume->minimum();
                }

                libvlc_audio_set_volume(m_mediaPlayer, value);
                QString strvolume = QString::number(value);
                strvolume =  strvolume + "%";

                ui->Slider_Volume->setSliderPosition(value);
                ui->label_volume->setText(strvolume);
            }
        }
        else if(event->type()  == QEvent::MouseMove)
        {
            int dur = ui->Slider_Volume->maximum() - ui->Slider_Volume->minimum();
            int value = ui->Slider_Volume->minimum() + dur * ((double)mouseEvent->x() / ui->Slider_Volume->width());

            if(value > ui->Slider_Volume->maximum())
            {
                value = 100;
            }
            if(value < ui->Slider_Volume->minimum())
            {
                value = 0;
            }

            Slider_Volume_sliderMoved(value);
        }
    }
    return QObject::eventFilter(obj,event);
}

void LCVLCMediaPlayer::enterEvent(QEvent *)
{
    ui->verticalWidget->show();
}

//void MainWindow::leaveEvent(QEvent *)
//{
//    ui->verticalWidget->hide();
//}

void LCVLCMediaPlayer::on_pushButton_play_clicked()
{
    if(m_bPlaying)
    {
//        ui->pushButton_play->setStyleSheet("border-image:url(:/Resources/media-png/play4.png)");
        ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/play4.png);}\
                                            QToolTip {border-image: url(:/Resources/media-png/white.png);}");
        ui->pushButton_play->setToolTip("播放");
        m_bPlaying = false;
        ui->pushButton_play2->setVisible(true);
        pause();
    }
    else
    {
//        ui->pushButton_play->setStyleSheet("border-image:url(:/Resources/media-png/pause2.png)");
        ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/pause2.png);} \
                                            QToolTip {border-image: url(:/Resources/media-png/white.png);}");
        ui->pushButton_play->setToolTip("暂停");

        m_bPlaying = true;
        ui->pushButton_play2->setVisible(false);
        play();
    }
}

void LCVLCMediaPlayer::on_pushButton_stop_clicked()
{
    stop();
    ui->centralwidget->setStyleSheet("background-color: rgb(0, 0, 0)");
    loadStyleSheet();


    ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/play4.png);} \
                                        QToolTip {border-image: url(:/Resources/media-png/white.png);}");
    ui->pushButton_play->setToolTip("播放");

    ui->horizontalSlider->setSliderPosition(0);
    ui->label_timing->setText("00:00:00");
    ui->label_end->setText("00:00:00");
}

void LCVLCMediaPlayer::on_pushButton_play2_clicked()
{
//    ui->centralwidget->setStyleSheet("background-color: rgb(0, 0, 0, 0)");//设置为透明
    if(m_bPlaying)
    {
       ui->pushButton_play->setToolTip("播放");
//        ui->pushButton_play->setStyleSheet("border-image:url(:/Resources/media-png/play4.png)");
       ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/play4.png);} \
                                           QToolTip {border-image: url(:/Resources/media-png/white.png);}");
       m_bPlaying = false;
       ui->pushButton_play2->setVisible(true);
       pause();
    }
    else
    {
       ui->pushButton_play->setToolTip("暂停");
//        ui->pushButton_play->setStyleSheet("border-image:url(:/Resources/media-png/pause2.png)");
       ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/pause2.png);} \
                                           QToolTip {border-image: url(:/Resources/media-png/white.png);}");

       m_bPlaying = true;
       ui->pushButton_play2->setVisible(false);
       play();
    }
}

void LCVLCMediaPlayer::on_pushButton_max_clicked()
{
    QWidget *pWindow = this->window();

    if(pWindow->isMaximized())
    {
        pWindow->showNormal();
        ui->verticalWidget->showNormal();
        ui->pushButton_max->setToolTip("全屏");
    }
    else
    {
        pWindow->showMaximized();
        ui->verticalWidget->showMaximized();
        ui->pushButton_max->setToolTip("窗口化");
    }
}

void LCVLCMediaPlayer::on_pushButton_close_clicked()
{
    ui->verticalWidget->close();
    QWidget *pWindow = this->window();
    pWindow->close();
}

void LCVLCMediaPlayer::on_pushButton_volume_clicked()
{
    int isplay = libvlc_media_player_is_playing(m_mediaPlayer);
    if (m_mediaPlayer != nullptr && isplay == 1)
    {
        int mute = libvlc_audio_get_mute(m_mediaPlayer);

        if(mute)
        {
            m_bMute = 0;
            libvlc_audio_set_mute(m_mediaPlayer, 0);
            ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/volume1.png);} \
                                                QToolTip {border-image: url(:/Resources/media-png/white.png);}");
            ui->Slider_Volume->setStyleSheet("QSlider::handle:horizontal{width:10px;background-color:rgb(255,255,255);margin:-11px 0px -11px 0px;border-radius:12px;}\
                                             QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
                                             QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
                                             QSlider::sub-page:horizontal{background-color:rgb(26,217,110);}");
        }else
        {
            m_bMute = 1;
            libvlc_audio_set_mute(m_mediaPlayer, 1);
            ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/mute1.png);} \
                                                QToolTip {border-image: url(:/Resources/media-png/white.png);}");
            ui->Slider_Volume->setStyleSheet("QSlider::handle:horizontal{width:10px;background-color:rgb(255,255,255);margin:-11px 0px -11px 0px;border-radius:12px;}\
                                             QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
                                             QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
                                             QSlider::sub-page:horizontal{background-color:rgb(150,150,150);}");
        }
    }
    else
    {
        if(m_bMute)
        {
            m_bMute = 0;
            ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/volume1.png);} \
                                                QToolTip {border-image: url(:/Resources/media-png/white.png);}");
            ui->Slider_Volume->setStyleSheet("QSlider::handle:horizontal{width:10px;background-color:rgb(255,255,255);margin:-11px 0px -11px 0px;border-radius:12px;}\
                                             QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
                                             QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
                                             QSlider::sub-page:horizontal{background-color:rgb(26,217,110);}");
        }else
        {
            m_bMute = 1;
            ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/mute1.png);} \
                                                QToolTip {border-image: url(:/Resources/media-png/white.png);}");
            ui->Slider_Volume->setStyleSheet("QSlider::handle:horizontal{width:10px;background-color:rgb(255,255,255);margin:-11px 0px -11px 0px;border-radius:12px;}\
                                             QSlider::groove:horizontal{height:2px;background-color:rgb(219,219,219);}\
                                             QSlider::add-page:horizontal{background-color:rgb(219,219,219);}\
                                             QSlider::sub-page:horizontal{background-color:rgb(150,150,150);}");
        }
    }
}

void LCVLCMediaPlayer::Slider_Volume_sliderMoved(int position)
{
    if (m_mediaPlayer != nullptr)
    {
        libvlc_audio_set_volume(m_mediaPlayer, position);
    }

    if(0 == position)
    {
        m_bMute = 1;
        libvlc_audio_set_mute(m_mediaPlayer, 1);
        ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/mute1.png);} \
                                              QToolTip {border-image: url(:/Resources/media-png/white.png);}");
    }else
    {
        m_bMute = 0;
        libvlc_audio_set_mute(m_mediaPlayer, 0);
        ui->pushButton_volume->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/volume1.png);} \
                                            QToolTip {border-image: url(:/Resources/media-png/white.png);}");
    }

    ui->Slider_Volume->setSliderPosition(position);
    QString strvolume = QString::number(position);
    strvolume = strvolume + "%";
    ui->label_volume->setText(strvolume);
}

void LCVLCMediaPlayer::on_Slider_Volume_valueChanged(int value)
{
    if(value)
    {
        m_bMute = 0;
    }else {
        m_bMute = 1;
    }
}
