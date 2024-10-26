#include "Geometry.h"

Sphere::Sphere(float radius, int sectorCount, int stackCount)
{
    this->radius = radius;
    this->sectorCount = sectorCount;
    this->stackCount = stackCount;

    // Inicializacion de variables a utilizar para calcular posicon, normales y coordenadas de la textura
    float x, y, z, xy;                    
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    // Inicializacion de distancias entre longitudes y latitudes
    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;

    // Variables para los angulos a utilizar
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        // Se determina el angulo de apertura vetical de la esfera (desde pi/2 a -pi/2)
        stackAngle = PI / 2 - i * stackStep; 

        // Vector del plano XY -> r * cos(u)
        xy = radius * cosf(stackAngle);

        // Vector de la componente Z -> r * sin(u)
        z = radius * sinf(stackAngle);              

        // Ciclo que recorre el numero de sectores de la esfera. 
        // El primer y ultimo vertice tendran la misma posicion y las mismas normales, pero las coordenadas de sus texturas varia
        for (int j = 0; j <= sectorCount; ++j)
        {
            // Recorrido horizontal de la esfera (de 0 a 2pi)
            sectorAngle = j * sectorStep;           

            // Posicion del vertice (x,y,z)

            // Componente X -> r * cos(u) * cos(v)
            x = xy * cosf(sectorAngle);             

            // Componente Y -> r * cos(u) * sin(v)
            y = xy * sinf(sectorAngle);  

            // Agregamso los vertices a la lista
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Agregando normales (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // Calculos de las coordenadas de los vertices en la textura, dentro del rango [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;

            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }
}


Cube::Cube(float width, float height, float depth)
{
    this->width = width;
    this->height = height;

    // Inicializacion del cambio de las caras con respecto al cubo unitario
    float w = width/2;
    float h = height/2;
    float d = depth/2;

    //Posicion de los vertices
    vertices = {
            // Cara trasera
            // Triangulo inferior
            -w, -h, -d,
            w, -h, -d,
            w,  h, -d,
            // Triangulo superior
            w,  h, -d,
            -w,  h, -d,
            -w, -h, -d, 

            // Cara frontal
            // Triangulo inferior
            -w, -h,  d,  
            w, -h,  d,  
            w,  h,  d, 
            // Triangulo superior
            w,  h,  d,  
            -w,  h,  d,  
            -w, -h,  d,

            // Cara lateral izquierda
            // Triangulo superior
            -w,  h,  d,
            -w,  h, -d,
            -w, -h, -d,
            // Triangulo inferior
            -w, -h, -d,
            -w, -h,  d,
            -w,  h,  d,

            // Cara lateral derecha
            // Triangulo superior
            w,  h,  d,
            w,  h, -d,
            w, -h, -d,
            // Triangulo inferior
            w, -h, -d,
            w, -h,  d,
            w,  h,  d,

            // Cara inferior
            // Triangulo superior
            -w, -h, -d,
            w, -h, -d,
            w, -h,  d,
            // Triangulo inferior
            w, -h,  d,
            -w, -h,  d,
            -w, -h, -d,

            // Cara superior
            // Triangulo superior
            -w,  h, -d,
            w,  h, -d,
            w,  h,  d,
            // Triangulo inferior
            w,  h,  d,
            -w,  h,  d,
            -w,  h, -d,
    };
     //Posicion de las coordenadas de la textura
    texCoords = {

        // Cara trasera
        // Triangulo inferior
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        // Triangulo superior
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        // Cara frontal
        // Triangulo inferior
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        // Triangulo superior
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        // Cara lateral izquierda
        // Triangulo superior
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Triangulo inferior
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        // Cara lateral derecha
        // Triangulo superior
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Triangulo inferior
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        // Cara inferior
        // Triangulo superior
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        // Triangulo inferior
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        // Cara superior
        // Triangulo superior
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        // Triangulo inferior
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f

    };
}