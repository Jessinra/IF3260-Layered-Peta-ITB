#include <assert.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <utility>
#include "Master.hpp"
#include "MoveableObject.hpp"
#include "Object.hpp"

using namespace std;

#define WINDOWHEIGHT 700
#define WINDOWWIDTH 1000
#define maxDilate 10
#define minDilate 0.1
#define constFactor 1.1

int zoom = 0;
int moveHor = 0;
int moveVer = 0;
bool keepreading;

void *readinput(void *thread id) {
    char c;
    while (keepreading) {
        c = getchar();
        /* TODO */
        if(c == 'w' || c == 'W'){
            --moveVer;
        }
        else if(c == 'a' || c == 'A'){
            --moveHor;
        }
        else if(c == 's' || c == 'S'){
            ++moveVer;
        }
        else if(c == 'd' || c == 'D'){
            ++moveHor;
        }
        else if(c == 'j' || c == 'J'){
            --zoom;
        }
        else if(c == 'k' || c == 'K'){
            ++zoom;
        }
        usleep(10000);
    }
    pthread_exit(nullptr);
}

class Runner : public Master {
protected:
    Object peta;

public:
    Runner(int h = WINDOWHEIGHT, int w = WINDOWWIDTH) : Master(h, w) {
        peta = Object(0, 0, "");
    }

    void start() {
        // Make map
        const float normal = 1 / max(1.0f * peta.getWidth() / xend, 1.0f * peta.getHeight() / yend);
        float ratio = 1;


        while(true){
            // Creating MAP
            MoveableObject map = peta;
            map.selfDilate(0, 0, normal * ratio);

            // Drawing
            clearWindow();
            drawObject(map);
            drawSolidObject(map);
            flush();

            // Update
            // either it's zooming or moving, not both
            if(zoom != 0){
                /* TODO
                 * When zoom out, fix position
                 */
                if(zoom > 0){
                    if(ratio < maxDilate){
                        ratio *= constFactor;
                        --zoom;
                    }
                    else{
                        zoom = 0;
                    }
                }
                else{
                    if(ratio > minDilate){
                        ratio /= constFactor;
                        ++zoom;
                    }
                    else{
                        zoom = 0;
                    }
                }
            }
            else{
                /* TODO
                 * Make it smart when size is small
                 */
                if(moveHor != 0){
                    if(moveHor > 0){
                        if((int)map.getRefPos().getX() < 0){
                            map.setPos(map.getRefPos().getX() + 1, map.getRefPos().getY());
                            --moveHor;
                        }
                        else{
                            moveHor = 0;
                        }
                    }
                    else{
                        if((int)map.getRefPos().getX() + map.getWidth() >= xend){
                            map.setPos(map.getRefPos().getX() - 1, map.getRefPos().getY());
                            ++moveHor;
                        }
                        else{
                            moveHor = 0;
                        }
                    }
                }
                if(moveVer != 0) {
                    if(moveVer > 0){
                        if((int)map.getRefPos().getY() < 0){
                            map.setPos(map.getRefPos().getX(), map.getRefPos().getY() + 1);
                            --moveVer;
                        }
                        else{
                            moveVer = 0;
                        }
                    }
                    else{
                        if((int)map.getRefPos().getY() + map.getHeight() >= yend){
                            map.setPos(map.getRefPos().getX(), map.getRefPos().getY() - 1);
                            ++moveVer;
                        }
                        else{
                            moveVer = 0;
                        }
                    }
                }
            }
        }
    }
};

int main() {
    srand(time(NULL));

    /* non-newline input */
    struct termios org_opts, new_opts;
    int res = 0;
    res = tcgetattr(STDIN_FILENO, &org_opts);
    assert(res == 0);
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &=
            ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);

    /* Multithreading part */
    pthread_t thread;
    int rc, id = 0;
    keepreading = true;
    rc = pthread_create(&thread, NULL, readinput, (void *) id);

    Runner run;
    run.start();
    keepreading = false;

    /* close */
    pthread_exit(NULL);
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    return 0;
}