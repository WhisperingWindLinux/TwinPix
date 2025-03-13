#ifndef PROPERTY_H
#define PROPERTY_H

#include <qstring.h>
#include <QStringList>

using namespace std;

class Property {
public:
    enum class Type { Integer, Real, Alternatives, FilePath };

    static Property createIntProperty(QString propertyName,
                                      QString propertyDescription,
                                      int defaultValue
                                      );

    static Property createIntProperty(QString propertyName,
                                      QString propertyDescription,
                                      int defaultValue,
                                      int minValue,
                                      int maxValue
                                      );

    static Property createRealProperty(QString propertyName,
                                      QString propertyDescription,
                                      int defaultValue
                                      );

    static Property createRealProperty(QString propertyName,
                                       QString propertyDescription,
                                       double defaultValue,
                                       double minValue,
                                       double maxValue
                                       );

    static Property createAlternativesProperty(QString propertyName,
                                         QString propertyDescription,
                                         QStringList alternatives,
                                         int deafultValueIndex
                                         );

    static Property createFilePathProperty(QString propertyName,
                                           QString propertyDescription,
                                           QString defaultValue
                                          );


    Type getPropertyType() const;
    QString getPropertyName() const;
    QString getPropertyDescription() const;
    double getValue() const;
    QStringList getAlternatives() const;
    double getMinValue() const;
    double getMaxValue() const;
    QString toString() const;
    QString getFilePath() const;

    friend class PropertyTests;

private:
    Type propertyType;
    QString propertyName;
    QString propertyDescription;
    double doubleValue;
    QStringList alternativesValue;
    QString filePathValue;
    double max, min;

    Property(QString propertyName,
             QString propertyDescription,
             double defaultValue,
             double minValue,
             double maxValue
             );

    Property(QString propertyName,
             QString propertyDescription,
             int defaultValue,
             int minValue,
             int maxValue
             );

    Property(QString propertyName,
             QString propertyDescription,
             QString defaultValue
             );
};

#endif // PROPERTY_H
