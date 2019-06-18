# (Work in progress) YAVE - Node based video editor powered by visual programming language.

`YAVE` is a research project to build open-source video editor with functional node based visual programming language. YAVE is aiming to to become hybrid of traditional timeline-based video editing and powerful, extensible node based visual programming environment.

## Development   

YAVE is currently under development. Following components are implemented so far (partially, or PoC stage).  

* Core 
  - Dynamic closure object library and type system.
  - Various data types for data transfer between nodes.
* Node 
  - Low-level node graph controls.
  - Node parser and compiler.
* ImGui
  - Full implementation of Vulkan backend for imgui (Immediate GUI library).
* other
  - Various interface for external libraries.

and other other things being worked on:  

*  GUI design and implementation.
*  Cross-platform audio I/O library.