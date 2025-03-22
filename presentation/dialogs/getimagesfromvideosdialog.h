#ifndef GETIMAGESFROMVIDEOSDIALOG_H
#define GETIMAGESFROMVIDEOSDIALOG_H

#include <presentation/views/videoplayerwidget.h>

#include <qdialog.h>
#include <qevent.h>


class GrabImagesFromVideosDialog : public QDialog {
    Q_OBJECT

public:
    GrabImagesFromVideosDialog(QWidget *parent, const QString &videoFilePath1, const QString &videoFilePath2);

    QString getFirstScreenshotPath();

    QString getSecondScreenshotPath();

    bool isCanceled();

private slots:
    void handleScreenshotTaken();

private:
    QString videoFilePath1;
    QString videoFilePath2;
    VideoPlayerWidget *player1;
    VideoPlayerWidget *player2;
    int totalScreenshotsTaken;
};
#endif // GETIMAGESFROMVIDEOSDIALOG_H
