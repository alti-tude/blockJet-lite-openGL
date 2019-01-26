#include "main.h"
#include "timer.h"
#include "Player.h"
#include "Particles.h"
#include "StaticObj.h"
#include "DynamicObj.h"
#include "Curve.h"
#include "Boomerang.h"

using namespace std;


GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;
unsigned int shaderColorId;
unsigned int shaderColorActivationId;
/**************************
* Customizable functions *
**************************/

Player player;

Boomerang* boomerang;
float boomerangAccelx = 0.001, boomerangSpeedx = -0.1, boomerangSpeedy = 1.0/60.0, boomerangRot = 10;
unsigned int boomerangFreq = 900;
color_t boomerangColor = {0,0,255};

StaticObj* sevenSegment[7][3];
float segmentWidth = 0.02, segmentHeight = 0.25,shiftSevenSegment = 0.5;

Curve* magnet;
float totalMagnetForce = 0.2;
unsigned int maxMagnetLife = 1, magnetFreq = 2400; //life in seconds

Curve* curve;
unsigned int curveFreq = 1500;
unsigned int maxPosy = 400;
bool arcCollision = 0;
float currentAngle, curveRadius = 2;

vector<Particle> jets;
vector<StaticObj*> platform;
vector<StaticObj*> coins;

vector<DynamicObj*> waterBalls;
float waterBallsLife = 120, waterBallRadius = 0.2, waterBallSpeedy = 0.05, waterBallSpeedx = 0.05;

vector<pair<DynamicObj*, int> > powerBalls;
float powerBallRadius = 0.2, powerBallSpeedy = 0.05, powerBallSpeedx = -0.05;
unsigned int powerBallFreq = 600, powerBallMaxSize = 1;
color_t powerBallColor1 = {0,255,0};
color_t powerBallColor2 = {0,0,255};

struct line{
    StaticObj* fireline;
    StaticObj* left;
    StaticObj* right;
};
vector<line> fireLine;
float maxFireLineHeight = 4, fireLineWidth = 0.1, minFireLineHeight = 1;
unsigned int fireLineFreq = 240, fireLineStagger = 0, fireLineCount = 2;

struct Beam{
    bool activated;
    DynamicObj* left;
    DynamicObj* right;
    DynamicObj* left2;
    DynamicObj* right2;
    vector<Particle*> beam;
} beam;
unsigned int beamParticleCount = 50, beamFreq = 1200, beamParticleLife = 20, beamLife = 240, beamTime=180; 
float beamTermRadius = 0.2, beamSpeedy = 0.01;
float beamParticleWidth = 0.05, beamParticleHeight = 0.05 ;

unsigned int points = 0;
bool closeWindow = 0;

unsigned int jetSize = 200;

unsigned int coinsCount = 3, coinsFreq=180, totalCoinCount = 10;
float coinRadius = 0.1;

float playerWidth = 0.5, playerHeight = 0.5, playerSpeed = 0.05, playerAccely = 0.0020, playerMaxSpeedy = 0.075;
color_t playerColor = {57, 155, 72};
color_t playerColorLoaded = {197, 99, 255};
color_t swordTop = {0,0,0};
color_t swordBottom = {255,0,0};
unsigned int swordTime = 4*60;

float reloadTime = 120;     //in frames
float platformSize = 0.5, platformSpeed = -0.015, maxPlatformSpeed = -0.06, minPlatformSpeed = -0.015;
unsigned int speedUpTime = 0, maxSpeedUpTime = 5*60;

float sideBorder = 0.01, topBorder = 0.01, collisionBuffer = 0.01;

float boundingBoxSize = 0.01;
float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float maxScreenZoom = 4, minScreenZoom = 1, zoomSpeed = 0.1;
float yOffset = -1000, prevYOffset = -1000;

float camera_rotation_angle = 0;

Timer t60(1.0 / 60);
int prevFireTime = -100;

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Send our transformation to the currently bound shader, in the "MVP" uniform
        // For each model you render, since the MVP will be different (at least the M part)
        // Don't change unless you are sure!!
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
    glm::mat4 SVP = VP*glm::scale(glm::vec3(screen_zoom, screen_zoom, 1));
        // Send our transformation to the currently bound shader, in the "MVP" uniform
        // For each model you render, since the MVP will be different (at least the M part)
        // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render

    if(curve!=NULL)curve->Draw(SVP);
    if(magnet!=NULL) magnet->Draw(SVP);

    for(int i=0;i<jets.size();i++)  
        jets[i].Draw(SVP);
    
    for(auto it:coins) (*it).Draw(SVP);
    
    if(beam.activated) {
        for(auto it:beam.beam) it->Draw(SVP);
        beam.left->Draw(SVP);
        beam.left2->Draw(SVP);
        beam.right->Draw(SVP);
        beam.right2->Draw(SVP);
    }
    for(auto it:fireLine)   it.fireline -> Draw(SVP), it.left->Draw(SVP), it.right->Draw(SVP);
    for(auto it:platform)   it -> Draw(SVP);
    for(auto it:waterBalls) it -> Draw(SVP);

    for(auto it:powerBalls)
        it.first->Draw(SVP);

    if(boomerang!=NULL && boomerang->speedx<0) boomerang->Draw(SVP);
    player.Draw(SVP);
    if(boomerang!=NULL && boomerang->speedx>0) boomerang->Draw(SVP);
    //seven segment
    {
        unsigned int temp = points;
        int j = 0;
        if(temp==0){
            sevenSegment[0][j]->Draw(VP);
            sevenSegment[1][j]->Draw(VP);
            sevenSegment[2][j]->Draw(VP);
            sevenSegment[3][j]->Draw(VP);
            sevenSegment[4][j]->Draw(VP);
            sevenSegment[5][j]->Draw(VP);
        }
        while(temp>0) {
            switch(temp%10){
            case 0:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[1][j]->Draw(VP);
                sevenSegment[2][j]->Draw(VP);
                sevenSegment[3][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                break;
            case 1:
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                break;
            case 2:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[2][j]->Draw(VP);
                sevenSegment[3][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            case 3:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[3][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            case 4:
                sevenSegment[1][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            case 5:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[1][j]->Draw(VP);
                sevenSegment[3][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            case 6:
                sevenSegment[1][j]->Draw(VP);
                sevenSegment[2][j]->Draw(VP);
                sevenSegment[3][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            case 7:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                break;
            case 8:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[1][j]->Draw(VP);
                sevenSegment[2][j]->Draw(VP);
                sevenSegment[3][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            case 9:
                sevenSegment[0][j]->Draw(VP);
                sevenSegment[1][j]->Draw(VP);
                sevenSegment[4][j]->Draw(VP);
                sevenSegment[5][j]->Draw(VP);
                sevenSegment[6][j]->Draw(VP);
                break;
            }
            j++;
            temp/=10;
        }
    }

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
    int bright  = glfwGetKey(window, GLFW_KEY_RIGHT);

    if(t60.frameCount-prevFireTime >= reloadTime){
        bool pushed = false;
        if(bup){
            waterBalls.push_back(new DynamicObj(player.position.x, player.position.y+playerHeight, waterBallRadius, 1000, 0, waterBallSpeedy, playerColorLoaded)); 
            waterBalls[waterBalls.size()-1]->life = waterBallsLife;
            player.ChangeColor(playerColor);
            prevFireTime = t60.frameCount;
            pushed = true;
        }
        else if(bdown){
            waterBalls.push_back(new DynamicObj(player.position.x, player.position.y-playerHeight, waterBallRadius, 1000, 0, -waterBallSpeedy, playerColorLoaded)); 
            waterBalls[waterBalls.size()-1]->life = waterBallsLife;
            player.ChangeColor(playerColor);
            prevFireTime = t60.frameCount;
            pushed = true;
        }

        if(bright){
            if(pushed)
                waterBalls[waterBalls.size()-1]->speedx = waterBallSpeedx;
            else{
                waterBalls.push_back(new DynamicObj(player.position.x+playerWidth, player.position.y, waterBallRadius, 1000, waterBallSpeedx, 0, playerColorLoaded)); 
                waterBalls[waterBalls.size()-1]->life = waterBallsLife;
                player.ChangeColor(playerColor);
                prevFireTime = t60.frameCount;
                pushed = true;
            }
        }
        else if(bleft){
            if(pushed)
                waterBalls[waterBalls.size()-1]->speedx = -waterBallSpeedx;
            else{
                waterBalls.push_back(new DynamicObj(player.position.x-playerWidth, player.position.y, waterBallRadius, 1000, -waterBallSpeedx, 0, playerColorLoaded)); 
                waterBalls[waterBalls.size()-1]->life = waterBallsLife;
                player.ChangeColor(playerColor);
                prevFireTime = t60.frameCount;
                pushed = true;
            }
        }
    }

    if(yOffset!=-1000){
        cout << yOffset << endl;
        screen_zoom = min(maxScreenZoom, screen_zoom+(yOffset)*zoomSpeed);
        screen_zoom = max(minScreenZoom, screen_zoom);
        cout << screen_zoom << endl;
        // reset_screen();
        yOffset = -1000;
    }

    if (up) {
        if(!arcCollision) player.Up();

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
        if(!arcCollision) player.Down();
    }
    if(left) {
        if(!arcCollision) player.speedx = -playerSpeed;
    }
    if(right) {
        if(!arcCollision) player.speedx = playerSpeed;
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

void processPlayerCollisions(){
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
    {
        auto it = powerBalls.begin();
        while(it!=powerBalls.end()){
            if(detect_collision( (*it).first->GetBoundingBox(), playerBoundingBox, 0.1 )){
                if(player.swordTime<=0) 
                {
                    if( (*it).second == 2)
                        platformSpeed = maxPlatformSpeed, speedUpTime = maxSpeedUpTime;
                    else 
                        player.GenSword(swordTime);
                }
                else points+=1;

                delete (*it).first;
                powerBalls.erase(it);
            }
            else it++;
        }
    }

    if(boomerang !=NULL && boomerang->speedx>0){
        if(detect_collision(boomerang->GetBoundingBox(), playerBoundingBox, -0.1))
            if(player.swordTime==0) quit(window);
            else boomerang->position.x = 6, points+=20;
    }

    if(beam.activated) 
        for(auto it:beam.beam){
            if(detect_collision(it->GetBoundingBox(), playerBoundingBox, -0.2))
                if(player.swordTime==0) quit(window);
                else beam.left->life = 1, points+=10;
        }
    
    
    {
        auto it = fireLine.begin();
        while(it!=fireLine.end()){
            float angle = (*it).fireline->rotation;
            bounding_box_t lineBoundingBox = (*it).fireline->GetBoundingBox();
            glm::vec3 position(lineBoundingBox.x, lineBoundingBox.y, 0);
            float length = lineBoundingBox.height;

            if(detect_collision_box_line(playerBoundingBox, position, length, angle)){
                cout << "LINE COLLIDE" << endl;
                if(player.swordTime==0) quit(window);
                else (*it).fireline->position.x = -10, points+=10;
            }
            it++;
        }
    }
    
    ////////////////with arc///////////////////////////////
    if(curve!=NULL)
    {
        if( detect_collision_radial(player.position, curve->position, playerBoundingBox.width/2+0.01, curve->radius+0.15) &&
            !detect_collision_radial(player.position, curve->position, playerBoundingBox.width/2+0.01, curve->radius-0.15) &&
            player.position.y > curve->position.y) {
                if(player.swordTime==0) arcCollision = 1;
                else curve->position.x = -7, points+=20;
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

void processWaterCollisions(DynamicObj* waterBall){
    bool floorCollide = 0, ceilCollide = 0, leftCollide = 0, rightCollide = 0;
    bounding_box_t waterBallBoundingBox = waterBall->GetBoundingBox(), collideBox;
    for(int i=0;i<platform.size();i++)
    {
        if(detect_collision( (*platform[i]).GetBoundingBox(), waterBallBoundingBox, collisionBuffer))
            floorCollide = 1, collideBox = (*platform[i]).GetBoundingBox();
    }

    {
        auto it = fireLine.begin();
        while(it!=fireLine.end()){
            float angle = (*it).fireline->rotation;
            bounding_box_t lineBoundingBox = (*it).fireline->GetBoundingBox();
            glm::vec3 position(lineBoundingBox.x, lineBoundingBox.y, 0);
            float length = lineBoundingBox.height;
            if(detect_collision_box_line(waterBallBoundingBox, position, length, angle)){
                cout << "LINE COLLIDE" << endl;
                delete it->fireline;
                delete it->left;
                delete it->right;
                fireLine.erase(it);
            }
            else it++;
        }
    }

    if(4-waterBallRadius/2-topBorder < waterBall->position.y)
        ceilCollide = 1;
  
    if( (floorCollide && waterBall->speedy<0) || (ceilCollide && waterBall->speedy>0) ) {
        waterBall->speedy = -waterBall->speedy;
        if( abs(waterBall->speedy)<= 1.6*GRAVITY )  waterBall->speedy = 0;
    }
    
}

void processPowerCollisions(DynamicObj* powerBall){
    bool floorCollide = 0, ceilCollide = 0, leftCollide = 0, rightCollide = 0;
    bounding_box_t powerBallBoundingBox = powerBall->GetBoundingBox(), collideBox;
    
    for(int i=0;i<platform.size();i++)
    {
        if(detect_collision( (*platform[i]).GetBoundingBox(), powerBallBoundingBox, collisionBuffer))
            floorCollide = 1, collideBox = (*platform[i]).GetBoundingBox();
    }

    if(4-powerBallRadius/2-topBorder < powerBall->position.y)
        ceilCollide = 1;
  

    if( (floorCollide && powerBall->speedy<0) || (ceilCollide && powerBall->speedy>0) ) {
        powerBall->speedy = -powerBall->speedy;
    }
   
}


void tick_elements() {
    if(speedUpTime <=0 ) platformSpeed = minPlatformSpeed;
    else speedUpTime-=1;

    if(!arcCollision)
    {   
        player.gravity();
        
        if(magnet!=NULL){
            float distance = (player.position.x - magnet->position.x)*(player.position.x - magnet->position.x) + (player.position.y - magnet->position.y)*(player.position.y - magnet->position.y);
            float magnetForce = totalMagnetForce;
            float magnetForcex = 0;
            float magnetForcey = 0;

            if(distance>=0.5){ 
                magnetForce = totalMagnetForce/distance;
                magnetForcex = magnetForce * (magnet->position.x - player.position.x)/sqrt(distance);
                magnetForcey = magnetForce * (magnet->position.y - player.position.y)/sqrt(distance);
            }

            player.speedx += magnetForcex;
            player.speedy += magnetForcey;
        }

        processPlayerCollisions();
    }
    else{
        player.speedy = 0;
        player.speedx = 0;
        if(player.position.x > curve->position.x && player.position.y <= curve->position.y) 
            arcCollision = 0, player.rotation = 0;
        else{
            float posx = player.position.x;
            float tmp = (curveRadius*curveRadius)-(posx - curve->position.x)*(posx - curve->position.x);
            float posy = tmp>0? sqrt(tmp)+curve->position.y:curve->position.y;
            player.position.y = posy;
            
            float slope;
            slope = (player.position.x != curve->position.x)? atan( abs(player.position.y - curve->position.y)/(player.position.x - curve->position.x) ) : M_PI/2;
            slope = slope*180/M_PI;
            slope += slope < 0? 180:0;
            slope += player.position.y < curve->position.y? 180:0;
            slope = 360-slope;
            
            player.rotation = slope;
            cout << slope << " " << player.position.x-curve->position.x << " " << player.position.y-curve->position.y<<endl;
        }
    }
    player.Tick();

    for(int i=0;i<platform.size();i++){
        platform[i]->speedx = platformSpeed;
        (*platform[i]).Tick();
        if((*platform[i]).position.x<-4-platformSize/2) 
            (*platform[i]).position.x = 8+2*platformSize + (*platform[i]).position.x;
    }

    for(int i=0;i<jets.size();i++){
        jets[i].Tick();
    }

    /////////////////////coins/////////////////////////
    {
        auto it = coins.begin();
        while(it!=coins.end()){
            (*it)->speedx = platformSpeed;        
            if((**it).position.x < -5) 
                delete *it, coins.erase(it);
            else (**it).Tick(), it++;
        }
    }

    //////////////////////////beam//////////////////////
    if(beam.activated) {
        beam.left->Tick();
        beam.left2->Tick();
        beam.right->Tick();
        beam.right2->Tick();

        if(beam.left->life==0) {
            beam.activated = false;
            delete beam.left;
            delete beam.left2;
            delete beam.right;
            delete beam.right2;
            
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

    ////////////////////////fireLine///////////////////
    {
        auto it = fireLine.begin();
        while(it!=fireLine.end()){
            (*it).fireline->speedx = platformSpeed;
            (*it).left->speedx = platformSpeed;
            (*it).right->speedx = platformSpeed;
            (*it).fireline->Tick();
            (*it).right->Tick();
            (*it).left->Tick();
            
            if((*it).fireline->position.x < -8){
                delete it->fireline;
                delete it->left;
                delete it->right;
                fireLine.erase(it);
            }
            else it++;
        }
    }

    //////////////////////waterBalls/////////////////
    {
        auto it = waterBalls.begin();
        while(it!=waterBalls.end()){
            (*it)->Gravity();
            processWaterCollisions(*it);
            (*it)->Tick();

            if((*it)->life == 0){
                delete *it;
                waterBalls.erase(it);
            }
            else it++;
        } 
    }  
    ///////////////////powerBall////////////////////
    {
        auto it = powerBalls.begin();
        while(it!=powerBalls.end()){
            (*it).first->Gravity();
            processPowerCollisions((*it).first);
            (*it).first->Tick();

            if((*it).first->position.x < -5){
                delete (*it).first;
                powerBalls.erase(it);
            }
            else it++;
        }
    }

    ///////////////////curve////////////////////////
    if(curve!=NULL){
        curve->speedx = platformSpeed;
        curve->Tick();
        if(curve->position.x < -7) delete curve, curve=NULL;
    }

    /////////////////magnet/////////////////////////
    if(magnet!=NULL){
        magnet->Tick();
        if(magnet->life<=0) delete magnet, magnet=NULL;
    }

    ///////////////boomerang///////////////////////
    if(boomerang!=NULL){
        boomerang->speedx += boomerangAccelx;
        boomerang->rotation += boomerangRot;
        if(boomerang->rotation >= 360) boomerang->rotation-=360;
        boomerang->Tick();
        if(boomerang->position.x > 5) delete boomerang, boomerang = NULL;
    }
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
        beam.left2 = new DynamicObj(-4+beamTermRadius, posy+2, beamTermRadius, 1000, 0, -beamSpeedy, COLOR_BLACK);
        beam.right = new DynamicObj(4-beamTermRadius, posy, beamTermRadius, 1000, 0, beamSpeedy, COLOR_BLACK);
        beam.right2 = new DynamicObj(4-beamTermRadius, posy+2, beamTermRadius, 1000, 0, -beamSpeedy, COLOR_BLACK);
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
            beam.beam.push_back(new Particle((8.0-5*beamTermRadius)/(float)beamParticleCount*i-4+beamTermRadius*2+posx, beam.left2->position.y+posy, beamParticleWidth, beamParticleHeight, beamParticleLife, COLOR_RED));            
        }
    }

    if((t60.frameCount-fireLineStagger)%fireLineFreq==0 && fireLine.size()<fireLineCount && rand()%2==0){
        float height = (float)(rand()%(int)(maxFireLineHeight*100))/100;
        if(height >= minFireLineHeight){
            float width = fireLineWidth;
            float angle = rand()%45+1;
            float posy = (float)(rand()%800)/100 - 4;
            StaticObj* fireline =  new StaticObj(5, posy, width, height, platformSpeed, angle, COLOR_BLACK);
            angle = angle * M_PI/180;
            StaticObj* left =  new StaticObj(5-height/2*sin(angle), posy-height/2*cos(angle), width, 1000, platformSpeed, {255,0,0});
            StaticObj* right =  new StaticObj(5+height/2*sin(angle), posy+height/2*cos(angle), width, 1000, platformSpeed, {255,0,0});
            fireLine.push_back({fireline, left, right});
        }
    }

    if( (t60.frameCount)%curveFreq==0 && curve==NULL) {
        float posy = ((rand()%maxPosy)-maxPosy/2.0)/100;

        curve = new Curve(7,posy, curveRadius, 0.5, platformSpeed, 0, COLOR_RED);
        curve -> CurveInside(0.05, COLOR_BACKGROUND);
    }

    if(t60.frameCount%magnetFreq==0 && magnet==NULL){
        magnet = new Curve(0,(rand()%40)/10-2, 0.5, 0.75, platformSpeed, 45, COLOR_RED);
        magnet->CurveInside(0.1, COLOR_BACKGROUND);
        magnet->life = (maxMagnetLife)*60;
    }

    if(t60.frameCount%powerBallFreq == 0 && powerBalls.size()<powerBallMaxSize){
        if(rand()%3!=1) {
            powerBalls.push_back(make_pair(new DynamicObj(5, (float)(rand()%600)/100-2.0, powerBallRadius, (int)1000, powerBallSpeedx, powerBallSpeedy, powerBallColor1), 1));
        }
        else {
            powerBalls.push_back(make_pair(new DynamicObj(5, (float)(rand()%600)/100-2.0, powerBallRadius, (int)1000, powerBallSpeedx, powerBallSpeedy, powerBallColor2), 2));
        }
    }

    if(t60.frameCount%boomerangFreq == 0 && boomerang==NULL){
        float posy = min((float)2, player.position.y);
        float posx = max(player.position.x+2, (float)0);
        boomerang = new Boomerang(posx, posy, playerWidth, playerHeight,boomerangSpeedx,boomerangSpeedy,playerHeight/5, boomerangColor, COLOR_BACKGROUND);
    }   
}
/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */


void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models
    player = Player(0, 0, playerWidth, playerHeight, playerAccely, playerMaxSpeedy, playerColor, swordTop, swordBottom);
    player.reloadSpeed = {
        (playerColorLoaded.r - playerColor.r)/reloadTime,
        (playerColorLoaded.g - playerColor.g)/reloadTime,
        (playerColorLoaded.b - playerColor.b)/reloadTime
    };
    player.loadedColor = playerColorLoaded;

    curve = NULL;
    magnet = NULL;
    boomerang = NULL;

    sevenSegment[0][0] = new StaticObj(0, segmentHeight, segmentWidth, segmentHeight, 0, 90, {0,0,0});
    sevenSegment[1][0] = new StaticObj(-segmentHeight/2, segmentHeight/2, segmentWidth, segmentHeight, 0, 0, {0,0,0});
    sevenSegment[2][0] = new StaticObj(-segmentHeight/2, -segmentHeight/2, segmentWidth, segmentHeight, 0, 0, {0,0,0});
    sevenSegment[3][0] = new StaticObj(0, -segmentHeight, segmentWidth, segmentHeight, 0, 90, {0,0,0});
    sevenSegment[4][0] = new StaticObj(segmentHeight/2, -segmentHeight/2, segmentWidth, segmentHeight, 0, 0, {0,0,0});
    sevenSegment[5][0] = new StaticObj(segmentHeight/2, segmentHeight/2, segmentWidth, segmentHeight, 0, 0, {0,0,0});
    sevenSegment[6][0] = new StaticObj(0, 0, segmentWidth, segmentHeight, 0, 90, {0,0,0});

    for(int i=1;i<3;i++)
        for(int j=0;j<7;j++){
            StaticObj* prev = sevenSegment[j][i-1];
            sevenSegment[j][i] = new StaticObj(prev->position.x-shiftSevenSegment, prev->position.y, segmentWidth, segmentHeight, 0, prev->rotation, {0,0,0});
        }

    for(int i=0;i<3;i++)
        for(int j=0;j<7;j++){
            sevenSegment[j][i]->position.x+=4-0.1-segmentHeight;
            sevenSegment[j][i]->position.y+=4-0.1-segmentHeight;
        }
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
    shaderColorId = glGetUniformLocation(programID, "u_vertexColor");
    shaderColorActivationId = glGetUniformLocation(programID, "u_vertexColorActivated");
    glUniform1i(shaderColorActivationId, 0);

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
            glUniform1i(shaderColorActivationId, 0);

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

template<typename T>
bool detect_collision_radial(T a, T b, float radius_b, float radius_a) {
    return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) < radius_b*radius_b+radius_a*radius_a;
}

bool detect_collision_line_line(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4){
    float uA = ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    float uB = ((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3)) / ((y4-y3)*(x2-x1) - (x4-x3)*(y2-y1));
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        return true;
    }
    return false;
}

bool detect_collision_box_line(bounding_box_t box, glm::vec3 line, float length, float angle){
    float x3 = line.x - length/2.0*sin(angle*M_PI/180), x4 = line.x+length/2.0*sin(angle*M_PI/180);
    float y3 = line.y - length/2.0*cos(angle*M_PI/180), y4 = line.y+length/2.0*cos(angle*M_PI/180);

    float x1 = box.x+box.width/2.0, x2 =  box.x+box.width/2.0;
    float y1 = box.y+box.height/2.0, y2 =  box.y-box.height/2.0;
    bool c1 = detect_collision_line_line(x1,y1,x2,y2,x3,y3,x4,y4);

    x1 = box.x+box.width/2.0, x2 =  box.x-box.width/2.0;
    y1 = box.y+box.height/2.0, y2 =  box.y+box.height/2.0;
    bool c2 = detect_collision_line_line(x1,y1,x2,y2,x3,y3,x4,y4);

    x1 = box.x+box.width/2.0, x2 =  box.x-box.width/2.0;
    y1 = box.y-box.height/2.0, y2 =  box.y-box.height/2.0;
    bool c3 = detect_collision_line_line(x1,y1,x2,y2,x3,y3,x4,y4);
    
    x1 = box.x-box.width/2.0, x2 =  box.x-box.width/2.0;
    y1 = box.y-box.height/2.0, y2 =  box.y+box.height/2.0;
    bool c4 = detect_collision_line_line(x1,y1,x2,y2,x3,y3,x4,y4);

    return c1||c2||c3||c4;
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
