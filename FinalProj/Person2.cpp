#include <GL/glew.h>  // GLEW must be included first
#include <GL/freeglut.h>

#include "Person1.h.cpp"

#include <cstdlib>
#include <iostream>

// ----------------------------------------
// SCREEN SETTINGS
// ----------------------------------------

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

// ----------------------------------------
// OBJECT POSITIONS
// Matching Person 1's original coordinates
// ----------------------------------------

// Musical note
const float NOTE_START_X = 20.0f;
const float NOTE_START_Y = 70.0f;

// Vinyl record
const float RECORD_POSITION_X = -300.0f;
const float RECORD_POSITION_Y = -25.0f;

// Speaker
const float SPEAKER_POSITION_X = 360.0f;
const float SPEAKER_POSITION_Y = -140.0f;


// ----------------------------------------
// TRANSFORMATION VARIABLES
// ----------------------------------------

// Musical note translation
float noteOffsetY = 0.0f;

// Vinyl record rotation
float recordRotation = 0.0f;

// Speaker scaling
float speakerScale = 1.0f;


// ----------------------------------------
// ANIMATION SETTINGS
// ----------------------------------------

const float NOTE_SPEED = 1.5f;
const float NOTE_MAX_HEIGHT = 250.0f;

// Speaker scaling limits
const float MIN_SCALE = 0.5f;
const float MAX_SCALE = 2.0f;
const float SCALE_STEP = 0.1f;


// ----------------------------------------
// DISPLAY FUNCTION
// ----------------------------------------

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw the background
    drawBackground();


    // ====================================
    // OBJECT 1: MUSICAL NOTE
    // AUTOMATIC TRANSLATION
    // ====================================

    glPushMatrix();

    glTranslatef(
        NOTE_START_X,
        NOTE_START_Y + noteOffsetY,
        0.0f
    );

    drawNote();

    glPopMatrix();


    // ====================================
    // OBJECT 2: VINYL RECORD
    // MOUSE-CLICK ROTATION
    // ====================================

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


    // ====================================
    // OBJECT 3: SPEAKER
    // MOUSE-WHEEL SCALING
    // ====================================

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


    // Display the completed scene
    glutSwapBuffers();
}


// ----------------------------------------
// AUTOMATIC TRANSLATION
// Musical note moves upward
// ----------------------------------------

void timer(int value)
{
    noteOffsetY += NOTE_SPEED;

    // Return note to starting position
    // when it reaches the maximum height.

    if (noteOffsetY >= NOTE_MAX_HEIGHT)
    {
        noteOffsetY = 0.0f;
    }

    // Request screen update
    glutPostRedisplay();

    // Repeat animation every 16 milliseconds
    glutTimerFunc(16, timer, 0);
}


// ----------------------------------------
// MOUSE CLICK ROTATION
// Vinyl record rotates when clicked
// ----------------------------------------

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON &&
        state == GLUT_DOWN)
    {
        recordRotation += 15.0f;

        // Keep rotation angle between 0 and 360
        if (recordRotation >= 360.0f)
        {
            recordRotation -= 360.0f;
        }

        glutPostRedisplay();
    }
}


// ----------------------------------------
// MOUSE WHEEL SCALING
// Speaker becomes bigger or smaller
// ----------------------------------------

void mouseWheel(
    int wheel,
    int direction,
    int x,
    int y)
{
    if (direction > 0)
    {
        // Scroll upward
        speakerScale += SCALE_STEP;
    }
    else if (direction < 0)
    {
        // Scroll downward
        speakerScale -= SCALE_STEP;
    }

    // Set minimum size
    if (speakerScale < MIN_SCALE)
    {
        speakerScale = MIN_SCALE;
    }

    // Set maximum size
    if (speakerScale > MAX_SCALE)
    {
        speakerScale = MAX_SCALE;
    }

    glutPostRedisplay();
}


// ----------------------------------------
// KEYBOARD CONTROL
// ESC exits the application
// ----------------------------------------

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        glutLeaveMainLoop();
    }
}


// ----------------------------------------
// WINDOW RESHAPE
// Maintains scene proportions
// ----------------------------------------

void reshape(int width, int height)
{
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height);

    float aspect =
        static_cast<float>(width) / height;

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


// ----------------------------------------
// MAIN FUNCTION
// ----------------------------------------

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    // Configure OpenGL 2.1 compatibility
    glutInitContextVersion(2, 1);

    // Enable double buffering and RGB colors
    glutInitDisplayMode(
        GLUT_DOUBLE | GLUT_RGB
    );

    glutInitWindowSize(
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    // Create application window
    int window = glutCreateWindow(
        "Music Room - Final Project"
    );

    // Initialize GLEW after creating the window
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

    // Clear any errors generated during GLEW initialization
    while (glGetError() != GL_NO_ERROR)
    {
    }

    // Initialize Person 1's graphics and VBOs
    if (!initMusicRoomGraphics())
    {
        std::cerr
            << "Music Room graphics initialization failed."
            << std::endl;

        glutDestroyWindow(window);
        return 1;
    }

    // Configure background rendering
    glClearColor(
        0.07f,
        0.09f,
        0.14f,
        1.0f
    );

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Register application callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Register mouse interactions
    glutMouseFunc(mouse);
    glutMouseWheelFunc(mouseWheel);

    // Start automatic translation
    glutTimerFunc(16, timer, 0);

    // Configure FreeGLUT to return after closing
    glutSetOption(
        GLUT_ACTION_ON_WINDOW_CLOSE,
        GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );

    // Release VBO resources when the window closes
    glutCloseFunc(destroyMusicRoomGraphics);

    // Enable full-screen mode
    glutFullScreen();

    std::cout
        << "Music Room application started!"
        << std::endl;

    std::cout
        << "Left Click: Rotate Vinyl Record"
        << std::endl;

    std::cout
        << "Mouse Wheel: Scale Speaker"
        << std::endl;

    std::cout
        << "Press ESC to Exit"
        << std::endl;

    // Start rendering loop
    glutMainLoop();

    return 0;
}
