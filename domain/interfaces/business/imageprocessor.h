#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <domain/valueobjects/property.h>

#include <qimage.h>

enum class ImageProcessorType { Comparator, Filter };

class IImageProcessor {
public:    
    // It must be unique; displayed in the menu.
    virtual QString getShortName() const = 0;

    // A getHotkey is used for quickly calling the comparator.
    // All hotkeys must be unique; if this is not the case,
    // an error will occur when the application starts.
    // Example:  "O", "Ctrl+O", etc.
    virtual QString getHotkey() const = 0;

    virtual QString getDescription()const = 0;

    virtual ImageProcessorType getType() const = 0;

    virtual QList<Property> getDefaultProperties() const = 0;

    virtual void setProperties(QList<Property>) = 0;

    // The full name to be printed in help.
    virtual QString getFullName() const = 0;

    virtual void reset();
};

typedef std::shared_ptr<IImageProcessor> IImageProcessorPtr;
typedef std::shared_ptr<QPixmap> QPixmapPtr;
typedef std::shared_ptr<QImage> QImagePtr;
typedef std::optional<QString> QStringOptional;

/*

The hotkeys table

-------------------------------------------------------------------------------------------------------------------
|      a image processor name                                                              |   a hotkey (one key) |
-------------------------------------------------------------------------------------------------------------------

Switch between images being compared                                                                    S

Show the absolute difference statistics for pixel values (Image, Momocolored) (Comparator)              D

Show the absolute difference statistics for pixel values (Text) (Comparator)                            V

Show the absolute difference statistics for pixel values (Image, Colored) (Comparator)                  M

Show the absolute custom ranged difference for pixel values (Image, Colored) (Comparator)               U

Show pixels' brigthness difference statistics (Comparator)                                              N

Show pixels' contrast difference statistics (Comparator)                                                K

Show pixels' saturation difference statistics (Comparator)                                              T

Dummy comparator (Comparator; For Testing Purposes Only)                                                X

Liner - non-liner difference comparator                                                                 L

Color picker                                                                                            P

Docked Color Picker                                                                                     A

Show Original Image (Filter)                                                                            C

Show Grayscale Image (Filter)                                                                           E

Show Red channel only (Filter)                                                                          R

Show Green channel only (Filter)                                                                        G

Show Blue channel only (Filter)                                                                         B

Sharpness comporator                                                                                    H

Compare the proximity of two images to an original image                                                Y

Fit In View                                                                                             F

Make Icons for iOS app                                                                                  I


*/

#endif // IMAGEPROCESSOR_H
