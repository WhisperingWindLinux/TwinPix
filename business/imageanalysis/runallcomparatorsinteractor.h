#ifndef RUNALLCOMPARATORSINTERACTOR_H
#define RUNALLCOMPARATORSINTERACTOR_H

#include <QObject>
#include <QList>
#include <QString>
#include <atomic>
#include <optional>
#include <vector>

#include <domain/valueobjects/autocomparisonreportentry.h>
#include <domain/valueobjects/comparableimage.h>
#include <domain/valueobjects/comparisonresultvariant.h>
#include <domain/valueobjects/images.h>

#include "business/utils/threadpool.h"

class IProgressDialog;

class RunAllComparatorsInteractor : public QObject
{
    Q_OBJECT
public:
    RunAllComparatorsInteractor(IProgressDialog *callback,
                                const ComparableImage &firstImage,
                                const ComparableImage &secondImage,
                                const QString &reportDirPath,
                                QObject *parent = nullptr);

    void run();

signals:
    void progressStep(int done, int total);
    void allComparatorsFinished();

private:
    IProgressDialog *mCallback;
    ComparableImage mFirstImage;
    ComparableImage mSecondImage;
    QString mReportDirPath;
    ThreadPool mPool;

    std::vector<std::optional<AutocomparisonReportEntry>> mSlots;
    std::atomic<int> mCompletedCount{0};
    std::atomic<bool> mCancelRequested{false};

    void generateReports(QVector<AutocomparisonReportEntry> &entries);

private slots:
    void onProgressStep(int done, int total);
    void finalizeAndGenerateReport();
};

#endif // RUNALLCOMPARATORSINTERACTOR_H