//#include <GL/glew.h>
#include "MyGLWidget.h"

#include <iostream>

MyGLWidget::MyGLWidget (QWidget* parent) : QOpenGLWidget(parent), program(NULL)
{
  setFocusPolicy(Qt::StrongFocus);  // per rebre events de teclat
}

MyGLWidget::~MyGLWidget ()
{
  if (program != NULL)
    delete program;
}

void MyGLWidget::initializeGL ()
{
  // Cal inicialitzar l'ús de les funcions d'OpenGL
  initializeOpenGLFunctions();
  glClearColor (0.5, 0.7, 1.0, 1.0); // defineix color de fons (d'esborrat)
  carregaShaders();
  creaBuffers();
 }

void MyGLWidget::paintGL ()
{
// En cas de voler canviar els paràmetres del viewport, descomenteu la crida següent i
// useu els paràmetres que considereu (els que hi ha són els de per defecte)
//  glViewport (0, 0, ample, alt);
  
    glClear (GL_COLOR_BUFFER_BIT);  // Esborrem el frame-buffer
    pintaSector();
    pintaCistella();
    pintaBase();
    // Desactivem el VAO
    glBindVertexArray(0);
}

void MyGLWidget::pintaSector(){
    // Pintem cada sector, 16 en total 
    for (int i = 0; i < 360 / angleSectorNoria; i++) {
        modelTransformSector(glm::radians(i * angleSectorNoria)); 
        glBindVertexArray(VAO_Sector);
        glDrawArrays(GL_TRIANGLES, 0, 21);
    }
}

void MyGLWidget::pintaBase(){
    modelTransformBase();
    glBindVertexArray(VAO_Base);

    // Indiquem al Fragment Shader que estem pintant la base
    glUniform1i(esBaseLoc, 1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    // Tornem a posar a false
    glUniform1i(esBaseLoc, 0);
}
void MyGLWidget::pintaCistella(){
    // Pintem les 8 cistelles, cadascuna a 360/8
    if (mostrar8cistelles) {
        for (int i = 0; i < 8; ++i) {
            float angle = glm::radians(i * 45.0f);
            modelTransformCistella(angle);
            glBindVertexArray(VAO_Cistella);
            glDrawArrays(GL_TRIANGLES, 0, 18);
        }
    // Pintem una sola cistella 
    } else {
        modelTransformCistella(0.0f);
        glBindVertexArray(VAO_Cistella);
        glDrawArrays(GL_TRIANGLES, 0, 18);
    }
}

void MyGLWidget::modelTransformCistella(float angleOffset){
    glm::mat4 TG (1.0f);

    // Baixem la cistella perquè segueixi la sínia
    // 0.7*0.1 = 0.07 de l'escalat, 0.25 + 0.7 = 0.32 cap a vall
    TG = glm::translate(TG, glm::vec3(0.0f, -0.32f, 0.0f));

    // Rotació de la cistella amb la sínia
    TG = glm::rotate(TG, angleNoria, glm::vec3(0.0f, 0.0f, 1.0f));

    // Posem cada cistella a la seva posició corresponent 
    TG = glm::rotate(TG, angleOffset, glm::vec3(0.0f, 0.0f, 1.0f));

    // Col.loquem la cistella 0.5 del centre de rotacio
    TG = glm::translate(TG, glm::vec3(0.5f, 0.0f, 0.0f));

    // Compensem la rotació de la sínia perquè la cistella quedi horitzontal
    TG = glm::rotate(TG, -angleNoria - angleOffset, glm::vec3(0.0f, 0.0f, 1.0f));

    // Escalem la cistella a 1.0/10.0 de la mida original
    TG = glm::scale(TG, glm::vec3(0.1f, 1.0f / 10.0f, 0.1f));

    glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformBase(){
    glm::mat4 TG (1.0f);
    
    // Traslladem amb l'escalat y = 0.375 + 0.25 = 0.625,
    // L'hem de moure 0.625 cap a vall
    TG = glm::translate(TG, glm::vec3(0.0f, -0.625f, 0.0f));
    
    // Escalem la base en y per aconseguir alçada 0.75
    // y = y que volem / ymax - ymin = 0.75 / (1-(-1))
    TG = glm::scale(TG, glm::vec3(1.0f, 0.375f, 1.0f));

    glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::modelTransformSector(float angleRadians){
    glm::mat4 TG (1.0f);

    // Centrem 
    TG = glm::translate(TG, glm::vec3(0.0f, -0.25f, 0.0f));
    
    // Afegim gir de la sínia
    TG = glm::rotate(TG, angleNoria, glm::vec3(0.0f, 0.0f, 1.0f));

    // Rotem el sector
    TG = glm::rotate(TG, angleRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Escalem a la meitat de la mida a la que estava 
    TG = glm::scale(TG, glm::vec3(0.5f, 0.5f, 0.5f));

    glUniformMatrix4fv(TGLoc, 1, GL_FALSE, &TG[0][0]);
}

void MyGLWidget::resizeGL (int w, int h)
{
// Aquest codi és necessari únicament per a MACs amb pantalla retina.
#ifdef __APPLE__
  GLint vp[4];
  glGetIntegerv (GL_VIEWPORT, vp);
  ample = vp[2];
  alt = vp[3];
#else
  ample = w;
  alt = h;
#endif
}

void MyGLWidget::keyPressEvent(QKeyEvent* event)
{
    makeCurrent();
    switch (event->key()) {

        case Qt::Key_A: {
            break;
        }
        case Qt::Key_D: {
            break;
        }
        case Qt::Key_Left: {
            angleNoria += glm::radians(5.0f);
            break;
        }
        case Qt::Key_Right: {
            angleNoria -= glm::radians(5.0f);
            break;
        }
        case Qt::Key_8: {
            mostrar8cistelles = true;
            break;
        } 
        case Qt::Key_1: {
            mostrar8cistelles = false;
            break;
        }
        default: event->ignore(); break;
    }
    update();
}

void MyGLWidget::creaBuffers (){
    creaBufferSector();
    creaBufferBase();
    creaBufferCistella();
}

void MyGLWidget::creaBufferBase(){
    glm::vec3 Colors[3];
    glm::vec3 Vertices[3];
    int i=0,c=0;
    Vertices[i++] = glm::vec3(0.0f , 1.0f, 0.0f);
    Vertices[i++] = glm::vec3(-0.15, -1.0f, 0.0f);
    Vertices[i++] = glm::vec3(0.15, -1.0f, 0.0f);
    Colors[c++]=COLOR_GROC;
    Colors[c++]=COLOR_GROC;
    Colors[c++]=COLOR_GROC;

    // Creació del Vertex Array Object (VAO) que usarem per pintar
    glGenVertexArrays(1, &VAO_Base);
    glBindVertexArray(VAO_Base);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO[2];
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Creació del buffer de colors
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // Desactivem el VAO
    glBindVertexArray(0);
}

void MyGLWidget::creaBufferSector()
{
  glm::vec3 Colors[21];
  glm::vec3 Vertices[21];
  int i=0,c=0;
  float alfa=glm::radians(angleSectorNoria);
  float r[]={0.4f, 0.7f,  1.0f};
  float gruix=0.05f;
  Vertices[i++] = glm::vec3(0.0f , 0.0f, 0.0f);
  Vertices[i++] = glm::vec3(1.0f, 0.0f, 0.0f);
  Vertices[i++] = glm::vec3((r[2]-gruix)*cos(alfa/6), (r[2]-gruix)*sin(alfa/6), 0.0f);
  Colors[c++]=COLOR_BLAU;
  Colors[c++]=COLOR_BLAU;
  Colors[c++]=COLOR_BLAU;

  for(int k=0;k<3;k++){
    Vertices[i++] = glm::vec3(r[k]*cos(alfa), r[k]*sin(alfa), 0.0);
    Vertices[i++] = glm::vec3((r[k]-gruix)*cos(alfa), (r[k]-gruix)*sin(alfa), 0.0);
    Vertices[i++] = glm::vec3(r[k]-gruix, 0, 0.0);

    Vertices[i++] = glm::vec3(r[k]*cos(alfa), r[k]*sin(alfa), 0.0);
    Vertices[i++] = glm::vec3(r[k]-gruix, 0, 0.0);
    Vertices[i++] = glm::vec3(r[k], 0, 0.0);

    for(int n=0;n<6;n++){    
        if(k==0){
            Colors[c++]=COLOR_VERMELL;
        } else if(k==1){
            Colors[c++]=COLOR_GROC;
        }else{
            Colors[c++]=COLOR_BLAU;
        }
    }
  }

  // Creació del Vertex Array Object (VAO) que usarem per pintar
  glGenVertexArrays(1, &VAO_Sector);
  glBindVertexArray(VAO_Sector);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO[2];
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Creació del buffer de colors
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // Desactivem el VAO
    glBindVertexArray(0);
}

void MyGLWidget::creaBufferCistella()
{
    float w = 0.6;
    float h = 0.7;

    glm::vec3 Colors[18];
    glm::vec3 Vertices[18];
    int i=0;

    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3(-1, 0.0, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3(-w,  h, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3(-w, -h, 0.0);

    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3(-w,  h , 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3(-w, -h, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3( w,  h, 0.0);

    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3( w,  h, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3(-w, -h, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3( w, -h, 0.0);

    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3( w, -h, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3( w,  h, 0.0);
    Colors[i]=COLOR_BLAU_FLUIX; Vertices[i++] = glm::vec3( 1, 0.0, 0.0);

    Colors[i]=COLOR_BLANC; Vertices[i++] = glm::vec3(-w*0.8,  h*0.8 , 0.0);
    Colors[i]=COLOR_BLANC; Vertices[i++] = glm::vec3(-w*0.8, 0, 0.0);
    Colors[i]=COLOR_BLANC; Vertices[i++] = glm::vec3( w*0.8,  h*0.8, 0.0);

    Colors[i]=COLOR_BLANC; Vertices[i++] = glm::vec3( w*0.8,  h*0.8, 0.0); 
    Colors[i]=COLOR_BLANC; Vertices[i++] = glm::vec3(-w*0.8, 0, 0.0);
    Colors[i]=COLOR_BLANC; Vertices[i++] = glm::vec3( w*0.8, 0, 0.0);


    // Creació del Vertex Array Object (VAO) que usarem per pintar
    glGenVertexArrays(1, &VAO_Cistella);
    glBindVertexArray(VAO_Cistella);

    // Creació del buffer amb les dades dels vèrtexs
    GLuint VBO[2];
    glGenBuffers(2, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    // Activem l'atribut que farem servir per vèrtex (només el 0 en aquest cas)
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    // Creació del buffer de colors
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    // Desactivem el VAO
    glBindVertexArray(0);
}

void MyGLWidget::carregaShaders()
{
  // Creem els shaders per al fragment shader i el vertex shader
  QOpenGLShader fs (QOpenGLShader::Fragment, this);
  QOpenGLShader vs (QOpenGLShader::Vertex, this);
  // Carreguem el codi dels fitxers i els compilem
  fs.compileSourceFile("shaders/basicShader.frag");
  vs.compileSourceFile("shaders/basicShader.vert");
  // Creem el program
  program = new QOpenGLShaderProgram(this);
  // Li afegim els shaders corresponents
  program->addShader(&fs);
  program->addShader(&vs);
  // Linkem el program
  program->link();
  // Indiquem que aquest és el program que volem usar
  program->bind();

  // Obtenim identificador per a l'atribut “vertex” del vertex shader
  vertexLoc = glGetAttribLocation (program->programId(), "vertex");
  colorLoc =  glGetAttribLocation (program->programId(), "color");
  TGLoc =  glGetUniformLocation(program->programId(), "TG");
  esBaseLoc = glGetUniformLocation(program->programId(), "esBase");
}