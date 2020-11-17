#include "mediactrl.h"
#include "ui_mediactrl.h"
#include <QPainter>
#include <QFile>
#include <QtDebug>

MediaCtrl::MediaCtrl(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MediaCtrl)
{
    ui->setupUi(this);
    m_playflag = -1;

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground,true);

    ui->pushButton_play->setToolTip("播放");
    ui->pushButton_stop->setToolTip("停止播放");
    ui->pushButton_max->setToolTip("全屏");
    ui->pushButton_close->setToolTip("关闭");

    //centralWidget()->setLayout(ui->verticalWidget->layout());
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0,0,0,0);
    vLayout->setSpacing(0);

    vLayout->addWidget(ui->verticalWidget);
    centralWidget()->setLayout(vLayout);

    loadStyleSheet();

    this->setParent(parent);

    ui->verticalWidget->setAttribute(Qt::WA_TranslucentBackground,true);
}

MediaCtrl::~MediaCtrl()
{
    delete ui;
}

void MediaCtrl::loadStyleSheet()
{
    QFile file(":/Resources/media-ui.css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
}

void MediaCtrl::on_pushButton_play_clicked()
{
    if(m_playflag == 1)
    {
        ui->pushButton_play->setToolTip("播放");
//        ui->pushButton_play->setStyleSheet("border-image:url(:/Resources/media-png/play.png)");
        ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/play.png);} \
                                            QToolTip {border-image: url(:/Resources/media-png/white.png)");
        m_playflag = 0;
        emit onPause();
    }
    else
    {
        ui->pushButton_play->setToolTip("暂停");
//        ui->pushButton_play->setStyleSheet("border-image:url(:/Resources/media-png/pause.png)");
        ui->pushButton_play->setStyleSheet("QPushButton {border-image: url(:/Resources/media-png/pause.png);} \
                                            QToolTip {border-image: url(:/Resources/media-png/white.png)");
        m_playflag = 1;
        emit onPlay();
    }
}

void MediaCtrl::on_pushButton_stop_clicked()
{
    emit onStop();
}

void MediaCtrl::on_pushButton_max_clicked()
{
    if(this->isMaximized())
    {
        showNormal();
        ui->pushButton_max->setToolTip("全屏");
    }
    else
    {
        showMaximized();
        ui->pushButton_max->setToolTip("窗口");
    }
    emit onMaxOrNormal();
}

void MediaCtrl::on_pushButton_close_clicked()
{
    emit onclose();
}

void MediaCtrl::enterEvent(QEvent *)
{
//    qDebug() << "111";
    ui->verticalWidget->show();
}

void MediaCtrl::leaveEvent(QEvent *)
{
//    qDebug() << "222";
    ui->verticalWidget->hide();
}

void MediaCtrl::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0,0,0,1));
}
