# Proyecto 1 - Código Base

## Objetivo
Para el proyecto 1 es necesario generar 3 imágenes diferentes modificando el código base:

### Imágenes a generar:
1. **Color de esfera**: El color del pixel corresponde al color de la esfera que se intersectó
2. **Visualización de normales**: El color del pixel es el valor de la normal en el punto que se intersectó
3. **Mapa de profundidad**: El color del pixel indica la distancia a la que sucedió la intersección (0 para la más cercana, 1 para la más lejana)

## Implementación
- Se debe modificar el código base que se encuentra en la carpeta `código-base` del repositorio
- El código base incluye un ray tracer minimalista en C++ con clases para Vector, Ray y Sphere
- Para compilar: `make` o `g++ -O3 -fopenmp rt.cpp -o rt`

## Entregables
- Código fuente modificado
- 3 imágenes resultantes en formato PPM o JPG
- Documentación de los cambios realizados
