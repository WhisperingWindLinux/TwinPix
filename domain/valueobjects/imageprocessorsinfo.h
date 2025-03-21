#ifndef IMAGEPROCESSORSINFO_H
#define IMAGEPROCESSORSINFO_H

#include "domain/interfaces/imageprocessor.h"
#include <qstring.h>

struct ImageProcessorInfo {
  ImageProcessorInfo(const QString &name,
                       const QString &description,
                       const QString &hotkey,
                       ImageProcessorType type
                       )
      : name(name), description(description),
        hotkey(hotkey), type(type) {}
  QString name;
  QString description;
  QString hotkey;
  ImageProcessorType type;
};


#endif // IMAGEPROCESSORSINFO_H
