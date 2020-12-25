#include "mousemovewidget.h"
#include <QPainter>

MouseMoveWidget::MouseMoveWidget(QWidget *parent) : QWidget(parent)
{

}

void MouseMoveWidget::enterEvent(QEvent *)
{
    this->show();
}
void MouseMoveWidget::leaveEvent(QEvent *)
{
    this->hide();
}

void MouseMoveWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0,0,0,1));
}
