#ifndef COLORSSATURATIONCOMPORATOR_H
#define COLORSSATURATIONCOMPORATOR_H

#include <QString>
#include <QImage>
#include <QColor>

#include <domain/interfaces/business/icomparator.h>

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

struct ColorsSaturationComparisonResult {
    QString firstImageName;
    QString secondImageName;
    double firstImageSaturation;       // Average saturation of the first image
    double secondImageSaturation;      // Average saturation of the second image
};

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

// This class calculates the average saturation based on pixel HSV values.
// It compares two images based on this criterion.

class ColorsSaturationComporator : public IComparator {
public:
    ColorsSaturationComporator() = default;
    virtual ~ColorsSaturationComporator() = default;

    // IComparator interface

    QString getShortName() const override;
    QString getHotkey() const override;
    QString getDescription() const override;
    QString getFullName() const override;
    ComparisonResultVariantPtr compare(const ComparableImage &first,
                                       const ComparableImage &second) override;

private:
    ColorsSaturationComparisonResult compareImages(const QImage &image1,
                                                   const QString &name1,
                                                   const QImage &image2,
                                                   const QString &name2);

    QString formatResultToHtml(const ColorsSaturationComparisonResult& result);

    // Helper function to calculate the average saturation of an image
    double calculateAverageSaturation(const QImage &image);
};

#endif // COLORSSATURATIONCOMPORATOR_H
