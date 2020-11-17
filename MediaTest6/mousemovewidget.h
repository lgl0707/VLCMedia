#ifndef MOUSEMOVEWIDGET_H
#define MOUSEMOVEWIDGET_H

#include <QWidget>

class MouseMoveWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MouseMoveWidget(QWidget *parent = nullptr);

protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

    void paintEvent(QPaintEvent *e);

signals:

public slots:
};

#endif // MOUSEMOVEWIDGET_H
