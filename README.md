## Image to Text as a service written in C++ and Javascript (NodeJs) Running on top of Node Addon Api, Tesseract and OpenCV

#### ***runs and compiles fine using Ubuntu 20 and gcc 10***

### Requirements
- C and C++ compiler, gcc https://gcc.gnu.org/install/binaries.html or other compilers
- Nodejs https://nodejs.org/en/download/
- cmake https://cmake.org/download/
- cmake-js https://github.com/cmake-js/cmake-js

### Current C/C++ Graphics and other dependencies
- OpenCV https://github.com/opencv/opencv
- OpenCV Contrib https://github.com/opencv/opencv_contrib
- ImageMagick https://github.com/ImageMagick/ImageMagick.git
- LibJpeg https://github.com/stohrendorf/libjpeg-cmake.git
- LibPng https://github.com/glennrp/libpng.git
- LibWebp https://github.com/webmproject/libwebp.git
- LibZ https://github.com/madler/zlib.git

### Building

Install `cmake-js`
```shell
$ npm install --location=global cmake-js
```

Install Nodejs dependencies
```shell
$ npm install
```

Build and Install Dependencies
```shell
$ make build-vendor
```

Build Project
```shell
$ make build
```

Running
```shell
$ npm start
```

### Build & run via Docker (TODO)

Build docker image
```shell
$ docker build --rm -t itt:latest .
```

Run service
```shell
$ docker run --rm -it -p 9010:9010 itt:latest
```
