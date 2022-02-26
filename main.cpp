#include <iostream>
#include <fstream>
#include <vector>
#include <GL/freeglut.h>

const int rows = 4;
const int columns = 3;
std::vector<GLuint> originalTextures;
std::vector<GLuint> textures;
int cellstates[rows * columns];
int originalCellstates[rows * columns];
const float squareHeight = 0.233f;
const float squareWidth = 0.323;
int img1 = -1;
int screenWidth = 0;
int screenHeight = 0;
bool hintUsed = false;
bool firstTime = true;


void drawCell(float x, float y, int t){
    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);

    switch(t){
        case 0:
        glColor3f(0.0f, 0.0f, 0.0f);
        break;
        case 1:
        glColor3f(1.0f, 0.0f, 0.0f);
        break;
        case 2:
        glColor3f(1.0f, 1.0f, 0.0f);
        break;
        case 3:
        glColor3f(0.0f, 1.0f, 0.0f);
        break;
    }

    glVertex2f(x - squareWidth, y - squareHeight);
    glVertex2f(x + squareWidth, y - squareHeight);

    glVertex2f(x + squareWidth, y - squareHeight);
    glVertex2f(x + squareWidth, y + squareHeight);

    glVertex2f(x - squareWidth, y - squareHeight);
    glVertex2f(x - squareWidth, y + squareHeight);

    glVertex2f(x - squareWidth, y + squareHeight);
    glVertex2f(x + squareWidth, y + squareHeight);

    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
}

// This function draws the grid
void drawGrid(int rows, int columns){
    glBegin(GL_LINES);
    for(int y = 0; y < rows; y++){
        for(int x = 0; x < columns; x++){
            float xPos = (x / (float)(columns)) * 2.0f - 1.0f;
            float yPos = (y / (float)(rows)) * 2.0f - 1.0f;
            drawCell(xPos + 1.0f / columns, yPos + 1.0f / rows, cellstates[y * columns + x]);
        }
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
    std::ifstream f;
    f.open(file_name, std::ios::in | std::ios::binary);

    if(!f.is_open()){
        std::cout << "Failed to open file" << std::endl;
        return nullptr;
    }

    const int fileHeaderSize = 14;
    const int infoHeaderSize = 40;

    unsigned char fileHeader[fileHeaderSize];
    f.read(reinterpret_cast<char*>(fileHeader), fileHeaderSize);

    if(fileHeader[0] != 'B' || fileHeader[1] != 'M'){
        std::cout << "File is not a bitmap" << std::endl;
        f.close();
        return nullptr;
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
void drawPictures(int rows, int columns){
    // Width and height of every cell
    float width = 2.0f / columns;
    float height = 2.0f / rows;
    glEnable(GL_TEXTURE_2D); // Enables texturing
    glActiveTexture(GL_TEXTURE0);
    glColor3f(1.0f, 1.0f, 1.0f);
d
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            GLuint index = i * columns + j; // Index of texture in textures array

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
    glDisable(GL_TEXTURE_2D); // Enables texturing

    glColor3f(1.0f, 1.0f, 1.0f);

}

void loadTextures(){
    std::vector<const char*> fileNames;
    fileNames.push_back("1.bmp");
    fileNames.push_back("2.bmp");
    fileNames.push_back("3.bmp");
    fileNames.push_back("4.bmp");
    fileNames.push_back("5.bmp");
    fileNames.push_back("6.bmp");
    fileNames.push_back("7.bmp");
    fileNames.push_back("8.bmp");
    fileNames.push_back("9.bmp");
    fileNames.push_back("10.bmp");
    fileNames.push_back("11.bmp");
    fileNames.push_back("12.bmp");

    // Loading all the textures and storing them in a vector of texture IDs(GLuint)
    for(int i = 0; i < rows*columns; i++){
        int w, h; // Width, height and number of channels per pixel in imagedfasdfasdfASDAsdaSD As 
        unsigned char* data = ReadImage(fileNames[i], &w, &h);// Loading the image
        std::cout << "width: " << w << " height: " << h << std::endl;

        textures.push_back(MakeTexture(data, w, h)); // Loading the texture and storing it in the textures array
        delete[] data;
    }

    originalTextures = textures;
}

void showHint(){
    
}

void switchTextures(int a, int b){
    GLuint tmp = textures[a];
    textures[a] = textures[b];
    textures[b] = tmp;
}

void display(void);

void randomize(int value){
    for(int i = 0; i < 20; i++){
        switchTextures(rand() % 12, rand() % 12);
    }
    display();
    glutSwapBuffers();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    if(firstTime){
        firstTime = false;
        drawPictures(rows, columns); // Drawing all the pictures
        drawGrid(4, 3);
        glutTimerFunc(2000, randomize, 0);
    }else{
        drawPictures(rows, columns); // Drawing all the pictures
         // We draw the grid *after* we draw the pictures to make sure the grid shows ontop of the pictures
        drawGrid(4, 3);
    }

    glFlush();
}

void reshape(int w, int h){
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

bool checkWin(){
    for(int i = 0; i < 12; i++){
        if(textures[i] != originalTextures[i]){
            return false;
        }
    }
    return true;
}

void mouseCallback(int button, int s, int x, int y){
    if(s != 0){
        return;
    }

    if(button == 0){
        if(img1 != -1){
            int ix = x / (screenWidth / columns);
            int iy = (screenHeight - y) / (screenHeight / rows);
            GLuint tmp = textures[iy * columns + ix];
            textures[iy * columns + ix] = textures[img1];
            textures[img1] = tmp;
            cellstates[img1] = 0;
            img1 = -1;
        }else{
            int ix = x / (screenWidth / columns);
            int iy = (screenHeight - y) / (screenHeight / rows);
            img1 = iy * columns + ix;
            cellstates[img1] = 2;
        }

        display();
        glutSwapBuffers();
    }
    if(checkWin()){
        std::cout << "Congratulations! You have won the game :-)" << std::endl;
    }
}

void restoreHint(int value){
    for(int i = 0; i < 12; i++){
        cellstates[i] = originalCellstates[i];
    }
    display();
    glutSwapBuffers();
}

void keyCallback(unsigned char c, int k, int i){
    if(c == 'h' && !hintUsed){
        hintUsed = true;
        showHint();
        for(int i = 0; i < 12; i++){
            originalCellstates[i] = cellstates[i];
        }
        for(int i = 0; i < 12; i++){
            if(textures[i] == originalTextures[i]){
                cellstates[i] = 3;
            }else{
                cellstates[i] = 1;
            }
        }
        display();
        glutSwapBuffers();
        glutTimerFunc(2000, restoreHint, 0);
    }
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
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseCallback);
    glutKeyboardFunc(keyCallback);
    loadTextures();
    for(int i = 0; i < 12; i++){
        cellstates[i] = 0;
    }
    srand(time(0));
    

    glutMainLoop();
    return 0;
}
