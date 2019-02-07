# Nbdl

>Network Based Data Layer
>
>C++ Application State Management Library

## Overview

This project is a work in progress.

It now relies on the experimental language feature [_Parametric Expressions_ (P1221)](https://ricejasonf.github.io/parametric_expressions/typeless-functions-2.html).

There are two compiler forks, one of Clang and another that applies the changes in that fork to Emscripten's fork of Clang:
  - [Clang](https://github.com/ricejasonf/clang/commits/parmexpr)
  - [Emscripten Clang](https://github.com/ricejasonf/emscripten-fastcomp-clang/commits/parmexpr2)

I'm keeping both up to date with bug fixes so I recommend using the Emscripten version since it supports all plarforms:
  - X86
  - ARM
  - JsBackend

Both are available on Dockerhub and can be used with CppDock. _(see the Build section below)_

## Basic Features

* Context object for managing state, network connection, and message dispatching
* Concepts for wrapping server/client implementations and Stores
* Concepts for generic recursive visitation of arbitrary data structures
* Concepts for serialization of Boost.Hana and user-defined data types
  * API hash versioning
* Promise interface for chaining events
* A variant type optimized for use on a single thread, with a safe interface,
  and most importantly it plays well with the serialization facilities
  which is used for dispatching messages and representing state.

## Features for Web Frontends

* Rendering state on a web frontend via DOM manipulations. (Requires Emscripten)
  * Navigation/Location is available as state
* Server/Client websocket integration and Browser Client Websocket API integration

## Features Coming Soon

* Database persistence integration
* Multi-platform UI toolkit
* Documentation

## Build the Development Environment

Nbdl uses [CppDock](https://github.com/ricejasonf/cppdock) with Docker to create a development environment with all of the dependencies installed.

See the `cppdock.json` file for information about dependencies.

To build both development environment images run the following:

```
cppdock build develop && cppdock build web_develop
```

To run the containers use the `cppdock dev` command:
```
cppdock dev develop
```

Then in the container's shell you can run one of the examples:

```
root@f2b146e7bad9:/opt/build# make run.example.pipes
```

> Note: You may have to build the CppDock platform images locally as both projects are in a state of flux.
