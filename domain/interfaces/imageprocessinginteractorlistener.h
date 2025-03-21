#ifndef IMAGEPROCESSINGINTERACTORLISTENER_H
#define IMAGEPROCESSINGINTERACTORLISTENER_H

#include <qpixmap.h>

class IImageProcessingInteractorListener {
public:
    virtual void onShowImageProcessorsHelp(const QString &html) = 0;
    virtual void onComparisonResultLoaded(const QPixmap &image, const QString &description) = 0;

    virtual void onComparisonResultLoaded(const QString &html,
                                          const QString &comparatorFullName,
                                          const QString &comparatorDescription,
                                          const QString &firstImagePath,
                                          const QString &secondImagePath) = 0;

    virtual void onFilteredResultLoaded(const QPixmap &firstImage, const QPixmap &secondImage) = 0;
    virtual void onImageProcessorFailed(const QString &error) = 0;
};

#endif // IMAGEPROCESSINGINTERACTORLISTENER_H
