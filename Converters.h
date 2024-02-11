#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <QString>
#include <QVector>
#include <QVariant>

class Converters
{
public:

template<typename T>
static QVector<T> ConvertStringToTVector(QString& str, QChar spliter)
{
    QStringList stringList = str.split(spliter);

    QVector<T> TVector;
    TVector.reserve(stringList.size());

    foreach (QString element, stringList)
      TVector.append(QVariant(element).value<T>());

    return TVector;
}
};

#endif // CONVERTERS_H
