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
Research                :active,  des2, 05-11, 12-11
Design                  :         des3, 11-11, 19-11
Coding                  :         des4, 11-11, 09-12
Art                     :         des4, 10-11, 17-11
Testing                 :         des4, 04-11, 16-12
Release                 :         des4, 09-12, 10-12
Finish Documentation    :         des4, 03-12, 16-12
Presentation            :         des4, 09-12, 17-12
```

```mermaid
gantt
dateFormat  DD-MM
title Personal timelines

section Ilkka
Scene graph             :active,  ides1, 05-11, 5d
Console                 :         ides2, after ides1, 12d
Level Loading           :         ides5, after ides1, 20d
Git control             :         ides3, 04-11, 17-12
Lead dev/assistance     :         ides4, 04-11, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Albert
Audio System            :         ades1, 12-11, 28d
ECS Implementation Desing:active, ades2, 04-11, 13d
ECS System              :         ades3, 13-11, 09-12
ECS Example             :         ades4, 03-12, 09-12
Video Presentation      :         ades5, 09-12, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Matti
Progress video          :         mdes2, 04-11, 16-12
Demo game planning      :active,  mdes4, 04-11, 11-11
Demo game assets        :         mdes5, 10-11, 12-11
Demo game coding        :         mdes6, 11-11, 03-12
Demo game video         :         mdes7, 03-12, 09-12
```
```mermaid
gantt
dateFormat  DD-MM

section Susanne
TDD set up              :done,    sdes1, 04-11, 06-11
Light fix  Desing       :         sdes2, after sdes1, 18-11
Light fix implementation:         sdes3, 10-11, 10-12
Commenting code         :active,  sdes4, 04-11, 03-12
ECS Implementation Desing:        sdes5, 04-11, 18-11
TDD documentation finish:         sdes6, after sdes1, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Markus
Menu System/UI          :         mades1, 19-11, 16-12
Scripting               :         mades2, 10-11, 09-12
Collision               :active,  mades3, 05-11, 19-11
Information research    :active,  mades6, 04-11, 11-11
Debugging               :         mades8, 10-12, 17-12
Documentation Info      :         mades9, 03-12, 17-12
```
```mermaid
gantt
dateFormat  DD-MM

section Oskari
Skeletal animations     :active, odes1, 12-11, 10-12
UI framework            :        odes2, 05-11, 26-11
Research                :active, odes3, 04-11, 18-11
Documentation           :        odes4, 26-11, 09-12

```













