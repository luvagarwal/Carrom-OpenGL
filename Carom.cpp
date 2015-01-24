#include <bits/stdc++.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include "classes.cpp"
using namespace std;

#define FRICTION 0.0005f
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
// Eccentricities
#define E_WALL_COIN 0.6
#define E_COIN_STRIKER 0.8
#define E_STRIKER_WALL 0.8
#define E_STRIKER_COIN 0.8

#define RATIO_MASS_STRIKER_COIN 0.75

#define MIN_VEL 0.001f

// COLORS
#define WHITE_COLOR 1.0f-MODE*0.5, 1.0f+MODE*0.5, 1.0f+MODE*0.5
#define BLACK_COLOR 0.2f, 0.2f, 0.2f
#define QUEEN_COLOR 1.0f, 0.0f, 0.0f
#define STRIKER_COLOR 0.0f+MODE*0.5, 1.0f, 0.1f
#define BACKGROUND_COLOR 0.1+MODE*0.5, 0.1+MODE*0.5, 0.1
#define CENTRE_TRIANGLE_COLOR_1 0.0f, 0.3f, 1.0f
#define CENTRE_CIRCLE_COLOR 1.000+MODE*0.5, 0.271, 0.000
#define INNER_STRIP_COLOR 1.000+MODE*0.5, 0.271, 0.000
#define POT_COLOR 0.0f, 0.0f, 0.0f
#define BROWN_COLOR 0.8, 0.5, 0.9
#define POINTER_COLOR 0.5, 0.5+MODE*0.5, 0.8
#define BAR_COLOR(a, b, c) 0.0+a, 1+b, 0.0+c
#define BOARD_INNER_COLOR 1.0f+MODE*0.5, 0.8f+MODE*0.5, 0.3f+MODE*0.5
#define BOARD_OUTER_COLOR 0.5+MODE*0.5, 0.3, 0.1
#define STRIP_CIRCLE_COLOR 1.000+MODE*0.5, 0.271, 0.000
#define SCORE_COLOR 1.0f+MODE*0.5,1.0f+MODE*0.5,0.0f


#define BAR_HEIGHT 5.0f
#define RAD_BALL 0.2f
#define RAD_STRIKER 0.24f
#define MAX_VELOCITY 0.2f
#define STRIKER_RESET_POSITION 1.95

int STATE = 0; 
int Score = 30;
int MODE = 0;
class Coin{

public:
    static vector<Coin *> instances;
    float px, py, radius, vx, vy, pointer_direction, magnitude;
    string type;
    int value;

	Coin(float px, float py, float radius, string type){
        this->px = px; this->py = py; this->radius = radius;
        this->type = type;
        this->vx = 0.0f; this->vy = 0.0f;
        this->pointer_direction = PI/2;
        instances.push_back(this);
    }

    void draw(){
        glPushMatrix();
        glTranslatef(px, py, 0.0f);
        if(type == "queen")
            glColor3f(QUEEN_COLOR);
        else if(type == "black")
            glColor3f(BLACK_COLOR);
        else if(type=="white")
            glColor3f(WHITE_COLOR);
        else if(type=="striker")
            glColor3f(STRIKER_COLOR);

        Shape::create_circle(this->radius, true);
        glPopMatrix();
    }

    float get_direction_sin(){
        float vx = this->vx;
        float vy = this->vy;
        return vy/sqrt(vx*vx+vy*vy);
    }
    float get_direction_cos(){
        float vx = this->vx;
        float vy = this->vy;
        return vx/sqrt(vx*vx+vy*vy);
    }
	void move(){
        this->px += this->vx;
        this->py += this->vy;
        if(abs(this->vx) > MIN_VEL)
            this->vx -= FRICTION*get_direction_cos();
       else
            this->vx = 0;
        if(abs(this->vy) > MIN_VEL)
            this->vy -= FRICTION*get_direction_sin();
        else
            this->vy = 0;
	}

    void set_velocity(float vx, float vy){
        this->vx = vx; this->vy = vy;
    }

    void reset_striker(){
        this->py = -STRIKER_RESET_POSITION;
        this->px = 0.0f;
        this->vx = 0; this->vy = 0;
    }

    void apply_pointer(){
        glColor3f(POINTER_COLOR);
        Shape::create_line(this->px, this->py, this->px + 1.0*cos(this->pointer_direction), this->py + 1.0*sin(this->pointer_direction));
    }

    void shift_right(){
        this->px += 0.1f;
        if(this->px > 1.6){
            this->px = 1.6;
        }
    }

    void shift_left(){
        this->px -= 0.1f;
        if(this->px < -1.6){
            this->px = -1.6;
        }
    }

};

vector<Coin *> Coin::instances;

class Board{
public:
    static float collision_extreme_x, collision_extreme_y, pot_radius;
    static Coin* striker;
    float length, breadth;
    
    Board(){
        collision_extreme_x = 2.8f;
        collision_extreme_y = collision_extreme_x;
    }
    void create(){
        create_square(6.3f, BOARD_OUTER_COLOR, true);
        create_square(2*collision_extreme_x, BOARD_INNER_COLOR, true);
        create_strips();
        create_centre(0.8f, CENTRE_CIRCLE_COLOR);
        create_pots();
    }

    void insert_coins(){
        new Coin(0.0f, 0.0f, RAD_BALL, "queen");
        for(int i=0; i<6; i++){
            new Coin((2*RAD_BALL+0.2)*cos(DEG2RAD(i*60)), (2*RAD_BALL+0.2)*sin(DEG2RAD(i*60)), RAD_BALL, (i%2==0)?"white":"black");
        }
        glPopMatrix();
    }

    void insert_striker(){
        striker = new Coin(2.8-1.15, 0.0f, RAD_STRIKER, "striker");
        striker->reset_striker();
    }

    void draw_coins(){
        for(unsigned i=0; i<Coin::instances.size(); i++){
            Coin::instances[i]->draw();
            Coin::instances[i]->move();
        }
    }

    void delete_coins_striker(){
        for(unsigned i=0; i<Coin::instances.size(); i++){
            Coin::instances.erase(Coin::instances.begin()+i);
            i--;
        }

    }

    void draw_score(int a, float x, float y) {
        glRasterPos2f(x, y);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, a);

    }

    static void draw_bar(float height){
        float width = 1.3f;
        glPushMatrix();
        glTranslatef(-3.6f, -2.5f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(-width, 0.0f);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, height);
        glVertex2f(-width, height);
        glEnd();
        glPopMatrix(); 
    }

private:
    void create_centre(float radius, float r, float g, float b){
        glColor3f(r, g, b);
        Shape::create_circle(radius, false);
        Shape::create_circle(radius-0.1, false);
    }

    void create_square(float length, float r, float g, float b, bool fill){

        glColor3f(r, g, b);
        if(!fill)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        Shape::create_square(length);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void create_strips(){
        glPushMatrix();
        for(int i=0; i<4; i++){
            create_strip();
            glRotatef(90, 0.0f, 0.0f, 1.0f);
        }
        glPopMatrix();
    }

    void create_strip(){
        float dis_bw_lines = 1.1 - 0.6;
        float half_length = Board::collision_extreme_x-1.2;
        glPushMatrix();
        
        glTranslatef(0.0f, Board::collision_extreme_x-1.1, 0.0f);
        glColor3f(INNER_STRIP_COLOR);
        Shape::create_line(-half_length, 0.0f, half_length, 0.0f);
        glTranslatef(0.0f, dis_bw_lines, 0.0f);
        Shape::create_line(-half_length, 0.0f, half_length, 0.0f);
        
        glTranslatef(-half_length, -dis_bw_lines/2, 0.0f);
        glColor3f(STRIP_CIRCLE_COLOR);
        Shape::create_circle(dis_bw_lines/2, false);
        Shape::create_circle(dis_bw_lines/2-0.1, true);
        glTranslatef(2*half_length, 0.0f, 0.0f);
        Shape::create_circle(dis_bw_lines/2, false);
        Shape::create_circle(dis_bw_lines/2-0.1, true);
        glPopMatrix();
    }

    void create_arrows(){

    }

    void create_pots(){
        glPushMatrix();
        for(int i=0; i<4; i++){
            glRotatef(90, 0, 0, 1);
            create_pot();
        }
        glPopMatrix();
    }

    void create_pot(){
        glPushMatrix();
        glColor3f(POT_COLOR);
        glTranslatef(Board::collision_extreme_x-Board::pot_radius, Board::collision_extreme_x-Board::pot_radius, 0.0f);
        Shape::create_circle(Board::pot_radius, true);
        glPopMatrix();

    }
};

float Board::collision_extreme_y;
float Board::collision_extreme_x;
float Board::pot_radius = 0.3f;
Coin* Board::striker;

class Collision{
public:

    static pair<float, float> get_newvel_along_normal(float ratio_mass, float v1, float v2){
        float newv1 = ((ratio_mass-1)*v1 + 2*v2)/(ratio_mass+1);
        float newv2 = (2*ratio_mass*v1+(1-ratio_mass)*v2)/(ratio_mass+1);
        return make_pair(newv1, newv2);
    }

    static void check_inter_collision(){
        for(unsigned i=0; i<Coin::instances.size(); i++){
            for(unsigned j=i+1; j<Coin::instances.size(); j++){
                float deltax = Coin::instances[j]->px - Coin::instances[i]->px;
                float deltay = Coin::instances[j]->py - Coin::instances[i]->py;
                float deltarad = Coin::instances[i]->radius + Coin::instances[j]->radius;
                if(sqrt(deltay*deltay+deltax*deltax) <= deltarad){
                    float cos_, sin_;
                    cos_ = deltax/sqrt(deltax*deltax+deltay*deltay);
                    sin_ = deltay/sqrt(deltax*deltax+deltay*deltay);

                    float v1n, v2n, v1t, v2t;
                    v1n = (Coin::instances[i]->vy)*sin_ + (Coin::instances[i]->vx)*cos_;
                    v1t = (Coin::instances[i]->vy)*cos_ - (Coin::instances[i]->vx)*sin_;
                    v2n = (Coin::instances[j]->vy)*sin_ + (Coin::instances[j]->vx)*cos_;
                    v2t = (Coin::instances[j]->vy)*cos_ - (Coin::instances[j]->vx)*sin_;
                    
                    pair<float, float> vnpair = get_newvel_along_normal(RATIO_MASS_STRIKER_COIN, v1n, v2n);
                    v1n = vnpair.first;
                    v2n = vnpair.second;

                    float newvx1 = v1n*cos_ - v1t*sin_;
                    float newvy1 = v1n*sin_ + v1t*cos_;
                    float newvx2 = v2n*cos_ - v2t*sin_;
                    float newvy2 = v2n*sin_ + v2t*cos_;

                    Coin::instances[i]->vx = newvx1;
                    Coin::instances[i]->vy = newvy1;
                    Coin::instances[j]->vx = newvx2;
                    Coin::instances[j]->vy = newvy2;

                    while(deltax*deltax+deltay*deltay < deltarad*deltarad){
                    
                    if(newvx1 !=0)
                        Coin::instances[i]->px += (Coin::instances[i]->vx)*1;
                    if(newvy1 !=0)
                        Coin::instances[i]->py += (Coin::instances[i]->vy)*1;
                    if(newvx2 !=0)
                        Coin::instances[j]->px += (Coin::instances[j]->vx)*1;
                    if(newvy1 !=0)
                        Coin::instances[j]->py += (Coin::instances[j]->vy)*1;
 
                    deltax = Coin::instances[j]->px - Coin::instances[i]->px;
                    deltay = Coin::instances[j]->py - Coin::instances[i]->py;
                   } 
                }
            }
        }
    }

    static void check_wall_collision(){
        for(unsigned i=0; i<Coin::instances.size(); i++){
            float tmpx = Coin::instances[i]->px;
            float tmpy = Coin::instances[i]->py;
            float tmpradius = Coin::instances[i]->radius;
            if(abs(tmpx) + tmpradius > Board::collision_extreme_x){
                Coin::instances[i]->vx *= -E_WALL_COIN;
                Collision::bahar_nikalo_x(Coin::instances[i]);
            }
            if(abs(tmpy) + tmpradius >= Board::collision_extreme_y){
                Coin::instances[i]->vy *= -E_WALL_COIN;
                Collision::bahar_nikalo_y(Coin::instances[i]);
            }
        }
    }

    static void check_pot(){
        for(unsigned i=0; i<Coin::instances.size(); i++){
            float tmpx = Coin::instances[i]->px;
            float tmpy = Coin::instances[i]->py;
            float tmpradius = Coin::instances[i]->radius;
            if((abs(tmpx) + tmpradius >= Board::collision_extreme_x - Board::pot_radius) && (abs(tmpy) + tmpradius >= Board::collision_extreme_y - Board::pot_radius)){
                string type = Coin::instances[i]->type;
                if(type != "striker"){
                    Coin::instances.erase(Coin::instances.begin()+i);
                    i--;
                }
                else{
                    Board::striker->reset_striker();
                }
                if(type == "queen")
                    Score += 15;
                else if(type == "black")
                    Score += 5;
                else if(type=="white")
                    Score += 10;
                else if(type=="striker"){
                    Score -= 5;
                }
            }
        }
    }

    static void bahar_nikalo_x(Coin* goti){
        while(abs(goti->px)+RAD_BALL > Board::collision_extreme_x)
            goti->px += goti->vx;
    }

    static void bahar_nikalo_y(Coin* goti){
        while(abs(goti->py)+RAD_BALL > Board::collision_extreme_y)
            goti->py += goti->vy;
    }
};

class Bar{
public:
    static float delcolor;
    static float direction;
    static void set_magnitude(){
        Board::striker->magnitude += (direction)*0.002f;
        if(Board::striker->magnitude <= 0){
            direction = 1;
            delcolor = 0;
        }
        if(Board::striker->magnitude >= MAX_VELOCITY)
            direction = -1;
    }
    static void draw(){
        Board::draw_bar(BAR_HEIGHT);
        delcolor += (direction)*0.01;
        glColor3f(BAR_COLOR(delcolor, -delcolor, 0));
        Board::draw_bar((Board::striker->magnitude)*BAR_HEIGHT/MAX_VELOCITY);
    }
};

float Bar::direction = 1;
float Bar::delcolor = 0;

Board board;

void handleResize(int w, int h);
void handleResize(int w, int h);
void drawScene();
void initRendering();
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);

void decrement_score_every_second(int value){
    Score--;
    glutTimerFunc(1000, decrement_score_every_second, 0);
}

float downX, downY;

void dragPointer(int x, int y){
    Board::striker->pointer_direction += ((downX-x)*0.02);
    downX = x;
    downY = y;
}

int main(int argc, char **argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    int w = glutGet(GLUT_SCREEN_WIDTH);
    int h = glutGet(GLUT_SCREEN_HEIGHT);
    int windowWidth = w * 2 / 3;
    int windowHeight = h * 2 / 3;

    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);
    glutCreateWindow("Carom-OpenGL");
    initRendering();

    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutMouseFunc(handleMouseclick);
    glutMotionFunc(dragPointer);
    glutReshapeFunc(handleResize);
    glutTimerFunc(1000, decrement_score_every_second, 0);

    board.insert_coins();
    board.insert_striker();

    glutMainLoop();
    return 0;
}

void initRendering() {

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearColor(BACKGROUND_COLOR, 1.0f);
}

void handleResize(int w, int h) {

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void drawScene(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(0.f, 0.f, -8.0f);

    board.create();
    board.draw_coins();

    float x = 4.0f, y = 3.0f;
    glColor3f(SCORE_COLOR);
    renderBitmapString(x, y, GLUT_BITMAP_TIMES_ROMAN_24, "Score");
    x += 0.8f;

    if(Score == 0){
        board.draw_score('0', x, y);
    }

    else{    
        int tmp = Score;
        if(tmp < 0){
            board.draw_score('-', x, y);
            x += 0.1f;
            tmp = abs(tmp);
        }
        x += 0.4;
        while(tmp!=0){
            board.draw_score(tmp%10 + 48, x, y);
            x -= 0.1f;
            tmp /= 10;
        }
    }

    if(STATE == 0){
        Board::striker->apply_pointer();
        Bar::set_magnitude();
        Bar::draw();
    }

    if(STATE == 1){
        Collision::check_inter_collision();
        Collision::check_wall_collision();
        Collision::check_pot();
    
        float flag = 0;
        for(unsigned i=0; i<Coin::instances.size(); i++){
            float tmpx = Coin::instances[i]->vx;
            float tmpy = Coin::instances[i]->vy;
            if(abs(tmpx)+ abs(tmpy) != 0){
                flag = 1;
            }
        }
        if(flag==0){
            STATE = 0;
            Board::striker->reset_striker();
        }
    }

    glPopMatrix();
    glutSwapBuffers();

}

void handleMouseclick(int button, int state, int x, int y){
    if (state == GLUT_DOWN)
    {
        downX = x;
        downY = y;
        if (button == GLUT_LEFT_BUTTON)
            Board::striker->pointer_direction += 0.05f;
        else if (button == GLUT_RIGHT_BUTTON)
            Board::striker->pointer_direction -= 0.05f;

    }
}

void handleKeypress1(unsigned char key, int x, int y) {
    if(key == 32){
        Board::striker->set_velocity((Board::striker->magnitude)*cos(Board::striker->pointer_direction), (Board::striker->magnitude)*sin(Board::striker->pointer_direction));
        STATE = 1;
    }

    if(key == 'a'){
        Board::striker->shift_left();
    }
    if(key == 'd'){
        Board::striker->shift_right();
    }
    if(key == 'q'){
        exit(0);
    }
    if(key == 'o'){
        Score = 30;
        board.delete_coins_striker();
        board.insert_coins();
        board.insert_striker();
    }
}

void handleKeypress2(int key, int x, int y) {

    if (key == GLUT_KEY_LEFT)
        Board::striker->pointer_direction += 0.01f;
    if (key == GLUT_KEY_RIGHT)
        Board::striker->pointer_direction -= 0.01f;
    if (key == GLUT_KEY_F1)
        MODE = 0;
    if (key == GLUT_KEY_F2)
        MODE = 1;
    if (key == GLUT_KEY_F3)
        MODE = 2;

}
