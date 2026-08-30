#include <presentation/mainwindow.h>

#include <QApplication>
#include <QFileOpenEvent>
#include <qthread.h>


class MyApplication : public QApplication {
public:
    MyApplication(int &argc, char **argv) : QApplication(argc, argv) {}

    void showMainWindow() {

        w.setWindowTitle("TwinPix");
        w.show();

        QStringList arguments = QCoreApplication::arguments();

        if (arguments.size() == 2) {
            QString filePath = arguments.at(1);
            w.openImageFromCommandLine(filePath);
        } else if (arguments.size() == 3) {
            QString firstFilePath = arguments.at(1);
            QString secondFilePath = arguments.at(2);
            w.openImagesFromCommandLine(firstFilePath, secondFilePath);
        }
    }

protected:
    bool event(QEvent *event) override {
        if (event->type() == QEvent::FileOpen) {
            QFileOpenEvent *fileOpenEvent = static_cast<QFileOpenEvent *>(event);
            QString filePath = fileOpenEvent->file();
            w.openImageFromCommandLine(filePath);
            return true;
        }
        return QApplication::event(event);
    }

private:
    MainWindow w;
};


int main(int argc, char *argv[]) {

/*#ifndef TARGET_OS_MAC
    #error The application was developed and tested for functionality only on macOS.
    #error Functionality or even the possibility of building it for other operating systems is not guaranteed.
    #error If you're feeling lucky, you can try to build and run the application on your chosen operating system.
#endif*/

    MyApplication a(argc, argv);
    a.showMainWindow();
    return a.exec();
}
