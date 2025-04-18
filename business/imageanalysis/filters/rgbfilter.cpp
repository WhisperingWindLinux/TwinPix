#include "rgbfilter.h"

GenericRgbFilter::GenericRgbFilter(RgbChannel channel)
    : mChannel(channel),
      mIsOutputImageColored(true)
{
}

QString GenericRgbFilter::getShortName() const {
    switch(mChannel) {
    case RgbChannel::R:
        return "Show Red Channel";
    case RgbChannel::G:
        return "Show Green Channel";
    case RgbChannel::B:
        return "Show Blue Channel";
    }
    throw std::runtime_error("Error: an incorrect RGB chnannel in GenericRgbFilter.");
}

QString GenericRgbFilter::getFullName() const {
    return getShortName();
}

QString GenericRgbFilter::getHotkey() const {
    switch(mChannel) {
    case RgbChannel::R:
        return "R";
    case RgbChannel::G:
        return "G";
    case RgbChannel::B:
        return "B";
    }
    throw std::runtime_error("Error: an incorrect RGB chnannel in GenericRgbFilter.");
}

QString GenericRgbFilter::getDescription() const {
    QString channelName;
    if (mChannel == RgbChannel::R) {
        channelName = "Red";
    } else if (mChannel == RgbChannel::G) {
        channelName = "Green";
    } else if (mChannel == RgbChannel::B) {
        channelName = "Blue";
    } else {
        throw std::runtime_error("Error: an incorrect RGB chnannel in GenericRgbFilter.");
    }

    QString description = QString("Leaves only the %1 channel on the RGB image.").arg(channelName);
    return description;
}

QImage GenericRgbFilter::filter(const QImage &image) {
    return extractChannel(image, mIsOutputImageColored, mChannel);
}

QImage GenericRgbFilter::extractChannel(const QImage &image,
                                        bool isImageColored,
                                        RgbChannel channel
                                        )
{

    QImage oneChannelImage { image.size(),
                            isImageColored ?
                                QImage::Format_ARGB32
                                           : QImage::Format_Grayscale8
                           };

    // Iterate over each pixel
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            // Extract the original pixel
            QRgb pixel = image.pixel(x, y);

            if (channel == RgbChannel::R) {
                int red = qRed(pixel);
                QRgb newPixel = qRgba(red, 0, 0, qAlpha(pixel));
                oneChannelImage.setPixel(x, y, newPixel);
            } else if (channel == RgbChannel::G) {
                int green = qGreen(pixel);
                QRgb newPixel = qRgba(0, green, 0, qAlpha(pixel));
                oneChannelImage.setPixel(x, y, newPixel);
            } else if (channel == RgbChannel::B) {
                int blue = qBlue(pixel);
                QRgb newPixel = qRgba(0, 0, blue, qAlpha(pixel));
                oneChannelImage.setPixel(x, y, newPixel);
            } else {
                throw std::runtime_error("Error: An incorrect RGB channel was requested.");
            }
        }
    }
    return oneChannelImage;
}

QList<Property> GenericRgbFilter::getDefaultProperties() const {
    QList<QString> alternatives = { "Colored", "Grayscale" };
    QString description = "Represents the choice between colored and grayscale image in an R/G/B mode.";
    Property colorModeProperty = Property::createAlternativesProperty("Color mode",
                                                                      description,
                                                                      alternatives,
                                                                      0
                                                                      );
    QList<Property> properties = { colorModeProperty };
    return properties;
}

void GenericRgbFilter::setProperties(QList<Property> properties) {
    if (properties.size() != 1) {
        return;
    }
    Property colorModeProperty = properties[0];

    if (colorModeProperty.getPropertyName() != "Color mode"
        || colorModeProperty.getPropertyType() != Property::Type::Alternatives)
    {
        return;
    }
    int iColorModeProperty = colorModeProperty.getValue();
    mIsOutputImageColored = (iColorModeProperty == 0);
}

void GenericRgbFilter::reset() {
    mIsOutputImageColored = true;
}
