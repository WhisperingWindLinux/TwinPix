#include <QtTest>
#include <QSettings>
#include <QTemporaryDir>

#include <presentation/mainwindow.h>
#include <presentation/views/imageviewer.h>

class TestImageViewerNavigation : public QObject {
    Q_OBJECT

private slots:
    void actualSizeMapsSourcePixelsToDisplayPixels_data() {
        QTest::addColumn<qreal>("imageDpr");
        QTest::addColumn<QString>("imageMode");
        for (qreal dpr : {1.0, 1.5, 2.0}) {
            for (const QString &mode : {QString("single"), QString("first"),
                                       QString("second"), QString("comparison")}) {
                const QByteArray name = QString("%1-dpr%2").arg(mode).arg(dpr).toLatin1();
                QTest::newRow(name.constData()) << dpr << mode;
            }
        }
    }

    void actualSizeMapsSourcePixelsToDisplayPixels() {
        QFETCH(qreal, imageDpr);
        QFETCH(QString, imageMode);

        MainWindow window;
        auto *view = window.findChild<ImageViewer *>();
        QVERIFY(view);
        view->setFixedSize(640, 480);

        QPixmap image(300, 180);
        image.fill(Qt::red);
        image.setDevicePixelRatio(imageDpr);
        QPixmap other(400, 240);
        other.fill(Qt::blue);
        other.setDevicePixelRatio(3.0);

        if (imageMode == "single") {
            view->displayImages(std::make_shared<ImageHolder>(image, "single.png"));
        } else if (imageMode == "first") {
            view->displayImages(std::make_shared<ImageHolder>(image, "first.png", other, "second.png"));
        } else {
            view->displayImages(std::make_shared<ImageHolder>(other, "first.png", image, "second.png"));
        }
        QCoreApplication::processEvents();
        if (imageMode == "second") {
            view->showSecondImage();
        } else if (imageMode == "comparison") {
            view->showImageFromComparator(image, "comparison");
        }

        // Actual Size must replace a previous zoom and remain idempotent.
        view->setTransform(QTransform::fromScale(1.7, 2.3));
        for (int i = 0; i < 2; ++i) {
            view->setToActualSize();
            int visibleImages = 0;
            for (auto *item : view->scene()->items()) {
                auto *pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem *>(item);
                if (!pixmapItem || !pixmapItem->isVisible()) {
                    continue;
                }
                ++visibleImages;
                const QRectF displayedRect = pixmapItem->deviceTransform(view->viewportTransform())
                                                 .mapRect(pixmapItem->boundingRect());
                const QSizeF physicalSize = displayedRect.size() * view->viewport()->devicePixelRatioF();
                QVERIFY(qAbs(physicalSize.width() - image.width()) < 0.001);
                QVERIFY(qAbs(physicalSize.height() - image.height()) < 0.001);
            }
            QCOMPARE(visibleImages, 1);
        }
    }

    void actualSizeWithoutImagePreservesTransform() {
        MainWindow window;
        auto *view = window.findChild<ImageViewer *>();
        QVERIFY(view);
        const QTransform original = QTransform::fromScale(1.7, 1.7);
        view->setTransform(original);
        view->setToActualSize();
        QCOMPARE(view->transform(), original);
    }

    void switchingPreservesViewport_data() {
        QTest::addColumn<QSize>("viewportSize");
        QTest::addColumn<int>("frameWidth");
        QTest::addColumn<double>("zoom");
        QTest::addColumn<bool>("fromComparator");

        for (const QSize size : {QSize(640, 480), QSize(641, 481)}) {
            for (int frame : {1, 4}) {
                for (double zoom : {0.5, 1.0, 1.25, 2.0}) {
                    for (bool comparator : {false, true}) {
                        const QByteArray name = QString("%1x%2-frame%3-zoom%4-comparator%5")
                            .arg(size.width()).arg(size.height()).arg(frame)
                            .arg(zoom).arg(comparator).toLatin1();
                        QTest::newRow(name.constData()) << size << frame << zoom << comparator;
                    }
                }
            }
        }
    }

    void switchingPreservesViewport() {
        QFETCH(QSize, viewportSize);
        QFETCH(int, frameWidth);
        QFETCH(double, zoom);
        QFETCH(bool, fromComparator);

        MainWindow window;
        auto *view = window.findChild<ImageViewer *>();
        QVERIFY(view);
        view->setFrameStyle(QFrame::Box | QFrame::Plain);
        view->setLineWidth(frameWidth);
        view->setFixedSize(viewportSize + QSize(2 * frameWidth, 2 * frameWidth));

        QPixmap first(2048, 1536);
        QPixmap second(first.size());
        first.fill(Qt::red);
        second.fill(Qt::blue);
        view->displayImages(std::make_shared<ImageHolder>(first, "first.png", second, "second.png"));
        QCoreApplication::processEvents(); // Complete the deferred initial fit.
        QCOMPARE(view->viewport()->size(), viewportSize);

        view->setTransform(QTransform::fromScale(zoom, zoom));
        view->centerOn(QPointF(1100.25, 850.75));
        if (fromComparator) {
            view->showImageFromComparator(first, "comparison");
        }
        const QTransform originalMapping = view->viewportTransform();

        // Compare after every switch: a forward/backward round trip alone
        // could hide alternating offsets or drift masked by scrollbar limits.
        for (int i = 0; i < 100; ++i) {
            view->showSecondImage();
            QCOMPARE(view->getImageShowedOnTheScreen().mSaveImageInfoType, SaveImageInfoType::SecondImage);
            QCOMPARE(view->viewportTransform(), originalMapping);
            view->showFirstImage();
            QCOMPARE(view->getImageShowedOnTheScreen().mSaveImageInfoType, SaveImageInfoType::FirstImage);
            QCOMPARE(view->viewportTransform(), originalMapping);
            view->toggleImage();
            QCOMPARE(view->getImageShowedOnTheScreen().mSaveImageInfoType, SaveImageInfoType::SecondImage);
            QCOMPARE(view->viewportTransform(), originalMapping);
            view->toggleImage();
            QCOMPARE(view->viewportTransform(), originalMapping);
        }
    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QTemporaryDir settings;
    if (!settings.isValid()) {
        return 1;
    }
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, settings.path());
    QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, settings.path());
    TestImageViewerNavigation test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_imageviewernavigation.moc"
