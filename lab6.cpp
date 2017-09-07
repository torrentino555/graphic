#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include <fstream>
#include <iostream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

int WIDTH = 600, HEIGHT = 600, spin_x = 0, spin_y = 0, spin_z = 0, step = 30,
    sign_animation = 1, buffer = 0;
float x = 0, y = 0, scale = 1, x_l = 0.95, y_l = 0.0, z_l = 0.0, angle = 0,
      spin_l = 0, t = 0, radiusCilindre = 0.1, heightCilindre = 0.3,
      timer = glfwGetTime(), dt;
bool Fill = true, moving = false, light = false, activeLight = false,
     drawT = false, animation = false, global_ambient = false, two_side = true,
     local_viewer = false;
GLuint texture[2];
struct vertex {
  float x, y, z, xn, yn, zn, xt, yt;
};
vector<vector<vertex>> v, l, vbuf, lbuf, g, gbuf;

void CalculateVertexs(double radius, double height) {
  v.clear();
  l.clear();
  g.clear();
  float angle = 0, h;
  for (int i = 0; i <= step + 1; angle += 2 * M_PI / step, i++) {
    h = 0;
    v.push_back(vector<vertex>());
    for (int j = 0; j < step; h += height / step, j++) {
      v[i].push_back({radius * cos(angle), h, radius * sin(angle),
                      radius * cos(angle), 0, sin(angle) * radius,
                      (float)(step + 1 - i) / (step + 1), (float)(j) / (step)});
    }
    v[i].push_back({radius * cos(angle), height, radius * sin(angle),
                    radius * cos(angle), 0, sin(angle) * radius,
                    (float)(step + 1 - i) / (step + 1), 1});
    float r = radius;
    l.push_back(vector<vertex>());
    g.push_back(vector<vertex>());
    for (int j = 0; j < step; r -= radius / step, j++) {
      l[i].push_back({r * cos(angle), 0, r * sin(angle), 0, 0, 0,
                      ((r / radius) * cos(angle) + 1) / 2,
                      ((r / radius) * sin(angle) + 1) / 2});
      g[i].push_back({r * cos(angle), v[0][step].y, r * sin(angle), 0, 0, 0,
                      ((r / radius) * cos(angle) + 1) / 2,
                      ((r / radius) * sin(angle) + 1) / 2});
    }
    l[i].push_back({0, 0, 0, 0, 0, 0, 0.5, 0.5});
    g[i].push_back({0, 0, 0, 0, 0, 0, 0.5, 0.5});
  }
  vbuf = vector<vector<vertex>>(v);
  lbuf = vector<vector<vertex>>(l);
  gbuf = vector<vector<vertex>>(g);
}

vertex p2 = {0.5f, 0, 0};
vertex p3 = {0, 0.5f, 0};

void twining() {
  if (sign_animation == 1)
    t += 0.5 * dt;
  else
    t -= 0.5 * dt;
  if (t >= 1.0)
    t = 0.0 + 0.01;
  if (t >= 1.0 || t <= 0.0)
    sign_animation = -1 * sign_animation;
  // cout << t << " " << sign_animation << endl;
  for (int i = 0; i <= step + 1; i++) {
    for (int j = 0; j <= step; j++) {
      vertex p4_l = {lbuf[i][j].x, lbuf[i][j].y, lbuf[i][j].z + 0.5f};
      vertex p4_v = {vbuf[i][j].x, vbuf[i][j].y, vbuf[i][j].z + 0.5f};
      vertex p4_g = {gbuf[i][j].x, gbuf[i][j].y, vbuf[i][j].z + 0.5f};
      float var1 = 1 - t;
      float var2 = var1 * var1 * var1;
      float var3 = t * t * t;
      v[i][j].x = var2 * vbuf[i][j].x + 3 * t * var1 * var1 * p2.x +
                  3 * t * t * var1 * p3.x + var3 * p4_v.x;
      v[i][j].y = var2 * vbuf[i][j].y + 3 * t * var1 * var1 * p2.y +
                  3 * t * t * var1 * p3.y + var3 * p4_v.y;
      v[i][j].z = var2 * vbuf[i][j].z + 3 * t * var1 * var1 * p2.z +
                  3 * t * t * var1 * p3.z + var3 * p4_v.z;
      l[i][j].x = var2 * lbuf[i][j].x + 3 * t * var1 * var1 * p2.x +
                  3 * t * t * var1 * p3.x + var3 * p4_l.x;
      l[i][j].y = var2 * lbuf[i][j].y + 3 * t * var1 * var1 * p2.y +
                  3 * t * t * var1 * p3.y + var3 * p4_l.y;
      l[i][j].z = var2 * lbuf[i][j].z + 3 * t * var1 * var1 * p2.z +
                  3 * t * t * var1 * p3.z + var3 * p4_l.z;
      g[i][j].x = var2 * gbuf[i][j].x + 3 * t * var1 * var1 * p2.x +
                  3 * t * t * var1 * p3.x + var3 * p4_g.x;
      g[i][j].y = var2 * gbuf[i][j].y + 3 * t * var1 * var1 * p2.y +
                  3 * t * t * var1 * p3.y + var3 * p4_g.y;
      g[i][j].z = var2 * gbuf[i][j].z + 3 * t * var1 * var1 * p2.z +
                  3 * t * t * var1 * p3.z + var3 * p4_g.z;
    }
  }
}

void save(string name) {
  ofstream fout(name);
  fout << WIDTH << " " << HEIGHT << " " << spin_x << " " << spin_y << " "
       << spin_z << " " << step << " " << x << " " << y << " " << scale << " "
       << x_l << " " << y_l << " " << z_l << " " << angle << " " << spin_l
       << " " << Fill << " " << moving << " " << light << " " << activeLight
       << " " << drawT;
  fout.close();
}

void load(string name) {
  std::ifstream file;
  file.open("save.txt");
  if (file.peek() != EOF) {
    file >> WIDTH >> HEIGHT >> spin_x >> spin_y >> spin_z >> step >> x >> y >>
        scale >> x_l >> y_l >> z_l >> angle >> spin_l >> Fill >> moving >>
        light >> activeLight >> drawT;
  }
  file.close();
  CalculateVertexs(radiusCilindre, heightCilindre);
}

void error(int code, const char *desc) { fputs(desc, stderr); }

void resize(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_S && action != GLFW_RELEASE)
    spin_x += 5;
  else if (key == GLFW_KEY_W && action != GLFW_RELEASE)
    spin_x -= 5;
  else if (key == GLFW_KEY_D && action != GLFW_RELEASE)
    spin_y += 5;
  else if (key == GLFW_KEY_A && action != GLFW_RELEASE)
    spin_y -= 5;
  else if (key == GLFW_KEY_E && action != GLFW_RELEASE)
    spin_z += 5;
  else if (key == GLFW_KEY_Q && action != GLFW_RELEASE)
    spin_z -= 5;

  else if (key == GLFW_KEY_EQUAL && action != GLFW_RELEASE)
    scale += 0.1;
  else if (key == GLFW_KEY_MINUS && action != GLFW_RELEASE)
    scale -= 0.1;

  else if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    Fill = !Fill;
  else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    animation = !animation;
  else if (key == GLFW_KEY_G && action == GLFW_PRESS)
    save("save.txt");
  else if (key == GLFW_KEY_L && action == GLFW_PRESS)
    load("save.txt");
  else if (key == GLFW_KEY_M && action == GLFW_PRESS)
    moving = !moving;
  else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    light = !light;
  else if (key == GLFW_KEY_T && action == GLFW_PRESS)
    drawT = !drawT;
  else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
    activeLight = !activeLight;
    glDisable(GL_LIGHTING);
  } else if (key == GLFW_KEY_R && action == GLFW_PRESS)
    spin_x = spin_y = spin_z = 0;
  else if (key == GLFW_KEY_U && action == GLFW_PRESS)
    global_ambient = !global_ambient;
  else if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    two_side = !two_side;
  else if (key == GLFW_KEY_H && action == GLFW_PRESS)
    local_viewer = !local_viewer;

  else if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE)
    x_l -= 0.1;
  else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE)
    x_l += 0.1;
  else if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
    z_l += 0.1;
  else if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
    z_l -= 0.1;
  else if (key == GLFW_KEY_Z && action != GLFW_RELEASE)
    y_l += 0.1;
  else if (key == GLFW_KEY_X && action != GLFW_RELEASE)
    y_l -= 0.1;

  else if (key == GLFW_KEY_LEFT_BRACKET && action != GLFW_RELEASE) {
    if (step > 3) {
      step--;
      CalculateVertexs(radiusCilindre, heightCilindre);
      printf("\r%3d", step);
      fflush(stdout);
    }
  } else if (key == GLFW_KEY_RIGHT_BRACKET && action != GLFW_RELEASE) {
    step++;
    CalculateVertexs(radiusCilindre, heightCilindre);
    printf("\r%3d", step);
    fflush(stdout);
  }
}

void load(string path, int number) {
  glGenTextures(1, &texture[number]);
  glBindTexture(GL_TEXTURE_2D, texture[number]);

  int width, height;
  unsigned char *image =
      SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
}

void init_light() {
  float light0_diffuse[] = {0.8f, 0.8f, 0.8f};
  float light0_position[] = {x_l, y_l, z_l, 1.0};
  float light0_ambient[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
  float light0_specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
  float ambient[] = {0.5f, 0.2f, 0.2f, 1.0f};
  float default_amb[] = {0.2f, 0.2f, 0.2f, 1.0f};
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  if (global_ambient)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  else
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, default_amb);
  if (two_side)
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  else
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
  if (local_viewer)
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  else
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR,specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
}

void MoveObject() {
  spin_x += 150.0 * dt;
  spin_y += 150.0 * dt;
  spin_z += 150.0 * dt;
}

void MoveLight() {
  x_l = cos(angle) * 0.9;
  z_l = sin(angle) * 0.9;
  spin_l -= 90 * dt;
  angle += M_PI * dt / 2;
}

void drawCube(float x) {
  glBegin(GL_QUADS);
  glColor3f(1, 1, 1);
  glVertex3f(-x, -x, -x);
  glVertex3f(x, -x, -x);
  glVertex3f(x, x, -x);
  glVertex3f(-x, x, -x);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(-x, -x, x);
  glVertex3f(x, -x, x);
  glVertex3f(x, x, x);
  glVertex3f(-x, x, x);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(x, -x, x);
  glVertex3f(x, -x, -x);
  glVertex3f(x, x, -x);
  glVertex3f(x, x, x);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(-x, -x, -x);
  glVertex3f(-x, -x, x);
  glVertex3f(-x, x, x);
  glVertex3f(-x, x, -x);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(x, x, x);
  glVertex3f(x, x, -x);
  glVertex3f(-x, x, -x);
  glVertex3f(-x, x, x);
  glEnd();
  glBegin(GL_QUADS);
  glVertex3f(-x, -x, -x);
  glVertex3f(x, -x, -x);
  glVertex3f(x, -x, x);
  glVertex3f(-x, -x, x);
  glEnd();
}

void drawLid(float radius, float h) {
  if (drawT)
    glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture[1]);
  for (int i = 0; i < step; i++) {
    glBegin(GL_QUAD_STRIP);
    glColor3f((float)138 / 255, (float)102 / 255, (float)66 / 255);
    for (int j = step; j >= 0; j--)
      for (int k = i; k < i + 2; k++) {
        glNormal3f(0, -1, 0);
        glTexCoord2f(l[k][j].xt, l[k][j].yt);
        glVertex3f(l[k][j].x, l[k][j].y, l[k][j].z);
      }
    glEnd();
  }
  for (int i = 0; i < step; i++) {
    glBegin(GL_QUAD_STRIP);
    glColor3f((float)138 / 255, (float)102 / 255, (float)66 / 255);
    for (int j = 0; j <= step; j++)
      for (int k = i; k < i + 2; k++) {
        glNormal3f(0, 1, 0);
        glTexCoord2f(l[k][j].xt, l[k][j].yt);
        glVertex3f(l[k][j].x, g[k][j].y, l[k][j].z);
      }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

void drawCilindre(float radius, float h) {
  glTranslatef(0, -h / 2, 0);
  drawLid(radius, h);
  if (drawT)
    glEnable(GL_TEXTURE_2D);
  for (int i = 0; i < step; i++) {
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_QUAD_STRIP);
    glColor3f(1, 1, 1);
    for (int j = step; j >= 0; j--) {
      for (int k = i + 1; k >= i; k--) {
        glNormal3f(v[k][j].xn, v[k][j].yn, v[k][j].zn);
        glTexCoord2f(v[k][j].xt, v[k][j].yt);
        glVertex3f(v[k][j].x, v[k][j].y, v[k][j].z);
      }
    }
    glEnd();
  }
  glDisable(GL_TEXTURE_2D);
}

void drawRoom() {
  glBegin(GL_QUADS);
  glColor3f(0.8, 0.8, 0.8);
  glNormal3f(0, 0, 1);
  glVertex3f(-1, -1, 0.8);
  glVertex3f(1, -1, 0.8);
  glVertex3f(1, 1, 0.8);
  glVertex3f(-1, 1, 0.8);
  glEnd();
}

float h = 0.0f;

void display(GLFWwindow *window) {
  timer = glfwGetTime();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (Fill)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  if (activeLight)
    glDisable(GL_LIGHTING);
  if (animation)
    twining();
  glPushMatrix();
  glTranslatef(x_l, y_l, z_l);
  glRotatef(spin_l, 0, 1, 0);
  drawCube(0.05);
  glPopMatrix();
  if (activeLight)
    glEnable(GL_LIGHTING);
  glPushMatrix();
  // drawRoom();
  glTranslatef(x, y, 0);
  glRotatef(spin_x, 1, 0, 0);
  glRotatef(spin_y, 0, 1, 0);
  glRotatef(spin_z, 0, 0, 1);
  glScalef(scale, scale, scale);
  drawCilindre(radiusCilindre, heightCilindre);
  glPopMatrix();
  dt = glfwGetTime() - timer;
  h += dt;
  if (buffer++ == 1000) {
    printf("\r%3f", h/1000);
    fflush(stdout);
    buffer = 0;
    h = 0;
  }
}

int main() {
  glfwSetErrorCallback(error);
  if (!glfwInit()) {
    cout << "Failed to init glfw" << endl;
    exit(1);
  }
  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Lab", NULL, NULL);
  if (!window) {
    cout << "Failed to create the window" << endl;
    glfwTerminate();
    exit(1);
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, resize);
  glfwSetKeyCallback(window, key_callback);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  // glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  resize(window, WIDTH, HEIGHT);
  CalculateVertexs(radiusCilindre, heightCilindre);
  load("Crate.bmp", 0);
  load("lid.png", 1);
  while (!glfwWindowShouldClose(window)) {
    if (moving)
      MoveObject();
    if (light)
      MoveLight();
    if (activeLight)
      init_light();
    display(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
