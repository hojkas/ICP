/**
 * @file mapwidget.cpp
 * @author xstrna14
 * @brief TODO
 * Additional description TODO
 */

#include "mapwidget.h"
#include "mainwindow.h"

MapWidget::MapWidget(QWidget *parent) : QWidget(parent)
{
    streets = new AllStreets();
    streets->loadStreets();

    //TODO actual solution
    streetNamesToggled = true;

}

MapWidget::~MapWidget()
{
    delete streets;
}


void MapWidget::onToggleStreetNames(int val)
{
    if(val) this->streetNamesToggled = true;
    else this->streetNamesToggled = false;
    update();
}


void MapWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    //getting default pen
    QPen p_pen = p.pen();
    //getting default font
    QFont p_font = p.font();


    p.setWindow(QRect(0,0,100,100));

    /* TODO figure out
    float scl = 1;
    p.scale(scl,scl);
    int trans = 0;
    p.translate(trans, trans);
    //p.translate(50,50);
    int off = 40;
    int start = 0 + off;
    int end = 100 - off;
    p.setWindow(50, 50, 100, 100);*/

    //TODO remove, just ui stuff
    p.drawLine(0,0,100,0);
    p.drawLine(0,0,0,100);
    p.drawLine(100,0,100,100);
    p.drawLine(0,100,100,100);

    //Formating for street painting
    QPen street_pen = p_pen;
    street_pen.setCapStyle(Qt::RoundCap);
    street_pen.setWidth(3);
    street_pen.setBrush(Qt::gray);
    p.setPen(street_pen);

    //Street painting (just lines)
    for(auto const & s : this->streets->street_list) {
        p.drawLine(s->x1, s->y1, s->x2, s->y2);
    }

    if(this->streetNamesToggled) {
        //setting drawing properties for writing street names (where there are)
        QPen s_name_pen = p_pen;
        QFont s_name_font = p_font;
        s_name_font.setPointSize(2);

        p.setFont(s_name_font);
        p.setPen(s_name_pen);

        //Painting street names
        for(auto const & s : this->streets->street_list) {
            p.drawText(((s->x1+s->x2)/2)-5, ((s->y1+s->y2)/2)+1, s->name);
        }
     }
}


/* @brief Override resizeEvent to ensure the map widget is square after resizing main window.
 */
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
