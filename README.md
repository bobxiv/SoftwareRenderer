# Software Renderer

Simple software renderer by rasterization.

## Table of Contents

- [Software Renderer](#software-renderer)
  - [Table of Contents](#table-of-contents)
  - [Dependencies](#dependencies)
  - [Building](#building)

## Dependencies

The project just depends on [libpng](https://github.com/glennrp/libpng) and [zlib](https://github.com/madler/zlib), which it reference as sub-module to the supported version.

## Building

To build the project we use [CMake](https://cmake.org/). Currently, we are using **CMake v3.16**. Follow this steps:

1. **ZLib**: Initialize the zlib sub-mocule and compile the install project.
2. **LibPng**: Initialize the libpng sub-mocule and compile the install project. This project depends on zlib.
3. **Create and compile the renderer**: Finally build the Software Renderer project and compite. You will have to setup CMAKE_PREFIX_PATH to the path where libpng is installed. To build you can use this command:
4. To run the program you will have to have the zlib share library on path, or copy it along the executable.

```bash
cmake -S . -B build/
```

If you want to configure the build process using a GUI you can use **cmake-gui** in Windows or using the following shell command:

```bash
ccmake -S . -B build/
```

Currently, we are using **Visual Studio 2019** for development.
