#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include <cmath>
#include <vector>
#include "Particle.h"
#include "Vector.h"
#include"Vector.h"
#include "Camera.h"
#include "gauss.h"

#define automaticSpeed 20
#define BOIDNUMBER  100

#define AMAX 0.1

int WIDTH = 1300;
int HEIGHT = 900;

int resetSign = 0;
bool showGrid = true;


double particleSize = 0.3;
double hStep = 0.1;

Vector3d sunPosition(0,0,0);
Vector3d obstaclePosition(7,7,0);

double obstacleRadius = 1.5;

Camera *camera;

//vector
std::vector<Particle>particles;
std::vector<Particle>::iterator it;

Vector3d boidStates[BOIDNUMBER*2];
Vector3d boidStatesNew[BOIDNUMBER*2];
Vector3d boidStatesA[BOIDNUMBER*2];

float mini(float a, float b)
{
    if (a>b) {
        return b;
    }
    else
    {
        return a;
    }
}


void mouseEventHandler(int button, int state, int x, int y) {
    // let the camera handle some specific mouse events (similar to maya)
    camera->HandleMouseEvent(button, state, x, y);
    glutPostRedisplay();
}

void motionEventHandler(int x, int y) {
    // let the camera handle some mouse motions if the camera is to be moved
    camera->HandleMouseMotion(x, y);
    glutPostRedisplay();
}




// draws a simple grid
void makeGrid()
{
    //Setting  material
    GLfloat ball_mat_ambient[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat ball_mat_diffuse[]  = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat ball_mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat ball_mat_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat ball_mat_shininess  = 10.0f;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
    glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
    
    glLineWidth(1.0);
    
    for (float i=-12; i<12; i++) {
        for (float j=-12; j<12; j++) {
            glBegin(GL_LINES);
            glVertex3f(i, 0, j);
            glVertex3f(i, 0, j+1);
            glEnd();
            glBegin(GL_LINES);
            glVertex3f(i, 0, j);
            glVertex3f(i+1, 0, j);
            glEnd();
            
            if (j == 11){
                glBegin(GL_LINES);
                glVertex3f(i, 0, j+1);
                glVertex3f(i+1, 0, j+1);
                glEnd();
            }
            if (i == 11){
                glBegin(GL_LINES);
                glVertex3f(i+1, 0, j);
                glVertex3f(i+1, 0, j+1);
                glEnd();
            }
        }
    }
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex3f(-12, 0, 0);
    glVertex3f(12, 0, 0);
    glEnd();
    glBegin(GL_LINES);
    glVertex3f(0, 0, -12);
    glVertex3f(0, 0, 12);
    glEnd();
    glLineWidth(1.0);
}




void boidGenerator()
{
    
    for (int i = 0; i<BOIDNUMBER; i++) {
        if (0 == i%5) {
            particles.push_back( Particle(Vector3d(gauss(0, 3, 1),gauss(10, 2, 1),0), Vector3d(2,0,0), Vector3d(0,0,0), Vector4d(0,1,0,0), 1, 0, particleSize, false,"leader") );
        }
        else{
            particles.push_back( Particle(Vector3d(gauss(0, 5, 1),gauss(10, 2, 1),gauss(0, 3, 1)), Vector3d(0,0,0), Vector3d(0,0,0), Vector4d(0,0,1,0), 1, 0, particleSize, false,"boid") );//position,velocity,aceleration,color,mass,lifespan,pointsize,stopSign,name
        }
    }
    
}


void myDisplay(void)
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // draw the camera created in perspective
    camera->PerspectiveDisplay(WIDTH, HEIGHT);
    
   
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 
    glTranslatef(0,0,0);
    glPushMatrix();
    if (showGrid)
        makeGrid();
    glPopMatrix();
    
  
    {
        //Setting  material
        GLfloat ball_mat_ambient[]  = {0.7f, 0.0f, 0.0f, 1.0f};
        GLfloat ball_mat_diffuse[]  = {0.5f, 0.0f, 0.0f, 1.0f};
        GLfloat ball_mat_specular[] = {0.5f, 0.0f, 0.0f, 1.0f};
        GLfloat ball_mat_emission[] = {0.3f, 0.0f, 0.0f, 1.0f};
        GLfloat ball_mat_shininess  = 10.0f;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
        glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
        
        glutSolidSphere(3, 20, 20); //radius of ball; number of faces creating a ball
    }
    
    //a blue obstacle
    {
        //Setting  material
        GLfloat ball_mat_ambient[]  = {0.0f, 0.5f, 0.7f, 1.0f};
        GLfloat ball_mat_diffuse[]  = {0.0f, 0.5f, 0.5f, 1.0f};
        GLfloat ball_mat_specular[] = {0.0f, 0.5f, 0.5f, 1.0f};
        GLfloat ball_mat_emission[] = {0.0f, 0.5f, 0.3f, 1.0f};
        GLfloat ball_mat_shininess  = 10.0f;
        
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
        glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
        
        glPushMatrix();
        glTranslated(obstaclePosition.x, obstaclePosition.y, obstaclePosition.z);
        glutSolidSphere(obstacleRadius-0.5, 20, 20); //radius of ball; number of faces creating a ball
        glPopMatrix();
    }
    
    {
        
        GLfloat ball_mat_ambient[]  = {0.0f, 0.0f, 0.7f, 1.0f};
        GLfloat ball_mat_diffuse[]  = {0.0f, 0.0f, 0.5f, 1.0f};
        GLfloat ball_mat_specular[] = {0.0f, 0.0f, 0.5f, 1.0f};
        GLfloat ball_mat_emission[] = {0.0f, 0.0f, 0.3f, 1.0f};
        GLfloat ball_mat_shininess  = 10.0f;
        
        /*
        glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
        glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
         */
         
        
        for (int i = 0; i < particles.size(); ++i) {
            if (particles[i].getName() == "leader") {
                
                glEnable(GL_COLOR_MATERIAL);
                glColorMaterial(GL_FRONT, GL_EMISSION);
                glColor4f(particles[i].getColor().x, particles[i].getColor().y, particles[i].getColor().z, particles[i].getColor().w);
                glColorMaterial(GL_FRONT, GL_SPECULAR);
                glColor4f(particles[i].getColor().x, particles[i].getColor().y, particles[i].getColor().z, particles[i].getColor().w);
                glDisable(GL_COLOR_MATERIAL);
                
            }
            else
            {
                glMaterialfv(GL_FRONT, GL_AMBIENT,   ball_mat_ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE,   ball_mat_diffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR,  ball_mat_specular);
                glMaterialfv(GL_FRONT, GL_EMISSION,  ball_mat_emission);
                glMaterialf (GL_FRONT, GL_SHININESS, ball_mat_shininess);
                
            }
            
            glPushMatrix();
            glTranslatef(particles[i].getPosition().x, particles[i].getPosition().y, particles[i].getPosition().z);
            glutSolidCone(particles[i].getPointSize(), particles[i].getPointSize()*2, 16, 16);
            glPopMatrix();
          
            
            
        }
        
    }

 
    
    
    glFlush();
    glutSwapBuffers();
}


void calculatePosition()
{
    Vector3d particlePositionNew, particleVelocityNew;
    for (it = particles.begin(); it!=particles.end(); ++it) {
        it->setLifeSpan(it->getLifeSpan()+1);
        if (it->getStopSign() == false) {
            if (it->getName() == "leader") {
                Vector3d Xai = it->getPosition() - sunPosition;
                double distance  = Xai.norm();
                it->setAcceleration( (-0.02 * (distance*distance))*(Xai.normalize()) );
                //it->setAcceleration((-50 * 1.0/(distance*distance))*(Xai.normalize()));
            }
            particleVelocityNew = it->getVelocity() + it->getAcceleration()*hStep;
            particlePositionNew = it->getPosition() + it->getVelocity()*hStep;
            
            it->setPosition(particlePositionNew);
            it->setVelocity(particleVelocityNew);
        }
    }
}


//***************************************//

void flockAcceleration()
{
    double KA = 0.1;
    double KV = 0.01;
    double KC = 0.001;
    double R1 = 8;
    double R2 = 20;
    
    //Computer environmental acceleration on all particals
    for (int i = 0; i<BOIDNUMBER; i++) {
        boidStatesA[i+BOIDNUMBER] = 0;
    }
    
    //Add interactive acceleration to all particals
    for (int i = 0; i<BOIDNUMBER; i++) {
        if (particles[i].getName() != "leader") {
            
            for (int j=0; j<BOIDNUMBER; j++) {
                if (i!=j) {
    
                    Vector3d Xij = boidStates[j] - boidStates[i];
                    double distance = Xij.norm();
                    //avoidance acceleration
                    Vector3d Aija = (Xij.normalize()*(-1*KA/distance)) / (particles[i].getMass());
                    //velocity matching acceleration
                    Vector3d Aijv = (KV * (boidStatesA[j] - boidStatesA[i])) / (particles[i].getMass());
                    //centering acceleration
                    Vector3d Aijc = (KC * Xij) / (particles[i].getMass());
                    
                    Vector3d acceleration(0,0,0);
                    
                    float ar = AMAX;
                    if (0 != Aija.norm()) {
                        acceleration = mini(ar, Aija.norm())*Aija.normalize();
                    }
                    else
                    {
                        acceleration = Aija;
                    }
                    
                    
                    ar = AMAX - acceleration.norm();
                    if (0 != Aijv.norm()) {
                        acceleration = acceleration + Aijv.normalize()*mini(ar, Aijv.norm());
                    }
                    else
                    {
                        acceleration = acceleration + Aijv;
                    }
                    
                    
                    ar = AMAX - acceleration.norm();
                    if (0 != Aijc.norm()) {
                        acceleration = acceleration + mini(ar, Aijc.norm())*Aijc.normalize();
                    }
                    else
                    {
                        acceleration = acceleration +Aijc;
                    }
                    
                    double KD = 0;
                    if (Xij.norm()<R1) {
                        KD = 1;
                    }else if(Xij.norm()>R2)
                    {
                        KD = 0;
                    }else
                    {
                        KD = (R2-Xij.norm())*1.0/(R2 - R1);
                    }
                    boidStatesA[i+BOIDNUMBER] =boidStatesA[i+BOIDNUMBER] + KD*acceleration;
                    
                }
            }
            
            double KAC = 5;
            //avoid collision with obstical
            Vector3d Xbo = obstaclePosition - boidStates[i];
            Vector3d ViN = boidStates[i+BOIDNUMBER].normalize();
            double Sclose = Xbo * ViN;
            if (Sclose > 0 && Sclose< 5) {
                Vector3d Xclose = boidStates[i] + Sclose*ViN;
                Vector3d Vp = Xclose - obstaclePosition;
                double d = Vp.norm();
                if (d < obstacleRadius) {
                    Vector3d Xt = obstaclePosition + obstacleRadius * (Vp.normalize());
                    double dt = (Xt - boidStates[i]).norm();
                    double vt = boidStates[i+BOIDNUMBER] * (Xt - boidStates[i]).normalize();
                    
                    double tt = dt / vt;
                    double Dvs = (boidStates[i+BOIDNUMBER].normalize() % (Xt - boidStates[i])).norm() * 1.0 / tt;
                    double as = 3 * Dvs / tt;
                    Vector3d As = as * Vp.normalize();
                    boidStatesA[i+BOIDNUMBER] =  boidStatesA[i+BOIDNUMBER] + As;
                }
            }
            else
            {
                
                Vector3d particlePositionNew, particleVelocityNew;
                Vector3d Xai = boidStates[i] - sunPosition;
                double distance  = Xai.norm();
                boidStatesA[i+BOIDNUMBER] =  boidStatesA[i+BOIDNUMBER] +  (-0.005 * (distance*distance))*(Xai.normalize()) ;
            }
            
            
        }
        else
        {
            Vector3d particlePositionNew, particleVelocityNew;
            Vector3d Xai = boidStates[i] - sunPosition;
            double distance  = Xai.norm();
            boidStatesA[i+BOIDNUMBER] = (-0.005 * (distance*distance))*(Xai.normalize()) ;
            //boidStatesA[i+BOIDNUMBER] = (-60 * 1.0/(distance*distance))*(Xai.normalize());
        }
        
    }
}

void plantAcceleration()
{
    //Computer environmental acceleration on all particals
    for (int i = 0; i<BOIDNUMBER; i++) {
        boidStatesA[i+BOIDNUMBER] = 0;
    }
    
    //Add interactive acceleration to all particals
    for (int i = 0; i<BOIDNUMBER-1; i++) {
        for (int j=i+1; j<BOIDNUMBER; j++) {
            Vector3d Xij = boidStates[j] - boidStates[i];
            double distance = Xij.norm();
            Vector3d Fij = 0.01 * (1.0/(distance*distance))*(Xij.normalize());
            
            Vector3d acceleration = Fij/(particles[i].getMass());

            
            boidStatesA[i+BOIDNUMBER] =boidStatesA[i+BOIDNUMBER] + acceleration;
            boidStatesA[j+BOIDNUMBER] =boidStatesA[j+BOIDNUMBER] - acceleration;
        }
    }
    
}

void sysDynaFunc()
{
    //plantAcceleration();
    flockAcceleration();
    for (int i =0; i<BOIDNUMBER; i++) {
        boidStatesA[i] = boidStates[i+BOIDNUMBER];
    }
}

//numerical integration function
void statesNumInt()
{
  
    for (int i = 0; i<BOIDNUMBER*2; i++) {
        boidStatesNew[i] = boidStates[i] + boidStatesA[i]*hStep;
    }
    
    
    for (int i = 0 ; i<BOIDNUMBER; i++) {
        if ((boidStatesNew[i] - sunPosition).norm() > 25) {
            boidStatesNew[i] = Vector3d(gauss(0, 5, 1),gauss(10, 2, 1),gauss(0, 3, 1));
            boidStatesNew[i+BOIDNUMBER] = Vector3d(0,0,0);
        }
    }
     
    
    
}

//entire process
void interactiveCalculate()
{
    sysDynaFunc();
    
    statesNumInt();
    for (int i=0; i<BOIDNUMBER; i++) {
        particles[i].setPosition(boidStatesNew[i]);
        particles[i].setVelocity(boidStatesNew[i+BOIDNUMBER]);
        particles[i].setAcceleration(boidStatesA[i+BOIDNUMBER]);
    }
    
    for (int i = 0; i<BOIDNUMBER*2; i++) {
        boidStates[i] = boidStatesNew[i];
    }
    
}


//***************************************//

void init() {
    //LoadParameters(ParamFilename);
    // set up camera
    // parameters are eye point, aim point, up vector
    camera = new Camera(Vector3d(0, 0, 20), Vector3d(0, 0, 0), Vector3d(0, 1, 0));
    
    // black background for window
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_NORMALIZE);
    
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    //glDepthRange(0, 1);
    //glClearDepth (1);
    //glDepthFunc(GL_LEQUAL);
     
    
    
    //Setting lights
    {
        GLfloat light0_position[] = {0.0f, 0.0f, 10.0f, 1.0f}; //spot light
        GLfloat light1_position[] = {0.0f, 10.0f, 0.0f, 1.0f};
        GLfloat light2_position[] = {-10.0f, 0.0f, 0.0f, 1.0f};
        GLfloat light_ambient[]  = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat light_diffuse[]  = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
         
        
        glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
        glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        
        
        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
        glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
        
        glLightfv(GL_LIGHT1, GL_POSITION, light2_position);
        glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
        
        
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_LIGHT2);
        glEnable(GL_LIGHTING);
    }
    
    boidGenerator();
    for (int i=0; i<BOIDNUMBER; i++) {
        boidStates[i] = particles[i].getPosition();
        boidStates[i+BOIDNUMBER] = particles[i].getVelocity();
    }
    particles.reserve(1000);
}

void timeProc(int id)
{
    if (id == 1) {
        //calculatePosition();
        interactiveCalculate();
        glutPostRedisplay();
        if (resetSign == 0) {
            glutTimerFunc(automaticSpeed, timeProc, 1);
        }
    }
    
}


void handleKey(unsigned char key, int x, int y){
    switch(key){
        case 'a':
        case 'A':
            resetSign = 0;
            glutTimerFunc(automaticSpeed, timeProc, 1);
            break;
            
        case 'q':       // q - quit
        case 'Q':
        case 27:        // esc - quit
            exit(0);
            
        case 'g':
        case 'G':
            showGrid = !showGrid;
            break;
            
            
        case 's':
        case 'S':
            resetSign = 1;
            break;
            
        default:        // not a valid key -- just ignore it
            return;
    }
}



int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Particle");
    
    init();
    
    glutDisplayFunc(myDisplay);
    glutMouseFunc(mouseEventHandler);
    glutMotionFunc(motionEventHandler);
    glutKeyboardFunc(handleKey);
    
    glutMainLoop();
    delete camera;
    return 0;
}