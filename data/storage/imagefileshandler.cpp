#include "imagefileshandler.h"

#include <QStringList>
#include <QtCore/qdir.h>
#include <qfileinfo.h>
#include <business/recentfilesmanager.h>
#include <business/imageanalysis/imageprocessinginteractor.h>
#include <data/storage/filedialoghandler.h>
#include <domain/valueobjects/images.h>
#include <business/utils/imagesinfo.h>
#include <business/validation/imagevalidationrulesfactory.h>

#define STB_IMAGE_IMPLEMENTATION
#include "data/storage/stb_image.h"


// The user can drag and drop one or two images into the application window.
// In the case of a single image, it will simply open for viewing; only filters
// will be available (but not comparators). In the case of two images, the user
// will be able to compare them; both filters and comparators will be available.
ImagesPtr ImageFilesHandler::openImages(const QList<QUrl> &urls) {
    if (urls.size() == 0 || urls.size() > 2) {
        QString err = QString("Drag and drop one or two images here.");
        throw std::runtime_error(err.toStdString());
    }
    if (urls.size() == 2 && urls[0].isLocalFile() && urls[1].isLocalFile()) {
        const QString file1 = urls[0].toLocalFile();
        const QString file2 = urls[1].toLocalFile();
        return openImages(file1, file2);
    } else if (urls.size() == 1 && urls[0].isLocalFile()) {
        const QString file = urls[0].toLocalFile();
        return openImages(file, file);
    }
    throw std::runtime_error("Incorrect path to image.");
}

ImagesPtr ImageFilesHandler::openImages() {
    FileDialogHandler handler;
    OptionalStringPair twoUserImagePaths = handler.getUserOpenTwoImagePaths("");
    if (!twoUserImagePaths) {
        return nullptr;
    }
    return openImages(twoUserImagePaths.value().first,
                      twoUserImagePaths.value().second
                     );
}

ImagesPtr ImageFilesHandler::openImage() {
    FileDialogHandler handler;
    std::optional<QString> userImagePath = handler.getUserOpenImagePath("");
    if (!userImagePath) {
        return nullptr;     // nullptr means that the user closed the file open
            // dialog and declined the open operation; we do nothing
    }
    return openImages(userImagePath.value(), userImagePath.value());
}

// The function never returns nullptr; if an error occurs,
// a runtime_error with its description will be thrown.
ImagesPtr ImageFilesHandler::openImages(const QString &image1Path, const QString &image2Path) {
    ImagesPtr images;

    QPixmap pixmap1 = openImage(image1Path);
    if (image1Path == image2Path) {
        images = std::make_shared<Images>(pixmap1, image1Path);
    } else {
        QPixmap pixmap2 = openImage(image2Path);
        images = std::make_shared<Images>(pixmap1, pixmap2, image1Path, image2Path);
    }

    validateImages(images);
    return images;
}

QPixmap ImageFilesHandler::openImage(const QString &imagePath) {

    int width, height, channels;

    unsigned char* data = stbi_load(imagePath.toStdString().c_str(), &width, &height, &channels, 4);
    if (!data) {
        QString error = QString("Unable to open " + imagePath + ". The image format might not be supported, "
                                                                 "or the file does not exist.");
        throw std::runtime_error(error.toStdString());
    }

    QImage image(reinterpret_cast<const uchar*>(data),
                 width,
                 height,
                 QImage::Format_RGBA8888
                 );


    QPixmap pixmap = QPixmap::fromImage(image.copy());

    stbi_image_free(data);

    return pixmap;
}

void ImageFilesHandler::validateImages(ImagesPtr images) {
    auto validationRules = ImageValidationRulesFactory::createImageFormatValidator(images);
    auto error = validationRules->isValid();
    if (error == std::nullopt) {
        return; // images are valid
    }
    std::string strError = error->toStdString();
    throw std::runtime_error(strError);
}

std::optional<FileSaveResult> ImageFilesHandler::saveImageAs(const SaveImageInfo &saveImageInfo,
                                              const ImagesPtr images
                                             )
{
    if (saveImageInfo.saveImageInfoType == SaveImageInfoType::None ||
        saveImageInfo.image.isNull() ||
        images == nullptr
        )
    {
        return std::make_optional<FileSaveResult>(false, QString("An internal error occurred: ")
                                                         + "the app is trying to save an "
                                                         + "image of an unsupported type "
                                                         + "or an empty image.");
    }

    ImagesInfo imagesInfo { images };

    auto extentionValidator = ImageValidationRulesFactory::createImageExtensionsInfoProvider();
    QString ext = extentionValidator->getDeafaultSaveExtension(true);

    const QString &file1Name = imagesInfo.getFirstImageBaseName();
    const QString &file2Name = imagesInfo.getSecondImageBaseName();
    const QString &file1DirPath = imagesInfo.getFirstImageDir();
    const QString &file2DirPath = imagesInfo.getSecondImageDir();
    QDir file1Dir { file1DirPath };
    QDir file2Dir { file2DirPath };
    QString fileName, fullPath;

    switch (saveImageInfo.saveImageInfoType) {
    case SaveImageInfoType::FirstImage:
        fileName = QString("%1_copy%2").arg(file1Name, ext);
        fullPath = file1Dir.filePath(fileName);
        break;
    case SaveImageInfoType::SecondImage:
        fileName = QString("%1_copy%2").arg(file2Name, ext);
        fullPath = file2Dir.filePath(fileName);
        break;
    case SaveImageInfoType::FirstImageArea:
        fileName = QString("%1_area%2").arg(file1Name, ext);
        fullPath = file1Dir.filePath(fileName);
        break;
    case SaveImageInfoType::SecondImageArea:
        fileName = QString("%1_area%2").arg(file2Name, ext);
        fullPath = file1Dir.filePath(fileName);
        break;
    case SaveImageInfoType::ComparisonImage:
        fileName = QString("%1_vs_%2_comparison%3")
                       .arg(file1Name, file2Name, ext);
        fullPath = file1Dir.filePath(fileName);
        break;
    case SaveImageInfoType::ComparisonImageArea:
        fileName = QString("%1_vs_%2_area_comparison%3")
                       .arg(file1Name, file2Name, ext);
        fullPath = file1Dir.filePath(fileName);
        break;
    default:
        break;
    }

    bool isSaved = false;
    FileDialogHandler saveFileDialog {};
    auto savePath = saveFileDialog.getUserSaveImagePath(fullPath);
    if (savePath) {
        isSaved = saveImageInfo.image.save(savePath.value());
    }

    return std::make_optional<FileSaveResult>(isSaved, fullPath);
}
