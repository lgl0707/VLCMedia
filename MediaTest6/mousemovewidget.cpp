#include "mousemovewidget.h"
#include <QDebug>
#include <QPainter>

MouseMoveWidget::MouseMoveWidget(QWidget *parent) : QWidget(parent)
{

}

void MouseMoveWidget::enterEvent(QEvent *)
{
    qDebug() << "111";
    this->show();
}
void MouseMoveWidget::leaveEvent(QEvent *)
{
    qDebug() << "222";
    this->hide();
}

void MouseMoveWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0,0,0,1));
}
