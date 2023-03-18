#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "math_3d.h"

GLuint VBO;
GLuint gWorldLocation;


static const char* pVS = "                                                          \n\
#version 330                                                                        \n\                                                                                 \n\
layout (location = 0) in vec3 Position;                                             \n\                                                                                   \n\
uniform mat4 gWorld;                                                                \n\                                                                                \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\                                                                                 \n\
out vec4 FragColor;                                                                 \n\                                                                                  \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);                                           \n\
}";

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT); // очищаем буфер цвета
    static float Scale = 0.0f; // статическая переменная для масштабирования
    Scale += 0.001f; // увеличиваем масштаб
    Matrix4f World; // создаем матрицу трансформации

    //перемещение
    //World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = sinf(Scale);
    //World.m[1][0] = 0.0f; World.m[1][1] = 1.0f; World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
    //World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
    //World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;

    //вращение
    World.m[0][0] = cosf(Scale); World.m[0][1] = -sinf(Scale); World.m[0][2] = 0.0f; World.m[0][3] = sinf(Scale);
    World.m[1][0] = sinf(Scale); World.m[1][1] = sinf(Scale);  World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
    World.m[2][0] = 0.0f;        World.m[2][1] = 0.0f;         World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
    World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;         World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
    
    //преобразование масштаба
    //World.m[0][0] = sinf(Scale); World.m[0][1] = 0.0f;        World.m[0][2] = 0.0f;        World.m[0][3] = 0.0f;
    //World.m[1][0] = 0.0f;        World.m[1][1] = cosf(Scale); World.m[1][2] = 0.0f;        World.m[1][3] = 0.0f;
    //World.m[2][0] = 0.0f;        World.m[2][1] = 0.0f;        World.m[2][2] = sinf(Scale); World.m[2][3] = 0.0f;
    //World.m[3][0] = 0.0f;        World.m[3][1] = 0.0f;        World.m[3][2] = 0.0f;        World.m[3][3] = 1.0f;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]); // передаем матрицу World в шейдер
    glEnableVertexAttribArray(0); // разрешаем использование атрибута 0 (координат вершин)
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // связываем буфер вершин
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // устанавливаем указатель на массив вершин
    glDrawArrays(GL_TRIANGLES, 0, 3); // рисуем треугольник
    glDisableVertexAttribArray(0); // отключаем использование атрибута 0
    glutSwapBuffers(); // выводим изображение на экран
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB); // Устанавливает функцию обратного вызова для отрисовки сцены
    glutIdleFunc(RenderSceneCB); // Устанавливает функцию обратного вызова для рендеринга сцены в неактивном состоянии
}

static void CreateVertexBuffer()
{
    Vector3f Vertices[3]; // Массив вершин треугольника
    Vertices[0] = Vector3f(-0.5f, -0.5f, 0.0f); // Координаты первой вершины
    Vertices[1] = Vector3f(0.5f, -0.5f, 0.0f); // Координаты второй вершины
    Vertices[2] = Vector3f(0.0f, 0.5f, 0.0f); // Координаты третьей вершины
    glGenBuffers(1, &VBO); // Генерирует идентификатор буфера вершин
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязывает буфер вершин к контексту OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // Копирует данные вершин в буфер и указывает, что данные не будут изменяться
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);// Создание шейдерного объекта

    // Проверка создан ли объект шейдера успешно. Если нет, выводим сообщение об ошибке и выходим из программы
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    // Записываем текст шейдера в массив строк
    const GLchar* p[1];
    p[0] = pShaderText;

    // Определяем длину текста шейдера
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);// Привязываем исходный код шейдера к шейдерному объекту
    glCompileShader(ShaderObj);// Компилируем шейдерный объект

    // Получаем информацию о статусе компиляции шейдера
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    // Если произошла ошибка компиляции, выводим сообщение об ошибке и выходим из программы
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);// Присоединяем объект шейдера к программе шейдеров
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();// Создание объекта шейдерной программы и получение его идентификатора

    // Проверка, что объект шейдерной программы был успешно создан
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    // Добавление вершинного и фрагментного шейдеров к шейдерной программе
    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    // Инициализация переменных Success и ErrorLog
    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    // Привязка всех шейдеров к шейдерной программе и проверка успешности связывания
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    // Проверка валидности шейдерной программы
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);// Использование шейдерной программы
    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld"); // Получение местоположения переменной uniform в шейдерной программе
    assert(gWorldLocation != 0xFFFFFFFF);// Проверка, что переменная была найдена
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);// Инициализация GLUT с передачей аргументов argc и argv в функцию glutInit 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);// Установка параметров отображения: двойная буферизация, цветовая модель RGBA
    glutInitWindowSize(1024, 768); // Установка размеров окна
    glutInitWindowPosition(100, 100);// Установка позиции окна
    glutCreateWindow("IKG_LAB_2"); // Создание окна с названием "IKG_LAB_2"  
    InitializeGlutCallbacks();// Инициализация обратных вызовов GLUT

    // Инициализация GLEW и проверка на ошибки
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        // Вывод сообщения об ошибке с использованием функции fprintf и возвращение значения 1
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    } 
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);// Установка цвета фона 
    CreateVertexBuffer();// Создание буфера вершин
    CompileShaders(); // Компиляция шейдеров
    glutMainLoop();// Запуск бесконечного цикла обработки сообщений GLUT

    return 0;
}