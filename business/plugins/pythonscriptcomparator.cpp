#include "pythonscriptcomparator.h"

#include <QBuffer>
#include <QtCore/qregularexpression.h>
#include <QtGui/qpixmap.h>
#include <qprocess.h>
#include <QtCore/qdebug.h>
#include <business/pluginsettingsinteractor.h>
#include <business/validation/imagevalidationrulesfactory.h>


PythonScriptComparator::PythonScriptComparator(const QString &pyScriptPath,
                                               const QString &shortName,
                                               const QString &hotkey,
                                               const QString &description,
                                               const QList<Property> &properties,
                                               const QString &fullName,
                                               bool isPartOfAutoReportingToolbox
                                                )
    : mShortName(shortName),
    mHotkey(hotkey),
    mDescription(description),
    mProperties(properties),
    mFullName(fullName),
    misPartOfAutoReportingToolbox(isPartOfAutoReportingToolbox),
    mPyScriptPath(pyScriptPath)
{
    auto validationRules = ImageValidationRulesFactory::createImageExtensionsInfoProvider();
    QString ext = validationRules->getDeafaultSaveExtension(false);
    mDefaultSaveImageExtention = ext.toUpper().toStdString();
}

QString PythonScriptComparator::getShortName() const {
    return mShortName;
}

QString PythonScriptComparator::getHotkey() const {
    return mHotkey;
}

QString PythonScriptComparator::getDescription() const {
    return mDescription;
}

QList<Property> PythonScriptComparator::getDefaultProperties() const {
    return mProperties;
}

void PythonScriptComparator::setProperties(QList<Property> properties) {
    this->mProperties = properties;
}

QString PythonScriptComparator::getFullName() const {
    return mFullName;
}

bool PythonScriptComparator::isPartOfAutoReportingToolbox() {
    return misPartOfAutoReportingToolbox;
}

optional<QString> PythonScriptComparator::validateText(QString &text) {
    QString validatedText = text.mid(0, qMin(mCharsInReportMax, text.size()));
    foreach (auto ch, validatedText) {
        if (!ch.isPrint()) {
            return nullopt;
        }
    }
    return make_optional(validatedText);
}


shared_ptr<ComparisonResultVariant> PythonScriptComparator::compare(const ComparableImage &first,
                                                                    const ComparableImage &second
                                                                    )
{
    QByteArray image1Data;
    QByteArray image2Data;

    QBuffer buffer1(&image1Data);
    QBuffer buffer2(&image2Data);

    buffer1.open(QIODevice::WriteOnly);
    buffer2.open(QIODevice::WriteOnly);

    if (!first.getImage().save(&buffer1, mDefaultSaveImageExtention.c_str()) ||
        !second.getImage().save(&buffer2, mDefaultSaveImageExtention.c_str())) {
        throw runtime_error("Failed to encode images to bytes array.");
    }

    auto pluginSettings = PluginsSettingsInteractor().getPluginSettings();
    if (pluginSettings.pythonInterpreterPath.isEmpty()) {
        throw runtime_error("Bad python interpreter.");
    }

    QStringList params;
    params << mPyScriptPath << first.getPath() << second.getPath();

    foreach (auto property, mProperties) {
        params << property.getAnyValueAsString();
    }

    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("Runner", "TwinPix");
    env.remove("DYLD_LIBRARY_PATH");
    env.remove("DYLD_FRAMEWORK_PATH");
    env.remove("DYLD_INSERT_LIBRARIES");
    process.setProcessEnvironment(env);

    process.start(pluginSettings.pythonInterpreterPath, params);

    if (!process.waitForStarted()) {
        throw runtime_error("Failed to start process.");
    }

    QByteArray inputData;

    int size1 = image1Data.size();
    inputData.append(QByteArray::fromRawData(
        reinterpret_cast<const char *>(&size1), sizeof(size1)));
    inputData.append(image1Data);

    int size2 = image2Data.size();
    inputData.append(QByteArray::fromRawData(
        reinterpret_cast<const char *>(&size2), sizeof(size2)));
    inputData.append(image2Data);
    process.write(inputData);
    process.closeWriteChannel();

    if (!process.waitForFinished()) {
        auto errorMsg = QString("Process error: ") + process.errorString();
        throw runtime_error(errorMsg.toStdString());
    }

    QByteArray output = process.readAllStandardOutput();
    QImage resultImage;
    if (resultImage.loadFromData(output, mDefaultSaveImageExtention.c_str()) &&
        !resultImage.isNull()) {
        return make_shared<ComparisonResultVariant>(resultImage);
    }

    QString text = QString::fromUtf8(output);
    auto validText = validateText(text);
    if (validText) {
        return make_shared<ComparisonResultVariant>(validText.value());
    }

    throw runtime_error("Error! The script returned '" +
                        process.readAllStandardError() + "'");
}
