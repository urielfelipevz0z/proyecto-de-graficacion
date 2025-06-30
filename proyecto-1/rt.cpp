// rt: un lanzador de rayos minimalista
 // g++ -O3 -fopenmp rt.cpp -o rt
#include <math.h>
#include <stdlib.h>
#include <stdio.h>  
#include <omp.h>

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

Sphere spheres[] = {
	//Escena: radio, posicion, color 
	Sphere(1e5,  Point(-1e5 - 49, 0, 0),   Color(.75, .25, .25)), // pared izq
	Sphere(1e5,  Point(1e5 + 49, 0, 0),    Color(.25, .25, .75)), // pared der
	Sphere(1e5,  Point(0, 0, -1e5 - 81.6), Color(.75, .75, .75)), // pared detras
	Sphere(1e5,  Point(0, -1e5 - 40.8, 0), Color(.75, .75, .75)), // suelo
	Sphere(1e5,  Point(0, 1e5 + 40.8, 0),  Color(.75, .75, .75)), // techo
	Sphere(16.5, Point(-23, -24.3, -34.6), Color(.999, .999, .999)), // esfera abajo-izq
	Sphere(16.5, Point(23, -24.3, -3.6),   Color(.999, .999, .999) ), // esfera abajo-der
	Sphere(10.5, Point(0, 24.3, 0),        Color(1, 1, 1)) // esfera arriba
};

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

// Modes de visualización
enum RenderMode {
	MODE_SPHERE_COLOR = 0,   // Color de la esfera
	MODE_NORMAL = 1,         // Visualización de normales  
	MODE_DEPTH = 2          // Mapa de profundidad
};

// Cambiar esta variable para alternar entre los diferentes modos de renderizado
const RenderMode currentMode = MODE_DEPTH;

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

// Calcula el valor de color para el rayo dado
Color shade(const Ray &r) {
	double t;
	int id = 0;
	// determinar que esfera (id) y a que distancia (t) el rayo intersecta
	if (!intersect(r, t, id))
		return Color();	// el rayo no intersecto objeto, return Vector() == negro
  
	const Sphere &obj = spheres[id];
	
	// determinar coordenadas del punto de interseccion
	Point x = r.o + r.d * t;

	// determinar la dirección normal en el punto de interseccion
	Vector n = (x - obj.p).normalize();

	// determinar el color que se regresara según el modo de renderizado
	Color colorValue;
	
	switch (currentMode) {
		case MODE_SPHERE_COLOR:
			// Modo 1: Color directo de la esfera
			colorValue = obj.c;
			break;
			
		case MODE_NORMAL:
			// Modo 2: Visualización de normales (mapear [-1,1] a [0,1])
			colorValue = Color(
				(n.x) * 0.5,  // Componente X de la normal
				(n.y) * 0.5,  // Componente Y de la normal
				(n.z) * 0.5   // Componente Z de la normal
			);
			break;
			
		case MODE_DEPTH:
			// Modo 3: Mapa de profundidad (normalizar distancia)
			// Necesitamos encontrar las distancias min y max en la escena
			// Para este caso usaremos un rango aproximado basado en la escena Cornell Box
			double minDepth = 10.0;   // Distancia mínima aproximada
			double maxDepth = 300.0;  // Distancia máxima aproximada
			double normalizedDepth = (t - minDepth) / (maxDepth - minDepth);
			normalizedDepth = clamp(normalizedDepth); // Asegurar que esté en [0,1]
			
			// Mapear profundidad a escala de grises (0 = negro/cercano, 1 = blanco/lejano)
			colorValue = Color(normalizedDepth, normalizedDepth, normalizedDepth);
			break;
	}

	return colorValue; 
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
			// para el pixel actual, computar la dirección que un rayo debe tener
			Vector cameraRayDir = cx * ( double(x)/w - .5) + cy * ( double(y)/h - .5) + camera.d;
			
			// computar el color del pixel para el punto que intersectó el rayo desde la camara
			pixelValue = shade( Ray(camera.o, cameraRayDir.normalize()) );

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
