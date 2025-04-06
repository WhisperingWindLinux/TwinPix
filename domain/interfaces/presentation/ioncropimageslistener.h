#ifndef IONCROPIMAGESLISTENER_H
#define IONCROPIMAGESLISTENER_H

#include "domain/valueobjects/images.h"

class OnCropImageListener {
public:
    virtual void onImagesCropped(ImagesPtr images) = 0;
};

#endif // IONCROPIMAGESLISTENER_H
