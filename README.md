# ALBUM EXPORTER

## Introduction

This C++ project implements a GUI using the QT framework to export the albums created in MacOS' `Photos` app. The default export feature supported by `Photos` does not allow exporting multiple albums all at once. However, since the raw files of the photos and videos are not stored by the albums but by the first character of the filename of the media files. It does not allow user to simply go into the filesystem and make copy based on the albums. 

Fortunately, `Photos` stores the information of the albums and the locations of the containing media files in a `SQLite` database. We could therefore query the database and fetch the information we need. There are some existing references that explains the structure in the database such as [Photos.sqlite Queries - Original Blog Posting](https://theforensicscooter.com/2021/11/23/photos-sqlite-queries/) and [this repo](https://github.com/muxcmux/apple-photos-forensics). There is also a cli app written in Python, [OSXPhotos](https://github.com/RhetTbull/osxphotos) that queries the same database and supports searching by keywords and exporting. But to me, I prefer to have a GUI for the task and intend to integrate the feature that uploads the exported albums to a NAS where I store all my photos and videos over the years. The uploading feature is not done currently and I will implement the feature in a different [repo](https://github.com/mikeliaohm/syno-uploader) as a C++ library. 

More details about the project can be found in my blog [here]()

## Features

- Preview the media file by hitting the space key (similar to the feature in MacOS' finder)
- Exployee worker threads to export albums in `~/Documents/export`.

## Features to be implemented

- Log the task execution result after done processing the selected albums.
- Persist the logged exeuction result so that user is notified which albums and what actions have been done on these albums.
- Change `ListView` to `TreeView` since albums in the `Photos` app can be placed in folders or subfolders.
- Provide functionality when user clicks `Cancel` in the progress dialog.
- Make the UI look better.