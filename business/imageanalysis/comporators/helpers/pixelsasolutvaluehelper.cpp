#include "pixelsasolutvaluehelper.h"

#include <QFile>


QList<PixelDifferenceRange> PixelsAbsolutValueHelper::generateDifferenceStringResult(const QImage &image1,
                                                                                     const QImage &image2
                                                                                    )
{
    // General image parameters
    int width = image1.width();
    int height = image1.height();
    int totalPixels = width * height;

    // Define difference ranges
    QList<PixelDifferenceRange> ranges = {
        PixelDifferenceRange(0, 0), PixelDifferenceRange(1, 1), PixelDifferenceRange(2, 2),
        PixelDifferenceRange(3, 3), PixelDifferenceRange(4, 4), PixelDifferenceRange(5, 5),
        PixelDifferenceRange(6, 10), PixelDifferenceRange(11, 15), PixelDifferenceRange(16, 20),
        PixelDifferenceRange(21, 30), PixelDifferenceRange(31, 40), PixelDifferenceRange(41, 50),
        PixelDifferenceRange(51, 60), PixelDifferenceRange(61, 70), PixelDifferenceRange(71, 80),
        PixelDifferenceRange(81, 90), PixelDifferenceRange(91, 100), PixelDifferenceRange(101, 150),
        PixelDifferenceRange(151, 200), PixelDifferenceRange(201, 255)
    };

    // Loop through each pixel and calculate the differences
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color1 = image1.pixelColor(x, y);
            QColor color2 = image2.pixelColor(x, y);
            int maxDiff = calculateDiff(color1, color2);
            // Increment the count for the corresponding range
            for (auto &range : ranges) {
                if (maxDiff >= range.minDifference && maxDiff <= range.maxDifference) {
                    range.pixelCount++; // Increment pixel count
                    break;
                }
            }
        }
    }

    // Calculate the percentage of pixels for each range
    for (auto &range : ranges) {
        range.percentage = (static_cast<double>(range.pixelCount) / totalPixels) * 100.0;
    }

    return ranges;
}

int PixelsAbsolutValueHelper::calculateDiff(QColor color1, QColor color2)
{
    int diffR = std::abs(color1.red() - color2.red());
    int diffG = std::abs(color1.green() - color2.green());
    int diffB = std::abs(color1.blue() - color2.blue());
    return std::max({diffR, diffG, diffB});
}

// Function to generate a color map for each range
std::map<int, QColor> PixelsAbsolutValueHelper::generateColorMap(const QList<PixelDifferenceRange>& ranges) {
    std::map<int, QColor> colorMap;
    QList<QColor> colors = {
        QColor(255, 255, 255), // White
        QColor(255, 0, 0),     // Red
        QColor(0, 255, 0),     // Green
        QColor(0, 0, 255),     // Blue
        QColor(255, 255, 0),   // Yellow
        QColor(255, 0, 255),   // Magenta
        QColor(0, 255, 255),   // Cyan
        QColor(128, 0, 128),   // Purple
        QColor(255, 165, 0),   // Orange
        QColor(139, 69, 19),   // Brown
        QColor(0, 100, 0),     // Dark Green
        QColor(128, 128, 128), // Gray
        QColor(0, 0, 0),       // Black
        QColor(0, 0, 0)        // Black
    };

    if (ranges.size() >= colors.size()) {
        throw std::runtime_error("Unable to generate color map.");
    }

    int colorIndex = 0;
    for (int i = 0; i < ranges.size(); ++i) {
        colorMap[i] = colors[colorIndex];
        colorIndex++;
    }
    return colorMap;
}

QString PixelsAbsolutValueHelper::getColorRangeDescription() {
    QFile file(":/html/color-range-description.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    return QString::fromUtf8(file.readAll());
}

QImage PixelsAbsolutValueHelper::generateDifferenceImageByCustomRage(const QImage &image1,
                                                                     const QImage &image2,
                                                                     int startOfRange,
                                                                     int endOfRange
                                                                     )
{
    if (startOfRange > endOfRange) {
        return {};
    }

    int width = image1.width();
    int height = image1.height();

    // Define difference ranges
    QList<PixelDifferenceRange> ranges = { PixelDifferenceRange(0, 0), // skip the white color
                                           PixelDifferenceRange(startOfRange, endOfRange)
                                         };

    // Generate a color map for each range
    std::map<int, QColor> colorMap = generateColorMap(ranges);

    // Create the output image
    QImage outputImage(width, height, QImage::Format_ARGB32);
    outputImage.fill(Qt::white); // Start with a white background

    // Loop through each pixel and calculate the differences
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color1 = image1.pixelColor(x, y);
            QColor color2 = image2.pixelColor(x, y);
            int maxDiff = calculateDiff(color1, color2);

            // Find the corresponding range for the difference and apply the color
            for (int i = 0; i < ranges.size(); ++i) {
                const auto& range = ranges[i];
                if (maxDiff >= range.minDifference && maxDiff <= range.maxDifference) {
                    // White background mode: draw only differing pixels
                    outputImage.setPixelColor(x, y, colorMap[i]);
                    break;
                }
            }
        }
    }

    return outputImage;

}

// Function to generate the difference visualization image
QImage PixelsAbsolutValueHelper::generateDifferenceImage(const QImage &image1,
                                                         const QImage &image2
                                                         )
{
    int width = image1.width();
    int height = image1.height();

    // Define difference ranges
    QList<PixelDifferenceRange> ranges = {
        PixelDifferenceRange(0, 0), PixelDifferenceRange(1, 1), PixelDifferenceRange(2, 2),
        PixelDifferenceRange(3, 3), PixelDifferenceRange(4, 4), PixelDifferenceRange(5, 5),
        PixelDifferenceRange(6, 10), PixelDifferenceRange(11, 15), PixelDifferenceRange(16, 20),
        PixelDifferenceRange(21, 30), PixelDifferenceRange(31, 40), PixelDifferenceRange(41, 50),
        PixelDifferenceRange(51, 255)
    };

    // Generate a color map for each range
    std::map<int, QColor> colorMap = generateColorMap(ranges);

    // Create the output image
    QImage outputImage(width, height, QImage::Format_ARGB32);
    outputImage.fill(Qt::white); // Start with a white background

    // Loop through each pixel and calculate the differences
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QColor color1 = image1.pixelColor(x, y);
            QColor color2 = image2.pixelColor(x, y);
            int maxDiff = calculateDiff(color1, color2);

            // Find the corresponding range for the difference and apply the color
            for (int i = 0; i < ranges.size(); ++i) {
                const auto& range = ranges[i];
                if (maxDiff >= range.minDifference && maxDiff <= range.maxDifference) {
                    // White background mode: draw only differing pixels
                    outputImage.setPixelColor(x, y, colorMap[i]);
                    break;
                }
            }
        }
    }

    return outputImage;
}
