# Technical Desing Document

**Product Name:** Eril Engine

# Introduction
### Purpose
Goal is to continue developing Ilkka Takalas Eril Engine and create version 0.2. Since the engine already has most of the basic functions, we are focusing on adding new features on top of the existing ones fixing issues that exist in the original verion. 

### Background

### Terminology


# Proposed Desing
# System Architecture
The engine consists of Core, Render Core and External.

Core
lisää luokka kaavio

Render Core
lisää luokka kaavio

External
lisää luokka kaavio

# workflow
blender:
mallin saaminen näkyviin. 
blenderissä tehdään malli, joka exportataan obj:nä. tehdään material file tekstieditorissa, märitellään mitä tekstuureja ja sheideriä se käyttää. 
tiedostojen tulee olla assets kansion sisällä. Tämän jälkeen enginen sisällä uusi classi, jossa määritellään onko se esimerkiksi actori ja muu tarvittava data. Sille luodaan visible object componentti. Visible objekytille valitaan malliksi valittu obj. alkuperäinen actor tai uusi classista luodan objekti.
