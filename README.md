# (Work in progress) YAVE - Functional Reactive Visual Programming Language for Multimedia Processing

## About

`YAVE` is a research project to build an open-source implementation for functional reactive visual programming language (FRVPL) for multimedia processing such as video editing and audio processing.  
It is built on top of dynamically typed, lazy functional embedded domain specific language (EDSL) in C++ with OpenGL(Vulkan) based GUI frontend.  

## About FRP

Functional Reactive Programming (FRP) is a method to build programs which are depending on dynamic input variables in (lazy) functional language.  
It was originally proposed as Haskell implementation by [Conal Elliott and Paul Hudak in 1997](http://conal.net/papers/icfp97/). The idea of FRP had been extended to various languages and frameworks.  
FRP can be very powerful tool to create animations and sounds, but current frameworks are mostly based on textual programming languages.

## Development   

YAVE is currently under development using C++17.  
Most of stuff are still PoC stage and contains lots of obsolete stuff.