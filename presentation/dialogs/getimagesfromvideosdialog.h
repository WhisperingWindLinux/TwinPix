#ifndef GETIMAGESFROMVIDEOSDIALOG_H
#define GETIMAGESFROMVIDEOSDIALOG_H

#include <presentation/views/videoplayerwidget.h>

#include <qdialog.h>
#include <qevent.h>


class GetImagesFromVideosDialog : public QDialog {
    Q_OBJECT

public:
    GetImagesFromVideosDialog(QWidget *parent,
                              const QString &videoFilePath1,
                              const QString &videoFilePath2
                              );

    std::optional<QString> getFirstScreenshotPath();

    std::optional<QString> getSecondScreenshotPath();

    bool isCanceled();

    void showError(const QString &errorMessage);

private slots:
    void handleScreenshotTaken();

private:
    QString mVideoFilePath1;
    QString mVideoFilePath2;
    VideoPlayerWidget *mPlayer1;
    VideoPlayerWidget *mPlayer2;
    int mTotalScreenshotsTaken;
};
#endif // GETIMAGESFROMVIDEOSDIALOG_H
