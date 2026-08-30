#ifndef COLOREDDIFFERENCEINPIXELVALUESCOMPORATOR_H
#define COLOREDDIFFERENCEINPIXELVALUESCOMPORATOR_H

#include "domain/valueobjects/comparisonresultvariant.h"
#include <domain/valueobjects/pixeldiffrencerange.h>
#include <domain/valueobjects/property.h>
#include <domain/interfaces/business/icomparator.h>


class ColoredDifferenceInPixelValuesComporator : public IComparator
{
public:
    enum class Result { Text, Image };

public:
    ColoredDifferenceInPixelValuesComporator(ColoredDifferenceInPixelValuesComporator::Result result);
    virtual ~ColoredDifferenceInPixelValuesComporator() = default;

    // AComparator interface

    QString getShortName() const override;
    QString getHotkey() const override;
    QString getDescription() const override;
    QString getFullName() const override;
    ComparisonResultVariantPtr compare(const ComparableImage &first, const ComparableImage &second) override;

private:
    Result mExpectedResult;
};

#endif // COLOREDDIFFERENCEINPIXELVALUESCOMPORATOR_H


