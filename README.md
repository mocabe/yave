# (Work in progress) YAVE - Node based video editor powered by visual programming language.

`YAVE` is a research project to build open-source video editor with functional node based visual programming language. YAVE is aiming to to become hybrid of traditional timeline-based video editing and powerful, extensible node based visual programming environment.

## Development   

YAVE is currently under development. Following components are implemented so far (partially, or PoC stage).  

* Core 
  - Dynamic closure object library and type system.
  - Utility to generate dynamic type information at compile time.
  - Basic data types.
* Node 
  - Low-level node graph controls.
  - Basic node graph operations.
  - Node parser, compiler (obsolate).
* GUI
  - Full implementation of Vulkan backend for imgui (Immediate GUI library).

Recent development tasks:
- [x] layer tree implementation (
    + `layered_node_graph`
- [ ] per-layer resource control implementation 
    + `scene_graph`
        + [x] basic interface and resuorce control
        + [ ] move_* functions
        + [ ] tests
- [ ] key-frame object implementation
    + `keyframe`
        + [x] add node info
        + [ ] implement value type
        + [ ] backend object impl
        + [ ] integrate to primitive types
- [ ] improve test structure
    + [ ] remove tests for obsolete classes
    + [ ] restructure tests based on backends
- [ ] parser rework
- [ ] compiler rework
- [ ] async command interfaec

Other personal projects I want to merge in the future if possible:
- modern NURBS library 
  + [ ] v2
- modern audio I/O library
  + [x] `WASAPI` backend basic
  + [ ] `WASAPI` backend streams
  + [ ] `PulseAudio` backend things
  + [ ] Adopting `std::audio` proposal