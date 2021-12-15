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

Final Responsibilities

| Name   | Task |
| ---    | ---  |
| Ilkka  | Project lead, design, programming |
| Albert | Producer, programming |
| Matti  | Demo design, programming |
| Markus | Research, collision programming |
| Leevi | Audio system |

## Features

These are the features we will add on top of the existing features:
* Scene graph and levels
* Console with custom logs
* Audio
* UI 
* Entity-Component-System framework
* Collisions

Addidional features if required features are finished:
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
Coding                  :done,    des4, 11-11, 09-12
Art                     :done,    des4, 10-11, 17-11
Testing                 :done,    des4, 04-11, 16-12
Release                 :done,    des4, 09-12, 10-12
Finish Documentation    :done,    des4, 03-12, 16-12
Presentation            :done,    des4, 09-12, 17-12
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
Scripting               :done,    ides8, 02-12, 3d
Bug fixing              :done,    ides6, 03-12, 17-12
Demo programming        :done,    ides7, 06-12, 15-12
Git control             :done,    ides3, 04-11, 17-12
Lead dev/assistance     :done,    ides4, 04-11, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Albert
ECS Implementation Desing:done,   ades2, 04-11, 13d
ECS System              :done,    ades3, 10-11, 09-12
ECS Example             :done,    ades4, 03-12, 09-12
Audio system            :done,    ades6, 03-12, 09-12
Video Presentation      :done,    ades5, 09-12, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Matti
Demo game planning      :done,    mdes4, 04-11, 16-11
Demo game assets        :done,    mdes5, 10-11, 12-11
Demo game coding        :done,    mdes6, 11-11, 03-12
Demo game video         :         mdes7, 03-12, 09-12
```
```mermaid
gantt
dateFormat  DD-MM

section Markus
Collision               :done,    mades3, 05-11, 16-12
Information research    :done,    mades6, 04-11, 11-11
Debugging               :done,    mades8, 10-12, 17-12
Documentation Info      :         mades9, 03-12, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Leevi
Research                :done,    ldes1, 11-11, 4d
Audio                   :done,    ldes2, 12-11, 16-12

```













