# Technical Desing Document

**Product Name:** Eril Engine

# Introduction
### Purpose
Goal is to continue developing Ilkka Takalas Eril Engine and create version 0.2. Since the engine already has most of the basic functions, we are focusing on adding new features on top of the existing ones fixing issues that exist in the original verion. 

### Background

### Terminology


# Proposed Desing
### System Architecture
The engine consists of Core, Render Core and External.

Core
lisää luokka kaavio

Render Core
lisää luokka kaavio

External
lisää luokka kaavio

### Workflow
Blender

Mallin saaminen näkyviin:
- Blenderissä tehdään malli, joka exportataan obj:na.
- Obj:lle tehdään materieal file tekstieditorissa, määritellään mitä tekstuuria ja sheideriä obj käyttää.
- Enginen sisällä tehdään uusi luokka, jossa määritellään onko se actori ja muut tarvittavat tiedot.
- Obj:lle luodaan visible componentti ja sille valitaan malliksi valittu obj.
- Alkuperäisestä actorista tai uudesta luokasta luodaan objekti.

Huom! Tiedostojen tulee olla assets kansion sisällä.

