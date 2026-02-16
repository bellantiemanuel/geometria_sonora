# Geometría Sonora

## UNIVERSIDAD NACIONAL DE LAS ARTES  
### Departamento de Artes Multimediales  

**Especialización en Sonido Aplicado a las Artes Digitales**  
**Imagen y Sonido en Tiempo Real**

**Docente:** Cohl, Daniel  
**Estudiante:** Emanuel Bellanti  
**Trabajo Final:** “Geometría Sonora”  
**Fecha:** Febrero 2026  

---

## Descripción

Este proyecto es un experimento audiovisual que busca conectar sonido e imagen a través de un sistema simple, sensible y con memoria. No se trata de un visualizador tradicional, sino de una escucha traducida a forma, donde el sonido deja rastros y el tiempo se vuelve parte de la imagen.

El objetivo principal fue aprender a interconectar distintos softwares (Reaper y openFrameworks), explorar el uso de FFT y análisis por bandas, y transformar datos técnicos (niveles RMS) en un comportamiento visual expresivo.

El resultado es básico en su implementación, pero significativo en su alcance: un sistema que reacciona, recuerda y construye una narrativa visual a partir del sonido.

---

## Idea General

El sistema escucha el audio entrante, lo separa en tres bandas de frecuencia (graves, medios y agudos), y asigna a cada una un lenguaje geométrico propio:

- **LOW (graves)** → círculos / masas  
- **MID (medios)** → cuadrados / estructuras  
- **HIGH (agudos)** → triángulos / destellos  

Cada banda no solo activa una forma distinta, sino que tiene un comportamiento temporal diferente, reflejando el carácter físico del sonido:

- Los graves son más pesados y persistentes.  
- Los medios construyen continuidad.  
- Los agudos son rápidos y efímeros.  

La imagen no se borra inmediatamente cuando el sonido desaparece.  
El sistema recuerda lo que ocurrió hace unos segundos.

---

## Flujo General del Sistema

```
Audio (mic / instrumento)
        ↓
      Reaper
        ↓
  Filtro por bandas (ReaFIR)
        ↓
  Medición RMS por banda
        ↓
     Envío OSC
        ↓
 openFrameworks
        ↓
 Interpretación visual
```

---

## Análisis de Audio en Reaper

El análisis sonoro se realiza completamente en Reaper, evitando cálculos FFT en openFrameworks y asegurando mayor estabilidad y control.

### Estructura en Reaper

#### Pista 1

- Entrada de audio (micrófono o instrumento)  
- Envía la señal a tres pistas auxiliares  

#### Pistas 2, 3 y 4

Cada una representa una banda de frecuencia:

- **ReaFIR** en modo *band split*  
  - Track 2 → LOW  
  - Track 3 → MID  
  - Track 4 → HIGH  

- **JS: Loudness Meter**  
  - Se utiliza RMS-M (Momentary RMS)  
  - Proporciona un valor estable de energía real por banda  

### Envío por OSC

Cada pista envía su nivel RMS mediante OSC:

- `/track/2/vu` → graves  
- `/track/3/vu` → medios  
- `/track/4/vu` → agudos  

Estos valores llegan normalizados en un rango aproximado `0.0 – 1.0`.

---

## Recepción e Interpretación en openFrameworks

openFrameworks actúa como intérprete visual, no como analizador de audio.

### Recepción OSC

- Se utiliza `ofxOscReceiver`.  
- Cada mensaje OSC contiene:  
  - Una dirección (`/track/x/vu`)  
  - Un valor flotante (RMS)  

El sistema:

- Verifica el tipo de dato.  
- Normaliza y clampa el valor.  
- Aplica un umbral por banda, eliminando ruido residual.  

```cpp
low = (v > lowThreshold) ? v : 0.0f;
```

Esto permite que cada banda responda solo cuando realmente está presente.

---

## Piso de Ruido y Activación del Sistema

Además de los umbrales por banda, existe un piso de ruido global (`noiseFloor`).

- Si la energía total es baja → el sistema entra en reposo.  
- Si la energía supera el piso → el sistema se activa gradualmente.  

Esto permite que:

- El ruido ambiente no dispare eventos visuales.  
- La voz o un sonido cercano despierte el sistema.  
- La activación no sea binaria, sino progresiva.  

El resultado es un comportamiento más orgánico y controlado.

---

## Memoria Visual y Tiempo

La imagen no se dibuja directamente en pantalla, sino en un framebuffer (`FBO`) llamado `memory`.

En cada frame:

- Se dibuja un rectángulo negro semi-transparente.  
- Esto genera un borrado lento.  
- Las formas dejan rastros que se desvanecen con el tiempo.  

El sistema introduce así:

- **Pasado** → lo que quedó en pantalla  
- **Presente** → nuevas formas  
- **Duración** → cuánto persiste cada gesto  

El visual deja de ser reactivo y empieza a narrar.

---

## Generación de Geometría

La cantidad de formas generadas depende de:

- Energía de la banda.  
- Duración perceptual (suavizado).  
- Nivel de activación global (gate).  

### Comportamiento por Banda

#### LOW → Círculos

- Grandes  
- Pocos  
- Pesados  

#### MID → Cuadrados

- Tamaño medio  
- Construyen estructura  

#### HIGH → Triángulos

- Pequeños  
- Muchos  
- Rápidos y nerviosos  

Las formas aparecen en posiciones aleatorias, construyendo un campo visual vivo, no una representación literal del espectro.

---

## Estado del Proyecto

Este proyecto es exploratorio y en proceso. La implementación es deliberadamente sencilla, priorizando comprensión, experimentación y apertura a futuras mutaciones.

El valor no está en la complejidad del código, sino en el gesto de escucha y en la posibilidad de seguir profundizando esa relación entre sonido, imagen y tiempo.

En cuanto a la cursada, permitió profundizar en el uso de openFrameworks y comprender que tanto sonido como video e incluso datos provenientes de otros medios pueden ser manipulados entre programas para lograr resultados impactantes.

Se proyecta continuar experimentando e investigando qué otras implementaciones pueden resultar de la combinación de imagen y sonido en tiempo real. Se agradecen las clases en vivo y la filosofía detrás del código compartido con los estudiantes.
