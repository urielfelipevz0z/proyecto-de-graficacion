# Proyecto Final - Iluminación Global y Materiales Conductores

## Objetivo
Implementar un sistema completo de iluminación global con soporte para materiales conductores ásperos.

## Especificaciones Técnicas

### Modelo de Material Microfacet para Conductores
- Cada esfera podrá tener un material **difuso** o un **conductor áspero**
- Implementar el modelo microfacet para conductores metálicos
- Soporte para diferentes tipos de metales (aluminio, oro, etc.)

### Iluminación Global
- Implementar **path tracing** (implícito o explícito - recomendado explícito)
- **Limitación sesgada**: máximo 10 rebotes de luz para evitar convergencia lenta
- **Estructura de muestreo**: El ciclo de muestras debe ser exterior al de caminos
  - Cada muestra debe ser un camino distinto
  - Esto mejora la convergencia y reduce la varianza

### Configuraciones de Renderizado
- Generar imágenes a **32 spp** y **512 spp**
- Incluir validación con fuente puntual en iluminación directa
- Comparar resultados entre iluminación directa y global

## Materiales Conductores Ásperos

### Especificaciones de materiales:
- **Esfera de aluminio**: 
  - Parámetro de rugosidad: α = 0.3 o α = 0.03 (según la escena)
- **Esfera de oro**: 
  - Parámetro de rugosidad: α = 0.3

### Propiedades físicas:
- Los materiales conductores deben seguir las leyes de Fresnel
- La rugosidad controla la distribución de las microfacetas
- Valores menores de α producen superficies más especulares

## Entregables
- Código fuente completo con path tracing e implementación de materiales
- Imágenes de referencia para iluminación directa (validación)
- Imágenes con iluminación global a diferentes configuraciones de spp
- Análisis comparativo entre materiales difusos y conductores
- Documentación del modelo microfacet implementado

## Imágenes de Referencia
Se adjuntan imágenes de referencia tanto para iluminación directa como para iluminación global, incluyendo validaciones del material conductor áspero.