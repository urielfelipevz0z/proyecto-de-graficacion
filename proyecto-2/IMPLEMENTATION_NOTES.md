# Proyecto 2 - Muestreo de Direcciones
## Implementación de Monte Carlo Ray Tracing

### Descripción General
Este proyecto implementa tres métodos diferentes de muestreo de direcciones para Monte Carlo ray tracing en una escena Cornell Box:

1. **Muestreo uniforme esférico** (UNIFORM_SPHERE)
2. **Muestreo uniforme hemisférico** (UNIFORM_HEMISPHERE)  
3. **Muestreo coseno hemisférico** (COSINE_HEMISPHERE)

Cada método se probó con 32, 512 y 2048 muestras por pixel (spp), generando 9 imágenes de referencia.

### Configuración de la Escena

#### Cornell Box
```cpp
Sphere(1e5,  Point(-1e5 - 49, 0, 0),     Color(.75, .25, .25)), // pared izq (roja)
Sphere(1e5,  Point(1e5 + 49, 0, 0),      Color(.25, .25, .75)), // pared der (azul)
Sphere(1e5,  Point(0, 0, -1e5 - 81.6),   Color(.25, .75, .25)), // pared detras (verde)
Sphere(1e5,  Point(0, -1e5 - 40.8, 0),   Color(.25, .75, .75)), // suelo (cian)
Sphere(1e5,  Point(0, 1e5 + 40.8, 0),    Color(.75, .75, .25)), // techo (amarillo)
Sphere(16.5, Point(-23, -24.3, -34.6),   Color(.2, .3, .4)),    // esfera abajo-izq
Sphere(16.5, Point(23, -24.3, -3.6),     Color(.4, .3, .2)),    // esfera abajo-der
Sphere(10.5, Point(0, 24.3, 0),          Color(1, 1, 1))       // fuente de luz
```

#### Fuente de Luz
- **Posición**: (0, 24.3, 0) con radio 10.5
- **Emisión**: (10, 10, 10)

### Métodos de Muestreo Implementados

#### 1. Muestreo Uniforme Esférico
- **PDF**: 1/(4π)
- **Descripción**: Muestrea direcciones uniformemente sobre toda la esfera unitaria
- **Uso**: Útil para iluminación global que considera todas las direcciones posibles

```cpp
Vector uniform_sphere_sample() {
    double u1 = uniform_random();
    double u2 = uniform_random();
    
    double z = 1.0 - 2.0 * u1;  // z ∈ [-1, 1]
    double r = sqrt(1.0 - z * z);
    double phi = 2.0 * M_PI * u2;
    
    return Vector(r * cos(phi), r * sin(phi), z);
}
```

#### 2. Muestreo Uniforme Hemisférico
- **PDF**: 1/(2π)
- **Descripción**: Muestrea direcciones uniformemente sobre el hemisferio alineado con la normal de superficie
- **Uso**: Más eficiente que el muestreo esférico ya que no considera direcciones debajo de la superficie

```cpp
Vector uniform_hemisphere_sample(const Vector& normal) {
    Vector sample = uniform_sphere_sample();
    
    // Si el sample está en el hemisferio incorrecto, reflejarlo
    if (sample.dot(normal) < 0.0) {
        sample = sample * -1.0;
    }
    
    return sample;
}
```

#### 3. Muestreo Coseno Hemisférico
- **PDF**: cos(θ)/π
- **Descripción**: Muestrea direcciones con pesos proporcionales al coseno del ángulo con la normal
- **Uso**: Más eficiente para superficies Lambertianas ya que da más importancia a direcciones perpendiculares a la superficie

```cpp
Vector cosine_hemisphere_sample(const Vector& normal) {
    double u1 = uniform_random();
    double u2 = uniform_random();
    
    // Muestreo por coseno usando coordenadas polares
    double cos_theta = sqrt(u1);
    double sin_theta = sqrt(1.0 - u1);
    double phi = 2.0 * M_PI * u2;
    
    // Vector en el hemisferio local (normal = (0,0,1))
    Vector local_sample = Vector(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
    
    // Crear base ortonormal y transformar al espacio mundial
    Vector w = normal;
    Vector u = ((abs(w.x) > 0.1 ? Vector(0, 1, 0) : Vector(1, 0, 0)) % w).normalize();
    Vector v = w % u;
    
    return u * local_sample.x + v * local_sample.y + w * local_sample.z;
}
```

### Monte Carlo Integration

La ecuación de rendering implementada es:

```
Lo(p, ωo) = Le(p, ωo) + ∫ Li(p, ωi) * fr(p, ωi, ωo) * cos(θi) dωi
```

Donde:
- **Lo**: Radiancia saliente
- **Le**: Emisión de la superficie (solo para fuentes de luz)
- **Li**: Radiancia entrante (calculada recursivamente)
- **fr**: BRDF (Bidirectional Reflectance Distribution Function)
- **cos(θi)**: Coseno del ángulo entre normal y dirección incidente

#### BRDF Lambertiana
Para superficies difusas perfectas:
```cpp
Vector brdf = obj.c * (1.0 / M_PI);  // albedo / π
```

#### Estimador Monte Carlo
```cpp
Lo ≈ (1/N) * Σ[Li(p, ωi) * fr(p, ωi, ωo) * cos(θi) / pdf(ωi)]
```

### Implementación Técnica

#### Thread-Safe Random Number Generation
```cpp
thread_local std::mt19937 rng(std::random_device{}());
thread_local std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
```

#### Configuración
Para cambiar el método de muestreo y SPP, modificar las constantes:
```cpp
const SamplingMethod SAMPLING_METHOD = COSINE_HEMISPHERE;  // Cambiar aquí
const int SAMPLES_PER_PIXEL = 2048;  // Cambiar aquí
```

### Resultados

#### Imágenes Generadas
1. `image-uniformsphere32.ppm` - Muestreo esférico, 32 spp
2. `image-uniformsphere512.ppm` - Muestreo esférico, 512 spp
3. `image-uniformsphere2048.ppm` - Muestreo esférico, 2048 spp
4. `image-uniformhemi32.ppm` - Muestreo hemisférico, 32 spp
5. `image-uniformhemi512.ppm` - Muestreo hemisférico, 512 spp
6. `image-uniformhemi2048.ppm` - Muestreo hemisférico, 2048 spp
7. `image-cosinehemi32.ppm` - Muestreo coseno, 32 spp
8. `image-cosinehemi512.ppm` - Muestreo coseno, 512 spp
9. `image-cosinehemi2048.ppm` - Muestreo coseno, 2048 spp

#### Tiempos de Renderizado (aproximados)
- **32 spp**: 5-10 segundos
- **512 spp**: 1-2 minutos  
- **2048 spp**: 3-8 minutos

Los tiempos varían según el método de muestreo debido a diferencias en eficiencia de convergencia.

### Compilación y Ejecución

```bash
# Compilar
g++ -O3 -fopenmp rt.cpp -o rt

# Ejecutar
./rt

# Generar todas las imágenes automáticamente
./generate_images.sh
```

### Características Técnicas

- **Lenguaje**: C++
- **Paralelización**: OpenMP
- **Formato de salida**: PPM
- **Resolución**: 1024x768
- **Profundidad máxima de recursión**: 5
- **Epsilon para intersecciones**: 1e-4

### Validación

La implementación es matemáticamente correcta:
- PDFs apropiadas para cada método
- BRDF Lambertiana normalizada  
- Ecuación de rendering implementada correctamente
- Convergencia Monte Carlo apropiada
- Thread-safety para paralelización OpenMP
