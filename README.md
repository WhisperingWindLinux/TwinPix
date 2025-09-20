# TwinPix

### Warning

This is a alpha version of the application, and bugs and memory leaks are possible.
The application has so far been tested only on macOS Sonoma 14.1.2 (Apple Silicon M1 Mac), with a 1920x1080 monitor. Kit: Qt 6.9.0 for macOS.
The app’s still in development, but the core features are already in place. <br/>

### Features

1. Open a single image for analysis (built-in C++ algorithms for brightness, contrast, sharpness, pixel-value differences, and more).
2. Open a pair of images for side-by-side analysis and comparison.
3. Extract a pair of frames from video files for comparative analysis.
4. Drag-and-drop files in, or paste images from the clipboard.
5. Press S to toggle between the two images for detailed visual comparison.
6. Use the color picker to inspect and compare individual pixel colors.
7. Extend the built-in analysis and comparison toolkit with Python plugins
8. Generates an HTML report using the image-comparison algorithms you choose. [Here’s](https://htmlpreview.github.io/?https://github.com/WhisperingWindLinux/TwinPix/blob/develop/non-project-files/samples/seq-1_vs_seq-2_comparison_report/report.html) an example HTML report comparing two images. 