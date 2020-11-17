#ifndef MEDIACTRL_H
#define MEDIACTRL_H

#include <QMainWindow>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

namespace Ui {
class MediaCtrl;
}

class MediaCtrl : public QMainWindow
{
    Q_OBJECT

public:
    explicit MediaCtrl(QWidget *parent = nullptr);
    ~MediaCtrl();

    void loadStyleSheet();

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    void paintEvent(QPaintEvent *e);

signals:
    void onPlay();
    void onPause();
    void onStop();
    void onMaxOrNormal();
    void onclose();

private slots:
    void on_pushButton_play_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_max_clicked();

    void on_pushButton_close_clicked();

private:
    Ui::MediaCtrl *ui;

    int m_playflag;
};

#endif // MEDIACTRL_H
