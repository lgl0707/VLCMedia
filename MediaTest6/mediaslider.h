#ifndef MEDIASLIDER_H
#define MEDIASLIDER_H

#include <QSlider>
#include <QMouseEvent>

class MediaSlider : public QSlider
{
public:
    MediaSlider(QWidget *parent = nullptr);

//protected:
//    void mousePressEvent(QMouseEvent *ev);
};

#endif // MEDIASLIDER_H
