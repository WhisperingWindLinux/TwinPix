#include "htmlreportpresenter.h"

#include <QFile>
#include <QTextStream>
#include <qdir.h>

#include <presentation/dialogs/formatters/helphtmlformatter.h>
#include <business/validation/imagevalidationrulesfactory.h>

namespace {

QString loadTemplate(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    return QString::fromUtf8(file.readAll());
}

}

bool HtmlReportPresenter::createExtendedReportPage(const QString &folderPath,
                                                   const ComparableImage &firstOriginalImage,
                                                   const ComparableImage &secondOriginalImage,
                                                   QList<AutocomparisonReportEntry> reportEntries
                                                   )
{
    // Create the main folder
    QDir dir(folderPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // Create the images subfolder
    QString imagesFolderPath = folderPath + "/images";
    QDir imagesDir(imagesFolderPath);
    if (!imagesDir.exists()) {
        imagesDir.mkpath(".");
    }

    auto provider = ImageValidationRulesFactory::createImageExtensionsInfoProvider();

    QImage firstOrigImage = firstOriginalImage.getImage();
    QImage secondOrigImage = secondOriginalImage.getImage();

    QString firtsOrigImageName = firstOriginalImage.getImageName() +
                                 provider->getDeafaultSaveExtension(true);

    QString secondOrigImageName = secondOriginalImage.getImageName() +
                                  provider->getDeafaultSaveExtension(true);;

    // Save the two main images
    firstOrigImage.save(imagesFolderPath + "/" + firtsOrigImageName);
    secondOrigImage.save(imagesFolderPath + "/" + secondOrigImageName);

    // Create the HTML report file
    QFile reportFile(folderPath + "/report.html");
    if (!reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false; // Exit if file couldn't be opened
    }

    QTextStream out(&reportFile);

    QString report = loadTemplate(":/html/reports/extended-report.html");
    report.replace("%FIRST_IMAGE%", firtsOrigImageName);
    report.replace("%SECOND_IMAGE%", secondOrigImageName);

    QString comparisonSections;

    for (int i = 0; i < reportEntries.size(); i++) {
        const auto &reportEntry =reportEntries[i];
        auto textReport = reportEntry.getTextReport();
        auto imageReport = reportEntry.getImagereport();
        auto processorInfo = reportEntry.getImageProcessorInfo();
        if (!processorInfo) {
            continue;
        }
        if (imageReport) {
            comparisonSections += R"(
            <div class="additional-section"><center><p><h2>)" + processorInfo.value().fullName + R"(</h2></p>)";
            QString additionalImagePath = imagesFolderPath +
                                          QDir::separator() +
                                          processorInfo.value().name + ".png";

            imageReport->save(additionalImagePath);
            comparisonSections += R"(<a href="images/)" + processorInfo.value().name + ".png" +
                                  R"("><img src="images/)" + processorInfo.value().name + ".png" +
                                  R"(" alt=")" + processorInfo.value().name + R"("></a></center><br/>)";
        }
        else if (textReport){
            comparisonSections += R"(<div class="additional-section"><p>)" + textReport.value() + R"(</p>)";
        } else {
            comparisonSections += R"(<div class="additional-section"><p><b><font color="red">Error! The comparator does not provide a comparison result!</font></b></p>)";
        }
        comparisonSections += "</div></dev><br/><br/>";
    }

    report.replace("%COMPARISON_SECTIONS%", comparisonSections);

    QString referenceSections;

    for (int i = 0; i < reportEntries.size(); i++) {
        const auto &reportEntry =reportEntries[i];
        auto processorInfo = reportEntry.getImageProcessorInfo();
        if (!processorInfo) {
            continue;
        }
        ImageProcessorInfo& info = processorInfo.value();

        referenceSections += R"(<div class="additional-section"><p><h2>)" +
                             processorInfo.value().fullName + R"(</h2></p>)";
        referenceSections += R"(<p style="text-align: left;">)" +
                             HelpHtmlFormatter::formatImageProcessorInfo(info) + R"(</p><br/>)";
     }
     report.replace("%REFERENCE_SECTIONS%", referenceSections + R"(</div>)");
     out << report;

    return true;
}

bool HtmlReportPresenter::createSimpleReportPage(const QString &filePath,
                                                 const QString &firstOriginalImageName,
                                                 const QString &secondOriginalImageName,
                                                 const QString &reportText
                                                )
{
    // Create the HTML report file
    QFile reportFile(filePath);
    if (!reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false; // Exit if file couldn't be opened
    }

    QTextStream out(&reportFile);

    QString report = loadTemplate(":/html/reports/simple-report.html");
    report.replace("%FIRST_IMAGE%", QFileInfo(firstOriginalImageName).fileName());
    report.replace("%SECOND_IMAGE%", QFileInfo(secondOriginalImageName).fileName());
    if (!reportText.isEmpty()){
        report.replace("%REPORT_CONTENT%", reportText);
    } else {
        report.replace("%REPORT_CONTENT%", "<b><font color=\"red\">Error! The comparator does not provide a comparison result!</font></b>");
    }
    out << report;
    return true;
}
