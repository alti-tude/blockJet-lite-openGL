#include "main.h"
#include "timer.h"
#include "Player.h"
#include "Particles.h"
#include "StaticObj.h"
#include "DynamicObj.h"

using namespace std;


GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Player player;
vector<Particle> jets;
vector<StaticObj*> platform;
vector<StaticObj*> coins;

vector<StaticObj*> fireLine;
float maxFireLineHeight = 4, fireLineWidth = 0.1;
unsigned int fireLineFreq = 240, fireLineStagger = 0, fireLineCount = 3;

struct Beam{
    bool activated;
    DynamicObj* left;
    DynamicObj* right;
    vector<Particle*> beam;
} beam;

vector<DynamicObj> waterBalls;

unsigned int beamParticleCount = 50, beamFreq = 1200, beamParticleLife = 20, beamLife = 240, beamTime=180; 
float beamTermRadius = 0.2, beamSpeedy = 0.01;
float beamParticleWidth = 0.05, beamParticleHeight = 0.05 ;

unsigned int points = 0;
bool closeWindow = 0;

unsigned int jetSize = 200;

unsigned int coinsCount = 3, coinsFreq=180, totalCoinCount = 10;
float coinRadius = 0.1;

float playerWidth = 0.5, playerHeight = 0.5, playerSpeed = 0.05, playerAccely = 0.0020, playerMaxSpeedy = 0.075;
float platformSize = 0.5, platformSpeed = -0.0075;
float sideBorder = 0.01, topBorder = 0.01, collisionBuffer = 0.01;

float boundingBoxSize = 0.01;
float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;

Timer t60(1.0 / 60);
int prev = -100;
/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 0,0,10 );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    player.Draw(VP);
    for(int i=0;i<jets.size();i++)  
        jets[i].Draw(VP);
    
    for(auto it:coins) (*it).Draw(VP);
    
    if(beam.activated) {
        for(auto it:beam.beam) it->Draw(VP);
        beam.left->Draw(VP);
        beam.right->Draw(VP);
    }
    for(auto it:fireLine) it -> Draw(VP);
    for(auto it:platform) (*it).Draw(VP);
}


void tick_input(GLFWwindow *window) {
    int left    = glfwGetKey(window, GLFW_KEY_A);
    int right   = glfwGetKey(window, GLFW_KEY_D);
    int up      = glfwGetKey(window, GLFW_KEY_W);
    int down    = glfwGetKey(window, GLFW_KEY_S);
    int print   = glfwGetKey(window, GLFW_KEY_P);
    int quit    = glfwGetKey(window, GLFW_KEY_Q);
    quit       += glfwGetKey(window, GLFW_KEY_ESCAPE);
    int bup     = glfwGetKey(window, GLFW_KEY_UP);
    int bdown   = glfwGetKey(window, GLFW_KEY_DOWN);
    int bleft   = glfwGetKey(window, GLFW_KEY_LEFT);
    int bright   = glfwGetKey(window, GLFW_KEY_RIGHT);


    if(bup){
        player.ChangeColor(COLOR_RED);
    }
    if (up) {
        player.Up();

        //jets ----------------------------------------
        int count = 3;
        for(int i=0;i<jets.size();i++){    
            
            int r = rand();
            float posx = (-1*r%2 + (1-r%2))*(r% (int)((playerWidth/2)*100))/100.0;
            r = rand();
            float posy = (-1*r%2 + (1-r%2))*(r%(int)((playerHeight/2)*100))/100.0;
 
            if(jets[i].GetLife() <= 0){
                jets[i].SetPosition(player.position.x + posx, player.position.y - 3.0/2.0*playerHeight + posy);
                jets[i].GiveLife(60);
                jets[i].speedy = player.speedy;
                count--;
            }

            if(count <=0) break;
        }
        //--------------------------------------------
    }
    if(down) {
        player.Down();
    }
    if(left) {
        player.speedx = -playerSpeed;
    }
    if(right) {
        player.speedx = playerSpeed;
    }

    if (print) {
        cout << "PRINT POS - " << player.position.x << "  " << player.position.y << endl;
        cout << "PRINT SPEED - " << player.speedx << " " << player.speedy << endl;
    }

    if(quit) {
        cout << "SCORE - " << points << endl;
        closeWindow = 1;
    }

}

void processCollisions(){
    bool floorCollide = 0, ceilCollide = 0, leftCollide = 0, rightCollide = 0;
    bounding_box_t playerBoundingBox = player.GetBoundingBox(), collideBox;
    for(int i=0;i<platform.size();i++)
    {
        if(detect_collision( (*platform[i]).GetBoundingBox(), playerBoundingBox, collisionBuffer))
            floorCollide = 1, collideBox = (*platform[i]).GetBoundingBox();
    }
    {
        auto it = coins.begin();
        while(it!=coins.end()){
            if(detect_collision( (**it).GetBoundingBox(), playerBoundingBox, 0))
                delete *it, coins.erase(it), points++;
            else it++;
        }
    }

    if(beam.activated) 
        for(auto it:beam.beam){
            if(detect_collision(it->GetBoundingBox(), playerBoundingBox, -0.2))
                quit(window);
        }
    
    {
        auto it = fireLine.begin();
        float playerRadius = sqrt(playerBoundingBox.height/2*playerBoundingBox.height/2 + playerBoundingBox.width/2*playerBoundingBox.width/2);
        while(it!=fireLine.end()){
            vector<bounding_box_t>& boundingBoxes = (*it)->GetBoundingBox(boundingBoxSize);
            for(auto it2:boundingBoxes){
                if(detect_collision_radial(it2, playerBoundingBox, boundingBoxSize/2,playerRadius))
                {
                    cout << it2.x << " " << it2.y << " " << playerBoundingBox.x << " " << playerBoundingBox.y << endl;
                    cout << "collide" << endl, quit(window);
                }
            }
            it++;
        }
    }

    if(4-playerHeight/2-topBorder < player.position.y)
        ceilCollide = 1;
    if(player.position.x<=-4+playerWidth/2+sideBorder) 
        leftCollide = 1;
    if(player.position.x>=4-playerWidth/2-sideBorder) 
        rightCollide = 1;

    if( (floorCollide && player.speedy<0) || (ceilCollide && player.speedy>0) ) {
        player.speedy = -player.speedy*3.0/5.0;
        if( abs(player.speedy)<= 1.6*GRAVITY )  player.speedy = 0;
    }
    if( (leftCollide && player.speedx<0) || (rightCollide && player.speedx > 0) )
        player.speedx = 0;
}


void tick_elements() {
    player.gravity();

    processCollisions();

    for(int i=0;i<platform.size();i++){
        (*platform[i]).Tick();
        if((*platform[i]).position.x<-4-platformSize/2) 
            (*platform[i]).position.x = 8+2*platformSize + (*platform[i]).position.x;
    }

    for(int i=0;i<jets.size();i++){
        jets[i].Tick();
    }


    {
        auto it = coins.begin();
        while(it!=coins.end()){
            if((**it).position.x < -5) 
                delete *it, coins.erase(it);
            else (**it).Tick(), it++;
        }
    }

    if(beam.activated) {
        beam.left->Tick();
        beam.right->Tick();

        if(beam.left->life==0) {
            beam.activated = false;
            delete beam.left;
            delete beam.right;
            
            auto it=beam.beam.begin();
            while(it!=beam.beam.end()){
                delete *it, beam.beam.erase(it);
            }
        }
        else{
            for(auto it:beam.beam)
                it -> Tick();
            auto it=beam.beam.begin();
            while(it!=beam.beam.end()){
                if((*it)->GetLife()==0) 
                    delete *it, beam.beam.erase(it);
                else it++;
            }
        }
    }
    {
        auto it = fireLine.begin();
        while(it!=fireLine.end()){
            (*it)->Tick();
            if((*it)->position.x < -8){
                delete *it;
                fireLine.erase(it);
            }
            else it++;
        }
    }
    player.Tick();
}

void levelGenerator(){ 

    if(t60.frameCount%coinsFreq==0 && coins.size()<totalCoinCount)
        for(int i=0;i<rand()%coinsCount;i++){
            coins.push_back(new StaticObj(5,(rand()%800)/100.0-4+3*platformSize/2,coinRadius,1000,platformSpeed,COLOR_RED));
    }

    if(t60.frameCount%beamFreq==0 && !beam.activated)
    {
        beam.activated = true;
        float posy = (rand()%600)/100 -4+5.0/2.0*platformSize;
        beam.left = new DynamicObj(-4+beamTermRadius, posy, beamTermRadius, 1000, 0, beamSpeedy, COLOR_BLACK);
        beam.right = new DynamicObj(4-beamTermRadius, posy, beamTermRadius, 1000, 0, beamSpeedy, COLOR_BLACK);
        beam.left -> life = beamLife;
        beam.right -> life = beamLife;
    }

    if(beam.activated && t60.frameCount%10==0 && beam.left->life<beamTime){
        for(int i=1;i<=beamParticleCount;i++){
            int r = rand();
            float posy = (-1*r%2 + (1-r%2))*(r%(int)(beamTermRadius*100))/100.0;
            r = rand();
            float posx = (-1*r%2 + (1-r%2))*(r%(int)(8.0/(float)beamParticleCount*100))/100.0;
            beam.beam.push_back(new Particle((8.0-5*beamTermRadius)/(float)beamParticleCount*i-4+beamTermRadius*2+posx, beam.left->position.y+posy, beamParticleWidth, beamParticleHeight, beamParticleLife, COLOR_RED));
        }
    }

    if((t60.frameCount-fireLineStagger)%fireLineFreq==0 && fireLine.size()<fireLineCount && rand()%2==0){
        float height = (float)(rand()%(int)(maxFireLineHeight*100))/100;
        float width = fireLineWidth;
        float angle = rand()%45+1;
        float posy = (float)(rand()%800)/100 - 4;
        fireLine.push_back( new StaticObj(5, posy, width, height, platformSpeed, angle, COLOR_BLACK) );
    }
}
/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */


void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    player = Player(0, 0, playerWidth, playerHeight, playerAccely, playerMaxSpeedy, COLOR_GREEN);
    
    for(unsigned int i=0;i<jetSize;i++)  
        jets.push_back(Particle(0,0,playerWidth/5, playerWidth/5, 0, COLOR_RED));
    
    //platform gen
    for(int i=0;i<8/platformSize+2;i++)
        if(i%2==0)
            platform.push_back(new StaticObj(i*platformSize-4+platformSize/2, -4+platformSize/2, platformSize, platformSize, platformSpeed, 0, COLOR_BLACK));
        else
            platform.push_back(new StaticObj(i*platformSize-4+platformSize/2, -4+platformSize/2, platformSize, platformSize, platformSpeed, 0, COLOR_RED));
    
    for(int i=0;i<8/platformSize+2;i++)
         if(i%2==1)
            platform.push_back(new StaticObj(i*platformSize-4+platformSize/2, -4+platformSize/2 + platformSize, platformSize, platformSize, platformSpeed, 0, COLOR_BLACK));
        else
            platform.push_back(new StaticObj(i*platformSize-4+platformSize/2, -4+platformSize/2 + platformSize, platformSize, platformSize, platformSpeed, 0, COLOR_RED));
    
        
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 800;
    int height = 800;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!closeWindow) {
        // Process timers

        if (t60.processTick()) {
            t60.frameCount++;
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);
            levelGenerator();
            tick_input(window);
            tick_elements();
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b, float buffer) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width + buffer)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height + buffer));
}
bool detect_collision_radial(bounding_box_t a, bounding_box_t b, float radius_b, float radius_a) {
    return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) < radius_b*radius_b+radius_a*radius_a;
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
