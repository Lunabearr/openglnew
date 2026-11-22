
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Person1.h.cpp"

#include <cstdlib>
#include <iostream>

// SCREEN SETTINGS
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

// OBJECT POSITIONS
const float NOTE_START_X = 20.0f;
const float NOTE_START_Y = 70.0f;

const float RECORD_POSITION_X = -300.0f;
const float RECORD_POSITION_Y = -25.0f;

const float SPEAKER_POSITION_X = 360.0f;
const float SPEAKER_POSITION_Y = -140.0f;

// TRANSFORMATION VARIABLES

// Musical note translation
float noteOffsetY = 0.0f;
float noteDirection = 1.0f;

// Vinyl record rotation
float recordRotation = 0.0f;

// Speaker scaling
float speakerScale = 1.0f;

// ANIMATION SETTINGS
const float NOTE_SPEED = 1.5f;
const float NOTE_MAX_HEIGHT = 40.0f;
const float NOTE_MIN_HEIGHT = -40.0f;

// Speaker scaling limits
const float MIN_SCALE = 0.5f;
const float MAX_SCALE = 2.0f;
const float SCALE_STEP = 0.1f;


// DISPLAY FUNCTION
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw background
    drawBackground();

    // OBJECT 1: MUSICAL NOTE
    // Automatic translation
    glPushMatrix();

    glTranslatef(
        NOTE_START_X,
        NOTE_START_Y + noteOffsetY,
        0.0f
    );

    drawNote();

    glPopMatrix();

    // OBJECT 2: VINYL RECORD
    // Mouse-click rotation
    glPushMatrix();

    glTranslatef(
        RECORD_POSITION_X,
        RECORD_POSITION_Y,
        0.0f
    );

    glRotatef(
        recordRotation,
        0.0f,
        0.0f,
        1.0f
    );

    drawRecord();

    glPopMatrix();

    // OBJECT 3: SPEAKER
    // Mouse-wheel scaling
    glPushMatrix();

    glTranslatef(
        SPEAKER_POSITION_X,
        SPEAKER_POSITION_Y,
        0.0f
    );

    glScalef(
        speakerScale,
        speakerScale,
        1.0f
    );

    drawSpeaker();

    glPopMatrix();

    glutSwapBuffers();
}


// AUTOMATIC TRANSLATION
// Musical note moves upward and downward
void timer(int value)
{
    noteOffsetY += NOTE_SPEED * noteDirection;

    // Reverse direction at upper limit
    if (noteOffsetY >= NOTE_MAX_HEIGHT)
    {
        noteOffsetY = NOTE_MAX_HEIGHT;
        noteDirection = -1.0f;
    }

    // Reverse direction at lower limit
    else if (noteOffsetY <= NOTE_MIN_HEIGHT)
    {
        noteOffsetY = NOTE_MIN_HEIGHT;
        noteDirection = 1.0f;
    }

    glutPostRedisplay();

    // Repeat every 16 milliseconds
    glutTimerFunc(16, timer, 0);
}


// MOUSE CLICK ROTATION
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON &&
        state == GLUT_DOWN)
    {
        recordRotation += 15.0f;

        if (recordRotation >= 360.0f)
        {
            recordRotation -= 360.0f;
        }

        glutPostRedisplay();
    }
}


// MOUSE WHEEL SCALING
void mouseWheel(int wheel, int direction, int x, int y)
{
    if (direction > 0)
    {
        speakerScale += SCALE_STEP;
    }
    else if (direction < 0)
    {
        speakerScale -= SCALE_STEP;
    }

    if (speakerScale < MIN_SCALE)
    {
        speakerScale = MIN_SCALE;
    }

    if (speakerScale > MAX_SCALE)
    {
        speakerScale = MAX_SCALE;
    }

    glutPostRedisplay();
}


// KEYBOARD CONTROL
void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        glutLeaveMainLoop();
    }
}


// WINDOW RESHAPE
void reshape(int width, int height)
{
    if (height == 0)
    {
        height = 1;
    }

    glViewport(0, 0, width, height);

    float aspect = static_cast<float>(width) / height;

    float halfWidth = 800.0f;
    float halfHeight = 450.0f;

    if (aspect > (16.0f / 9.0f))
    {
        halfWidth = halfHeight * aspect;
    }
    else
    {
        halfHeight = halfWidth / aspect;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(
        -halfWidth,
        halfWidth,
        -halfHeight,
        halfHeight,
        -1.0,
        1.0
    );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


// MAIN FUNCTION
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    // Configure OpenGL 2.1 compatibility
    glutInitContextVersion(2, 1);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    int window = glutCreateWindow(
        "Music Room - Final Project"
    );

    // Initialize GLEW
    GLenum glewStatus = glewInit();

    if (glewStatus != GLEW_OK)
    {
        std::cerr
            << "GLEW initialization failed: "
            << glewGetErrorString(glewStatus)
            << std::endl;

        glutDestroyWindow(window);
        return 1;
    }

    // Clear initialization errors
    while (glGetError() != GL_NO_ERROR)
    {
    }

    // Initialize Person 1's graphics
    if (!initMusicRoomGraphics())
    {
        std::cerr
            << "Music Room graphics initialization failed."
            << std::endl;

        glutDestroyWindow(window);
        return 1;
    }

    glClearColor(0.07f, 0.09f, 0.14f, 1.0f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMouseFunc(mouse);
    glutMouseWheelFunc(mouseWheel);

    // Start automatic animation
    glutTimerFunc(16, timer, 0);

    // Configure window closing behavior
    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

    glutCloseFunc(destroyMusicRoomGraphics);

    // Fullscreen mode
    glutFullScreen();

    std::cout << "Music Room application started!" << std::endl;
    std::cout << "Left Click: Rotate Vinyl Record" << std::endl;
    std::cout << "Mouse Wheel: Scale Speaker" << std::endl;
    std::cout << "Press ESC to Exit" << std::endl;

    glutMainLoop();

    return 0;
}
