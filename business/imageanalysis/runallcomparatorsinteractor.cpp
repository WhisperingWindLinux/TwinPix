#include "runallcomparatorsinteractor.h"

#include <qdir.h>
#include <qtextdocument.h>
#include <QtCore/qdebug.h>
#include <QtGui/qpainter.h>
#include <QDesktopServices>

#include <domain/interfaces/presentation/iprogressdialog.h>
#include <business/utils/imagesinfo.h>
#include <business/imageanalysis/comporators/formatters/htmlreportpresenter.h>

#include "imageprocessorsmanager.h"

RunAllComparatorsInteractor::RunAllComparatorsInteractor(IProgressDialog *callback,
                                                         const ComparableImage &firstImage,
                                                         const ComparableImage &secondImage,
                                                         const QString &reportDirPath,
                                                         QObject *parent)
    : QObject(parent),
    mCallback(callback),
    mFirstImage(firstImage),
    mSecondImage(secondImage),
    mReportDirPath(reportDirPath)
{
    connect(this, &RunAllComparatorsInteractor::progressStep,
            this, &RunAllComparatorsInteractor::onProgressStep,
            Qt::AutoConnection);

    connect(this, &RunAllComparatorsInteractor::allComparatorsFinished,
            this, &RunAllComparatorsInteractor::finalizeAndGenerateReport,
            Qt::AutoConnection);
}

void RunAllComparatorsInteractor::run()
{
    auto manager = ImageProcessorsManager::instance();
    auto comparators = manager->getAllComparators();

    if (comparators.size() == 0) {
        return;
    }

    mCallback->showProgressDialog("Run All Comparators", comparators.size());

    QVector<shared_ptr<IComparator>> runnable;
    runnable.reserve(comparators.size());
    foreach (auto comparator, comparators) {
        if (!comparator->isPartOfAutoReportingToolbox()) continue;
        if (!comparator->isEnabled()) continue;
        runnable.append(comparator);
    }

    if (runnable.isEmpty()) {
        mCallback->onUpdateProgressValue(INT32_MAX);
        return;
    }

    mSlots.clear();
    mSlots.reserve(runnable.size());
    mCompletedCount = 0;
    mCancelRequested = false;

    const int total = runnable.size();

    for (int i = 0; i < total; ++i) {
        auto comparator = runnable[i];

        mPool.submit(
            [this, i, comparator, manager] {
                if (mCancelRequested.load(std::memory_order_relaxed)) {
                    return;
                }
                try {
                    comparator->reset();
                    auto result = comparator->compare(mFirstImage, mSecondImage);
                    if (result != nullptr) {
                        auto processorInfo = manager->getProcessorInfoByProcessorShortName(
                            comparator->getShortName());
                        mSlots[i] = AutocomparisonReportEntry{ result, processorInfo };
                    }
                } catch (std::runtime_error &e) {
                    qDebug() << e.what();
                }
            },
            [this, total] {
                int done = mCompletedCount.fetch_add(1, std::memory_order_relaxed) + 1;
                emit progressStep(done, total);
                if (done == total) {
                    emit allComparatorsFinished();
                }
            }
            );
    }

    return;
}

void RunAllComparatorsInteractor::onProgressStep(int done, int /*total*/)
{
    mCallback->onUpdateProgressValue(done);

    if (mCallback->wasCanceled()) {
        mCancelRequested.store(true, std::memory_order_relaxed);
    }
}

void RunAllComparatorsInteractor::finalizeAndGenerateReport()
{
    mCallback->onUpdateProgressValue(INT32_MAX);

    if (mCancelRequested.load(std::memory_order_relaxed)) {
        return;
    }

    QVector<AutocomparisonReportEntry> entries;
    entries.reserve(static_cast<int>(mSlots.size()));
    for (auto &slot : mSlots) {
        if (slot.has_value()) {
            entries.append(*slot);
        }
    }

    if (!entries.isEmpty()) {
        generateReports(entries);
    }
}

void RunAllComparatorsInteractor::generateReports(QVector<AutocomparisonReportEntry> &entries)
{
    bool isOk = HtmlReportPresenter::createExtendedReportPage(mReportDirPath,
                                                              mFirstImage,
                                                              mSecondImage,
                                                              entries
                                                              );
    if (isOk) {
        mCallback->onMessage("The report saved to " + mReportDirPath + ".");
        QDesktopServices::openUrl("file://" + mReportDirPath + QDir::separator() + "report.html");
    } else {
        mCallback->onError("Unable to generate the report.");
    }
}