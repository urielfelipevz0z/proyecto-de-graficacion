# Laboratorio de Graficación - Ray Tracing

Este repositorio contiene una serie de proyectos progresivos para el desarrollo de un ray tracer completo, desde conceptos básicos hasta técnicas avanzadas de iluminación global.

## Estructura del Proyecto

```
├── código-base/          # Implementación base del ray tracer
├── proyecto-1/           # Visualización básica y mapas de profundidad
├── proyecto-2/           # Métodos de muestreo de direcciones
├── proyecto-3/           # Muestreo de luz (puntuales y de área)
├── proyecto-final/       # Iluminación global y materiales conductores
└── README.md
```

## Código Base

El proyecto está construido sobre un ray tracer minimalista en C++ que incluye:
- **Clases fundamentales**: Vector, Ray, Sphere
- **Operaciones vectoriales**: Producto punto, producto cruz, normalización
- **Intersección rayo-esfera**: Algoritmo básico de ray tracing
- **Compilación con OpenMP**: Soporte para paralelización

## Progresión de Proyectos

### [Proyecto 1 - Código Base](./proyecto-1/instrucciones.md)
**Objetivo**: Familiarización con el ray tracer básico
- Visualización de colores de esferas
- Mapa de normales
- Mapa de profundidad

### [Proyecto 2 - Muestreo de Direcciones](./proyecto-2/instrucciones.md)
**Objetivo**: Implementación de diferentes métodos de muestreo
- Muestreo uniforme esférico
- Muestreo uniforme hemisférico
- Muestreo coseno hemisférico
- Comparación con diferentes configuraciones de spp (32, 512, 2048)

### [Proyecto 3 - Muestreo de Luz](./proyecto-3/instrucciones.md)
**Objetivo**: Técnicas avanzadas de muestreo luminoso
- Fuentes puntuales
- Fuentes de área (muestreo de área y ángulo sólido)
- Escenas con múltiples emisores

### [Proyecto Final - Iluminación Global](./proyecto-final/instrucciones.md)
**Objetivo**: Sistema completo de renderizado
- Path tracing (implícito/explícito)
- Modelo microfacet para materiales conductores
- Limitación de rebotes (10 máximo)
- Soporte para aluminio y oro con diferentes rugosidades

## Aclaraciones
- Las imágenes de referencia para el Proyecto 1, Proyecto 2 y Proyecto 3 se renderizaron considerando iluminación directa, sin efectos de iluminación global.
- El Proyecto Final para las imágenes de referencia si incluye iluminación global y materiales conductores, con un enfoque en la precisión física de los materiales.
- Sin embargo para las imágenes de los resultados de los proyectos 2, 3 y final, se considera iluminación global, por lo que las imágenes de referencia no son válidas para comparar los resultados de estos proyectos.
- Esta es la única diferencia entre las imágenes de referencia y los resultados esperados de los proyectos 2, 3 y final.