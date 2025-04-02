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
#include <data/storage/savefiledialoghandler.h>
#include <domain/interfaces/presentation/imageprocessinginteractorlistener.h>
#include <business/utils/imagesinfo.h>
#include "domain/interfaces/presentation/iprocessorpropertiesdialogcallback.h"
#include "imageprocessorsmanager.h"
#include "runallcomparatorsinteractor.h"

ImageProcessingInteractor::ImageProcessingInteractor(
                                    const ImagesPtr images,
                                    IPropcessorPropertiesDialogCallback *propertiesDialogCallback,
                                    IProgressDialog *progressDialogCallback
                                        )
    : propertiesDialogCallback(propertiesDialogCallback),
    progressDialogCallback(progressDialogCallback),
    originalImages(images),
    displayedImages(images)
{
}

ImageProcessingInteractor::~ImageProcessingInteractor() {
    listeners.clear();
    originalImages = nullptr;
    displayedImages = nullptr;
    propertiesDialogCallback = nullptr;
    progressDialogCallback = nullptr;
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
        callComparator(dynamic_pointer_cast<IComparator>(processor), displayedImages);
    } else if (processor->getType() == ImageProcessorType::Filter) {
        callFilter(dynamic_pointer_cast<IFilter>(processor));
    } else {
        throw std::runtime_error("Error: An unknown image processor type.");
    }
}

void ImageProcessingInteractor::clearLastComparisonImage() {
    lastDisplayedComparisonResult.clear();
    notifyFastSwitchingToComparisonImageStatusChanged(false);
}

void ImageProcessingInteractor::setLastComparisonImage(const QPixmap &pixmap, const QString &description) {
    lastDisplayedComparisonResult.set(pixmap, description);
    notifyFastSwitchingToComparisonImageStatusChanged(true);
}

void ImageProcessingInteractor::showLastComparisonImage() {
    if (lastDisplayedComparisonResult.hasLastDisplayedComparisonResult()) {
        notifyComparisonResultLoaded(lastDisplayedComparisonResult.getImage(),
                                     lastDisplayedComparisonResult.getDescription()
                                     );
    }
}

// If the user holds down the Command (Ctrl) key along with the comparator hotkey and selects
// a specific area while holding the left mouse button, the comparator will run only for
// the selected area.
void ImageProcessingInteractor::analyzeSelectedArea(ImagesPtr images, std::optional<int> key) {
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
    if (originalImages == nullptr) {
        return;
    }
    displayedImages = originalImages;
    clearLastComparisonImage();
    notifyFilteredResultLoaded(displayedImages->image1, displayedImages->image2);
}

QList<ImageProcessorInfo> ImageProcessingInteractor::showImageProcessorsHelp() {
    return ImageProcessorsManager::instance()->getAllProcessorsInfo();
}

void ImageProcessingInteractor::handleProcessorPropertiesIfNeed(IImageProcessorPtr processor) {
    auto properties = processor->getDefaultProperties();
    if (properties.empty()) {
        return;
    }
    auto newProperties = propertiesDialogCallback->showImageProcessorPropertiesDialog(
                                                                    processor->getShortName(),
                                                                    processor->getDescription(),
                                                                    properties
                                                                );
    if (!newProperties.empty()) {
        processor->setProperties(newProperties);
    }
}

void ImageProcessingInteractor::callComparator(IComparatorPtr comparator, ImagesPtr images) {
    if (images == nullptr) {
        return;
    }
    ImagesInfo info { images };

    auto firstImage = images->image1;
    auto firstImagePath = images->path1;
    auto secondImage = images->image2;
    auto secondImagePath = images->path2;
    auto firstImageName = info.getFirstImageName();
    auto secondImageName = info.getSecondImageName();

    ComparableImage comapableImage1 { firstImage, firstImageName };
    ComparableImage comapableImage2 { secondImage, secondImageName };

    auto result = comparator->compare(comapableImage1, comapableImage2);

    if (result.get() == nullptr) {
        throw std::runtime_error("Error: The comparator returns nothing.");
    }

    if (result->type() == ComparisonResultVariantType::Image) {
        QImage imageResult = result->imageResult();
        QPixmap pixmap = QPixmap::fromImage(imageResult);
        if (pixmap.isNull()) {
            throw std::runtime_error("Error: The comparator returns an empty result.");
        }
        int originalWidth = originalImages->image1.width();
        int originalHeight = originalImages->image1.height();
        int resultWidth = imageResult.width();
        int resultHeight = imageResult.height();
        if (originalWidth == resultWidth && originalHeight == resultHeight) {
            setLastComparisonImage(pixmap, comparator->getShortName());
            notifyComparisonResultLoaded(pixmap, comparator->getShortName());
        } else {
            notifyShowImageInExternalViewer(pixmap, comparator->getShortName());
        }
    }
    else if (result->type() == ComparisonResultVariantType::String) {
        QString stringResult = result->stringResult();
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

void ImageProcessingInteractor::callFilter(IFilterPtr filter) {
    if (displayedImages == nullptr) {
        return;
    }
    auto firstImage = displayedImages->image1.toImage();
    auto secondImage = displayedImages->image2.toImage();

    if (firstImage.isNull() || secondImage.isNull()) {
        throw std::runtime_error("Error: An error occurred during the loading of one of the images");
    }

    QImage transformedImage1 = filter->filter(firstImage);
    QImage transformedImage2 = filter->filter(secondImage);

    if (transformedImage1.isNull() || transformedImage2.isNull()) {
        throw std::runtime_error("Error: The filter returns an empty result.");
    }

    auto pixmap1 = QPixmap::fromImage(transformedImage1);
    auto pixmap2 = QPixmap::fromImage(transformedImage2);

    if (pixmap1.isNull() || pixmap2.isNull()) {
        throw std::runtime_error("Error: The filter returns an empty result.");
    }

    displayedImages = std::make_shared<Images>(pixmap1, pixmap2, displayedImages->path1, displayedImages->path2);
    clearLastComparisonImage();
    notifyFilteredResultLoaded(displayedImages->image1, displayedImages->image2);
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

    ImagesInfo info { displayedImages };

    auto image1 = displayedImages->image1;
    auto image2 = displayedImages->image2;
    auto baseName1 = info.getFirstImageBaseName();
    auto baseName2 = info.getSecondImageBaseName();
    auto fullName1 = info.getFirstImageName();
    auto fullName2 = info.getSecondImageName();

    QString reportDirName = QString("%1_vs_%2_comparison_report").arg(baseName1, baseName2);

    QString saveReportDirPath = info.getFirstImageDir() + QDir::separator() + reportDirName;

    ComparableImage firstComparableImage {image1, fullName1};
    ComparableImage secondComparableImage {image2, fullName2};

    RunAllComparatorsInteractor runAllComparatorsInteractor {
                                            progressDialogCallback,
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
    if (listeners.contains(listener)) {
        return false;
    }
    listeners.append(listener);
    notifyFastSwitchingToComparisonImageStatusChanged(
                        lastDisplayedComparisonResult.hasLastDisplayedComparisonResult()
                                );
    return true;
}

bool ImageProcessingInteractor::unsubscribe(const IImageProcessingInteractorListener *listener) {
    if (listener == nullptr) {
        return false;
    }
    notifyFastSwitchingToComparisonImageStatusChanged(false);
    return listeners.removeOne(listener);
}

void ImageProcessingInteractor::notifyComparisonResultLoaded(const QPixmap &image,
                                                             const QString &description
                                                             )
{
    foreach (auto listener, listeners) {
        listener->onComparisonResultLoaded(image, description);
    }
}

// TBD Refactoring
void ImageProcessingInteractor::notifyComparisonResultLoaded(const QString &html,
                                                             const QString &comporatorFullName,
                                                             const QString &firstImagePath,
                                                             const QString &secondImagePath
                                                            )
{
    foreach (auto listener, listeners) {
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
    foreach (auto listener, listeners) {
        listener->onShowImageInExternalViewer(image, description);
    }
}

void ImageProcessingInteractor::notifyFilteredResultLoaded(const QPixmap &firstImage,
                                                           const QPixmap &secondImage
                                                           )
{
    foreach (auto listener, listeners) {
        listener->onFilteredResultLoaded(firstImage, secondImage);
    }
}

void ImageProcessingInteractor::notifyImageProcessorFailed(const QString &error) {
    foreach (auto listener, listeners) {
        listener->onImageProcessorFailed(error);
    }
}

void ImageProcessingInteractor::notifyFastSwitchingToComparisonImageStatusChanged(bool isSwitchingAvailable) {
    foreach (auto listener, listeners) {
        listener->onFastSwitchingToComparisonImageStatusChanged(isSwitchingAvailable);
    }
}
