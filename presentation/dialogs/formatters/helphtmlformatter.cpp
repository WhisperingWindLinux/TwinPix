#include "helphtmlformatter.h"

#include <QFile>

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

QString HelpHtmlFormatter::formatImageProcessorInfo(const ImageProcessorInfo &info)
{
    QString html = loadTemplate(":/html/help/processor-info.html");
    html.replace("%TYPE%", enumToString(info.type));
    html.replace("%FULL_NAME%", info.fullName);
    html.replace("%NAME%", info.name);
    html.replace("%HOTKEY%", info.hotkey);
    html.replace("%DESCRIPTION%", info.description);

    if (!info.properties.isEmpty()) {
        QString properties = loadTemplate(":/html/help/properties.html");
        QString rows;
        for (const Property &property : info.properties) {
            QString row = loadTemplate(":/html/help/property-row.html");
            row.replace("%PROPERTY_NAME%", property.mPropertyName);
            row.replace("%PROPERTY_DESCRIPTION%", formatProperty(property));
            rows += row;
        }
        properties.replace("%PROPERTY_ROWS%", rows);
        html.replace("%PROPERTIES%", properties);
    } else {
        html.replace("%PROPERTIES%", QString{});
    }

    return html;
}

QString HelpHtmlFormatter::imageAreaSelectionHelp()
{
    return loadTemplate(":/html/help/image-area-selection.html");
}

QString HelpHtmlFormatter::aboutApplicationName()
{
    return loadTemplate(":/html/about/application-name.html");
}

QString HelpHtmlFormatter::aboutAlphaWarning()
{
    return loadTemplate(":/html/about/alpha-warning.html");
}

QString HelpHtmlFormatter::aboutBugWarning()
{
    return loadTemplate(":/html/about/bug-warning.html");
}

QString HelpHtmlFormatter::enumToString(ImageProcessorType type)
{
    switch (type) {
    case ImageProcessorType::Comparator:
        return "Comparator";
    case ImageProcessorType::Filter:
        return "Filter";
    default:
        return "Unknown";
    }
}

QString HelpHtmlFormatter::formatProperty(const Property &property)
{
    QString result;

    switch (property.mPropertyType) {
    case Property::Type::Integer:
        result = loadTemplate(":/html/help/property.html");
        result.replace("%TYPE%", "Integer");
        result.replace("%DESCRIPTION%", property.mPropertyDescription);
        result.replace("%DEFAULT%", QString::number(static_cast<int>(property.mDoubleValue)));
        result.replace("%MAX%", QString::number(static_cast<int>(property.mMax)));
        result.replace("%MIN%", QString::number(static_cast<int>(property.mMin)));
        break;
    case Property::Type::Real:
        result = loadTemplate(":/html/help/property.html");
        result.replace("%TYPE%", "Real");
        result.replace("%DESCRIPTION%", property.mPropertyDescription);
        result.replace("%DEFAULT%", QString::number(property.mDoubleValue));
        result.replace("%MAX%", QString::number(property.mMax));
        result.replace("%MIN%", QString::number(property.mMin));
        break;
    case Property::Type::Alternatives:
        result = loadTemplate(":/html/help/alternatives-property.html");
        result.replace("%DESCRIPTION%", property.mPropertyDescription);
        result.replace("%ALTERNATIVES%", property.mAlternativesValue.join(", "));
        result.replace("%DEFAULT%", property.mAlternativesValue.value(static_cast<int>(property.mDoubleValue)));
        break;
    case Property::Type::FilePath:
        result = loadTemplate(":/html/help/file-path-property.html");
        result.replace("%DESCRIPTION%", property.mPropertyDescription);
        break;
    }

    return result;
}
