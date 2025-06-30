// rt: un lanzador de rayos minimalista
 // g++ -O3 -fopenmp rt.cpp -o rt
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <omp.h>
#include <random>

// Thread-safe random number generation for OpenMP
thread_local std::mt19937 rng(std::random_device{}());
thread_local std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);

// Genera un número aleatorio uniformemente distribuido en [0, 1)
inline double uniform_random() {
    return uniform_dist(rng);
}

class Vector 
{
public:        
	double x, y, z; // coordenadas x,y,z 
  
	// Constructor del vector, parametros por default en cero
	Vector(double x_= 0, double y_= 0, double z_= 0){ x=x_; y=y_; z=z_; }
  
	// operador para suma y resta de vectores
	Vector operator+(const Vector &b) const { return Vector(x + b.x, y + b.y, z + b.z); }
	Vector operator-(const Vector &b) const { return Vector(x - b.x, y - b.y, z - b.z); }
	// operator multiplicacion vector y escalar 
	Vector operator*(double b) const { return Vector(x * b, y * b, z * b); }
  
	// operator % para producto cruz
	Vector operator%(Vector&b){return Vector(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x);}
	
	// producto punto con vector b
	double dot(const Vector &b) const { return x * b.x + y * b.y + z * b.z; }

	// producto elemento a elemento (Hadamard product)
	Vector mult(const Vector &b) const { return Vector(x * b.x, y * b.y, z * b.z); }
	
	// normalizar vector 
	Vector& normalize(){ return *this = *this * (1.0 / sqrt(x * x + y * y + z * z)); }
};
typedef Vector Point;
typedef Vector Color;

class Ray 
{ 
public:
	Point o;
	Vector d; // origen y direcccion del rayo
	Ray(Point o_, Vector d_) : o(o_), d(d_) {} // constructor
};

class Sphere 
{
public:
	double r;	// radio de la esfera
	Point p;	// posicion
	Color c;	// color  

	Sphere(double r_, Point p_, Color c_): r(r_), p(p_), c(c_) {}
  
	// determina si el rayo intersecta a esta esfera
	// Implementa la intersección rayo-esfera usando la ecuación cuadrática
	// Retorna la distancia más cercana si hay intersección, 0.0 si no hay intersección
	double intersect(const Ray &ray) const {
		// Vector desde el origen del rayo al centro de la esfera
		Vector oc = ray.o - p;
		
		// Coeficientes de la ecuación cuadrática: at² + bt + c = 0
		// donde: a = d·d, b = 2(oc·d), c = oc·oc - r²
		double a = ray.d.dot(ray.d);
		double b = 2.0 * oc.dot(ray.d);
		double c = oc.dot(oc) - r * r;
		
		// Calcular discriminante
		double discriminant = b * b - 4 * a * c;
		
		// Si discriminante < 0, no hay intersección
		if (discriminant < 0) {
			return 0.0;
		}
		
		// Calcular las dos soluciones posibles
		double sqrt_discriminant = sqrt(discriminant);
		double t1 = (-b - sqrt_discriminant) / (2.0 * a);
		double t2 = (-b + sqrt_discriminant) / (2.0 * a);
		
		// Queremos la intersección más cercana que sea positiva (delante del rayo)
		// t1 es siempre <= t2
		if (t1 > 1e-4) {  // Usamos epsilon para evitar problemas de precisión numérica
			return t1;
		}
		if (t2 > 1e-4) {
			return t2;
		}
		
		// No hay intersección válida (ambas son negativas o muy cercanas a cero)
		return 0.0;
	}
};

// Cornell Box scene configuration para Proyecto 2
Sphere spheres[] = {
	// Geometría de la escena Cornell Box
	Sphere(1e5,  Point(-1e5 - 49, 0, 0),     Color(.75, .25, .25)), // pared izq (roja)
	Sphere(1e5,  Point(1e5 + 49, 0, 0),      Color(.25, .25, .75)), // pared der (azul)
	Sphere(1e5,  Point(0, 0, -1e5 - 81.6),   Color(.25, .75, .25)), // pared detras (verde)
	Sphere(1e5,  Point(0, -1e5 - 40.8, 0),   Color(.25, .75, .75)), // suelo (cian)
	Sphere(1e5,  Point(0, 1e5 + 40.8, 0),    Color(.75, .75, .25)), // techo (amarillo)
	Sphere(16.5, Point(-23, -24.3, -34.6),   Color(.2, .3, .4)),    // esfera abajo-izq
	Sphere(16.5, Point(23, -24.3, -3.6),     Color(.4, .3, .2)),    // esfera abajo-der
	Sphere(10.5, Point(0, 24.3, 0),          Color(1, 1, 1))       // esfera arriba (fuente)
};

// Identificadores de objetos especiales
const int LIGHT_SPHERE_ID = 7;  // Índice de la esfera fuente de luz

// Emisión de la fuente luminosa
const Vector LIGHT_EMISSION = Vector(10, 10, 10);

// limita el valor de x a [0,1]
inline double clamp(const double x) { 
	if(x < 0.0)
		return 0.0;
	else if(x > 1.0)
		return 1.0;
	return x;
}

// convierte un valor de color en [0,1] a un entero en [0,255]
inline int toDisplayValue(const double x) {
	return int( pow( clamp(x), 1.0/2.2 ) * 255 + .5); 
}

// Sampling methods para Proyecto 2 - Monte Carlo direction sampling
enum SamplingMethod {
	UNIFORM_SPHERE = 0,      // Uniform spherical sampling
	UNIFORM_HEMISPHERE = 1,  // Uniform hemispherical sampling  
	COSINE_HEMISPHERE = 2    // Cosine-weighted hemispherical sampling
};

// Configuration for Proyecto 2
const SamplingMethod SAMPLING_METHOD = COSINE_HEMISPHERE; // Change this to test different methods
const int SAMPLES_PER_PIXEL = 2048;  // Change this to 32, 512, or 2048

// calcular la intersección del rayo r con todas las esferas
// regresar true si hubo una intersección, falso de otro modo
// almacenar en t la distancia sobre el rayo en que sucede la interseccion
// almacenar en id el indice de spheres[] de la esfera cuya interseccion es mas cercana
inline bool intersect(const Ray &r, double &t, int &id) {
	double n = sizeof(spheres) / sizeof(Sphere);  // número de esferas en la escena
	double d;      // distancia temporal para cada intersección
	double inf = 1e20;  // valor "infinito" para inicializar distancia mínima
	t = inf;       // inicializar con distancia infinita
	
	// Probar intersección con cada esfera
	for (int i = 0; i < n; ++i) {
		d = spheres[i].intersect(r);  // calcular distancia de intersección
		
		// Si hay intersección (d > 0) y es más cercana que la mejor encontrada
		if (d > 0 && d < t) {
			t = d;     // actualizar distancia mínima
			id = i;    // actualizar índice de la esfera más cercana
		}
	}
	
	// Retornar true si encontramos alguna intersección válida
	return t < inf;
}

// Genera un punto uniformemente distribuido en una esfera unitaria
Vector uniform_sphere_sample() {
    double u1 = uniform_random();
    double u2 = uniform_random();
    
    double z = 1.0 - 2.0 * u1;  // z ∈ [-1, 1]
    double r = sqrt(1.0 - z * z);
    double phi = 2.0 * M_PI * u2;
    
    return Vector(r * cos(phi), r * sin(phi), z);
}

// Genera un punto uniformemente distribuido en un hemisferio unitario
Vector uniform_hemisphere_sample(const Vector& normal) {
    Vector sample = uniform_sphere_sample();
    
    // Si el sample está en el hemisferio incorrecto, reflejarlo
    if (sample.dot(normal) < 0.0) {
        sample = sample * -1.0;
    }
    
    return sample;
}

// Genera un punto con distribución coseno en un hemisferio unitario  
Vector cosine_hemisphere_sample(const Vector& normal) {
    double u1 = uniform_random();
    double u2 = uniform_random();
    
    // Muestreo por coseno usando coordenadas polares
    double cos_theta = sqrt(u1);
    double sin_theta = sqrt(1.0 - u1);
    double phi = 2.0 * M_PI * u2;
    
    // Vector en el hemisferio local (normal = (0,0,1))
    Vector local_sample = Vector(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta);
    
    // Crear base ortonormal usando el vector normal
    Vector w = normal;
    Vector u = ((abs(w.x) > 0.1 ? Vector(0, 1, 0) : Vector(1, 0, 0)) % w).normalize();
    Vector v = w % u;
    
    // Transformar del espacio local al espacio mundial
    return u * local_sample.x + v * local_sample.y + w * local_sample.z;
}

// Calcula PDF (Probability Density Function) para cada método de muestreo
double get_pdf(const SamplingMethod method, const Vector& sample_dir, const Vector& normal) {
    switch (method) {
        case UNIFORM_SPHERE:
            return 1.0 / (4.0 * M_PI);  // PDF = 1/4π
        case UNIFORM_HEMISPHERE:
            return sample_dir.dot(normal) > 0 ? 1.0 / (2.0 * M_PI) : 0.0;  // PDF = 1/2π
        case COSINE_HEMISPHERE:
            return sample_dir.dot(normal) > 0 ? sample_dir.dot(normal) / M_PI : 0.0;  // PDF = cos(θ)/π
        default:
            return 1.0;
    }
}

// Calcula el valor de color para el rayo dado usando Monte Carlo ray tracing
Color shade(const Ray &r, int depth = 0) {
	double t;
	int id = 0;
	
	// Límite de recursión para evitar bucles infinitos
	if (depth > 5) return Color();
	
	// Determinar que esfera (id) y a que distancia (t) el rayo intersecta
	if (!intersect(r, t, id))
		return Color();	// El rayo no intersectó objeto, retornar negro
  
	const Sphere &obj = spheres[id];
	
	// Determinar coordenadas del punto de intersección
	Point x = r.o + r.d * t;

	// Determinar la dirección normal en el punto de intersección
	Vector n = (x - obj.p).normalize();
	
	// Ajustar normal para que apunte hacia el hemisfério correcto
	Vector normal = n.dot(r.d) < 0 ? n : n * -1;

	// Color acumulado del punto
	Color result = Color();
	
	// Si es la fuente de luz, agregar emisión
	if (id == LIGHT_SPHERE_ID) {
		result = result + LIGHT_EMISSION.mult(obj.c);
		return result; // Las fuentes de luz no reflejan otras luces
	}
	
	// Monte Carlo integration para iluminación indirecta
	Color indirect_light = Color();
	
	// Solo hacer muestreo si no hemos alcanzado la profundidad máxima
	if (depth < 5) {
		// Número de muestras para esta intersección (solo 1 por rayo para eficiencia)
		const int samples = 1;
		
		for (int s = 0; s < samples; s++) {
			Vector sample_dir;
			double pdf;
			
			// Generar dirección de muestra según el método configurado
			switch (SAMPLING_METHOD) {
				case UNIFORM_SPHERE:
					sample_dir = uniform_sphere_sample();
					pdf = get_pdf(UNIFORM_SPHERE, sample_dir, normal);
					break;
				case UNIFORM_HEMISPHERE:
					sample_dir = uniform_hemisphere_sample(normal);
					pdf = get_pdf(UNIFORM_HEMISPHERE, sample_dir, normal);
					break;
				case COSINE_HEMISPHERE:
					sample_dir = cosine_hemisphere_sample(normal);
					pdf = get_pdf(COSINE_HEMISPHERE, sample_dir, normal);
					break;
				default:
					sample_dir = uniform_hemisphere_sample(normal);
					pdf = get_pdf(UNIFORM_HEMISPHERE, sample_dir, normal);
					break;
			}
			
			// Verificar que la dirección está en el hemisferio correcto
			if (sample_dir.dot(normal) > 0 && pdf > 0) {
				// Crear rayo secundario
				Ray secondary_ray = Ray(x + normal * 1e-4, sample_dir);
				
				// Obtener color de iluminación indirecta recursivamente
				Color Li = shade(secondary_ray, depth + 1);
				
				// BRDF Lambertiana: fr = albedo / π
				Vector brdf = obj.c * (1.0 / M_PI);
				
				// Coseno del ángulo entre normal y dirección de muestra
				double cos_theta = sample_dir.dot(normal);
				
				// Aplicar ecuación de rendering: Lo = (Li * fr * cos_theta) / pdf
				if (cos_theta > 0) {
					indirect_light = indirect_light + Li.mult(brdf) * (cos_theta / pdf);
				}
			}
		}
		
		// Promedio de las muestras
		indirect_light = indirect_light * (1.0 / samples);
	}
	
	// Resultado final: emisión + iluminación indirecta
	result = result + indirect_light;
	
	return result;
}


int main(int argc, char *argv[]) {
	int w = 1024, h = 768; // image resolution
  
	// fija la posicion de la camara y la dirección en que mira
	Ray camera( Point(0, 11.2, 214), Vector(0, -0.042612, -1).normalize() );

	// parametros de la camara
	Vector cx = Vector( w * 0.5095 / h, 0., 0.); 
	Vector cy = (cx % camera.d).normalize() * 0.5095;
  
	// auxiliar para valor de pixel y matriz para almacenar la imagen
	Color *pixelColors = new Color[w * h];

	// usar openmp para paralelizar el ciclo: cada hilo computara un renglon (ciclo interior)
	#pragma omp parallel for schedule(dynamic, 1)
	for(int y = 0; y < h; y++) 
	{ 
		// recorre todos los pixeles de la imagen
		fprintf(stderr,"\r%5.2f%%",100.*y/(h-1));
		for(int x = 0; x < w; x++ ) {
			int idx = (h - y - 1) * w + x; // index en 1D para una imagen 2D x,y son invertidos
			Color pixelValue = Color(); // pixelValue en negro por ahora
			
			// Monte Carlo sampling: usar múltiples muestras por pixel
			for (int s = 0; s < SAMPLES_PER_PIXEL; s++) {
				// para el pixel actual, computar la dirección que un rayo debe tener
				Vector cameraRayDir = cx * ( double(x)/w - .5) + cy * ( double(y)/h - .5) + camera.d;
				
				// computar el color del pixel para el punto que intersectó el rayo desde la camara
				Color sampleColor = shade( Ray(camera.o, cameraRayDir.normalize()) );
				
				// Acumular el color de la muestra
				pixelValue = pixelValue + sampleColor;
			}
			
			// Promedio de todas las muestras
			pixelValue = pixelValue * (1.0 / SAMPLES_PER_PIXEL);

			// limitar los tres valores de color del pixel a [0,1]
			pixelColors[idx] = Color(clamp(pixelValue.x), clamp(pixelValue.y), clamp(pixelValue.z));
		}
	}

	fprintf(stderr,"\n");

	// PROYECTO 1
	// Investigar formato ppm
	FILE *f = fopen("image.ppm", "w");
	// escribe cabecera del archivo ppm, ancho, alto y valor maximo de color
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255); 
	for (int p = 0; p < w * h; p++) 
	{ // escribe todos los valores de los pixeles
    		fprintf(f,"%d %d %d ", toDisplayValue(pixelColors[p].x), toDisplayValue(pixelColors[p].y), 
				toDisplayValue(pixelColors[p].z));
  	}
  	fclose(f);

  	delete[] pixelColors;

	return 0;
}
