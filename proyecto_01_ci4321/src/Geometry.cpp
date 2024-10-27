#include "Geometry.h"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

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

            // Agregamos los vertices a la lista
            attributes.push_back(x);
            attributes.push_back(y);
            attributes.push_back(z);

            // Agregando normales (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            attributes.push_back(nx);
            attributes.push_back(ny);
            attributes.push_back(nz);

            // Calculos de las coordenadas de los vertices en la textura, dentro del rango [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;

            attributes.push_back(s);
            attributes.push_back(t);
        }
    }

    int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        // Inicializacion de la esfera por sectores. Se usara el termino "aro" como analogia, al referirse a cada nivel de la esfera
        // Comienzo desde el "aro" actual
        k1 = i * (sectorCount + 1);     

        // Comienzo desde el "aro" actual
        k2 = k1 + sectorCount + 1;      

        for (int j = 0; j < sectorCount; ++j)
        {
            // Hay 2 triangulos por sector, excluyendo el aro superior y el inferior
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }

            ++k1;
            ++k2;
        }
    }
}

void Sphere::Draw(const Shader& ourShader)
{
    // Creacion de transformaciones
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    
    // Recuperacion de las ubicaciones de los uniforms
    unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
   
    // Pase de ubicaciones a los shaders
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);

    glDrawElements(GL_TRIANGLES,(unsigned int)indices.size(),GL_UNSIGNED_INT, (void*)0);
}

void Sphere::SetupGL()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //Datos de vertices
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);           
    glBufferData(GL_ARRAY_BUFFER, (unsigned int)attributes.size() * sizeof(float), attributes.data(), GL_STATIC_DRAW);
    
    // Datos de indices
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);   
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (unsigned int)indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // Setear los atributos en el orden indicado (posicion, normales, coord Text) 
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, 8 * sizeof(float), (void*)(sizeof(float) * 6));
}

void Sphere::CleanGL()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

Cube::Cube(float width, float height, float depth)
{
    this->width = width;
    this->height = height;

    // Inicializacion del cambio de las caras con respecto al cubo unitario
    float w = width/2;
    float h = height/2;
    float d = depth/2;

    // Lista de atributos del cubo (Posicion / Tex Coords)
    attributes = {
        // Cara trasera
        // Triangulo inferior
        -w, -h, -d,  0.0f, 0.0f,
         w, -h, -d,  1.0f, 0.0f,
         w,  h, -d,  1.0f, 1.0f,
        //Triangulo superior
         w,  h, -d,  1.0f, 1.0f,
        -w,  h, -d,  0.0f, 1.0f,
        -w, -h, -d,  0.0f, 0.0f,

        // Cara frontal
        // Triangulo inferior
        -w, -h,  d,  0.0f, 0.0f,
         w, -h,  d,  1.0f, 0.0f,
         w,  h,  d,  1.0f, 1.0f,
        // Triangulo superior
         w,  h,  d,  1.0f, 1.0f,
        -w,  h,  d,  0.0f, 1.0f,
        -w, -h,  d,  0.0f, 0.0f,

        // Cara lateral izquierda
        // Triangulo superior
        -w,  h,  d,  1.0f, 0.0f,
        -w,  h, -d,  1.0f, 1.0f,
        -w, -h, -d,  0.0f, 1.0f,
        // Triangulo inferior
        -w, -h, -d,  0.0f, 1.0f,
        -w, -h,  d,  0.0f, 0.0f,
        -w,  h,  d,  1.0f, 0.0f,

        // Cara lateral derecha
        // Triangulo superior
         w,  h,  d,  1.0f, 0.0f,
         w,  h, -d,  1.0f, 1.0f,
         w, -h, -d,  0.0f, 1.0f,
        // Triangulo inferior
         w, -h, -d,  0.0f, 1.0f,
         w, -h,  d,  0.0f, 0.0f,
         w,  h,  d,  1.0f, 0.0f,

        // Cara inferior
        // Triangulo superior
        -w, -h, -d,  0.0f, 1.0f,
         w, -h, -d,  1.0f, 1.0f,
         w, -h,  d,  1.0f, 0.0f,
        // Triangulo inferior
         w, -h,  d,  1.0f, 0.0f,
        -w, -h,  d,  0.0f, 0.0f,
        -w, -h, -d,  0.0f, 1.0f,

        // Cara superior
        // Triangulo superior
        -w,  h, -d,  0.0f, 1.0f,
         w,  h, -d,  1.0f, 1.0f,
         w,  h,  d,  1.0f, 0.0f,
        // Triangulo inferior
         w,  h,  d,  1.0f, 0.0f,
        -w,  h,  d,  0.0f, 0.0f,
        -w,  h, -d,  0.0f, 1.0f
    };

};

void Cube::Draw(const Shader& ourShader)
{
    // Creacion de transformaciones
    glm::mat4 model = glm::mat4(1.0f); 
   
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

    // Recuperacion de las ubicaciones de los uniforms
    unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");

    // Pase de ubicaciones a los shaders
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);

    // Dibujamos el cubo
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

void Cube::SetupGL()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Iniciamos el proceso de binding/vinculacion
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (unsigned int)attributes.size() * sizeof(float), attributes.data(), GL_STATIC_DRAW);

    // Atributos de posicion
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Atributos de coordenadas de texturas
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Cube::CleanGL()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
