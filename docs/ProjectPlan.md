# Project plan - Eril

### __[See branch docs for faster updates](../../tree/docs/docs/ProjectPlan.md)__

## Goals
Our goal is to advance the Eril Project and create version 0.2.
Eril is a game engine previously developed by Ilkka Takala. It has most of the basic functionality, but requires much work to be called complete. 

Feature list of Eril v0.1
* OpenGL renderer (Forward+ pipeline with blended transparency)
* Instanced objects
* Technical support for 1k dynamic lights
* Shaders, materials, textures
* Garbage collection, reference counting
* Transformable objects
* Modular design

## Team and Responsibilities

| Name   | Task |
| ---    | ---  |
| Ilkka  | Project lead, design, programming |
| Albert | Producer, programming |
| Matti  | Demo design, programming |
| Susanne| Documentation, design assistant, bug fixing |
| Markus | Research, programming |
| Oskari | UI workflow design |
| Leevi | Audio framework |

**Updated responsibilities**

| Name   | Task |
| ---    | ---  |
| Ilkka  | Project lead, design, programming |
| Albert | Producer, programming |
| Matti  | Demo design, programming |
| Markus | Research, collision programming |
| Oskari | Compiler warning fixing |
| Leevi | Audio system |

## Features

These are the features we will add on top of the existing features:
* Scene graph and levels
* Console with custom logs
* Audio
* UI 
* Entity-Component-System framework
* Collisions

Addidional features:
* Networking
* Scripting
* Level editor in Blender

## Timeline

```mermaid
gantt
dateFormat  DD-MM
title Project Timeline

section Project
Kick Off                :done,    des1, 04-11, 2d
Research                :done,    des2, 05-11, 12-11
Design                  :done,    des3, 11-11, 19-11
Coding                  :active,  des4, 11-11, 09-12
Art                     :active,  des4, 10-11, 17-11
Testing                 :active,  des4, 04-11, 16-12
Release                 :         des4, 09-12, 10-12
Finish Documentation    :         des4, 03-12, 16-12
Presentation            :         des4, 09-12, 17-12
```

```mermaid
gantt
dateFormat  DD-MM
title Personal timelines

section Ilkka
Scene graph             :done,    ides1, 05-11, 5d
Console                 :done,    ides2, after ides1, 12d
UI Framework            :done,    ides6, 13-11, 14d 
Level Loading           :done,    ides5, after ides1, 14d
Bug fixing              :active,  ides6, 03-12, 17-12
Demo programming        :active,  ides7, 06-12, 15-12
Git control             :         ides3, 04-11, 17-12
Lead dev/assistance     :         ides4, 04-11, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Albert
ECS Implementation Desing:done,   ades2, 04-11, 13d
ECS System              :done,    ades3, 10-11, 09-12
ECS Example             :done,    ades4, 03-12, 09-12
Audio system            :active,  ades6, 03-12, 09-12
Video Presentation      :         ades5, 09-12, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Matti
Progress video          :         mdes2, 04-11, 16-12
Demo game planning      :done,    mdes4, 04-11, 16-11
Demo game assets        :active,  mdes5, 10-11, 12-11
Demo game coding        :active,  mdes6, 11-11, 03-12
Demo game video         :         mdes7, 03-12, 09-12
```
```mermaid
gantt
dateFormat  DD-MM

section Markus
Collision               :active,  mades3, 05-11, 16-12
Information research    :active,  mades6, 04-11, 11-11
Debugging               :         mades8, 10-12, 17-12
Documentation Info      :         mades9, 03-12, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Oskari
Keyframe animations     :active, odes1, 12-11, 10-12
Research                :active, odes3, 04-11, 18-11
Documentation           :        odes4, 26-11, 09-12

```
```mermaid
gantt
dateFormat  DD-MM

section Leevi
Research                :active, ldes1, 11-11, 4d
Audio                   :active, ldes2, 12-11, 16-12

```













