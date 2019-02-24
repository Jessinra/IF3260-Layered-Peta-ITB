#include <bits/stdc++.h>
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
map<string, bool> display;
map<string, int> moveHors;
map<string, int> moveVers;
map<string, float> dxs;
map<string, float> dys;
vector<string> objectIds;

void *readinput(void *thread_id) {
    char c;
    while (keepreading) {
        c = getchar();
        if(c == 'w' || c == 'W'){
            ++moveVer;
            for (int i = 0; i < objectIds.size(); i++) {
                ++moveVers[objectIds[i]];
            }
        }
        else if(c == 'a' || c == 'A'){
            ++moveHor;
            for (int i = 0; i < objectIds.size(); i++) {
                ++moveHors[objectIds[i]];
            }
        }
        else if(c == 's' || c == 'S'){
            --moveVer;
            for (int i = 0; i < objectIds.size(); i++) {
                --moveVers[objectIds[i]];
            }
        }
        else if(c == 'd' || c == 'D'){
            --moveHor;
            for (int i = 0; i < objectIds.size(); i++) {
                --moveHors[objectIds[i]];
            }
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
        } else if (c == '1') {
            display[objectIds[0]] = !display[objectIds[0]];
        } else if (c == '2') {
            display[objectIds[1]] = !display[objectIds[1]];
        } else if (c == '3') {
            display[objectIds[2]] = !display[objectIds[2]];
        }

        usleep(10000);
    }
    pthread_exit(nullptr);
}

void checkShift(MoveableObject *mObject, int xend, int yend) {
    if (moveHors[mObject->getId()] != 0) {
        if (moveHors[mObject->getId()] > 0) {
            if ((int) dxs[mObject->getId()] < 0) {
                dxs[mObject->getId()] += speedx;
                --moveHors[mObject->getId()];
                mObject->setPos(dxs[mObject->getId()], dys[mObject->getId()]);
            } else {
                moveHors[mObject->getId()] = 0;
            }
        } else {
            if ((int) dxs[mObject->getId()] + mObject->getWidth() >= xend) {
                dxs[mObject->getId()] -= speedx;
                ++moveHors[mObject->getId()];
                mObject->setPos(dxs[mObject->getId()], dys[mObject->getId()]);
            } else {
                moveHors[mObject->getId()] = 0;
            }
        }
    }

    if (moveVers[mObject->getId()] != 0) {
        if (moveVers[mObject->getId()] > 0) {
            if ((int)dys[mObject->getId()] < 0) {
                dys[mObject->getId()] += speedy;
                --moveVers[mObject->getId()];
                mObject->setPos(dxs[mObject->getId()], dys[mObject->getId()]);
            } else {
                moveVers[mObject->getId()] = 0;
            }
        } else {
            if ((int)dys[mObject->getId()] + mObject->getHeight() >= yend) {
                dys[mObject->getId()] -= speedy;
                ++moveVers[mObject->getId()];
                mObject->setPos(dxs[mObject->getId()], dys[mObject->getId()]);
            } else {
                moveVers[mObject->getId()] = 0;
            }
        }
    }
}

class Runner : public Master {
protected:
    Object peta;
    Object building, road;

public:
    Runner(int h = WINDOWHEIGHT, int w = WINDOWWIDTH) : Master(h, w) {
        peta = Object(0, 0, "Asset/object_map.text");
        
        building = Object(0, 0, "Asset/object_building.txt");
        road = Object(0, 0, "Asset/object_road.txt");

        display[building.getId()] = true;
        objectIds.push_back(building.getId());
        display[road.getId()] = true;
        objectIds.push_back(road.getId());
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
        // MoveableObject map = peta;
        // map.selfDilate(0, 0, normal*ratio);

        MoveableObject mBuilding = building;
        mBuilding.selfDilate(0, 0, normal*ratio);
        MoveableObject mRoad = road;
        mRoad.selfDilate(0, 0, normal*ratio);

        dxs[mBuilding.getId()] = mBuilding.getPos().getX();
        dys[mBuilding.getId()] = mBuilding.getPos().getY();
        dxs[mRoad.getId()] = mRoad.getPos().getX();        
        dys[mRoad.getId()] = mRoad.getPos().getY();

        while(true){
            // Drawing
            clearWindow();
//            drawObject(view1, map);
            // drawSolidObject(view1, map);
            drawObject(smallmap);
            drawSolidObject(smallmap);

            if (display[mBuilding.getId()]) {
                drawObject(view1, mBuilding);
                drawSolidObject(view1, mBuilding);
            }

            if (display[mRoad.getId()]) {
                drawObject(view1, mRoad);
                drawSolidObject(view1, mRoad);
            }

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
                // map = peta;
                // map.selfDilate(0, 0, normal*ratio);
                // map.setPos(dx, dy);

                mBuilding = building;
                mBuilding.selfDilate(0, 0, normal*ratio);
                dxs[mBuilding.getId()] = mBuilding.getPos().getX();
                dys[mBuilding.getId()] = mBuilding.getPos().getY();
                mBuilding.setPos(dxs[mBuilding.getId()], dys[mBuilding.getId()]);

                mRoad = road;
                mRoad.selfDilate(0, 0, normal*ratio);
                dxs[mRoad.getId()] = mRoad.getPos().getX();        
                dys[mRoad.getId()] = mRoad.getPos().getY();
                mRoad.setPos(dxs[mRoad.getId()], dys[mRoad.getId()]);                
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
                            // map.setPos(dx, dy);
                        }
                        else{
                            moveHor = 0;
                        }
                    }
                    else{
                        cout << "TAI KUCING 2\n";
                        // if((int)dx + map.getWidth() >= xend){
                        //     dx -= speedx;
                        //     ++moveHor;
                        //     map.setPos(dx, dy);
                        // }
                        // else{
                        //     moveHor = 0;
                        // }
                    }
                }
                if(moveVer != 0) {
                    if(moveVer > 0){
                        if((int)dy < 0){
                            dy += speedy;
                            --moveVer;
                            // map.setPos(dx, dy);
                        }
                        else{
                            moveVer = 0;
                        }
                    }
                    else{
                        // if((int)dy + map.getHeight() >= yend){
                        //     dy -= speedy;
                        //     ++moveVer;
                        //     map.setPos(dx, dy);
                        // }
                        // else{
                        //     moveVer = 0;
                        // }
                    }
                }
                checkShift(&mBuilding, xend, yend);
                checkShift(&mRoad, xend, yend);
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