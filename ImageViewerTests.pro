# Build in a separate directory with qmake ../ImageViewerTests.pro && make.
# Run with QT_QPA_PLATFORM=offscreen ./ImageViewerTests.
include(TwinPix.pro)

QT += testlib
CONFIG += console testcase
CONFIG -= app_bundle
TARGET = ImageViewerTests

SOURCES -= main.cpp
SOURCES += Tests/tst_imageviewernavigation.cpp
