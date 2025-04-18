#ifndef COLORPICKERCONTROLLER_H
#define COLORPICKERCONTROLLER_H

#include <QDockWidget>
#include <QObject>
#include <QWidget>

#include <domain/interfaces/presentation/icolorpickercontroller.h>

class ColorPickerPanel;
class MainWindow;

class ColorPickerController : public QDockWidget, public IColorPickerController
{
    Q_OBJECT

public slots:
    void onDockLocationChanged(Qt::DockWidgetArea);

public:
    ColorPickerController(MainWindow *mainWindow);

    virtual ~ColorPickerController();

    void openColorPickerDialog() override;
    void placeColorPickerToRightSideOfMainWindow() override;
    void placeColorPickerToLeftSideOfMainWindow() override;

    void onImagesClosed() override;
    void onImagesOpened() override;

    void onColorUnderCursorChanged(const ImagePixelColor &visibleImageRgbValue,
                                   const std::optional<ImagePixelColor> &hiddenImageRgbValue) override;

private:
    MainWindow *mMainWindow;
    ColorPickerPanel *mColorPicker;

    void createColorPicker();
};

#endif // COLORPICKERCONTROLLER_H
