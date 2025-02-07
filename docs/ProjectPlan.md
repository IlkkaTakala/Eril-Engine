# Project plan - Eril

### __[See branch "docs" for faster updates](../../tree/docs/docs/ProjectPlan.md)__

## Goals
Our goal is to advance the Eril Project and create version 0.3.
Eril is a game engine previously developed by Ilkka Takala. It has most of the basic functionality, but requires much work to be called complete. 

Feature list of Eril v0.2
* OpenGL renderer (Forward+ pipeline with blended transparency)
* Instanced objects
* Technical support for 1k dynamic lights
* Shaders, materials, textures
* Garbage collection, reference counting
* Transformable objects
* Modular design
* Scene graph and levels
* Console with custom logs
* Audio System with ECS
* UI Framework
* Entity-Component-System framework
* Collisions

## Team and Responsibilities

Final Responsibilities

| Name   | Task |
| ---    | ---  |
| Ilkka  | Project lead, design, General programming |
| Matti | Programming |

## Features

These are the features we will add on top of the existing features:
- [x] Input system rework
- [x] Scripting
- [x] SSAO
- [x] Reflections (Screen space)
- [ ] Global illumination, IBL
- [ ] Shadows
- [x] Animations
- [ ] Editors
- [ ] UI Framework improvements
- [x] Particles
- [x] Collisions
- [x] Quaternions


Addidional features if required features are finished:
* Networking
* Level editor
* CMake/Custom build system
* Custom file types
* LODs

## Timeline

```mermaid
gantt
dateFormat  DD-MM
title Project Timeline

section Project
Kick Off                :done,      des1, 13-01, 2d
Research                :done,      des2, 13-01, 8d
Coding                  :active,    des4, 13-01, 17w
Testing                 :active,    des5, after des4, 4d
Release                 :           des6, after des5, 2d
Presentation            :           des8, 19-05, 20-05
```

```mermaid
gantt
dateFormat  DD-MM
title Personal timelines

section Ilkka
Scripting               :done,      ides1, 13-01, 18-02
Quaternions             :done,      ides6, after ides5, 08-04
Animations              :done,      ides2, after ides5, 8w
Particles               :done,      ides5, after ides1, 11-03
Scene loading improvement:active,   ides7, after ides2, 25-05
Git control             :           ides3, 13-01, 18w
Lead dev/assistance     :           ides4, 13-01, 18w
```
```mermaid
gantt
dateFormat  DD-MM

section Matti
Input Rework           :done,       mdes3, 13-01, 1w
Collisions             :done,       mdes1, after mdes3, 4w
Collision improvements :done,       mdes5, after mdes1, 31-03
Script integrations    :active,     mdes4, after mdes1, 4w
Graphical improvements :done,       mdes6, after mdes5, 4w
Particle functions     :active,     mdes7, after mdes6, 25-05
UI Improvements        :            mdes2, after mdes1, 3w
```












