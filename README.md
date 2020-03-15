# (Work in progress) YAVE - Functional Reactive Visual Programming Language for Multimedia Processing

## About

`YAVE` is a research project to build an open-source implementation for functional reactive visual programming language (FRVPL) for multimedia processing such as video editing and audio processing.  
It is built on top of dynamically typed, lazy functional embedded domain specific language (EDSL) in C++ with OpenGL(Vulkan) based GUI frontend.  

Most of stuff are still PoC stage and contains lots of obsolete stuff.

WIP screenshots:

![](yave.jpg)

## About FRP

Functional Reactive Programming (FRP) is a method to build programs which are depending on dynamic input variables in (lazy) functional language.  
It was originally proposed as Haskell implementation by [Conal Elliott and Paul Hudak in 1997](http://conal.net/papers/icfp97/). The idea of FRP had been extended to various languages and frameworks.  
FRP can be very powerful tool to create animations and sounds, but current frameworks are mostly based on textual programming languages.

## Development   

### Compiler

YAVE is currently under development using C++17.  
To build this project, C++17 capable compiler is required.  
MSVC support is still rough, and requires latest preview version of VS2019.

### Building 

This project uses CMake as build system.  
On windows, MSYS2 shell is also required.

### Dependency

This project depending on various libraries and API.  
Most of dependencies are in `external` folder as git submodules.  
Other than that, it requires Vulkan SDK for rendering engine.