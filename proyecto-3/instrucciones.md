# Proyecto 3 - Muestreo de Luz

## Objetivo
Implementar muestreo de luz para diferentes tipos de fuentes luminosas:
- **Fuentes puntuales**
- **Fuentes de área** utilizando:
  - Muestreo de área
  - Muestreo de ángulo sólido

## Especificaciones de Fuentes Luminosas

### Etapa 1: Fuente Puntual
- **Posición**: En el mismo punto que el centro de la esfera que ha sido hasta ahora nuestra fuente de luz (0, 24.3, 0)
- **Intensidad radiante**: (4000, 4000, 4000)
- La fuente puntual reemplaza la esfera emisiva para comparar resultados

### Etapa 1: Fuentes de Área  
- Utilizar la misma escena que el proyecto anterior
- Implementar dos métodos de muestreo:
  1. **Muestreo de área**: Selección uniforme de puntos en la superficie de la fuente
  2. **Muestreo de ángulo sólido**: Muestreo basado en el ángulo sólido subtendido por la fuente

## Etapa 2: Escena con Múltiples Emisores

### Configuración de fuentes:
1. **Luz puntual**: 
   - Intensidad: I = (2000, 2000, 2000)
   - Posición: (0, 24.3, 0)

2. **Fuente de área 1**:
   - Emisión: Le = (12, 5, 5)
   - Radio: 10.5
   - Posición: (-23, 24.3, 0)

3. **Fuente de área 2**:
   - Emisión: Le = (5, 5, 12)  
   - Radio: 5
   - Posición: (23, 24.3, -50)

## Entregables
- Código fuente con implementación de muestreo de luz
- Imágenes de referencia para validación
- Implementación de múltiples fuentes luminosas
- Comparación entre métodos de muestreo de área vs ángulo sólido