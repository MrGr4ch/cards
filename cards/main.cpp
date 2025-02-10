#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <math.h>
using namespace std;

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
float perspective_matrix[16] = {};
float ModelViewMatrix[16] = {};
float rotx = 0.0f;
float roty = 0.0f;
float TransZ = -2.5f;
unsigned int prg_id;

//card measures in cm
float card_width = 6.35;
float card_height = 8.89;
float corner_radius = 0.35;
float card_thickness = 0.03048;

//testing
//float card_thickness = 0.5;
//float card_thickness = 0.03048;

//// check OpenGL error
//GLenum err;
//while ((err = glGetError()) != GL_NO_ERROR) { 
//    cerr << "1. OpenGL error: " << err << endl;
//}


string read_file(string filePath) {
    ifstream file(filePath);

    if (!file.is_open()) {
        cerr << "Failed to open file: " << filePath << endl;
        return "empty";
    }
    string fileContent((istreambuf_iterator<char>(file)),
        istreambuf_iterator<char>());
    file.close();
    return fileContent;
}

unsigned int load_texture() {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        cerr << "glTexParameteri. OpenGL error: " << err << endl;
    } 

    int width, height, nrChannels;
    unsigned char* data = stbi_load("images/terramorphic_expanse.jpg", &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            cerr << "glTexImage OpenGL error: " << err << endl;
        }
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return texture;
}

void update_perspective_matrix(int width, int height) {
    float nearVal = 1.0f;
    float farVal = 20.0f;
    float angle = 30.0f;
    float top = nearVal * tanf(angle / 2.0f);
    float bottom = -top;
    float aspect_ratio = (float)width / (float)height;
    float right = aspect_ratio * top;
    float left = -right;

    perspective_matrix[0] = (2.0f * nearVal) / (right - left);
    perspective_matrix[8] = (right + left) / (right - left);
    perspective_matrix[5] = (2.0f * nearVal) / (top - bottom);
    perspective_matrix[9] = (top + bottom) / (top - bottom);
    perspective_matrix[10] = -((farVal + nearVal) / (farVal - nearVal));
    perspective_matrix[14] = -((2.0f * farVal * nearVal) / (farVal - nearVal));
    perspective_matrix[11] = -1;


    int perspective_location = glGetUniformLocation(prg_id, "perspective_matrix");
    glUniformMatrix4fv(perspective_location, 1, false, perspective_matrix);
    
}

void update_modelviewmatrix() {
    float fAngX = rotx * 3.14159265f / 180.0f;
    float fAngY = roty * 3.14159265f / 180.0f;
    for (unsigned int i = 0; i < 16; i++) ModelViewMatrix[i] = 0.0f;

    ModelViewMatrix[0] = std::cos(fAngY);
    ModelViewMatrix[2] = -std::sin(fAngY);
    ModelViewMatrix[4] = std::sin(fAngX) * std::sin(fAngY);
    ModelViewMatrix[5] = std::cos(fAngX);
    ModelViewMatrix[6] = std::sin(fAngX) * std::cos(fAngY);
    ModelViewMatrix[8] = std::cos(fAngX) * std::sin(fAngY);
    ModelViewMatrix[9] = -std::sin(fAngX);
    ModelViewMatrix[10] = std::cos(fAngX) * std::cos(fAngY);
    ModelViewMatrix[14] = TransZ;
    ModelViewMatrix[15] = 1.0f;

    /* Identitymatrix für Tests
    ModelViewMatrix[0] = 1;
    ModelViewMatrix[1] = 0;
    ModelViewMatrix[2] = 0;
    ModelViewMatrix[3] = 0;
    ModelViewMatrix[4] = 0;
    ModelViewMatrix[5] = 1;
    ModelViewMatrix[6] = 0;
    ModelViewMatrix[7] = 0;
    ModelViewMatrix[8] = 0;
    ModelViewMatrix[9] = 0;
    ModelViewMatrix[10] = 1;
    ModelViewMatrix[11] = 0;
    ModelViewMatrix[12] = 0;
    ModelViewMatrix[13] = 0;
    ModelViewMatrix[14] = 0;
    ModelViewMatrix[15] = 1;
    */
    int modelview_location = glGetUniformLocation(prg_id, "ModelViewMatrix");
    glUniformMatrix4fv(modelview_location, 1, false, ModelViewMatrix);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    update_perspective_matrix(width, height);
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        roty = roty + 1.0f;
        update_modelviewmatrix();
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        roty = roty - 1.0f;
        update_modelviewmatrix();
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        rotx = rotx + 1.0f;
        update_modelviewmatrix();
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        rotx = rotx - 1.0f;
        update_modelviewmatrix();
    }
}
/*
float * normalize_card_measurements(float width, float height) {
    
    float ratio = height / width;
    height = 1;
    width = height / ratio;
    float help[2] = { width, height };

    return help;

}

float * create_card_vertices(float width, float height, float radius, float thickness) {
    float ratio = height / width;
    float ratio_r = height / radius;
    float ratio_t = height / thickness;
    height = 1;
    width = height / ratio;
    radius = height / ratio_r;
    thickness = height / ratio_t;

    //all x,y values possible
    float x_end = width;
    float x_radius = width - radius;
    float y_end = height;
    float y_radius = height - radius;

    //create box_vertices
    float vertices[72] = {
      x_end,      y_radius   , thickness,
      x_end,     -y_radius   , thickness,
      x_radius,   y_end      , thickness,
      x_radius,   y_radius   , thickness,
      x_radius,  -y_radius   , thickness,
      x_radius,  -y_end      , thickness,
      -x_radius,  y_end      , thickness,
      -x_radius,  y_radius   , thickness,
      -x_radius, -y_radius   , thickness,
      -x_radius, -y_end      , thickness,
      -x_end,     y_radius   , thickness,
      -x_end,    -y_radius   , thickness,
      x_end,      y_radius   , -thickness,
      x_end,     -y_radius   , -thickness,
      x_radius,   y_end      , -thickness,
      x_radius,   y_radius   , -thickness,
      x_radius,  -y_radius   , -thickness,
      x_radius,  -y_end      , -thickness,
      -x_radius,  y_end      , -thickness,
      -x_radius,  y_radius   , -thickness,
      -x_radius, -y_radius   , -thickness,
      -x_radius, -y_end      , -thickness,
      -x_end,     y_radius   , -thickness,
      -x_end,    -y_radius   , -thickness
    };


    int size = sizeof(vertices) / sizeof(vertices[0]);
    std::cout << size;
    std::cout << "\n";
    for (int i = 0; i < size; i++) {
        std::cout << vertices[i];
        std::cout << ", ";
    };
    std::cout << "\n";

    

    
  
    return vertices;

}

unsigned int* create_card_indices() {
    
    unsigned int indices[] = {
        //rim counterclockwise
        0, 1,12,
        1,12,13,
        0, 3,12,
        3,12,15,
        3, 2,15,
        2,15,14,
        2, 6,14,
        6,14,18,
        6,7,18,
        7,18,19,
        7,10,19,
        10,19,22,
        10,11,22,
        11,22,23,
        11,8,23,
        8,23,20,
        8,9,20,
        9,20,21,
        9,5,21,
        5,21,17,
        5,4,17,
        4,17,16,
        4,1,16,
        1,16,13,

        //faces
        0,4,1,
        0,3,4,
        3,2,6,
        6,3,7,
        3,7,4,
        7,8,4,
        7,10,8,
        10,8,11,
        8,9,5,
        8,5,4,

        12,16,13,
        12,15,16,
        15,14,18,
        18,15,19,
        15,19,16,
        19,20,16,
        19,22,20,
        22,20,23,
        20,21,17,
        20,17,16
    };
    wi = 84;
int new_vertice = 24;

//tr:(0,2)
indices[wi] = 0;
indices[wi + 1] = new_vertice;
indices[wi + 2] = 3;

indices[wi + 3] = 0;
indices[wi + 4] = new_vertice;
indices[wi + 5] = 12;

indices[wi + 6] = new_vertice + 1;
indices[wi + 7] = new_vertice;
indices[wi + 8] = 12;

indices[wi + 9] = 12;
indices[wi + 10] = new_vertice + 1;
indices[wi + 11] = 15;

wi = wi + 12;

for (int i = 0; i < 8; i++) {
    indices[wi] = new_vertice;
    indices[wi + 1] = new_vertice + 2;
    indices[wi + 2] = 3;

    indices[wi + 3] = new_vertice;
    indices[wi + 4] = new_vertice + 2;
    indices[wi + 5] = new_vertice + 1;

    indices[wi + 6] = new_vertice + 3;
    indices[wi + 7] = new_vertice + 2;
    indices[wi + 8] = new_vertice + 1;

    indices[wi + 9] = new_vertice + 1;
    indices[wi + 10] = new_vertice + 3;
    indices[wi + 11] = 15;

    wi = wi + 12;
    new_vertice = new_vertice + 2;
}
indices[wi] = new_vertice;
indices[wi + 1] = 2;
indices[wi + 2] = 3;

indices[wi + 3] = new_vertice;
indices[wi + 4] = 2;
indices[wi + 5] = new_vertice + 1;

indices[wi + 6] = 14;
indices[wi + 7] = 2;
indices[wi + 8] = new_vertice + 1;

indices[wi + 9] = new_vertice + 1;
indices[wi + 10] = 14;
indices[wi + 11] = 15;

wi = wi + 12;
new_vertice = new_vertice + 2;


//tl:(6,10)
indices[wi] = 6;
indices[wi + 1] = new_vertice;
indices[wi + 2] = 7;

indices[wi + 3] = 6;
indices[wi + 4] = new_vertice;
indices[wi + 5] = 18;

indices[wi + 6] = new_vertice + 1;
indices[wi + 7] = new_vertice;
indices[wi + 8] = 18;

indices[wi + 9] = 18;
indices[wi + 10] = new_vertice + 1;
indices[wi + 11] = 19;

wi = wi + 12;

for (int i = 0; i < 8; i++) {
    indices[wi] = new_vertice;
    indices[wi + 1] = new_vertice + 2;
    indices[wi + 2] = 7;

    indices[wi + 3] = new_vertice;
    indices[wi + 4] = new_vertice + 2;
    indices[wi + 5] = new_vertice + 1;

    indices[wi + 6] = new_vertice + 3;
    indices[wi + 7] = new_vertice + 2;
    indices[wi + 8] = new_vertice + 1;

    indices[wi + 9] = new_vertice + 1;
    indices[wi + 10] = new_vertice + 3;
    indices[wi + 11] = 19;

    wi = wi + 12;
    new_vertice = new_vertice + 2;
}
indices[wi] = new_vertice;
indices[wi + 1] = 10;
indices[wi + 2] = 7;

indices[wi + 3] = new_vertice;
indices[wi + 4] = 10;
indices[wi + 5] = new_vertice + 1;

indices[wi + 6] = 22;
indices[wi + 7] = 10;
indices[wi + 8] = new_vertice + 1;

indices[wi + 9] = new_vertice + 1;
indices[wi + 10] = 22;
indices[wi + 11] = 19;

wi = wi + 12;
new_vertice = new_vertice + 2;

//br:(1,5)
indices[wi] = 1;
indices[wi + 1] = new_vertice;
indices[wi + 2] = 4;

indices[wi + 3] = 1;
indices[wi + 4] = new_vertice;
indices[wi + 5] = 13;

indices[wi + 6] = new_vertice + 1;
indices[wi + 7] = new_vertice;
indices[wi + 8] = 13;

indices[wi + 9] = 13;
indices[wi + 10] = new_vertice + 1;
indices[wi + 11] = 16;

wi = wi + 12;

for (int i = 0; i < 8; i++) {
    indices[wi] = new_vertice;
    indices[wi + 1] = new_vertice + 2;
    indices[wi + 2] = 4;

    indices[wi + 3] = new_vertice;
    indices[wi + 4] = new_vertice + 2;
    indices[wi + 5] = new_vertice + 1;

    indices[wi + 6] = new_vertice + 3;
    indices[wi + 7] = new_vertice + 2;
    indices[wi + 8] = new_vertice + 1;

    indices[wi + 9] = new_vertice + 1;
    indices[wi + 10] = new_vertice + 3;
    indices[wi + 11] = 16;

    wi = wi + 12;
    new_vertice = new_vertice + 2;
}
indices[wi] = new_vertice;
indices[wi + 1] = 5;
indices[wi + 2] = 4;

indices[wi + 3] = new_vertice;
indices[wi + 4] = 5;
indices[wi + 5] = new_vertice + 1;

indices[wi + 6] = 17;
indices[wi + 7] = 5;
indices[wi + 8] = new_vertice + 1;

indices[wi + 9] = new_vertice + 1;
indices[wi + 10] = 17;
indices[wi + 11] = 16;

wi = wi + 12;
new_vertice = new_vertice + 2;

//bl:(9,11)
indices[wi] = 9;
indices[wi + 1] = new_vertice;
indices[wi + 2] = 8;

indices[wi + 3] = 9;
indices[wi + 4] = new_vertice;
indices[wi + 5] = 21;

indices[wi + 6] = new_vertice + 1;
indices[wi + 7] = new_vertice;
indices[wi + 8] = 21;

indices[wi + 9] = 21;
indices[wi + 10] = new_vertice + 1;
indices[wi + 11] = 20;

wi = wi + 12;

for (int i = 0; i < 8; i++) {
    indices[wi] = new_vertice;
    indices[wi + 1] = new_vertice + 2;
    indices[wi + 2] = 8;

    indices[wi + 3] = new_vertice;
    indices[wi + 4] = new_vertice + 2;
    indices[wi + 5] = new_vertice + 1;

    indices[wi + 6] = new_vertice + 3;
    indices[wi + 7] = new_vertice + 2;
    indices[wi + 8] = new_vertice + 1;

    indices[wi + 9] = new_vertice + 1;
    indices[wi + 10] = new_vertice + 3;
    indices[wi + 11] = 20;

    wi = wi + 12;
    new_vertice = new_vertice + 2;
}
indices[wi] = new_vertice;
indices[wi + 1] = 11;
indices[wi + 2] = 8;

indices[wi + 3] = new_vertice;
indices[wi + 4] = 11;
indices[wi + 5] = new_vertice + 1;

indices[wi + 6] = 23;
indices[wi + 7] = 11;
indices[wi + 8] = new_vertice + 1;

indices[wi + 9] = new_vertice + 1;
indices[wi + 10] = 23;
indices[wi + 11] = 20;

wi = wi + 12;
new_vertice = new_vertice + 2;

    
    return indices;
}*/

void createCornerFaceIndices(unsigned int * indices, unsigned int vertice_current, unsigned int corner_number, unsigned int start, unsigned int end, int writing_start) {
    indices[writing_start] = corner_number;
    indices[writing_start + 1] = start;
    indices[writing_start + 2] = vertice_current;
    int writing_index = writing_start + 3;

    for (writing_index; writing_index < writing_start+27; writing_index = writing_index + 3) {
        indices[writing_index] = corner_number;
        indices[writing_index + 1] = vertice_current;
        indices[writing_index + 2] = vertice_current + 1;
        vertice_current++;
    }
    indices[writing_index] = corner_number;
    indices[writing_index + 1] = vertice_current;
    indices[writing_index + 2] = end;
}

void createCornerRimIndices(unsigned int* indices, unsigned int vertice_current, unsigned int start_pos, unsigned int start_neg, unsigned int end_pos, unsigned int end_neg, int writing_start) {
    indices[writing_start] = start_pos;
    indices[writing_start+1] = vertice_current;
    indices[writing_start+2] = start_neg;

    indices[writing_start+3] = vertice_current + 1;
    indices[writing_start+4] = vertice_current;
    indices[writing_start+5] = start_neg;
    int writing_index = writing_start + 6;

    for (writing_index; writing_index < writing_start + 54; writing_index = writing_index + 6) {
        indices[writing_index] = vertice_current;
        indices[writing_index + 1] = vertice_current + 2;
        indices[writing_index + 2] = vertice_current + 1;

        indices[writing_index + 3] = vertice_current + 3;
        indices[writing_index + 4] = vertice_current + 2;
        indices[writing_index + 5] = vertice_current + 1;

        vertice_current = vertice_current + 2;
    }
    indices[writing_index] = vertice_current;
    indices[writing_index + 1] = end_pos;
    indices[writing_index + 2] = vertice_current + 1;

    indices[writing_index + 3] = end_neg;
    indices[writing_index + 4] = end_pos;
    indices[writing_index + 5] = vertice_current + 1;

}


int main()
{
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);



GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cards", NULL, NULL);
if (window == NULL)
{
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
}
/*normalize card measurements
float *normalized_values = normalize_card_measurements(card_width, card_height);
float y = normalized_values[1];
float x = normalized_values[0];
*/

//float * vertices = create_card_vertices(card_width, card_height, corner_radius, card_thickness);
//unsigned int * indices = create_card_indices();


float ratio = card_height / card_width;
float ratio_r = card_height / (corner_radius*3);
float ratio_t = card_height / card_thickness;
float height = 1;
float width = height / ratio;
float radius = height / ratio_r;
float thickness = height / ratio_t;
thickness = roundf(thickness * 10000) / 10000;

//all x,y values possible
float x_end = width;
float x_radius = width - radius;
x_radius = roundf(x_radius * 10000) / 10000;
float y_end = height;
float y_radius = height - radius;
y_radius = roundf(y_radius * 10000) / 10000;

//create quad_vertices
float vertices[] = {
x_end,      y_radius   , thickness,
x_end,     -y_radius   , thickness,
x_radius,   y_end      , thickness,
x_radius,   y_radius   , thickness,
x_radius,  -y_radius   , thickness, 
x_radius,  -y_end      , thickness,
-x_radius,  y_end      , thickness, 
-x_radius,  y_radius   , thickness,
-x_radius, -y_radius   , thickness,
-x_radius, -y_end      , thickness,
-x_end,     y_radius   , thickness,
-x_end,    -y_radius   , thickness,
x_end,      y_radius   , -thickness,
x_end,     -y_radius   , -thickness,
x_radius,   y_end      , -thickness,
x_radius,   y_radius   , -thickness,
x_radius,  -y_radius   , -thickness,
x_radius,  -y_end      , -thickness,
-x_radius,  y_end      , -thickness,
-x_radius,  y_radius   , -thickness,
-x_radius, -y_radius   , -thickness,
-x_radius, -y_end      , -thickness,
-x_end,     y_radius   , -thickness,
-x_end,    -y_radius   , -thickness, //72 quad vertices, [72] starting point
0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, //fill with zeros for corner vertices (9*3*2 for each corner)          
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//Bezier for corners
//corner vertices points are {(0,2,top_right);(6,10,top_left);(1,5,bottom_right);(9,11,bottom_left)}
//we are mapping 9 points along the curve


float input_array_tr[] = { vertices[0], vertices[1],vertices[6], vertices[7], x_end, y_end };
float input_array_tl[] = { vertices[18], vertices[19],vertices[30], vertices[31], -x_end, y_end };
float input_array_br[] = { vertices[3], vertices[4],vertices[15], vertices[16], x_end, -y_end };
float input_array_bl[] = { vertices[27], vertices[28],vertices[33], vertices[34], -x_end, -y_end };

int wi = 72; //writing_index
float memory_array[4] = {};
for(float t = 0.1f; t < 1.0f; t = t + 0.1f) {
    //P1 = input_array[4,5], P0,P3 der Rest
    memory_array[0] = (((1.0f - t) * (1.0f - t)) * (input_array_tr[0] - input_array_tr[4]));
    memory_array[1] = (((1.0f - t) * (1.0f - t)) * (input_array_tr[1] - input_array_tr[5]));
    memory_array[2] = ((t * t) * (input_array_tr[2] - input_array_tr[4]));
    memory_array[3] = ((t * t) * (input_array_tr[3] - input_array_tr[5]));
    vertices[wi    ] = (memory_array[0] + memory_array[2] + input_array_tr[4]);
    vertices[wi + 1] = (memory_array[1] + memory_array[3] + input_array_tr[5]);
    vertices[wi + 2] = thickness;
    vertices[wi + 3] = (memory_array[0] + memory_array[2] + input_array_tr[4]);
    vertices[wi + 4] = (memory_array[1] + memory_array[3] + input_array_tr[5]);
    vertices[wi + 5] = -thickness;
    wi = wi + 6;
};
for (float t = 0.1f; t < 1.0f; t = t + 0.1f) {
    //P1 = input_array[4,5], P0,P3 der Rest
    memory_array[0] = (((1.0f - t) * (1.0f - t)) * (input_array_tl[0] - input_array_tl[4]));
    memory_array[1] = (((1.0f - t) * (1.0f - t)) * (input_array_tl[1] - input_array_tl[5]));
    memory_array[2] = ((t * t) * (input_array_tl[2] - input_array_tl[4]));
    memory_array[3] = ((t * t) * (input_array_tl[3] - input_array_tl[5]));
    vertices[wi] = (memory_array[0] + memory_array[2] + input_array_tl[4]);
    vertices[wi + 1] = (memory_array[1] + memory_array[3] + input_array_tl[5]);
    vertices[wi + 2] = thickness;
    vertices[wi + 3] = (memory_array[0] + memory_array[2] + input_array_tl[4]);
    vertices[wi + 4] = (memory_array[1] + memory_array[3] + input_array_tl[5]);
    vertices[wi + 5] = -thickness;
    wi = wi + 6;
};
for (float t = 0.1f; t < 1.0f; t = t + 0.1f) {
    //P1 = input_array[4,5], P0,P3 der Rest
    memory_array[0] = (((1.0f - t) * (1.0f - t)) * (input_array_br[0] - input_array_br[4]));
    memory_array[1] = (((1.0f - t) * (1.0f - t)) * (input_array_br[1] - input_array_br[5]));
    memory_array[2] = ((t * t) * (input_array_br[2] - input_array_br[4]));
    memory_array[3] = ((t * t) * (input_array_br[3] - input_array_br[5]));
    vertices[wi] = (memory_array[0] + memory_array[2] + input_array_br[4]);
    vertices[wi + 1] = (memory_array[1] + memory_array[3] + input_array_br[5]);
    vertices[wi + 2] = thickness;
    vertices[wi + 3] = (memory_array[0] + memory_array[2] + input_array_br[4]);
    vertices[wi + 4] = (memory_array[1] + memory_array[3] + input_array_br[5]);
    vertices[wi + 5] = -thickness;
    wi = wi + 6;
};
for (float t = 0.1f; t < 1.0f; t = t + 0.1f) {
    //P1 = input_array[4,5], P0,P3 der Rest
    memory_array[0] = (((1.0f - t) * (1.0f - t)) * (input_array_bl[0] - input_array_bl[4]));
    memory_array[1] = (((1.0f - t) * (1.0f - t)) * (input_array_bl[1] - input_array_bl[5]));
    memory_array[2] = ((t * t) * (input_array_bl[2] - input_array_bl[4]));
    memory_array[3] = ((t * t) * (input_array_bl[3] - input_array_bl[5]));
    vertices[wi] = (memory_array[0] + memory_array[2] + input_array_bl[4]);
    vertices[wi + 1] = (memory_array[1] + memory_array[3] + input_array_bl[5]);
    vertices[wi + 2] = thickness;
    vertices[wi + 3] = (memory_array[0] + memory_array[2] + input_array_bl[4]);
    vertices[wi + 4] = (memory_array[1] + memory_array[3] + input_array_bl[5]);
    vertices[wi + 5] = -thickness;
    wi = wi + 6;
};

int size = sizeof(vertices) / sizeof(vertices[0]);
/*std::cout << size;
std::cout << "\n";
for (int i = 0; i < size;i++) {
    std::cout << vertices[i];
    std::cout << ", ";
};
std::cout << "\n \n";
*/

/*
size = sizeof(indices) / sizeof(indices[0]);
std::cout << size;
std::cout << "\n";
for (int i = 133; i < size;i++) {
    std::cout << indices[i];
    std::cout << ", ";
};*/

float vert[480] = {};
int help_one = 0;
for (int i = 0; i < 480; i=i+5) {
    vert[i] = vertices[help_one];
    vert[i+1] = vertices[help_one+1]; 
    vert[i + 2] = vertices[help_one + 2];
    vert[i + 3] = ((vertices[help_one] / x_end) * 0.5f) + 0.5f;
    vert[i + 4] = ((vertices[help_one + 1] / y_end) * 0.5f) + 0.5f;
    help_one = help_one + 3;
}
//((vertices[help_one] / x_end) * 0.5f)+0.5f;
//((vertices[help_one+1] / y_end) * 0.5f) + 0.5f;

//create normals:
//96 different vertices, 88 need to be at least doubled (hard borders)
// 
//define face_vertices with their respective normals (48 per face * 8 values = 384):
//      - front_face:
float verts[1472] = {};
help_one = 0;
for (int i = 0; i < 384; i = i + 8) {
    while (vert[help_one + 2] < 0) {
        help_one = help_one + 5;
    };
    verts[i] = vert[help_one];
    verts[i + 1] = vert[help_one + 1];
    verts[i + 2] = vert[help_one + 2];
    verts[i + 3] = vert[help_one + 3];
    verts[i + 4] = vert[help_one + 4];
    verts[i + 5] = 0.0f;
    verts[i + 6] = 0.0f;
    verts[i + 7] = 1.0f;
    help_one = help_one + 5;
}
//      - back_face:
help_one = 0;
for (int i = 384; i < 768; i = i + 8) {
    while (vert[help_one + 2] > 0) {
        help_one = help_one + 5;
    };
    verts[i] = vert[help_one];
    verts[i + 1] = vert[help_one + 1];
    verts[i + 2] = vert[help_one + 2];
    verts[i + 3] = vert[help_one + 3];
    verts[i + 4] = vert[help_one + 4];
    verts[i + 5] = 0.0f;
    verts[i + 6] = 0.0f;
    verts[i + 7] = -1.0f;
    help_one = help_one + 5;
}
//      - rim (88 vertices * 8 values = 704)
help_one = 0;
for (int i = 768; i < 1472; i = i + 8) {
    while (fabs(vert[help_one+1]) == y_radius && fabs(vert[help_one]) == x_radius){
        help_one = help_one + 5;
    };
    verts[i] = vert[help_one];
    verts[i + 1] = vert[help_one + 1];
    verts[i + 2] = vert[help_one + 2];
    verts[i + 3] = vert[help_one + 3];
    verts[i + 4] = vert[help_one + 4];
    if (vert[help_one] == x_end) {
        verts[i + 5] = 1.0f;
        verts[i + 6] = 0.0f;
        verts[i + 7] = 0.0f;
    }else
    if (vert[help_one] == -x_end) {
        verts[i + 5] = -1.0f;
        verts[i + 6] = 0.0f;
        verts[i + 7] = 0.0f;
    }else
    if (vert[help_one+1] == y_end) {
        verts[i + 5] = 0.0f;
        verts[i + 6] = 1.0f;
        verts[i + 7] = 0.0f;
    }else
    if (vert[help_one+1] == -y_end) {
        verts[i + 5] = 0.0f;
        verts[i + 6] = -1.0f;
        verts[i + 7] = 0.0f;
    }
    //todo: calculate normals for corners
    else {
        verts[i + 5] = 0.0f;
        verts[i + 6] = 0.0f;
        verts[i + 7] = 0.0f;
    }
    help_one = help_one + 5;
}
size = sizeof(verts) / sizeof(verts[0]);
std::cout << size;
std::cout << "\n";
for (int i = 0; i < size;i = i + 8) {
    std::cout << verts[i];
    std::cout << ", ";
    std::cout << verts[i + 1];
    std::cout << ", ";
    std::cout << verts[i + 2];
    std::cout << ", ";
    std::cout << verts[i + 3];
    std::cout << ", ";
    std::cout << verts[i + 4];
    std::cout << ", ";
    std::cout << verts[i + 5];
    std::cout << ", ";
    std::cout << verts[i + 6];
    std::cout << ", ";
    std::cout << verts[i + 7];
    std::cout << ", ";
    std::cout << "\n";
};
std::cout << "\n \n";


//indices
unsigned int indices[] = {
    //front_face
    0,4,1,
    0,3,4,
    3,2,6,
    6,3,7,
    3,7,4,
    7,8,4,
    7,10,8,
    10,8,11,
    8,9,5,
    8,5,4, //index 29, 30 next

    //corners_front 10 triangles per corner (40*3)
    
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,

    //back_face
    48,52,49,
    48,51,52,
    51,50,54,
    54,51,55,
    51,55,52,
    55,56,52,
    55,58,56,
    58,56,59,
    56,57,53,
    56,53,52, //index 179, 180 next
    
    //corners_back 10 triangles per corner (40)
    
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,

    //rim counterclockwise (starts at 300)
    96,97,104,
    104,97,105,

    98,100,106,
    106,100,108,

    99,101,107,
    107,101,109,

    102,103,110,
    110,103,111,
    //corners (per corner 60 entries for 20 triangles)
    
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,
    0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,  0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//input corner indices front (3,7,4,8)
createCornerFaceIndices(indices, 12, 3, 0, 2, 30);
createCornerFaceIndices(indices, 21, 7, 6, 10, 60);
createCornerFaceIndices(indices, 30, 4, 1, 5, 90);
createCornerFaceIndices(indices, 39, 8, 9, 11, 120);


//input corner indices back (51, 55, 52, 56)
createCornerFaceIndices(indices, 60, 51, 48, 50, 180);
createCornerFaceIndices(indices, 69, 55, 54, 58, 210);
createCornerFaceIndices(indices, 78, 52, 49, 53, 240);
createCornerFaceIndices(indices, 87, 56, 57, 59, 270);

//input rim corner indices
createCornerRimIndices(indices, 112, 96, 104, 98, 106, 324);
createCornerRimIndices(indices, 130, 100, 104, 102, 110, 384);
createCornerRimIndices(indices, 148, 97, 104, 99, 107, 444);
createCornerRimIndices(indices, 166, 101, 104, 103, 111, 504);

size = sizeof(indices) / sizeof(indices[0]);
std::cout << size;
std::cout << "\n";
for (int i = 0; i < size;i = i + 3) {
    std::cout << indices[i];
    std::cout << ", ";
    std::cout << indices[i + 1];
    std::cout << ", ";
    std::cout << indices[i + 2];
    std::cout << "\n";
};
std::cout << "\n \n";










//corner coordinates tr:(0,2), tl:(6,10), br:(1,5), bl:(9,11), first new vertice=24 
/*



unsigned int texture = load_texture();
GLenum err;
while ((err = glGetError()) != GL_NO_ERROR) {
    cerr << "1. OpenGL error: " << err << endl;
}*/
/*
float vert[] = {
    // positions          // texture coords
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, // top right
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,  // top left 

    0.5f,  0.5f, 0.5f,   1.0f, 1.0f, // top right
     0.5f, -0.5f, 0.5f,   1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.5f,   0.0f, 1.0f  // top left 
};
unsigned int indices_test[] = {
    0, 1, 3, // first triangle
    1, 2, 3,  // second triangle 

    1, 5, 2, // first triangle
    5, 2, 6,  // second triangle

    0, 4, 1, // first triangle
    4, 1, 5, // second triangle

    2, 6, 3, // first triangle
    6, 3, 7,  // second triangle

    3, 7, 4, // first triangle
    7, 4, 0, // second triangle

    4, 5, 7, // first triangle
    5, 6, 7,  // second triangle
};*/

size = sizeof(indices) / sizeof(indices[0]);

//creation of buffer object, array and element object
unsigned int VBO, VAO, EBO;
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glGenBuffers(1, &EBO);


glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);

glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
glEnableVertexAttribArray(2);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//unsigned int size_of_indices_array = sizeof(indices)/sizeof(indices[1]);










//vertex shader
string helpString;
helpString = read_file("vertex_shader_sourcecode.txt");
const char* vertexShaderSourcecode = helpString.c_str();
unsigned int vertexShader;
vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSourcecode, NULL);
glCompileShader(vertexShader);

//fragment shader
helpString = read_file("fragment_shader_sourcecode.txt");
const char* fragmentShaderSourcecode = helpString.c_str();
unsigned int fragmentShader;
fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fragmentShaderSourcecode, NULL);
glCompileShader(fragmentShader);

//check compile success
GLint iOK;
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &iOK);
if (!iOK)
{
    GLint iLength;
    GLchar* pcMessage;
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &iLength);
    pcMessage = new GLchar[iLength];
    glGetShaderInfoLog(vertexShader, iLength, &iLength, pcMessage);
    std::cout << "compile error: " << pcMessage << std::endl;
    delete[] pcMessage;
}

glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &iOK);
if (!iOK)
{
    GLint iLength;
    GLchar* pcMessage;
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &iLength);
    pcMessage = new GLchar[iLength];
    glGetShaderInfoLog(fragmentShader, iLength, &iLength, pcMessage);
    std::cout << "compile error: " << pcMessage << std::endl;
    delete[] pcMessage;
}

//attach to program
prg_id = glCreateProgram();
glAttachShader(prg_id, vertexShader);
glAttachShader(prg_id, fragmentShader);
glLinkProgram(prg_id);
glGetProgramiv(prg_id, GL_LINK_STATUS, &iOK);
if (!iOK) {
    GLint iLength;
    GLchar* pcMessage;
    glGetProgramiv(fragmentShader, GL_INFO_LOG_LENGTH, &iLength);
    pcMessage = new GLchar[iLength];
    glGetProgramInfoLog(prg_id, 512, NULL, pcMessage);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << pcMessage << std::endl;
}
glUseProgram(prg_id);


//texture
unsigned int texture1, texture2, rainbow;
glGenTextures(1, &texture1);
glBindTexture(GL_TEXTURE_2D, texture1);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

GLenum err;
while ((err = glGetError()) != GL_NO_ERROR) {
    cerr << "glTexParameteri. OpenGL error: " << err << endl;
}

int width_tex, height_tex, nrChannels;
unsigned char *data = stbi_load("images/terramorphic_expanse.jpg", &width_tex, &height_tex, &nrChannels, 0);


if (data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex, height_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load texture1" << std::endl;
}

glGenTextures(1, &texture2);
glBindTexture(GL_TEXTURE_2D, texture2);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

while ((err = glGetError()) != GL_NO_ERROR) {
    cerr << "glTexParameteri. OpenGL error: " << err << endl;
}

data = stbi_load("images/card_back.jpeg", &width_tex, &height_tex, &nrChannels, 0);
if (data)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex, height_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load texture2" << std::endl;
}

//rainbow_texture

glGenTextures(2, &rainbow);
glBindTexture(GL_TEXTURE_2D, rainbow);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

while ((err = glGetError()) != GL_NO_ERROR) {
    cerr << "glTexParameteri. OpenGL error: " << err << endl;
}

data = stbi_load("images/rainbow_gradient.jpg", &width_tex, &height_tex, &nrChannels, 0);
if (data)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_tex, height_tex, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}
else
{
    std::cout << "Failed to load rainbow_gradient" << std::endl;
}
while ((err = glGetError()) != GL_NO_ERROR) {
    cerr << "glTexParameteri 2. OpenGL error: " << err << endl;
}

stbi_image_free(data);


int tex_location = glGetUniformLocation(prg_id, "texture1");
glUniform1i(tex_location, 0);
tex_location = glGetUniformLocation(prg_id, "texture2");
glUniform1i(tex_location, 1);
tex_location = glGetUniformLocation(prg_id, "rainbow");
glUniform1i(tex_location, 2);

//perspective matrix SCR_HEIGHT SCR_WIDTH
update_perspective_matrix(SCR_WIDTH, SCR_HEIGHT);

//model_view matrix
update_modelviewmatrix();

//light_definitions
int light_location = glGetUniformLocation(prg_id, "lightPos");
glUniform3f(light_location, 6.0f, 2.0f, 5.0f);

glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

glDisable(GL_CULL_FACE);
glEnable(GL_DEPTH_TEST);
//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

while (!glfwWindowShouldClose(window))
{
    processInput(window);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, rainbow);

    glUseProgram(prg_id);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, (void*)0);
    // check OpenGL error
    while ((err = glGetError()) != GL_NO_ERROR) { 
        cerr << "1. OpenGL error: " << err << endl;
    }
    
    glfwSwapBuffers(window);
    glfwPollEvents();
   
    
}
glfwTerminate();
return 0;

}

