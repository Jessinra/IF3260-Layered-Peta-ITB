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
int speedx = 1;
int speedy = 1;
bool keepreading;

void *readinput(void *thread_id) {
    char c;
    while (keepreading) {
        c = getchar();
        if(c == 'w' || c == 'W'){
            ++moveVer;
        }
        else if(c == 'a' || c == 'A'){
            ++moveHor;
        }
        else if(c == 's' || c == 'S'){
            --moveVer;
        }
        else if(c == 'd' || c == 'D'){
            --moveHor;
        }
        else if(c == 'j' || c == 'J'){
            --zoom;
        }
        else if(c == 'k' || c == 'K'){
            ++zoom;
        }
        else if(c == 'u' || c == 'U'){
            if(speedx>1 && speedy>1){
                --speedx;
                --speedy;
            }
        }
        else if(c == 'i' || c == 'I'){
            if(speedx<100 && speedy<100){
                ++speedx;
                ++speedy;
            }
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
        peta = Object(0, 0, "Asset/object_map.text");
    }

    void start() {
        // Make map
        const float normal = 1 / max(1.0f * peta.getWidth() / xend, 1.0f * peta.getHeight() / yend);
        float ratio = 1;
        float dx = 0;
        float dy = 0;
        Rectangle view1(0, 0, 700, WINDOWHEIGHT);
        Rectangle view2(710, 0, 1000, WINDOWHEIGHT);

        MoveableObject smallmap = peta;
        smallmap.setPos(720, 0);
        smallmap.selfDilate(720, 0, 1.4);
        MoveableObject map = peta;
        map.selfDilate(0, 0, normal*ratio);
        while(true){
            // Drawing
            clearWindow();
            drawObject(view1, map);
            drawSolidObject(view1, map);
            drawObject(smallmap);
            drawSolidObject(smallmap);
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
                map = peta;
                map.selfDilate(0, 0, normal*ratio);
                map.setPos(dx, dy);
            }
            else{
                /* TODO
                 * Make it smart when size is small
                 */
                if(moveHor != 0){
                    if(moveHor > 0){
                        if((int) dx < 0){
                            dx += speedx;
                            --moveHor;
                            map.setPos(dx, dy);
                        }
                        else{
                            moveHor = 0;
                        }
                    }
                    else{
                        if((int)dx + map.getWidth() >= xend){
                            dx -= speedx;
                            ++moveHor;
                            map.setPos(dx, dy);
                        }
                        else{
                            moveHor = 0;
                        }
                    }
                }
                if(moveVer != 0) {
                    if(moveVer > 0){
                        if((int)dy < 0){
                            dy += speedy;
                            --moveVer;
                            map.setPos(dx, dy);
                        }
                        else{
                            moveVer = 0;
                        }
                    }
                    else{
                        if((int)dy + map.getHeight() >= yend){
                            dy -= speedy;
                            ++moveVer;
                            map.setPos(dx, dy);
                        }
                        else{
                            moveVer = 0;
                        }
                    }
                }
            }
            usleep(6000);
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
    pthread_join(thread, nullptr);
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    assert(res == 0);
    return 0;
}