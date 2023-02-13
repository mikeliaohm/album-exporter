# ALBUM EXPORTER

## Introduction

This C++ project implements a GUI using the QT framework to export the albums created in MacOS' `Photos` app. The default export feature supported by `Photos` does not allow exporting multiple albums all at once. However, since the raw files of the photos and videos are not stored by the albums but by the first character of the filename of the media files. It does not allow user to simply go into the filesystem and make copy based on the albums. 

Fortunately, `Photos` stores the information of the albums and the locations of the containing media files in a `SQLite` database. We could therefore query the database and fetch the information we need. There are some existing references that explains the structure in the database such as [Photos.sqlite Queries - Original Blog Posting](https://theforensicscooter.com/2021/11/23/photos-sqlite-queries/) and [this repo](https://github.com/muxcmux/apple-photos-forensics). There is also a cli app written in Python, [OSXPhotos](https://github.com/RhetTbull/osxphotos) that queries the same database and supports searching by keywords and exporting. But to me, I prefer to have a GUI for the task and intend to integrate the feature that uploads the exported albums to a NAS where I store all my photos and videos over the years. The uploading feature is not done currently and I will implement the feature in a different [repo](https://github.com/mikeliaohm/syno-uploader) as a C++ library. 

More details about the implementation of the project can be found in my blog [here](https://mikeliaohm.github.io/c++/qt/gui/multithreading/2023/02/10/export-photo-albums.html).

## Build the project

Since the project is built using Qt Framework, to build the project on your own machine, you will need to download the SDK from the [Qt website](https://www.qt.io/download). The easiest way to do so is to download its IDE, Qt Creator, and build the project. I use `CMake` as the build system, instea of `qmake`. After installing Qt Creator, open the CMakeLists.txt file in the root directory of the repo and configure the project and build from there.

Qt has its own licenses. Check [Qt Licensing](https://www.qt.io/licensing/) for more information.

### Build from the command line

The easiest way to build the app for testing is using Qt Creator. However, you could instead build the project using cmake through the command line with proper arguments. Here are two versions of shell commands that work for me. In the 2nd version, you could generate Makefiles using ninja as suggested in the Qt documentation.

**sample script 1**

```bash
# the default cmake version in my machine is 3.18
/usr/local/Cellar/cmake/3.18.3/bin/cmake -S . -B 'build/' '-DCMAKE_GENERATOR:STRING=Unix Makefiles' -DCMAKE_BUILD_TYPE:STRING=Debug -DQT_QMAKE_EXECUTABLE:FILEPATH= -DCMAKE_PREFIX_PATH:PATH= -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++
# Build
/usr/local/Cellar/cmake/3.18.3/bin/cmake --build build/ --target all
# Package the app (you can remove -verbose=3)
macdeployqt album-exporter.app -verbose=3  -always-overwrite -dmg
```

**sample script 2 - using ninja**

```bash
# There seems to be an issue if trying to build platform "x86_64;arm64"
/Users/mikeliao/Qt/Tools/CMake/CMake.app/Contents/bin/cmake -S . -B 'build/' -DCMAKE_BUILD_TYPE:STRING=Debug -DQT_QMAKE_EXECUTABLE:FILEPATH= -DCMAKE_PREFIX_PATH:PATH= -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++ -DCMAKE_OSX_ARCHITECTURES="x86_64" -G Ninja
# Build with ninja
cd build && ninja
# Package the app (you can remove -verbose=3)
macdeployqt album-exporter.app -verbose=3  -always-overwrite -dmg
```

## Features

- Preview the media file by hitting the space key (similar to the feature in MacOS' finder)
- Exployee worker threads to export albums in `~/Documents/export`.

## Features to be implemented

- Log the task execution result after done processing the selected albums.
- Persist the logged exeuction result so that user is notified which albums and what actions have been done on these albums.
- Change `ListView` to `TreeView` since albums in the `Photos` app can be placed in folders or subfolders.
- Provide functionality when user clicks `Cancel` in the progress dialog.
- Make the UI look better.