WARNING 

This is an alpha version of the application, and bugs and memory leaks are possible.

ABOUT

This application allows you to work with 8-bit-per-channel RGB images. Users can open two images and compare them by switching between them using the S key. Additionally, various image processing algorithms and automatic comparison tools based on specific metrics can be applied.

TWINPIX.APP IS DAMAGED AND CAN'T BE OPENED. YOU SHOULD MOVE IT TO THE BIN

When an application is downloaded from a source that Apple does not deem trustworthy, it is assigned an extended attribute called "com.apple.Quarantine." This causes the error message to appear: "<application> is damaged and can't be opened. You should move it to the Bin."

To resolve this, you can remove the attribute, allowing the application to open.

Simply open a terminal and enter the following command:

xattr -c <path/to/TwinPix.app>

PYTHON PLUGINS

Please install the following Python packages to enable plugin support: opencv-python, numpy, matplotlibtplotlib.

Use the following command to install the packages:

pip3 install opencv-python numpy matplotlib

LICENSE

This program is licensed under the GNU General Public License Version 3.
You can redistribute it and/or modify it under the terms of the GNU.
General Public License as published by the Free Software Foundation.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details: https://www.gnu.org/licenses/gpl-3.0.txt.
