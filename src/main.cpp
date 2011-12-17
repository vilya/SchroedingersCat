#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <GLUT/glut.h>
#include <OpenGL/gl.h>

namespace cat {

  //
  // Constants
  //

  const char* kGameName = "Schroedinger's Cat: The Game";
  const char* kCopyrightMessage = "(c) Vilya Harvey, 2011";

  static const int kWindowWidth = 1024;
  static const int kWindowHeight = 1024;

  static const double kMinFrameTime = 1000.0 / 60.0; // Targetting 60 fps.


  //
  // Forward declarations
  //

  void start();
  void render();
  void resize(int x, int y);
  void keyPressed(unsigned char key, int x, int y);
  void keyReleased(unsigned char key, int x, int y);
  void specialKeyPressed(int key, int x, int y);
  void specialKeyReleased(int key, int x, int y);
  void mainLoop();

  // Get the current system time in milliseconds (may include a fraction of a millisecond).
  double now();
  void sleepFor(double milliseconds);


  //
  // Functions
  //

  void start()
  {
    int argc = 0;
    char* argv[] = { NULL };

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow(kGameName);

    glutDisplayFunc(render);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutSpecialFunc(specialKeyPressed);
    glutSpecialUpFunc(specialKeyReleased);
    glutIdleFunc(mainLoop);

    glutMainLoop(); // This doesn't return until the main window closes.
  }


  void render()
  {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: drawPlayArea();
    // TODO: drawParticles();
    // TODO: drawPlayer();
    // TODO: drawEffects();

    glutSwapBuffers();
  }


  void resize(int width, int height)
  {
    glViewport(0, 0, width, height);
    glutPostRedisplay();
  }


  void keyPressed(unsigned char key, int x, int y)
  {
    const unsigned char kEsc = 27;
    if (key == kEsc)
      exit(0);

    fprintf(stderr, "key %d pressed\n", key);
  }


  void keyReleased(unsigned char key, int x, int y)
  {
    fprintf(stderr, "key %d released\n", key);
  }


  void specialKeyPressed(int key, int x, int y)
  {
    fprintf(stderr, "special key %d pressed\n", key);
  }


  void specialKeyReleased(int key, int x, int y)
  {
    fprintf(stderr, "special key %d released\n", key);
  }


  void mainLoop()
  {
    double frameStartTime = now();

    // TODO: Calculations for the current frame.

    double frameEndTime = now();
    double frameTime = frameEndTime - frameStartTime;
    if (frameTime < kMinFrameTime)
      sleepFor(kMinFrameTime - frameTime);

    glutPostRedisplay();
  }


  double now()
  {
    struct timeval t;
    gettimeofday(&t, NULL);
    
    double result = t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
    return result;
  }


  void sleepFor(double milliseconds)
  {
    usleep((useconds_t)(milliseconds * 1000));
  }

} // namespace cat

int main(int argc, char** argv)
{
  printf("%s\n", cat::kGameName);
  printf("%s\n", cat::kCopyrightMessage);
  cat::start();
}

