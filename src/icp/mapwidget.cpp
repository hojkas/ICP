/**
 * @file mapwidget.cpp
 * @author xstrna14
 * @brief TODO
 * Additional description TODO
 */

#include "mapwidget.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    streets = new AllStreets();
    streets->loadStreets();
}

MapWidget::~MapWidget()
{
    delete streets;
}


void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    p.setWindow(QRect(0,0,100,100));
    p.drawLine(50,50,100,0);
    //p.drawEllipse(rect());
}
