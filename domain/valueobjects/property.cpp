#include "property.h"

#include <stdexcept>
#include <float.h>
#include <climits>

// Constructor for real property
Property::Property(const QString &propertyName,
                   const QString &propertyDescription,
                   double defaultValue,
                   double minValue,
                   double maxValue)
    : mPropertyType(Type::Real),
    mPropertyName(propertyName),
    mPropertyDescription(propertyDescription),
    mDoubleValue(defaultValue),
    mMax(maxValue),
    mMin(minValue) {}

// Constructor for integer property
Property::Property(const QString &propertyName,
                   const QString &propertyDescription,
                   int defaultValue,
                   int minValue,
                   int maxValue)
    : mPropertyType(Type::Integer),
    mPropertyName(propertyName),
    mPropertyDescription(propertyDescription),
    mDoubleValue(static_cast<double>(defaultValue)),
    mMax(static_cast<double>(maxValue)),
    mMin(static_cast<double>(minValue)) {}

Property::Property(const QString &propertyName,
                   const QString &propertyDescription,
                   const QString &defaultValue)
    : mPropertyType(Type::FilePath),
     mPropertyName(propertyName),
     mPropertyDescription(propertyDescription),
     mFilePathValue(defaultValue) {}


// Static method to create an integer property without min/max constraints
Property Property::createIntProperty(const QString &propertyName,
                                     const QString &propertyDescription,
                                     int defaultValue
                                     )
{
    return Property(propertyName,
                    propertyDescription,
                    defaultValue,
                    INT_MIN,
                    INT_MAX
                    );
}


// Static method to create an integer property with min/max constraints
Property Property::createIntProperty(const QString &propertyName,
                                     const QString &propertyDescription,
                                     int defaultValue,
                                     int minValue,
                                     int maxValue
                                     )
{
    return Property(propertyName,
                    propertyDescription,
                    defaultValue,
                    minValue,
                    maxValue
                    );
}


// Static method to create a real property without min/max constraints
Property Property::createRealProperty(const QString &propertyName,
                                      const QString &propertyDescription,
                                      double defaultValue
                                      )
{
    return Property(propertyName,
                    propertyDescription,
                    defaultValue,
                    -DBL_MAX,
                    DBL_MAX
                    );
}


// Static method to create a real property with min/max constraints
Property Property::createRealProperty(const QString &propertyName,
                                      const QString &propertyDescription,
                                      double defaultValue,
                                      double minValue,
                                      double maxValue
                                      )
{
    return Property(propertyName,
                    propertyDescription,
                    defaultValue,
                    minValue,
                    maxValue
                    );
}

// Static method to create a string property with mAlternativesValue
Property Property::createAlternativesProperty(const QString &propertyName,
                                              const QString &propertyDescription,
                                              const QStringList &alternatives,
                                              int defaultValueIndex
                                             )
{
    if (alternatives.isEmpty()) {
        throw std::runtime_error("An internal error occurred: alternatives list cannot be empty");
    }

    if (defaultValueIndex < 0 || defaultValueIndex >= alternatives.size()) {
        throw std::runtime_error("An internal error occurred: default value index is out of range");
    }

    Property prop(std::move(propertyName), std::move(propertyDescription), 0.0, 0.0, 0.0);
    prop.mPropertyType = Type::Alternatives;
    prop.mAlternativesValue = alternatives;
    prop.mDoubleValue = static_cast<double>(defaultValueIndex);

    return prop;
}

Property Property::createFilePathProperty(const QString &propertyName,
                                          const QString &propertyDescription,
                                          const QString &defaultValue
                                          )
{
    return Property(propertyName, propertyDescription, defaultValue);
}

// Getters
Property::Type Property::getPropertyType() const {
    return mPropertyType;
}

QString Property::getPropertyName() const {
    return mPropertyName;
}

QString Property::getPropertyDescription() const {
    return mPropertyDescription;
}

double Property::getValue() const {
    return mDoubleValue;
}

QString Property::getFilePath() const {
    return mFilePathValue;
}

QStringList Property::getAlternatives() const {
    if (mPropertyType != Type::Alternatives) {
        throw std::runtime_error("An internal error occurred: this property type does not have alternatives");
    }
    return mAlternativesValue;
}

double Property::getMinValue() const {
    if (mPropertyType == Type::Alternatives || mPropertyType == Type::FilePath) {
        throw std::runtime_error("An internal error occurred: this property type does not have min/max values");
    }
    return mMin;
}

QString Property::getAnyValueAsString() const {
    switch (mPropertyType) {
    case Type::Integer:
        return QString::number((int)mDoubleValue);
    case Type::Real:
        return QString::number(mDoubleValue);
    case Type::Alternatives:
        if (mAlternativesValue.size() == 0) {
            return "";
        }
        if ((int)mDoubleValue > mAlternativesValue.size() || (int)mDoubleValue < 0) {
            return "";
        }
        return mAlternativesValue[(int)mDoubleValue];
    case Type::FilePath:
        return mFilePathValue;
    }
    return "";
}

double Property::getMaxValue() const {
    if (mPropertyType == Type::Alternatives || mPropertyType == Type::FilePath) {
        throw std::runtime_error("An internal error occurred: this property type does not have min/max values.");
    }
    return mMax;
}

QString Property::toString() const {
    QString result = "Property: \n";
    result += "  Name: " + getPropertyName() + "\n";
    result += "  Description: " + getPropertyDescription() + "\n";

    switch (getPropertyType()) {
    case Type::Integer:
        result += "  Type: Integer\n";
        result += "  Value: " + QString::number(static_cast<int>(getValue())) + "\n";
        result += "  Min Value: " + QString::number(static_cast<int>(getMinValue())) + "\n";
        result += "  Max Value: " + QString::number(static_cast<int>(getMaxValue())) + "\n";
        break;

    case Type::Real:
        result += "  Type: Real\n";
        result += "  Value: " + QString::number(getValue(), 'f', 2) + "\n";
        result += "  Min Value: " + QString::number(getMinValue(), 'f', 2) + "\n";
        result += "  Max Value: " + QString::number(getMaxValue(), 'f', 2) + "\n";
        break;

    case Type::Alternatives:
        result += "  Type: Alternatives\n";
        result += "  Value Index: " + QString::number(static_cast<int>(getValue())) + "\n";
        result += "  Alternatives: [" + getAlternatives().join(", ") + "]\n";
        break;

    case Type::FilePath:
        result += "  Type: FilePath\n";
        result += "  Value: " + mFilePathValue + "\n";
        break;
    }

    return result;
}
