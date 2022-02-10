// badprog.com
#include <iostream>
#include <freeglut.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// This function draws the grid
void drawGrid(int rows, int columns){
    glBegin(GL_LINES);

    // We draw one line for each row which stretches from the far left of the screen to the far right of the window
    for(int i = 0; i < rows; i++){
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(-1.0f, (i/(float)rows * 2) - 1.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f(1.0f, (i/(float)rows * 2) - 1.0f);
    }

    // And we draw one line for each column which streches from the bottom to the top of the window
    for(int i = 0; i < columns; i++){
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f((i/(float)columns * 2) - 1.0f, 1.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex2f((i/(float)columns * 2) - 1.0f, -1.0f);
    }
    glEnd();
}

// This function creates an opengl texture from raw image data stored in unsigned char* pixels
GLuint MakeTexture(unsigned char* pixels, int w, int h){
    GLuint tid;
    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    return tid;
}

// This function draws a picture in every cell of the grid
void drawPictures(std::vector<GLuint> textures, int rows, int columns){
    // Width and height of every cell
    float width = 2.0f / columns;
    float height = 2.0f / rows;
    glEnable(GL_TEXTURE_2D); // Enables texturing
    glActiveTexture(GL_TEXTURE0);

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            GLuint index = i * columns + j; // Index of texture in textures array
           std::cout << index << std::endl;

            // We bind the current texture
           glBindTexture(GL_TEXTURE_2D, textures[index]);
           glBegin(GL_QUADS); // Begin rendering

            glTexCoord2f(0.0f, 0.0f);
            glVertex2f((width * j) - 1.0f, (height * i) - 1.0f);

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f((width * j) - 1.0f, (height * i) - 1.0f + height);

            glTexCoord2f(1.0f, 1.0f);
            glVertex2f((width * j) - 1.0f + width, (height * i) - 1.0f + height);

            glTexCoord2f(1.0f, 0.0f);
            glVertex2f((width * j) - 1.0f + width, (height * i) - 1.0f);
            glEnd();
        }
    }
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    const int rows = 4;
    const int columns = 3;
    stbi_set_flip_vertically_on_load(true); // Making sure the image flips vertically on load

    // Loading all the textures and storing them in a vector of texture IDs(GLuint)
    std::vector<GLuint> textures;
    for(int i = 1; i <= rows*columns; i++){
        std::string fileName = std::to_string(i);
        fileName += ".bmp"; // Adding bmp extension to file name
        int w, h, c; // Width, height and number of channels per pixel in image
        unsigned char* data = stbi_load(fileName.c_str(), &w, &h, &c, 0); // Loading the image
        textures.push_back(MakeTexture(data, w, h)); // Loading the texture and storing it in the textures array
        stbi_image_free(data); // De-allocating the memory used for that image
    }

    drawPictures(textures, rows, columns); // Drawing all the pictures
    // We draw the grid *after* we draw the pictures to make sure the grid shows ontop of the pictures
    drawGrid(4, 3);

    glFlush();
}

int main(int argc, char** argv)
{
    // Initializing GLUT
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_SINGLE);
    glutCreateWindow("Window");
    glLineWidth(10.0f); // A large line width to clearly see the grid
    glClearColor(1.0, 1.0, 1.0, 0.0); // Setting the clear color to white for white background
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
