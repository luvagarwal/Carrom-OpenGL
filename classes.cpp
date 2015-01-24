#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)

class Shape{
public:
    static void create_circle(float radius, bool fill){
        if(fill == true)
            glBegin(GL_TRIANGLE_FAN);
        else
            glBegin(GL_LINE_STRIP);

        for(int i=0 ; i<360 ; i++) {
           glVertex2f(radius * cos(DEG2RAD(i)), radius * sin(DEG2RAD(i)));
        }
        glEnd();
    }
    static void create_line(float x1, float y1, float x2, float y2){
        glBegin(GL_LINE_STRIP);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    static void create_square(float length){
        glBegin(GL_QUADS);
        glVertex2f(-length/2, -length/2);
        glVertex2f(length/2, -length/2);
        glVertex2f(length/2, length/2);
        glVertex2f(-length/2, length/2);
        glEnd();
    }

};
