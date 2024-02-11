#ifndef APLICSETTINGS_H
#define APLICSETTINGS_H
#include <QString>
#include <QVector>

struct GraphParam
{
    int measParam = 0;
    int measFormat = 0;
};
struct AplicSettings
{

    static QVector<QString> measParams;
    static QVector<QString> measFormats;
    static QVector<GraphParam> Graphs;

};
QVector<QString> AplicSettings::measParams = QVector<QString>{"S11","S12","S22","S21","A1","B1"};
QVector<QString> AplicSettings::measFormats = QVector<QString>{"MLOG","PHAS"};
QVector<GraphParam> AplicSettings::Graphs;

#endif // APLICSETTINGS_H
