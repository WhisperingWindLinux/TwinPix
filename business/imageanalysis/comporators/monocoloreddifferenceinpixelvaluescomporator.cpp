#include <QPainter>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "monocoloreddifferenceinpixelvaluescomporator.h"

QImage MonoColoredDifferenceInPixelValuesComporator::compareImages(const QImage &image1,
                                                                   const QImage &image2
                                                                  )
{
    // Create a resulting image (copy of the first image)
    QImage resultImg = image1.copy();
    QPainter painter(&resultImg);

    painter.drawImage(0, 0, image1);

    // Compare pixels and highlight differences in red
    for (int y = 0; y < image1.height(); ++y) {
        for (int x = 0; x < image1.width(); ++x) {
            if (image1.pixel(x, y) != image2.pixel(x, y)) {
                resultImg.setPixelColor(x, y, QColor(255, 0, 0, 255)); // Red color
            }
        }
    }

    painter.end();
    return resultImg;
}

QString MonoColoredDifferenceInPixelValuesComporator::getShortName() const {
    return "Difference In Pixel Values v.1 (Image, Single Color)";
}

QString MonoColoredDifferenceInPixelValuesComporator::getFullName() const {
    return "Difference In Pixel Values v.1 (Image, Single Color)";
}

QString MonoColoredDifferenceInPixelValuesComporator::getHotkey() const {
    return "D";
}

QString MonoColoredDifferenceInPixelValuesComporator::getDescription() const {
    return QString("Show the difference in pixel values as an image. "
                   "Pixels that differ are marked with red dots.");
}

ComparisonResultVariantPtr MonoColoredDifferenceInPixelValuesComporator::compare(const ComparableImage &first,
                                                                             const ComparableImage &second
                                                                             )
{
    auto result = compareImages(first.getImage(), second.getImage());
    std::shared_ptr<ComparisonResultVariant> resultVariant =
                                std::make_shared<ComparisonResultVariant>(result);
    return resultVariant;
}
