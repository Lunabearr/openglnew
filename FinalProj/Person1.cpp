#define MUSIC_ROOM_NO_PREVIEW
// Music Room - Person 1 graphics
// AI assistance: Codex
// Define MUSIC_ROOM_NO_PREVIEW when using another main().
#include <GL/glew.h> // Include GLEW first.
#include <GL/freeglut.h>
// Call after creating the window and initializing GLEW.
bool initMusicRoomGraphics();
void destroyMusicRoomGraphics(); // Call before closing the window.

// Background: 1600 x 900 units.
void drawBackground();
void drawNote();
// Record: centered at (0, 0).
void drawRecord();
// Speaker: centered at (0, 0).
void drawSpeaker();

// Starting positions
constexpr float RECORD_X = -300.0f;
constexpr float RECORD_Y = -25.0f;
constexpr float NOTE_X = 20.0f;
constexpr float NOTE_Y = 70.0f;
constexpr float SPEAKER_X = 360.0f;
constexpr float SPEAKER_Y = -140.0f;
#include <cmath>
#include <cstddef>
#include <vector>

namespace {
constexpr float PI = 3.14159265358979323846f;
struct Color { float r, g, b; };
struct Vertex { float x, y, r, g, b; };
struct Batch { GLenum primitive; GLint first; GLsizei count; };
struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<Batch> batches;
    GLuint vbo = 0;
};
Mesh background, note, record, speaker;
bool ready = false;

constexpr Color cream{0.95f, 0.89f, 0.75f};
constexpr Color dark{0.07f, 0.09f, 0.14f};
constexpr Color teal{0.27f, 0.78f, 0.74f};
constexpr Color coral{0.98f, 0.44f, 0.38f};
constexpr Color gold{1.0f, 0.77f, 0.30f};

void vertex(Mesh& mesh, float x, float y, Color c) {
    mesh.vertices.push_back({x, y, c.r, c.g, c.b});
}
void beginShape(Mesh& mesh, GLenum primitive) {
    mesh.batches.push_back({primitive,
        static_cast<GLint>(mesh.vertices.size()), 0});
}
void endShape(Mesh& mesh) {
    Batch& b = mesh.batches.back();
    b.count = static_cast<GLsizei>(mesh.vertices.size()) - b.first;
}

// Two triangles form a rectangle.
void rectangle(Mesh& m, float l, float b, float r, float t, Color c) {
    beginShape(m, GL_TRIANGLES);
    vertex(m,l,b,c); vertex(m,r,b,c); vertex(m,r,t,c);
    vertex(m,l,b,c); vertex(m,r,t,c); vertex(m,l,t,c);
    endShape(m);
}
void triangle(Mesh& m, float x1, float y1, float x2, float y2,
              float x3, float y3, Color c) {
    beginShape(m, GL_TRIANGLES);
    vertex(m,x1,y1,c); vertex(m,x2,y2,c); vertex(m,x3,y3,c);
    endShape(m);
}
// Filled circle or oval
void ellipse(Mesh& m, float x, float y, float rx, float ry, Color c) {
    constexpr int segments = 64;
    beginShape(m, GL_TRIANGLE_FAN);
    vertex(m,x,y,c);
    for (int i=0; i<=segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        vertex(m, x+rx*std::cos(angle), y+ry*std::sin(angle), c);
    }
    endShape(m);
}
// Line segment
void line(Mesh& m, float x1, float y1, float x2, float y2, Color c) {
    beginShape(m, GL_LINES);
    vertex(m,x1,y1,c); vertex(m,x2,y2,c);
    endShape(m);
}
void ring(Mesh& m, float x, float y, float radius, Color c) {
    beginShape(m, GL_LINES);
    for (int i=0; i<64; ++i) {
        float a=2.0f*PI*i/64, b=2.0f*PI*(i+1)/64;
        vertex(m,x+radius*std::cos(a),y+radius*std::sin(a),c);
        vertex(m,x+radius*std::cos(b),y+radius*std::sin(b),c);
    }
    endShape(m);
}

void makeBackground() {
    rectangle(background,-800,-450,800,450,{0.12f,0.16f,0.23f});
    rectangle(background,-800,-450,800,-285,{0.33f,0.23f,0.22f});
    rectangle(background,-800,-295,800,-280,{0.72f,0.49f,0.35f});
    // Floor boards
    for (int x=-800; x<=800; x+=160)
        line(background,static_cast<float>(x),-450,
             static_cast<float>(x+100),-295,{0.43f,0.30f,0.26f});
    line(background,-800,-360,800,-360,{0.43f,0.30f,0.26f});
    line(background,-800,-425,800,-425,{0.43f,0.30f,0.26f});
    // Acoustic panels
    for (int x : {-700,-610,570,660}) {
        rectangle(background,static_cast<float>(x),-120,
                  static_cast<float>(x+55),245,{0.17f,0.22f,0.30f});
        for (int y=-100; y<240; y+=24)
            line(background,static_cast<float>(x+8),static_cast<float>(y),
                 static_cast<float>(x+47),static_cast<float>(y),
                 {0.23f,0.29f,0.37f});
    }
    // Waveform poster
    rectangle(background,-180,215,180,400,gold);
    rectangle(background,-171,224,171,391,dark);
    for (int i=0; i<23; ++i) {
        float x=-143.0f+i*13.0f;
        float h=12.0f+46.0f*std::fabs(std::sin(i*0.65f));
        rectangle(background,x,307-h,x+5,307+h,i%2 ? teal : coral);
    }
    // Table
    ellipse(background,-290,-286,240,15,{0.20f,0.17f,0.19f});
    rectangle(background,-530,-205,-70,-180,{0.75f,0.50f,0.33f});
    rectangle(background,-515,-265,-495,-205,{0.43f,0.28f,0.24f});
    rectangle(background,-105,-265,-85,-205,{0.43f,0.28f,0.24f});
    rectangle(background,-495,-250,-105,-215,{0.23f,0.25f,0.30f});
    // Record player base
    rectangle(background,-460,-174,-140,125,{0.37f,0.26f,0.25f});
    rectangle(background,-450,-164,-150,115,{0.57f,0.39f,0.30f});
    ellipse(background,-163,93,7,7,gold);
    line(background,-163,93,-181,-30,cream);
    line(background,-181,-30,-210,-46,cream);
    rectangle(background,-220,-52,-203,-40,teal);
}
void makeNote() {
    // Use CPU vertex arrays for the note.
    ellipse(note,-17,-12,26,19,gold);
    rectangle(note,0,-11,11,91,gold);
    triangle(note,11,91,51,62,11,54,gold);
    triangle(note,11,72,51,62,43,45,gold);
    ellipse(note,-23,-7,9,4,cream);
}
void makeRecord() {
    ellipse(record,0,0,128,128,{0.025f,0.035f,0.055f});
    for (int r=52; r<=120; r+=8)
        ring(record,0,0,static_cast<float>(r),{0.15f,0.18f,0.23f});
    ellipse(record,0,0,45,45,coral);
    ellipse(record,0,0,34,34,{0.96f,0.67f,0.48f});
    // Markings show the rotation.
    triangle(record,-12,12,21,12,5,31,cream);
    rectangle(record,-20,-22,20,-17,dark);
    ellipse(record,0,0,8,8,dark);
    ellipse(record,0,0,3,3,cream);
    rectangle(record,-3,103,3,118,teal);
}
void makeSpeaker() {
    rectangle(speaker,-85,-145,85,145,{0.045f,0.06f,0.095f});
    rectangle(speaker,-77,-137,77,137,{0.24f,0.18f,0.18f});
    rectangle(speaker,-67,-127,67,127,{0.12f,0.15f,0.20f});
    // Speaker cones
    ellipse(speaker,0,76,34,34,dark);
    ring(speaker,0,76,29,teal);
    ellipse(speaker,0,76,16,16,{0.32f,0.39f,0.45f});
    ellipse(speaker,0,-38,58,58,dark);
    ellipse(speaker,0,-38,49,49,{0.22f,0.28f,0.35f});
    ring(speaker,0,-38,44,teal);
    ellipse(speaker,0,-38,24,24,{0.08f,0.11f,0.17f});
    ellipse(speaker,-7,-30,7,4,{0.32f,0.39f,0.45f});
    rectangle(speaker,-22,-117,22,-110,gold);
    for (int x : {-57,57})
        for (int y : {-117,117})
            ellipse(speaker,static_cast<float>(x),static_cast<float>(y),3,3,cream);
}

void upload(Mesh& mesh) {
    glGenBuffers(1,&mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER,mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(mesh.vertices.size()*sizeof(Vertex)),
        mesh.vertices.data(),GL_STATIC_DRAW);
}
void draw(const Mesh& mesh) {
    if (!ready) return;
    GLint previousBuffer=0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&previousBuffer);
    // Save array settings and color.
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
    glPushAttrib(GL_CURRENT_BIT);
    glBindBuffer(GL_ARRAY_BUFFER,mesh.vbo);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    if (mesh.vbo) {
        // Read vertices from the VBO.
        glVertexPointer(2,GL_FLOAT,sizeof(Vertex),nullptr);
        glColorPointer(3,GL_FLOAT,sizeof(Vertex),
                       reinterpret_cast<const void*>(offsetof(Vertex,r)));
    } else {
        // Read vertices from CPU memory.
        glVertexPointer(2,GL_FLOAT,sizeof(Vertex),&mesh.vertices[0].x);
        glColorPointer(3,GL_FLOAT,sizeof(Vertex),&mesh.vertices[0].r);
    }
    for (const Batch& batch : mesh.batches)
        glDrawArrays(batch.primitive,batch.first,batch.count);
    glPopAttrib();
    glPopClientAttrib();
    glBindBuffer(GL_ARRAY_BUFFER,static_cast<GLuint>(previousBuffer));
}
}

bool initMusicRoomGraphics() {
    if (ready) return true;
    if (!GLEW_VERSION_1_5) return false;
    makeBackground(); makeNote(); makeRecord(); makeSpeaker();
    GLint previousBuffer=0;
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&previousBuffer);
    upload(background); upload(record); upload(speaker);
    glBindBuffer(GL_ARRAY_BUFFER,static_cast<GLuint>(previousBuffer));
    if (glGetError()!=GL_NO_ERROR) {
        destroyMusicRoomGraphics();
        return false;
    }
    ready=true;
    return true;
}
void destroyMusicRoomGraphics() {
    for (Mesh* mesh : {&background,&note,&record,&speaker}) {
        if (mesh->vbo) glDeleteBuffers(1,&mesh->vbo);
        *mesh=Mesh{};
    }
    ready=false;
}
void drawBackground() { draw(background); }
void drawNote() { draw(note); }
void drawRecord() { draw(record); }
void drawSpeaker() { draw(speaker); }

#ifndef MUSIC_ROOM_NO_PREVIEW
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

namespace {
int previewWidth=1280, previewHeight=720;
void reshapePreview(int width,int height) {
    previewWidth=std::max(1,width); previewHeight=std::max(1,height);
    glViewport(0,0,previewWidth,previewHeight);
    float aspect=static_cast<float>(previewWidth)/previewHeight;
    // Keep the scene proportions.
    float halfW=800.0f, halfH=450.0f;
    if (aspect>16.0f/9.0f) halfW=halfH*aspect;
    else halfH=halfW/aspect;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-halfW,halfW,-halfH,halfH,-1,1);
    glMatrixMode(GL_MODELVIEW);
}
void renderPreview() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawBackground();
    // Place each object separately.
    glPushMatrix();
    glTranslatef(RECORD_X,RECORD_Y,0); drawRecord();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(NOTE_X,NOTE_Y,0); drawNote();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(SPEAKER_X,SPEAKER_Y,0); drawSpeaker();
    glPopMatrix();
}
void displayPreview() { renderPreview(); glutSwapBuffers(); }
void keyboardPreview(unsigned char key,int,int) {
    if (key==27) glutLeaveMainLoop();
}
// Save a preview image.
bool capturePreview(const std::string& path) {
    renderPreview();
    std::vector<unsigned char> pixels(previewWidth*previewHeight*3);
    glReadBuffer(GL_BACK);
    glPixelStorei(GL_PACK_ALIGNMENT,1);
    glReadPixels(0,0,previewWidth,previewHeight,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    if (glGetError()!=GL_NO_ERROR) return false;
    std::ofstream out(path,std::ios::binary);
    out<<"P6\n"<<previewWidth<<" "<<previewHeight<<"\n255\n";
    for (int y=previewHeight-1;y>=0;--y)
        out.write(reinterpret_cast<const char*>(pixels.data()+y*previewWidth*3),
                  previewWidth*3);
    return static_cast<bool>(out);
}
}
int main(int argc,char** argv) {
    const bool capture=argc==3 && std::string(argv[1])=="--capture";
    const std::string path=capture ? argv[2] : "";
    // Pass only the program name to FreeGLUT.
    int glutArgc=1;
    glutInit(&glutArgc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitContextVersion(2,1); // Use OpenGL 2.1.
    glutInitWindowSize(previewWidth,previewHeight);
    int window=glutCreateWindow("Final Project - Music Room Scene");
    if (capture) glutHideWindow();
    GLenum status=glewInit();
    if (status!=GLEW_OK) {
        std::cerr<<"GLEW: "<<glewGetErrorString(status)<<'\n';
        glutDestroyWindow(window); return 1;
    }
    while (glGetError()!=GL_NO_ERROR) {} // Clear GLEW errors.
    if (!initMusicRoomGraphics()) {
        std::cerr<<"Graphics initialization failed; OpenGL 1.5+ required.\n";
        glutDestroyWindow(window); return 1;
    }
    glClearColor(0.07f,0.09f,0.14f,1);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    reshapePreview(previewWidth,previewHeight);
    glutDisplayFunc(displayPreview);
    glutReshapeFunc(reshapePreview);
    glutKeyboardFunc(keyboardPreview);
    glutCloseFunc(destroyMusicRoomGraphics);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    if (capture) {
        bool ok=capturePreview(path);
        std::cout<<(ok ? "Render check passed: " : "Render check failed: ")<<path<<'\n';
        destroyMusicRoomGraphics();
        glutDestroyWindow(window);
        return ok ? 0 : 1;
    }
    std::cout<<"Static graphics preview. Press Escape to exit.\n";
    glutMainLoop();
    return 0;
}
#endif
