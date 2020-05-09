#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

class connection : public QObject
{
    Q_OBJECT
public:
    explicit connection(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CONNECTION_H