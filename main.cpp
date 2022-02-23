#include <iostream>
#include <fstream>
#include <vector>
#include <GLUT/glut.h>
using namespace std;

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

unsigned char* ReadImage(const char* file_name, int* w, int* h){
    ifstream f;
    f.open(file_name, ios::in | ios::binary);

    if(!f.is_open()){
        cout << "Failed to open file" << endl;
        return NULL;
    }

    const int fileHeaderSize = 14;
    const int infoHeaderSize = 40;

    unsigned char fileHeader[fileHeaderSize];
    f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

    if(fileHeader[0] != 'B' || fileHeader[1] != 'M'){
        cout << "File is not a bitmap" << endl;
        f.close();
        return NULL;
    }

    unsigned char infoHeader[infoHeaderSize];
    f.read(reinterpret_cast<char*>(infoHeader), infoHeaderSize);

    int fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);
    *w = infoHeader[4] + (infoHeader[5] << 8) + (infoHeader[6] << 16) + (infoHeader[7] << 24);
    *h = infoHeader[8] + (infoHeader[9] << 8) + (infoHeader[10] << 16) + (infoHeader[11] << 24);

    unsigned char* data = new unsigned char[*w * *h * 3];

    const int paddingAmount = ((4 - (*w * 3) % 4) % 4);
    int index = 0;

    for(int y = 0; y < *h; y++){
        for(int x = 0; x < *w; x++){
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color), 3);
            data[index + 0] = color[2];
            data[index + 1] = color[1];
            data[index + 2] = color[0];
            index+=3;
        }
        f.ignore(paddingAmount);
    }

    f.close();
    return data;
}

// This function draws a picture in every cell of the grid
void drawPictures(vector<GLuint> textures, int rows, int columns){
    // Width and height of every cell
    float width = 2.0f / columns;
    float height = 2.0f / rows;
    glEnable(GL_TEXTURE_2D); // Enables texturing

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            GLuint index = i * columns + j; // Index of texture in textures array
           cout << index << endl;

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

    vector<const char*> fileNames = {
        "1.bmp",
        "2.bmp",
        "3.bmp",
        "4.bmp",
        "5.bmp",
        "6.bmp",
        "7.bmp",
        "8.bmp",
        "9.bmp",
        "10.bmp",
        "11.bmp",
        "12.bmp"
    };

    // Loading all the textures and storing them in a vector of texture IDs(GLuint)
    vector<GLuint> textures;
    for(int i = 0; i < rows*columns; i++){
        int w, h; // Width, height and number of channels per pixel in image
        unsigned char* data = ReadImage(fileNames[i], &w, &h);// Loading the image
        cout << "width: " << w << " height: " << h << endl;
        textures.push_back(MakeTexture(data, w, h)); // Loading the texture and storing it in the textures array
        delete[] data;
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
