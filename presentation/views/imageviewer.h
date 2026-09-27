#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include "domain/valueobjects/images.h"
#include "graphicspixmapitem.h"
#include <domain/valueobjects/savefileinfo.h>

#include <qgraphicsview.h>

class MainWindow;
class IDropListener;
class QPixmap;

class ImageViewer : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageViewer(IDropListener *dropListener, MainWindow *parent);

    virtual ~ImageViewer();
    
    void displayImages(const ImageHolderPtr images);

    void showImageFromComparator(const QPixmap &image, const QString& description);

    void cleanUp();

    void toggleImage();

    void showFirstImage();

    void showSecondImage();

    // Implementation of a method to capture the image displayed
    // in QGraphicsView, taking into account the current scale (zoom)
    // and visible area. In other words, everything outside the
    // currently visible area will be cropped.
    SaveImageInfo getCurrentVisiableArea();
    SaveImageInfo getImageShowedOnTheScreen();

    // The RGB values of the pixel under the mouse cursor are displayed
    // in the Color Picker if it is open. If true, we track the color
    // changes under the cursor; if false, we do not.
    void onColorUnderCursorTrackingStatusChanged(bool isActivate);

    // Filters can be applied to modify images. This function removes
    // all filters applied to the compared images.
    void replaceDisplayedImages(const ImageHolderPtr imageHolder);

    void zoomIn();
    void zoomOut();
    void setToActualSize();
    void setToFitImageInView();

    bool hasActiveSession();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override ;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    MainWindow *mParent;
    IDropListener *mDropListener;
    QGraphicsScene *mCustomScene;
    QString mFirstImagePath;
    QString mSecondImagePath;
    QString mFirstImageBaseName;
    QString mSecondImageBaseName;
    QString mFirstImageName;
    QString mSecondImageName;
    QGraphicsPixmapItem *mFirstDisplayedImage;
    QGraphicsPixmapItem *mSecondDisplayedImage;
    QGraphicsPixmapItem *mComparatorResultDisplayedImage;
    int mCurrentImageIndex;
    bool mIsColorUnderCursorTrackingActive;
    std::optional<QPoint> mLastCursorPos;
    std::optional<int> mPressedKey;
    bool mIsSingleImageMode;
    QColor mInvalidColor;
    bool mFitImageToView = false;

    // Zoom to selection
    bool mIsSelecting;                       // Whether the user is currently selecting an area
    bool mIsZoomToSelectionEnabled;
    QPoint mSelectionStart;                  // Start point of the selection (in view coordinates)
    QRect mSelectionRect;                    // Rectangle being selected (in view coordinates)

    QPixmap getVisiblePixmap();

    ImageHolderPtr getCroppedImages(const QRectF &rect);

    void sendPixelColorValuesForTwoImages(const QImage &visibleImage, int &x, int &y);
    void sendPixelColorValuesForSingleImage(const QImage &visibleImage, int &x, int &y);
    void sendPixelColorUnderCursor(std::optional<QPoint> cursorPos);

    void sendPixelColorValues(const QString &visibleImageName,
                              const QColor &colorOfVisibleImage,
                              const std::optional<QString> &hiddenImageName,
                              const std::optional<QColor> &colorOfHiddenImage
                              );
    void setCenterToViewRectCenter();
};


#endif // IMAGEVIEWER_H
