#include "imageviewer.h"

#include <QtCore/qmimedata.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qgraphicsitem.h>
#include <QtWidgets/qstatusbar.h>
#include <qmainwindow.h>
#include <qmessagebox.h>
#include <qapplication.h>
#include <qprocess.h>
#include <qlabel.h>
#include <QTimer>
#include <QGraphicsView>
#include <presentation/mainwindow.h>
#include <business/utils/imagesinfo.h>


ImageViewer::ImageViewer(IDropListener *dropListener, MainWindow *parent)
    : QGraphicsView(parent),
    mParent(parent),
    mDropListener(dropListener),
    mIsSingleImageMode(false),
    mInvalidColor({-1, -1, -1})
{    
    mCustomScene = nullptr;
    mFirstDisplayedImage = nullptr;
    mSecondDisplayedImage = nullptr;
    mComparatorResultDisplayedImage = nullptr;

    QColor backgroundColor = QApplication::palette().color(QPalette::Window);
    setBackgroundBrush(backgroundColor);

    setMouseTracking(true);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    cleanUp();
}

ImageViewer::~ImageViewer() {
    cleanUp();
}

/* Color Picker is run { */

void ImageViewer::onColorUnderCursorTrackingStatusChanged(bool isActivate) {
    if (isActivate) {
        mIsColorUnderCursorTrackingActive = true;
        sendPixelColorUnderCursor(mLastCursorPos);
    } else {
        mIsColorUnderCursorTrackingActive = false;
    }
}

/* } =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Zoom { */

void ImageViewer::wheelEvent(QWheelEvent *event) {
    if (!hasActiveSession()) {
        event->ignore();
        return;
    }
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
    event->accept();
}

void ImageViewer::zoomIn() {
    setCenterToViewRectCenter();
    scale(1.25, 1.25);
    sendPixelColorUnderCursor(mLastCursorPos);
}

void ImageViewer::zoomOut() {
    setCenterToViewRectCenter();
    scale(0.8, 0.8);
    sendPixelColorUnderCursor(mLastCursorPos);
}

void ImageViewer::setCenterToViewRectCenter() {
    QRectF viewRect = mapToScene(viewport()->geometry()).boundingRect();
    centerOn(viewRect.center());
}

void ImageViewer::setToActualSize() {
    if (!hasActiveSession()) {
        return;
    }
    // Pixmap items use logical coordinates; Actual Size maps each source
    // pixel to one physical viewport pixel, including DPR-tagged images.
    const qreal imageDpr = getImageShowedOnTheScreen().mImage.devicePixelRatio();
    const qreal actualScale = imageDpr / viewport()->devicePixelRatioF();
    setTransform(QTransform::fromScale(actualScale, actualScale));
}

void ImageViewer::setToFitImageInView() {
    if (!hasActiveSession()) {
        return;
    }
    fitInView(mFirstDisplayedImage, Qt::KeepAspectRatio);
    if (mSecondDisplayedImage != nullptr) {
        fitInView(mSecondDisplayedImage, Qt::KeepAspectRatio);
    }
    if (mComparatorResultDisplayedImage != nullptr) {
        fitInView(mComparatorResultDisplayedImage, Qt::KeepAspectRatio);
    }
}

/* } =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Show images in QGraphicsView { */

void ImageViewer::displayImages(const ImageHolderPtr images) {

    cleanUp();

    if (images == nullptr) {
        return;
    }

    mIsSingleImageMode = images->isSingleImage();

    setAcceptDrops(true);
    mCustomScene = new QGraphicsScene(this);
    setScene(mCustomScene);

    ImagesInfo info { images };
    mFirstImagePath = images->getFirstImagePath();
    mFirstImageBaseName = info.getFirstImageBaseName();;
    mFirstImageName = info.getFirstImageName();
    mFirstDisplayedImage = new GraphicsPixmapItem(images->getFirstImage(), mDropListener);
    mCustomScene->addItem(mFirstDisplayedImage);
    mParent->onComparebleImageDisplayed(mFirstImageName);

    if (!mIsSingleImageMode) {
        mSecondImagePath = images->getSecondImagePath();
        mSecondImageBaseName = info.getSecondImageBaseName();
        mSecondImageName = info.getSecondImageName();
        mSecondDisplayedImage = new GraphicsPixmapItem(images->getSecondImage(), mDropListener);
        mSecondDisplayedImage->setVisible(false);
        mCustomScene->addItem(mSecondDisplayedImage);
    }

    QTimer::singleShot(0, this, [this]() {
        setToFitImageInView();
    });
}

void ImageViewer::showImageFromComparator(const QPixmap &image, const QString &description) {
    if (!hasActiveSession() || mIsSingleImageMode) {
        return;
    }
    if (mComparatorResultDisplayedImage != nullptr) {
        mCustomScene->removeItem(mComparatorResultDisplayedImage);
        delete mComparatorResultDisplayedImage;
        mComparatorResultDisplayedImage = nullptr;
    }

    mComparatorResultDisplayedImage = new GraphicsPixmapItem(image, mDropListener);
    mCustomScene->addItem(mComparatorResultDisplayedImage);
    mFirstDisplayedImage->setVisible(false);
    mSecondDisplayedImage->setVisible(false);
    mComparatorResultDisplayedImage->setVisible(true);
    mParent->onComparisonImageDisplayed(mFirstImageName, mSecondImageName, description);
}

void ImageViewer::replaceDisplayedImages(const ImageHolderPtr imageHolder) {
    if (!hasActiveSession() || imageHolder == nullptr) {
        return;
    }

    QRectF viewRect = mapToScene(viewport()->geometry()).boundingRect();

    if (mFirstDisplayedImage != nullptr) {
        mCustomScene->removeItem(mFirstDisplayedImage);
        delete mFirstDisplayedImage;
        mFirstDisplayedImage = nullptr;
    }
    if (mSecondDisplayedImage != nullptr) {
        mCustomScene->removeItem(mSecondDisplayedImage);
        delete mSecondDisplayedImage;
        mSecondDisplayedImage = nullptr;
    }
    if (mComparatorResultDisplayedImage != nullptr) {
        mCustomScene->removeItem(mComparatorResultDisplayedImage);
        delete mComparatorResultDisplayedImage;
        mComparatorResultDisplayedImage = nullptr;
    }

    mFirstDisplayedImage = new GraphicsPixmapItem(imageHolder->getFirstImage(), mDropListener);
    mCustomScene->addItem(mFirstDisplayedImage);

    if (!mIsSingleImageMode) {
        mSecondDisplayedImage = new GraphicsPixmapItem(imageHolder->getSecondImage(), mDropListener);

        if (mCurrentImageIndex == 0) {
            mSecondDisplayedImage->setVisible(false);
            mParent->onComparebleImageDisplayed(mFirstImageName);
        } else {
            mFirstDisplayedImage->setVisible(false);
            mParent->onComparebleImageDisplayed(mSecondImageName);
        }
        mCustomScene->addItem(mSecondDisplayedImage);
    }
    centerOn(viewRect.center());
    if (mLastCursorPos) {
        sendPixelColorUnderCursor(mLastCursorPos.value());
    }
}

bool ImageViewer::hasActiveSession() {
    if (mIsSingleImageMode) {
        return (mFirstDisplayedImage != nullptr);
    } else {
        return (mFirstDisplayedImage != nullptr && mSecondDisplayedImage != nullptr);
    }
}

void ImageViewer::cleanUp() {
    if (mFirstDisplayedImage != nullptr) {
        mCustomScene->removeItem(mFirstDisplayedImage);
        delete mFirstDisplayedImage;
        mFirstDisplayedImage = nullptr;
    }
    if (mSecondDisplayedImage != nullptr) {
        mCustomScene->removeItem(mSecondDisplayedImage);
        delete mSecondDisplayedImage;
        mSecondDisplayedImage = nullptr;
    }
    if (mComparatorResultDisplayedImage != nullptr) {
        mCustomScene->removeItem(mComparatorResultDisplayedImage);
        delete mComparatorResultDisplayedImage;
        mComparatorResultDisplayedImage = nullptr;
    }
    if (mCustomScene != nullptr) {
        setScene(nullptr);
        delete mCustomScene;
        mCustomScene = nullptr;
    }
    mFirstImagePath = "";
    mSecondImagePath = "";
    mFirstImageBaseName = "";
    mSecondImageBaseName = "";
    mCurrentImageIndex = 0;
    mIsColorUnderCursorTrackingActive = false;
    mLastCursorPos = std::nullopt;
    mIsSelecting = false;
    mIsZoomToSelectionEnabled = false;
    mSelectionStart = {};
    mSelectionRect = {};
}

/* } =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/*  Switching between images displayed in QGraphicsView.
 *  There are two images that the user compares,
 *  and the switching occurs between them.
 *  The result of the comparator's work is a third image,
 *  which can also be displayed in QGraphicsView
 *  but is not involved in the switching.
 *  It can only be displayed by running the corresponding
 *  comparator, which will compare the two images
 *  and return the comparison result as an image.
 *  {   */

void ImageViewer::toggleImage() {
    if (!hasActiveSession() || mIsSingleImageMode) {
        return;
    }

    // Switching visibility already preserves the viewport's transform and
    // scroll position. Recentring here introduces drift on repeated switches.

    if (mComparatorResultDisplayedImage != nullptr) {
        mCustomScene->removeItem(mComparatorResultDisplayedImage);
        delete mComparatorResultDisplayedImage;
        mComparatorResultDisplayedImage = nullptr;
        mCurrentImageIndex == 1 ?
            mCurrentImageIndex = 0 :
            mCurrentImageIndex = 1;
    }

    if (mCurrentImageIndex == 0) {
        mFirstDisplayedImage->setVisible(false);
        mSecondDisplayedImage->setVisible(true);
        mCurrentImageIndex = 1;
        mParent->onComparebleImageDisplayed(mSecondImageName);
    } else {
        mFirstDisplayedImage->setVisible(true);
        mSecondDisplayedImage->setVisible(false);
        mCurrentImageIndex = 0;
        mParent->onComparebleImageDisplayed(mFirstImageName);
    }

    if (mLastCursorPos) {
        sendPixelColorUnderCursor(mLastCursorPos.value());
    }
}

void ImageViewer::showFirstImage() {
    if (mComparatorResultDisplayedImage != nullptr) {
        mCurrentImageIndex = 0;
    } else {
        mCurrentImageIndex = 1;
    }
    toggleImage();
}

void ImageViewer::showSecondImage() {
    if (mComparatorResultDisplayedImage != nullptr) {
        mCurrentImageIndex = 1;
    } else {
        mCurrentImageIndex = 0;
    }
    toggleImage();
}

/* } =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Saving an image (or an area of the image) to disk { */


// Implementation of a method to capture the image displayed
// in QGraphicsView, taking into account the current scale (zoom)
// and visible area. In other words, everything outside the
// currently visible area will be cropped.
SaveImageInfo ImageViewer::getCurrentVisiableArea() {
    if (!hasActiveSession()) {
        return {};
    }

    QPixmap pixmap = getVisiblePixmap();

    SaveImageInfoType contentType = SaveImageInfoType::None;
    bool isComparisonImageShowing = (mComparatorResultDisplayedImage != nullptr);
    if (isComparisonImageShowing) {
        contentType = SaveImageInfoType::ComparisonImageArea;
    } else if (mCurrentImageIndex == 0) {
        contentType = SaveImageInfoType::FirstImageArea;
    } else if (mCurrentImageIndex == 1) {
        contentType = SaveImageInfoType::SecondImageArea;
    }

    return SaveImageInfo(contentType, pixmap);
}

SaveImageInfo ImageViewer::getImageShowedOnTheScreen() {
    if (!hasActiveSession()) {
        return {};
    }
    bool isComparisonImageShowing = (mComparatorResultDisplayedImage != nullptr);
    if (isComparisonImageShowing) {
        return SaveImageInfo(SaveImageInfoType::ComparisonImage, mComparatorResultDisplayedImage->pixmap());
    } else if (mCurrentImageIndex == 0) {
        return SaveImageInfo(SaveImageInfoType::FirstImage, mFirstDisplayedImage->pixmap());
    } else if (mCurrentImageIndex == 1) {
        return SaveImageInfo(SaveImageInfoType::SecondImage, mSecondDisplayedImage->pixmap());
    }
    return {};
}

// Function to get the visible portion of the image in QGraphicsView
QPixmap ImageViewer::getVisiblePixmap() {

    // Get the visible area in scene coordinates
    QRectF visibleSceneRect = mapToScene(viewport()->geometry()).boundingRect();

    auto items = mCustomScene->items(visibleSceneRect);
    QGraphicsPixmapItem* pixmapItem = nullptr;

    for (auto it = items.begin(); it != items.end(); ++it) {
        pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>((*it));
        if (pixmapItem) {
            break; // Take the first image found
        }
    }

    if (!pixmapItem) {
        return QPixmap(); // If no image is found, return an empty QPixmap
    }

    // Get the original image (pixmap)
    QPixmap originalPixmap = pixmapItem->pixmap();

    // Convert the visible area from scene coordinates to image coordinates
    QRectF pixmapRect = pixmapItem->sceneBoundingRect(); // Image boundaries in scene coordinates
    QRectF intersection = visibleSceneRect.intersected(pixmapRect); // Intersection of visible area and image

    if (intersection.isEmpty()) {
        return QPixmap(); // If there is no intersection, return an empty QPixmap
    }

    // Transform coordinates from scene to image pixel coordinates
    QRectF sourceRect(
        (intersection.left() - pixmapRect.left()) / pixmapRect.width() * originalPixmap.width(),
        (intersection.top() - pixmapRect.top()) / pixmapRect.height() * originalPixmap.height(),
        intersection.width() / pixmapRect.width() * originalPixmap.width(),
        intersection.height() / pixmapRect.height() * originalPixmap.height()
        );

    // Create a new pixmap with the size of the visible area
    QSize targetSize = viewport()->size();
    QPixmap result(targetSize);
    result.fill(Qt::transparent);

    // Draw the portion of the image on the resulting QPixmap while preserving proportions
    QPainter painter(&result);
    painter.drawPixmap(
        QRectF(0, 0, targetSize.width(), targetSize.height()), // Where to draw (entire result)
        originalPixmap,
        sourceRect // Which part of the original image to draw
        );
    painter.end();

    return result;
}

ImageHolderPtr ImageViewer::getCroppedImages(const QRectF &rect) {
    QRect selectionRect = rect.toAlignedRect();
    auto firstPixmap = mFirstDisplayedImage->pixmap();
    QRect boundedRect1 = selectionRect.intersected(firstPixmap.rect());
    QPixmap croppedPixmap1 = firstPixmap.copy(boundedRect1);
    if (mIsSingleImageMode) {
        return std::make_shared<ImageHolder>(croppedPixmap1, mFirstImageBaseName);
    }
    auto secondPixmap = mSecondDisplayedImage->pixmap();
    QRect boundedRect2 = selectionRect.intersected(secondPixmap.rect());
    QPixmap croppedPixmap2 = secondPixmap.copy(boundedRect2);
    if (mCurrentImageIndex == 0) {
        return std::make_shared<ImageHolder>(croppedPixmap1,
                                             mFirstImageBaseName,
                                             croppedPixmap2,
                                             mSecondImageBaseName
                                             );
    } else {
        return std::make_shared<ImageHolder>(croppedPixmap2,
                                             mSecondImageBaseName,
                                             croppedPixmap1,
                                             mFirstImageBaseName
                                             );
    }
}

/* } =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* Events { */

void ImageViewer::mouseMoveEvent(QMouseEvent* event) {
    if (event == nullptr || !hasActiveSession()) {
        return;
    }
    mLastCursorPos = event->pos();
    QGraphicsView::mouseMoveEvent(event);

    if (mIsSelecting) {
        // Zoom to selection.
        // Update the selection rectangle as the user drags the mouse
        QPoint currentPoint = event->pos();
        mSelectionRect = QRect(mSelectionStart, currentPoint).normalized();
        viewport()->update();
    }

    // Implement RGB values tracking under the mouse cursor. It needs for Color Picker.
    sendPixelColorUnderCursor(event->pos());
}

void ImageViewer::sendPixelColorUnderCursor(std::optional<QPoint> cursorPos) {
    if (!mIsColorUnderCursorTrackingActive || !cursorPos || !hasActiveSession()) {
        return;
    }

    // Convert mouse position to scene coordinates
    QPointF scenePos = mapToScene(cursorPos.value());

    // Get the item under the cursor (if any)
    QGraphicsItem *item = mCustomScene->itemAt(scenePos, transform());
    if (item) {
        // Check if the item is a pixmap (image)
        QGraphicsPixmapItem *pixmapItem = dynamic_cast<QGraphicsPixmapItem *>(item);
        if (pixmapItem) {
            // Get the pixmap (image) from the item
            const QPixmap pixmap = pixmapItem->pixmap();
            const QImage visibleImage = pixmap.toImage();

            // Convert scene coordinates to image coordinates
            QPointF itemPos = pixmapItem->mapFromScene(scenePos);
            int x = static_cast<int>(itemPos.x());
            int y = static_cast<int>(itemPos.y());

            if (mIsSingleImageMode) {
                sendPixelColorValuesForSingleImage(visibleImage, x, y);
            } else {
                sendPixelColorValuesForTwoImages(visibleImage, x, y);
            }
        }
    }
}

void ImageViewer::sendPixelColorValuesForSingleImage(const QImage &visibleImage, int &x, int &y) {
    // Ensure the coordinates are within the image bounds
    if (x >= 0 && x < visibleImage.width() && y >= 0 && y < visibleImage.height()) {
        // Get the color of the pixel
        QColor colorOfImage;
        QString imageName;
        colorOfImage = mFirstDisplayedImage->pixmap().toImage().pixelColor(x, y);
        imageName = mFirstImageBaseName;
        sendPixelColorValues(imageName, colorOfImage, std::nullopt, std::nullopt);
    }
}

void ImageViewer::sendPixelColorValuesForTwoImages(const QImage &visibleImage, int &x, int &y) {
    // Ensure the coordinates are within the image bounds
    if (x >= 0 && x < visibleImage.width() && y >= 0 && y < visibleImage.height()) {
        // Get the color of the pixel
        QColor colorOfVisibleImage, colorOfHiddenImage;
        QString visibleImageName, hiddenImageName;
        if (mCurrentImageIndex == 0) {
            colorOfVisibleImage =
                mFirstDisplayedImage->pixmap().toImage().pixelColor(x, y);
            colorOfHiddenImage =
                mSecondDisplayedImage->pixmap().toImage().pixelColor(x, y);
            visibleImageName = mFirstImageBaseName;
            hiddenImageName = mSecondImageBaseName;
        } else {
            colorOfVisibleImage =
                mSecondDisplayedImage->pixmap().toImage().pixelColor(x, y);
            colorOfHiddenImage =
                mFirstDisplayedImage->pixmap().toImage().pixelColor(x, y);
            visibleImageName = mSecondImageBaseName;
            hiddenImageName = mFirstImageBaseName;
        }
        sendPixelColorValues(visibleImageName,
                             colorOfVisibleImage,
                             hiddenImageName,
                             colorOfHiddenImage
                             );
    }
}

void ImageViewer::sendPixelColorValues(const QString &visibleImageName,
                                       const QColor &colorOfVisibleImage,
                                       const std::optional<QString> &hiddenImageName,
                                       const std::optional<QColor> &colorOfHiddenImage
                                       )
{
    ImagePixelColor pixelValueOfVisibleImage = {
        visibleImageName,
        colorOfVisibleImage.red(),
        colorOfVisibleImage.green(),
        colorOfVisibleImage.blue()
    };
    std::optional<ImagePixelColor> pixelValueOfHiddenImage;
    if (hiddenImageName && colorOfHiddenImage) {

        pixelValueOfHiddenImage = {
                hiddenImageName.value(),
                colorOfHiddenImage.value().red(),
                colorOfHiddenImage.value().green(),
                colorOfHiddenImage.value().blue()
        };
    }
    mParent->onColorUnderCursorChanged(pixelValueOfVisibleImage, pixelValueOfHiddenImage);
}

// Implement zoom to selection
void ImageViewer::mousePressEvent(QMouseEvent *event) {
    if (!hasActiveSession()) {
        return;
    }
    bool isComparisonImageDisplayed = (mComparatorResultDisplayedImage != nullptr);
    auto shiftModifier = event->modifiers() & Qt::ShiftModifier;
    auto controlModifier = (event->modifiers() & Qt::ControlModifier) && !isComparisonImageDisplayed;
    auto altModifier = (event->modifiers() & Qt::AltModifier) && !mIsSingleImageMode && !isComparisonImageDisplayed;
    if (event->button() == Qt::LeftButton && (shiftModifier || controlModifier || altModifier)) {
        mIsSelecting = true;
        mSelectionStart = event->pos(); // Save the starting point of the selection in view coordinates
        mSelectionRect = QRect();       // Reset the selection rectangle
        setDragMode(NoDrag);
    }
    QGraphicsView::mousePressEvent(event);
}

// Implement zoom to selection
void ImageViewer::mouseReleaseEvent(QMouseEvent *event) {
    if (!hasActiveSession()) {
        return;
    }
    if (mIsSelecting && event->button() == Qt::LeftButton) {
        mIsSelecting = false;

        // Convert the selection rectangle from view coordinates to scene coordinates
        QRectF sceneSelectionRect = mapToScene(mSelectionRect).boundingRect();

        // Zoom into the selected area
        if (!sceneSelectionRect.isEmpty() &&
            sceneSelectionRect.isValid() &&
            event->modifiers() & Qt::ShiftModifier
            )
        {
            fitInView(sceneSelectionRect, Qt::KeepAspectRatio); // Adjust view to fit the selected area
        }
        else if (!sceneSelectionRect.isEmpty() &&
                 sceneSelectionRect.isValid() &&
                 event->modifiers() & Qt::ControlModifier
                 )
        {
            ImageHolderPtr images = getCroppedImages(sceneSelectionRect);
            mParent->onImagesCropped(images);
        }
        else if (!sceneSelectionRect.isEmpty() &&
                 sceneSelectionRect.isValid() &&
                 event->modifiers() & Qt::AltModifier
                 )
        {
            // If the user holds down the Command (Ctrl) key along with the comparator hotkey and selects
            // a specific area while holding the left mouse button, the comparator will run only for
            // the selected area.
            ImageHolderPtr images = getCroppedImages(sceneSelectionRect);
            mParent->onSelectedAreaShouldBeAnalyzed(images, mPressedKey);
        }
        mSelectionRect = QRect(); // Clear the selection rectangle
        viewport()->update();    // Request a repaint of the view
        setDragMode(ScrollHandDrag);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void ImageViewer::keyPressEvent(QKeyEvent *event) {
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) {
        mPressedKey = event->key();
    }
    QGraphicsView::keyPressEvent(event);
}

void ImageViewer::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z) {
        mPressedKey = std::nullopt;
    }
    QGraphicsView::keyReleaseEvent(event);
}

// Implement zoom to selection
void ImageViewer::paintEvent(QPaintEvent *event) {
    // Call base class paint event first
    QGraphicsView::paintEvent(event);

    if (!hasActiveSession()) {
        return;
    }
    // Draw the selection rectangle if it exists
    if (!mSelectionRect.isNull()) {
        QPainter painter(viewport());
        painter.setPen(QPen(Qt::blue, 1, Qt::DashLine)); // Dashed blue border for selection rectangle
        painter.setBrush(QBrush(Qt::transparent));       // Transparent fill
        painter.drawRect(mSelectionRect);
    }
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("text/uri-list") && mDropListener) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageViewer::dropEvent(QDropEvent *event) {
    if (!event->mimeData()->hasUrls() || !mDropListener) {
        event->ignore();
    } else {
        event->acceptProposedAction();
        QList<QUrl> urls = event->mimeData()->urls();
        mDropListener->onDrop(urls);
    }
}

/* } =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
