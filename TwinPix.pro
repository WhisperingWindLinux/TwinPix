QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    business/getimagesfromvideosinteractor.cpp \
    business/imageanalysis/autoanalysissettingsinteractor.cpp \
    business/imageanalysis/comporators/coloreddifferenceinpixelvaluescomporator.cpp \
    business/imageanalysis/comporators/colorssaturationcomporator.cpp \
    business/imageanalysis/comporators/contrastcomporator.cpp \
    business/imageanalysis/comporators/customrangeddifferenceinpixelvaluescomparator.cpp \
    business/imageanalysis/comporators/formatters/pixelsabsolutevalueformatter.cpp \
    business/imageanalysis/comporators/helpers/mathhelper.cpp \
    business/imageanalysis/comporators/helpers/pixelsasolutvaluehelper.cpp \
    business/imageanalysis/comporators/imageproximitytoorigincomparator.cpp \
    business/imageanalysis/comporators/linernonlinerdifferencecomparator.cpp \
    business/imageanalysis/comporators/monocoloreddifferenceinpixelvaluescomporator.cpp \
    business/imageanalysis/comporators/pixelsbrightnesscomparator.cpp \
    business/imageanalysis/comporators/sharpnesscomparator.cpp \
    business/imageanalysis/filters/grayscalefilter.cpp \
    business/imageanalysis/filters/rgbfilter.cpp \
    business/imageanalysis/imageprocessinginteractor.cpp \
    business/imageanalysis/imageprocessorsmanager.cpp \
    business/utils/imagesinfo.cpp \
    business/validation/imageextensionsinfoprovider.cpp \
    business/validation/imagevalidationrules.cpp \
    business/imagefilesinteractors.cpp \
    business/otherappinstancesinteractor.cpp \
    business/plugins/imageprocessordeserializer.cpp \
    business/plugins/pluginsmanager.cpp \
    business/imageanalysis/runallcomparatorsinteractor.cpp \
    business/plugins/pythonscriptcomparator.cpp \
    business/plugins/pythonscriptfilter.cpp \
    business/pluginsettingsinteractor.cpp \
    business/recentfilesmanager.cpp \
    business/recentfilesinteractor.cpp \
    data/storage/filedialoghandler.cpp \
    data/storage/imagefileshandler.cpp \
    data/repositories/pluginsrepository.cpp \
    domain/interfaces/business/icomparator.cpp \
    domain/interfaces/business/ifilter.cpp \
    domain/interfaces/business/imageprocessor.cpp \
    domain/valueobjects/comparableimage.cpp \
    domain/valueobjects/comparisonresultvariant.cpp \
    domain/valueobjects/images.cpp \
    domain/valueobjects/property.cpp \
    domain/valueobjects/recentfilesrecord.cpp \
    main.cpp \
    presentation/colorpickercontroller.cpp \
    presentation/dialogs/aboutdialog.cpp \
    presentation/dialogs/colorpickerpanel.cpp \
    presentation/dialogs/comparatorresultdialog.cpp \
    presentation/dialogs/externalimageviewerdialog.cpp \
    presentation/dialogs/formatters/helphtmlformatter.cpp \
    presentation/dialogs/getimagesfromvideosdialog.cpp \
    presentation/dialogs/helpdialog.cpp \
    presentation/dialogs/imageautoanalysissettingsdialog.cpp \
    presentation/dialogs/pluginssettingsdialog.cpp \
    presentation/dialogs/propertyeditordialog.cpp \
    presentation/imageprocessorsmenucontroller.cpp \
    presentation/mainwindow.cpp \
    business/imageanalysis/comporators/formatters/htmlreportpresenter.cpp \
    presentation/views/externalgraphicsview.cpp \
    presentation/views/graphicspixmapitem.cpp \
    presentation/views/imageviewer.cpp \
    presentation/views/videodialogslider.cpp \
    presentation/views/videoplayerwidget.cpp \
    domain/valueobjects/autocomparisonreportentry.cpp

HEADERS += \
    business/getimagesfromvideosinteractor.h \
    business/imageanalysis/autoanalysissettingsinteractor.h \
    business/imageanalysis/comporators/coloreddifferenceInpixelvaluescomporator.h \
    business/imageanalysis/comporators/colorssaturationcomporator.h \
    business/imageanalysis/comporators/contrastcomporator.h \
    business/imageanalysis/comporators/customrangeddifferenceinpixelvaluescomparator.h \
    business/imageanalysis/comporators/formatters/pixelsabsolutevalueformatter.h \
    business/imageanalysis/comporators/helpers/mathhelper.h \
    business/imageanalysis/comporators/helpers/pixelsasolutvaluehelper.h \
    business/imageanalysis/comporators/imageproximitytoorigincomparator.h \
    business/imageanalysis/comporators/linernonlinerdifferencecomparator.h \
    business/imageanalysis/comporators/monocoloreddifferenceinpixelvaluescomporator.h \
    business/imageanalysis/comporators/pixelsbrightnesscomparator.h \
    business/imageanalysis/comporators/sharpnesscomparator.h \
    business/imageanalysis/filters/grayscalefilter.h \
    business/imageanalysis/filters/rgbfilter.h \
    business/imageanalysis/imageprocessinginteractor.h \
    business/imageanalysis/imageprocessorsmanager.h \
    business/validation/imageextensionsinfoprovider.h \
    business/validation/imagevalidationrules.h \
    business/validation/imagevalidationrulesfactory.h \
    business/validation/interfaces/iimageextensionsinfoprovider.h \
    business/validation/interfaces/iimagevalidationrules.h \
    business/imagefilesinteractors.h \
    business/otherappinstancesinteractor.h \
    business/plugins/imageprocessordeserializer.h \
    business/plugins/pluginsmanager.h \
    business/imageanalysis/runallcomparatorsinteractor.h \
    business/plugins/pythonscriptcomparator.h \
    business/plugins/pythonscriptfilter.h \
    business/pluginsettingsinteractor.h \
    business/recentfilesmanager.h \
    business/recentfilesinteractor.h \
    business/utils/imagesinfo.h \
    data/storage/filedialoghandler.h \
    data/storage/imagefileshandler.h \
    data/repositories/pluginsrepository.h \
    data/storage/stb_image.h \
    domain/interfaces/business/irecentfilesmanager.h \
    domain/interfaces/presentation/icolorpickercontroller.h \
    domain/interfaces/presentation/icolorundercursorchangelistener.h \
    domain/interfaces/business/icomparator.h \
    domain/interfaces/presentation/idroptarget.h \
    domain/interfaces/business/ifilter.h \
    domain/interfaces/presentation/imagefilesinteractorlistener.h \
    domain/interfaces/presentation/imageprocessinginteractorlistener.h \
    domain/interfaces/business/imageprocessor.h \
    domain/interfaces/presentation/ioncropimageslistener.h \
    domain/interfaces/presentation/iotherappinstancesinteractorcallback.h \
    domain/interfaces/presentation/iprocessorpropertiesdialogcallback.h \
    domain/interfaces/presentation/iprogressdialog.h \
    domain/valueobjects/autocomparisonreportentry.h \
    domain/valueobjects/comparableimage.h \
    domain/valueobjects/comparisonresultvariant.h \
    domain/valueobjects/imagepixelcolor.h \
    domain/valueobjects/imageprocessorsinfo.h \
    domain/valueobjects/images.h \
    domain/valueobjects/lastdisplayedcomparisonresult.h \
    domain/valueobjects/pixeldiffrencerange.h \
    domain/valueobjects/pluginssettings.h \
    domain/valueobjects/property.h \
    domain/valueobjects/pyscriptinfo.h \
    domain/valueobjects/recentfilesrecord.h \
    domain/valueobjects/savefileinfo.h \
    presentation/colorpickercontroller.h \
    presentation/dialogs/aboutdialog.h \
    presentation/dialogs/colorpickerpanel.h \
    presentation/dialogs/comparatorresultdialog.h \
    presentation/dialogs/externalimageviewerdialog.h \
    presentation/dialogs/formatters/helphtmlformatter.h \
    presentation/dialogs/getimagesfromvideosdialog.h \
    presentation/dialogs/helpdialog.h \
    presentation/dialogs/imageautoanalysissettingsdialog.h \
    presentation/dialogs/pluginssettingsdialog.h \
    presentation/dialogs/propertyeditordialog.h \
    presentation/imageprocessorsmenucontroller.h \
    presentation/mainwindow.h \
    business/imageanalysis/comporators/formatters/htmlreportpresenter.h \
    presentation/valueobjects/rgbwidgets.h \
    presentation/views/externalgraphicsview.h \
    presentation/views/graphicspixmapitem.h \
    presentation/views/imageviewer.h \
    presentation/views/videodialogslider.h \
    presentation/views/videoplayerwidget.h

FORMS += \
    presentation/forms/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ICON = Resources/app_icon.icns

DISTFILES += \
    business/plugins/comparator_example.json \
    business/plugins/filter_example.json

QMAKE_INFO_PLIST = Info.plist
