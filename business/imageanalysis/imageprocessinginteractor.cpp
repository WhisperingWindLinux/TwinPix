#include "imageprocessinginteractor.h"

#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <qcoreapplication.h>
#include <qfileinfo.h>
#include <business/imageanalysis/comporators/colorssaturationcomporator.h>
#include <business/imageanalysis/comporators/contrastcomporator.h>
#include <business/imageanalysis/comporators/monocoloreddifferenceinpixelvaluescomporator.h>
#include <business/imageanalysis/comporators/imageproximitytoorigincomparator.h>
#include <business/imageanalysis/comporators/coloreddifferenceInpixelvaluescomporator.h>
#include <business/imageanalysis/comporators/pixelsbrightnesscomparator.h>
#include <business/imageanalysis/comporators/sharpnesscomparator.h>
#include <business/imageanalysis/comporators/customrangeddifferenceinpixelvaluescomparator.h>
#include <business/imageanalysis/comporators/linernonlinerdifferencecomparator.h>
#include <business/imageanalysis/filters/grayscalefilter.h>
#include <business/imageanalysis/filters/rgbfilter.h>
#include <data/storage/filedialoghandler.h>
#include <domain/interfaces/presentation/imageprocessinginteractorlistener.h>
#include <business/utils/imagesinfo.h>
#include "domain/interfaces/presentation/iprocessorpropertiesdialogcallback.h"
#include "imageprocessorsmanager.h"
#include "runallcomparatorsinteractor.h"

ImageProcessingInteractor::ImageProcessingInteractor(
                                    const ImageHolderPtr images,
                                    IPropcessorPropertiesDialogCallback *propertiesDialogCallback,
                                    IProgressDialog *progressDialogCallback
                                        )
    : mPropertiesDialogCallback(propertiesDialogCallback),
    mProgressDialogCallback(progressDialogCallback),
    mOriginalImages(images),
    mDisplayedImages(images)
{
}

ImageProcessingInteractor::~ImageProcessingInteractor() {
    mListeners.clear();
    mOriginalImages = nullptr;
    mDisplayedImages = nullptr;
    mPropertiesDialogCallback = nullptr;
    mProgressDialogCallback = nullptr;
    clearLastComparisonImage();
}

void ImageProcessingInteractor::callImageProcessor(const QVariant &callerData) {
    try {
        coreCallImageProcessor(callerData);
    } catch(std::runtime_error &e) {
        notifyImageProcessorFailed(e.what());
    } catch (std::exception &e) {
        qDebug() << e.what();
    }
}

void ImageProcessingInteractor::coreCallImageProcessor(const QVariant &callerData) {
    if (!callerData.isValid() || callerData.isNull()) {
        throw std::runtime_error("Error: An incorrect caller data.");
    }

    QString processorName = callerData.toString();

    auto processor = ImageProcessorsManager::instance()->findProcessorByShortName(processorName);

    if (processor == nullptr) {
        throw std::runtime_error("Error: Unable to find the requested image processor.");
    }

    processor->reset();

    handleProcessorPropertiesIfNeed(processor);

    if (processor->getType() == ImageProcessorType::Comparator) {
        callComparator(dynamic_pointer_cast<IComparator>(processor), mDisplayedImages);
    } else if (processor->getType() == ImageProcessorType::Filter) {
        callFilter(dynamic_pointer_cast<IFilter>(processor));
    } else {
        throw std::runtime_error("Error: An unknown image processor type.");
    }
}

void ImageProcessingInteractor::clearLastComparisonImage() {
    mLastDisplayedComparisonResult.clear();
    notifyFastSwitchingToComparisonImageStatusChanged(false);
}

void ImageProcessingInteractor::setLastComparisonImage(const QPixmap &pixmap, const QString &description) {
    mLastDisplayedComparisonResult.set(pixmap, description);
    notifyFastSwitchingToComparisonImageStatusChanged(true);
}

void ImageProcessingInteractor::showLastComparisonImage() {
    if (mLastDisplayedComparisonResult.hasLastDisplayedComparisonResult()) {
        notifyComparisonResultLoaded(mLastDisplayedComparisonResult.getImage(),
                                     mLastDisplayedComparisonResult.getDescription()
                                     );
    }
}

// If the user holds down the Command (Ctrl) key along with the comparator hotkey and selects
// a specific area while holding the left mouse button, the comparator will run only for
// the selected area.
void ImageProcessingInteractor::analyzeSelectedArea(ImageHolderPtr images, std::optional<int> key) {
    if (!key) {
        return;
    }
    QChar hotkey{key.value()};
    auto processorsManager = ImageProcessorsManager::instance();
    auto processor = processorsManager->findProcessorByHotkey(hotkey);
    if (processor == nullptr || processor->getType() != ImageProcessorType::Comparator) {
        return;
    }
    try {
        callComparator(dynamic_pointer_cast<IComparator>(processor), images);
    } catch(std::runtime_error &e) {
        notifyImageProcessorFailed(e.what());
    } catch (std::exception &e) {
        qDebug() << e.what();
    }
}

void ImageProcessingInteractor::restoreOriginalImages() {
    if (mOriginalImages == nullptr) {
        return;
    }
    mDisplayedImages = mOriginalImages;
    clearLastComparisonImage();
    notifyFilteredResultLoaded(mDisplayedImages);
}

QList<ImageProcessorInfo> ImageProcessingInteractor::showImageProcessorsHelp() {
    return ImageProcessorsManager::instance()->getAllProcessorsInfo();
}

void ImageProcessingInteractor::handleProcessorPropertiesIfNeed(IImageProcessorPtr processor) {
    auto properties = processor->getDefaultProperties();
    if (properties.empty()) {
        return;
    }
    auto newProperties = mPropertiesDialogCallback->showImageProcessorPropertiesDialog(
                                                                    processor->getShortName(),
                                                                    processor->getDescription(),
                                                                    properties
                                                                );
    if (!newProperties.empty()) {
        processor->setProperties(newProperties);
    }
}

void ImageProcessingInteractor::callComparator(IComparatorPtr comparator, ImageHolderPtr images) {
    if (images == nullptr || images->isSingleImage()) {
        return;
    }
    ImagesInfo info { images };

    auto firstImage = images->getFirstImage();
    auto firstImagePath = images->getFirstImagePath();
    auto secondImage = images->getSecondImage();
    auto secondImagePath = images->getSecondImagePath();
    auto firstImageName = info.getFirstImageName();
    auto secondImageName = info.getSecondImageName();

    ComparableImage comapableImage1 { firstImage, firstImageName };
    ComparableImage comapableImage2 { secondImage, secondImageName };

    auto result = comparator->compare(comapableImage1, comapableImage2);

    if (result.get() == nullptr) {
        throw std::runtime_error("Error: The comparator returns nothing.");
    }

    if (result->getType() == ComparisonResultVariantType::Image) {
        QImage imageResult = result->getImageResult();
        QPixmap pixmap = QPixmap::fromImage(imageResult);
        if (pixmap.isNull()) {
            throw std::runtime_error("Error: The comparator returns an empty result.");
        }
        int originalWidth = mOriginalImages->getFirstImage().width();
        int originalHeight = mOriginalImages->getSecondImage().height();
        int resultWidth = imageResult.width();
        int resultHeight = imageResult.height();
        if (originalWidth == resultWidth && originalHeight == resultHeight) {
            setLastComparisonImage(pixmap, comparator->getShortName());
            notifyComparisonResultLoaded(pixmap, comparator->getShortName());
        } else {
            notifyShowImageInExternalViewer(pixmap, comparator->getShortName());
        }
    }
    else if (result->getType() == ComparisonResultVariantType::String) {
        QString stringResult = result->getStringResult();
        if (stringResult.isNull() || stringResult.isEmpty()) {
            throw std::runtime_error("Error: The comparator returns an empty result.");
        }
        notifyComparisonResultLoaded(stringResult,
                                     comparator->getFullName(),
                                     firstImagePath,
                                     secondImagePath
                                    );
    }
}

QPixmap ImageProcessingInteractor::applyFilter(const QPixmap &pixmap, IFilterPtr filter) {
    QImage image = pixmap.toImage();
    if (image.isNull()) {
        throw std::runtime_error("An error occurred during the loading of one of the images");
    }
    QImage filteredImage = filter->filter(image);
    if (filteredImage.isNull()) {
        throw std::runtime_error("The filter returns an empty result.");
    }
    auto filteredPixmap = QPixmap::fromImage(filteredImage);
    if (filteredPixmap.isNull()) {
        throw std::runtime_error("The filter returns an empty result.");
    }
    return filteredPixmap;
}

void ImageProcessingInteractor::callFilter(IFilterPtr filter) {
    if (mDisplayedImages == nullptr) {
        return;
    }
    if (mDisplayedImages->isSingleImage()) {
        QPixmap firstPixmap = applyFilter(mDisplayedImages->getFirstImage(), filter);
        mDisplayedImages = std::make_shared<ImageHolder>(firstPixmap, mDisplayedImages->getFirstImagePath());
    } else {
        QPixmap firstPixmap = applyFilter(mDisplayedImages->getFirstImage(), filter);
        QPixmap secondPixmap = applyFilter(mDisplayedImages->getSecondImage(), filter);
        mDisplayedImages = std::make_shared<ImageHolder>(firstPixmap,
                                                        mDisplayedImages->getFirstImagePath(),
                                                        secondPixmap,
                                                        mDisplayedImages->getSecondImagePath());
    }
    clearLastComparisonImage();
    notifyFilteredResultLoaded(mDisplayedImages);
}

QList<ImageProcessorInfo> ImageProcessingInteractor::getImageProcessorsInfo() {

    PluginsManager *pluginsManager = new PluginsManager();
    ImageProcessorsManager *processorsManager = ImageProcessorsManager::instance();
    processorsManager->clear();

    // add comparators

    auto imageComparator = make_shared<MonoColoredDifferenceInPixelValuesComporator>();
    auto imageSaturationComporator = make_shared<ColorsSaturationComporator>();
    auto imageContrastComporator = make_shared<ContrastComporator>();
    auto imagePixelsAbsoluteValueComparatorTxt = make_shared<ColoredDifferenceInPixelValuesComporator>(
                                                            ColoredDifferenceInPixelValuesComporator::Result::Text
                                                                );
    auto imagePixelsAbsoluteValueComparatorImg = make_shared<ColoredDifferenceInPixelValuesComporator>(
                                                            ColoredDifferenceInPixelValuesComporator::Result::Image
                                                            );
    auto imagePixelsBrightnessComparator = make_shared<PixelsBrightnessComparator>();
    auto sharpnessComparator = make_shared<SharpnessComparator>();
    auto imageProximityComparator = make_shared<ImageProximityToOriginComparator>();
    auto customRangedPixelsComparatorImg = make_shared<CustomRangedDifferenceInPixelValuesComparator>();
    auto linerNonLinerDifferenceComparator = make_shared<LinerNonLinerDifferenceComparator>();

    processorsManager->addProcessor(imageComparator);
    processorsManager->addProcessor(imageSaturationComporator);
    processorsManager->addProcessor(imageContrastComporator);
    processorsManager->addProcessor(imagePixelsAbsoluteValueComparatorTxt);
    processorsManager->addProcessor(imagePixelsAbsoluteValueComparatorImg);
    processorsManager->addProcessor(imagePixelsBrightnessComparator);
    processorsManager->addProcessor(sharpnessComparator);
    processorsManager->addProcessor(imageProximityComparator);
    processorsManager->addProcessor(customRangedPixelsComparatorImg);
    processorsManager->addProcessor(linerNonLinerDifferenceComparator);

    // add filters

    auto redChannelFilter = make_shared<RedChannelFilter>();
    auto greenChannelFilter = make_shared<GreenChannelFilter>();
    auto blueChannelFilter = make_shared<BlueChannelFilter>();
    auto grayscaleFilter = make_shared<GrayscaleFilter>();
    processorsManager->addProcessor(redChannelFilter);
    processorsManager->addProcessor(greenChannelFilter);
    processorsManager->addProcessor(blueChannelFilter);
    processorsManager->addProcessor(grayscaleFilter);

    // add plugins (comparators and filters)

    auto processors = pluginsManager->loadPlugins();

    foreach (auto processor, processors) {
        processorsManager->addProcessor(processor);
    }

    return processorsManager->getAllProcessorsInfo();
}

void ImageProcessingInteractor::runAllComparators() {

    ImagesInfo info { mDisplayedImages };

    auto image1 = mDisplayedImages->getFirstImage();
    auto image2 = mDisplayedImages->getSecondImage();
    auto baseName1 = info.getFirstImageBaseName();
    auto baseName2 = info.getSecondImageBaseName();
    auto fullName1 = info.getFirstImageName();
    auto fullName2 = info.getSecondImageName();

    QString reportDirName = QString("%1_vs_%2_comparison_report").arg(baseName1, baseName2);

    QString saveReportDirPath = info.getFirstImageDir() + QDir::separator() + reportDirName;

    ComparableImage firstComparableImage {image1, fullName1};
    ComparableImage secondComparableImage {image2, fullName2};

    RunAllComparatorsInteractor runAllComparatorsInteractor {
                                            mProgressDialogCallback,
                                            firstComparableImage,
                                            secondComparableImage,
                                            saveReportDirPath
                                        };

    runAllComparatorsInteractor.run();
}

bool ImageProcessingInteractor::subscribe(IImageProcessingInteractorListener *listener) {
    if (listener == nullptr) {
        return false;
    }
    if (mListeners.contains(listener)) {
        return false;
    }
    mListeners.append(listener);
    notifyFastSwitchingToComparisonImageStatusChanged(
                        mLastDisplayedComparisonResult.hasLastDisplayedComparisonResult()
                                );
    return true;
}

bool ImageProcessingInteractor::unsubscribe(const IImageProcessingInteractorListener *listener) {
    if (listener == nullptr) {
        return false;
    }
    notifyFastSwitchingToComparisonImageStatusChanged(false);
    return mListeners.removeOne(listener);
}

void ImageProcessingInteractor::notifyComparisonResultLoaded(const QPixmap &image,
                                                             const QString &description
                                                             )
{
    foreach (auto listener, mListeners) {
        listener->onComparisonResultLoaded(image, description);
    }
}

void ImageProcessingInteractor::notifyComparisonResultLoaded(const QString &html,
                                                             const QString &comporatorFullName,
                                                             const QString &firstImagePath,
                                                             const QString &secondImagePath
                                                            )
{
    foreach (auto listener, mListeners) {
        listener->onComparisonResultLoaded(html,
                                           comporatorFullName,
                                           firstImagePath,
                                           secondImagePath
                                           );
    }
}

void ImageProcessingInteractor::notifyShowImageInExternalViewer(const QPixmap &image,
                                                                const QString &description
                                                                )
{
    foreach (auto listener, mListeners) {
        listener->onShowImageInExternalViewer(image, description);
    }
}

void ImageProcessingInteractor::notifyFilteredResultLoaded(const ImageHolderPtr imageHolder) {
    foreach (auto listener, mListeners) {
        listener->onFilteredResultLoaded(imageHolder);
    }
}

void ImageProcessingInteractor::notifyImageProcessorFailed(const QString &error) {
    foreach (auto listener, mListeners) {
        listener->onImageProcessorFailed(error);
    }
}

void ImageProcessingInteractor::notifyFastSwitchingToComparisonImageStatusChanged(bool isSwitchingAvailable) {
    foreach (auto listener, mListeners) {
        listener->onFastSwitchingToComparisonImageStatusChanged(isSwitchingAvailable);
    }
}
