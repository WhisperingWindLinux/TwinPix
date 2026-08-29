#ifndef IMAGEPROCESSINGINTERACTOR_H
#define IMAGEPROCESSINGINTERACTOR_H

#include <QtCore/qvariant.h>
#include <qpixmap.h>
#include <domain/interfaces/presentation/imagefilesinteractorlistener.h>
#include <domain/interfaces/business/icomparator.h>
#include <domain/interfaces/business/ifilter.h>
#include <domain/valueobjects/imageprocessorsinfo.h>
#include <domain/valueobjects/lastdisplayedcomparisonresult.h>
#include <domain/valueobjects/savefileinfo.h>
#include <business/recentfilesmanager.h>
#include <domain/interfaces/presentation/imageprocessinginteractorlistener.h>
#include "runallcomparatorsinteractor.h"


class PluginsManager;
class ImageProcessorsManager;
class IProgressDialog;
class IPropcessorPropertiesDialogCallback;

class ImageProcessingInteractor
{
public:
    static QList<ImageProcessorInfo> getImageProcessorsInfo();

    ImageProcessingInteractor(const ImageHolderPtr images,
                              IPropcessorPropertiesDialogCallback *propertiesDialogCallback,
                              IProgressDialog *progressDialogCallback
                              );

    ~ImageProcessingInteractor();

    static QList<ImageProcessorInfo> showImageProcessorsHelp();

    void callImageProcessor(const QVariant &callerData);
    void runAllComparators();
    void restoreOriginalImages();

    bool subscribe(IImageProcessingInteractorListener *listener);
    bool unsubscribe(const IImageProcessingInteractorListener *listener);

    void showLastComparisonImage();

    void analyzeSelectedArea(ImageHolderPtr, std::optional<int> key);

    QPixmap applyFilter(const QPixmap &pixmap, IFilterPtr filter);

private:
    IPropcessorPropertiesDialogCallback *mPropertiesDialogCallback;
    IProgressDialog *mProgressDialogCallback;
    QList<IImageProcessingInteractorListener*> mListeners;
    ImageHolderPtr mOriginalImages;
    ImageHolderPtr mDisplayedImages;
    LastDisplayedComparisonResult mLastDisplayedComparisonResult;

    std::unique_ptr<RunAllComparatorsInteractor> mRunAllComparatorsInteractor;

    void coreCallImageProcessor(const QVariant &callerData);
    void callComparator(IComparatorPtr comparator, ImageHolderPtr images);
    void callFilter(IFilterPtr filter);
    void handleProcessorPropertiesIfNeed(IImageProcessorPtr processor);

    void notifyComparisonResultLoaded(const QPixmap &image, const QString &description);

    void notifyComparisonResultLoaded(const QString &html,
                                      const QString &comporatorFullName,
                                      const QString &firstImagePath,
                                      const QString &secondImagePath);

    void notifyFilteredResultLoaded(const ImageHolderPtr imageHolder);
    void notifyImageProcessorFailed(const QString &error);
    void notifyFastSwitchingToComparisonImageStatusChanged(bool isSwitchingAvailable);
    void clearLastComparisonImage();
    void setLastComparisonImage(const QPixmap &pixmap, const QString &description);
    void notifyShowImageInExternalViewer(const QPixmap &image, const QString &description);
};

#endif // IMAGEPROCESSINGINTERACTOR_H
