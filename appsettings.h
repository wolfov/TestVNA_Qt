#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QLineSeries>
QT_CHARTS_USE_NAMESPACE


struct GraphParam
{
    int measParam = 0;
    int measFormat = 0;
    QLineSeries* series = new QLineSeries();
    static QVector<QColor> Colors;
};
struct measParam
{
    measParam(){}
    measParam(QString name,QString param,int port):Name(name),Param(param),Port(port){}
    QString Name;
    QString Param;
    int Port = 1;
};
struct measFormat
{
    measFormat(){}
    measFormat(QString name,QString param):Name(name),Format(param){}
    QString Name;
    QString Format;
};

struct AppSettings
{
    static const int maxGraphs = 2;
    static QVector<measParam> measParams;
    static QVector<measFormat> measFormats;
    static QVector<GraphParam> Graphs;
    static QVector<int> rowParams;
};


#endif // APPSETTINGS_H
