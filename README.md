# (Work in progress) YAVE - Functional Reactive Visual Programming Language for Multimedia Processing

`YAVE` is a research project to build an open-source backend for functional reactive visual programming language (FRVPL) for multimedia processing such as video editing and audio processing.  

## What is Functional Reactive Programming?  

Functional Reactive Programming (FRP) is theoretical background of `YAVE`.  
It was appeared in a famous paper ["Functional Reactive Animation"](http://conal.net/papers/icfp97/) by Conal Elliott and Paul Hudak in 1997.  
The core idea of FRP is pretty simple: lifting functions to make them possible to transfer dependant parameters across call graph.  
In other words, FRP is a programming model to represent demand-driven dataflow in functional programming language.  
Although FRP is pretty similar to the idea of dataflow programming languages, visual programming language (VPL) implementation of FRP is almost nonexistent.  
This project is aiming to implement reliable, easy-to-use backend (and some frontend stuff) for functional reactive visual programming language for static multimedia processing.  

The main advantages of FRVPL compared to traditional data-driven VPLs are:
* Avoiding unneccesary computations with lazy evaluation.
* Parameter-independent representation of program.
* Incremental computation by graph reduction and data flow analysis.
* Modern language functionality like higher-order functions.

Some disclaimers:
* `YAVE` can probably be used for realtime media interaction, but that is not in current scope of development.
* `YAVE` uses discrete time representation, which might be disappointment for people who like continuous time representation. 
* `YAVE` is dynamic language. It behaves like a glue to connect low-level codes written in C++.
* `YAVE` is NOT **pure** functional visual programming language strictly speaking, but backends are expected to encapsulate internal side effects. As a result, `YAVE` looks like purely functional language, but backends still have full access to C/C++ features, which makes it much easier to implement low-level operations.

## Development   

YAVE is currently under development using C++17.  
Most of stuff are still PoC stage and contains lots of obsolete stuff.

Current development targets:

* Build simple node based language for testing and practical use.
* Dataflow analsys and sharing.
* Parallel reduction.
* Concurrent execution.
* Lifetime based constant caching.

## Creating custom node function in C++

Here's sneak peak of implementation of node functions in `YAVE` using C++.  
In FRP, functions like `a1 -> a2 -> ... -> an` are lifted into `(time->a1) -> ... -> (time->an-1) -> time -> an`.  
Evaluation of each input values are done by applying given `time` parameter to input closure.  

`YAVE` supports automatic generation of dynamically-typed closure object of lifted functions, which can be executed by runtime.

```cpp
  // lifted function of (Int->Int->Int)
  struct AddInt : NodeFunction<Int, Int, Int> 
  {
    // body of lifted function (you can write any C++ code).
    return_type code() const 
    {
      // eval_arg<N>() evaluates argument value. You can use arg<N>() to get unevaluated thunk.
      // Here I just allocate new Int object and return it.
      return make_object<Int>(*eval_arg<0>() + *eval_arg<1>());

      // This won't compile, because compile-time type checker checks return type. 
      return make_object<Double>(42);
    }
  };

  // create object with vtables and type info.
  auto addInt = make_object<AddInt>();
```