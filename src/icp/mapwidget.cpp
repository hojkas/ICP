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

    for(auto const & s : this->streets->street_list) {
        p.drawLine(s->x1, s->y1, s->x2, s->y2);
    }
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    int nWidth = width();
    int nHeight = height();
    if(nWidth < nHeight) {
        nHeight = nWidth;
    }
    else nWidth = nHeight;
    this->resize(nWidth, nHeight);
    update();
    QWidget::resizeEvent(event);
}
