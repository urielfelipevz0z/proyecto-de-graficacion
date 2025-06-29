# Proyecto 2 - Muestreo de Direcciones

## Objetivo
Implementar diferentes métodos de muestreo de direcciones para el renderizado.

## Parámetros de la Escena

Las imágenes de referencia que se proporcionan fueron generadas utilizando los siguientes parámetros:

```cpp
// Geometría de la escena Cornell Box
Sphere(1e5,  Point(-1e5 - 49, 0, 0),     Color(.75, .25, .25)), // pared izq (roja)
Sphere(1e5,  Point(1e5 + 49, 0, 0),      Color(.25, .25, .75)), // pared der (azul)
Sphere(1e5,  Point(0, 0, -1e5 - 81.6),   Color(.25, .75, .25)), // pared detras (verde)
Sphere(1e5,  Point(0, -1e5 - 40.8, 0),   Color(.25, .75, .75)), // suelo (cian)
Sphere(1e5,  Point(0, 1e5 + 40.8, 0),    Color(.75, .75, .25)), // techo (amarillo)
Sphere(16.5, Point(-23, -24.3, -34.6),   Color(.2, .3, .4)),    // esfera abajo-izq
Sphere(16.5, Point(23, -24.3, -3.6),     Color(.4, .3, .2)),    // esfera abajo-der
Sphere(10.5, Point(0, 24.3, 0),          Color(1, 1, 1))       // esfera arriba (fuente)
```

## Fuente Luminosa
- **Posición**: Esfera superior en (0, 24.3, 0) con radio 10.5
- **Emisión**: (10, 10, 10)

## Métodos de Muestreo a Implementar
1. **Muestreo uniforme esférico**
2. **Muestreo uniforme hemisférico**  
3. **Muestreo coseno hemisférico**

## Configuraciones de Renderizado
Cada método debe ser probado con diferentes números de muestras por pixel (spp):
- 32 spp
- 512 spp
- 2048 spp

## Entregables
- Código fuente con los 3 métodos de muestreo implementados
- 9 imágenes de referencia (3 métodos × 3 configuraciones de spp)
- Análisis comparativo de los resultados