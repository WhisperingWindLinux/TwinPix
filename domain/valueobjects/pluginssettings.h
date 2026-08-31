#ifndef PLUGINSSETTINGS_H
#define PLUGINSSETTINGS_H

#include <QString>

struct PluginsSettings {
    // A single constructor handling both lvalues and rvalues efficiently via Qt's implicit sharing
    PluginsSettings(QString pythonPath, QString pluginsPath)
        : pythonInterpreterPath(std::move(pythonPath).trimmed()),
        pluginsDirectoryPath(std::move(pluginsPath).trimmed()) {}

    const QString pythonInterpreterPath;
    const QString pluginsDirectoryPath;
};

#endif // PLUGINSSETTINGS_H
