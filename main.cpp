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

#define MAXHEALTH 8
#define explosiontime 20
#define selectedtime 300
#define degreechange 5
#define maxdegree 50
#define mindegree -50
#define WINDOWHEIGHT 700
#define WINDOWWIDTH 1000

/* Aku sedih */
bool keepreading;

void *readinput(void *threadid) {
    char c;
    while (keepreading) {
        c = getchar();
        /* TODO */
        usleep(10000);
    }
    pthread_exit(NULL);
}

/* Aku sedih */

class Runner : public Master {
protected:
    Object pesawat, meriam, peluru, puing1, puing2, puing3;
    Object revpesawat, revpuing1, revpuing2, revpuing3;
    Object ledakan, misil, hati, roda;

public:
    Runner(int h = WINDOWHEIGHT, int w = WINDOWWIDTH) : Master(h, w) {
        revpesawat = Object(0, 0, "Asset/object_plane3d_right.txt");
        pesawat = Object(0, 0, "Asset/object_plane3d_left.txt");
        meriam = Object(0, 0, "Asset/object_gun.txt");
        peluru = Object(0, 0, "Asset/object_star.txt");
        puing1 = Object(0, 0, "Asset/object_plane3d_part_front.txt");
        puing2 = Object(0, 0, "Asset/object_plane3d_part_wing.txt");
        puing3 = Object(0, 0, "Asset/object_plane3d_part_back.txt");
        revpuing1 = Object(0, 0, "Asset/object_plane3d_right_part_front.txt");
        // revpuing1.reverseHorizontal();
        revpuing2 = Object(0, 0, "Asset/object_plane3d_right_part_wing.txt");
        // revpuing2.reverseHorizontal();
        revpuing3 = Object(0, 0, "Asset/object_plane3d_right_part_back.txt");
        // revpuing3.reverseHorizontal();
        ledakan = Object(0, 0, "Asset/object_ledakan.txt");
        misil = Object(0, 0, "Asset/object_misil.txt");
        hati = Object(0, 0, "Asset/object_life.txt");
        roda = Object(0, 0, "Asset/object_wheel.txt");
    }

    void start() {
        int sudut_meriam = 0;
        float titik_acuan_x, titik_acuan_y;
        titik_acuan_x = xend / 2.0f;
        titik_acuan_y = yend - 2;

        pesawat.setPos(Point(xend, 0));
        revpesawat.setPos(Point(-revpesawat.getWidth() + 1, 0));
        meriam.setPos(Point((xend - meriam.getWidth()) / 2.0f,
                            yend - meriam.getHeight() - 2));
        peluru.setPos(
                Point((xend - peluru.getWidth()) / 2.0f,
                      yend - meriam.getHeight() - peluru.getHeight() - 2));
        vector<MoveableObject> planes, rplanes;
        vector<MoveableObject> debris;
        vector<MoveableObject> bullets;
        vector<pair<MoveableObject, int>> explosion;
        vector<MoveableObject> missile;
        MoveableObject cannon = meriam;
        planes.emplace_back(-3, 1, 1, pesawat);
        vector<MoveableObject> lifes;
        MoveableObject life = hati;
        life.selfDilate(0, 0, 0.4);
        MoveableObject wheel = roda;
        wheel.selfDilate(0, 0, 3);
        vector<pair<MoveableObject, float> > wheelup, wheeldown;
        vector<MoveableObject> wheelconst;
        for(int i=0;i<MAXHEALTH;++i){
            life.setPos(i * (life.getWidth() + 5) + 5, yend - life.getHeight() - 5);
            lifes.push_back(life);
        }
        for (int i = 1;; i = (i + 1) % 800) {
            // draw
            clearWindow();

            drawObject(cannon);
            drawSolidObject(cannon);
            for (MoveableObject &movableObject : lifes) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : planes) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : rplanes) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : debris) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (MoveableObject &movableObject : bullets) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (pair<MoveableObject, int> &movableObject : explosion) {
                float middleX_explosion = movableObject.first.getWidth() / 2.0f;
                float middleY_explosion = movableObject.first.getHeight() / 2.0f;
                movableObject.first.selfDilate(middleX_explosion, middleY_explosion, 1.05);
                drawObject(movableObject.first);
                drawSolidObject(movableObject.first);
                movableObject.second--;
            }
            for (MoveableObject &movableObject : missile) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }
            for (pair<MoveableObject, float> &movableObject : wheeldown) {
                drawObject(movableObject.first);
                drawSolidObject(movableObject.first);
            }
            for (pair<MoveableObject, float> &movableObject : wheelup) {
                drawObject(movableObject.first);
                drawSolidObject(movableObject.first);
            }
            for (MoveableObject &movableObject : wheelconst) {
                drawObject(movableObject);
                drawSolidObject(movableObject);
            }

            // move and rotate :/
            if (deg != 0) {
                if (deg > 0) {
                    if (sudut_meriam <= maxdegree) {
                        sudut_meriam += degreechange;
                        cannon = MoveableObject(meriam);
                        cannon.selfRotate(titik_acuan_x, titik_acuan_y,
                                          sudut_meriam);
                        deg -= degreechange;
                    } else {
                        deg = 0;
                    }
                } else {
                    if (sudut_meriam >= mindegree) {
                        sudut_meriam -= degreechange;
                        cannon = MoveableObject(meriam);
                        cannon.selfRotate(titik_acuan_x, titik_acuan_y,
                                          sudut_meriam);
                        deg += degreechange;
                    } else {
                        deg = 0;
                    }
                }
            }
            explosion.erase(remove_if(explosion.begin(), explosion.end(), [](const pair<MoveableObject, int> &a) {
                                return a.second <= 0;
                            }),
                            explosion.end());


            vector<char> checkr(rplanes.size(), 1);
            vector<char> checkp(planes.size(), 1);
            vector<char> checkd(debris.size(), 1);
            vector<char> checkm(missile.size(), 1);
            vector<char> checkwd(wheeldown.size(), 1);
            vector<char> checkwu(wheelup.size(), 1);
            vector<char> checkwc(wheelconst.size(), 1);
            vector<MoveableObject> tmpr; // rplane
            vector<MoveableObject> tmpp; // plane
            vector<MoveableObject> tmpb; // bullet
            vector<MoveableObject> tmpd; // debris
            vector<MoveableObject> tmpm; // missile
            vector<pair<MoveableObject, float> > tmpwd; // wheel down
            vector<pair<MoveableObject, float> > tmpwu; // wheel up
            vector<MoveableObject> tmpwc; // wheel constant
            for (int j=0;j<planes.size();++j) {
                planes[j].move();
                if(planes[j].outOfWindow(WINDOWHEIGHT, WINDOWWIDTH)){
                    checkp[j] = 0;
                }
            }
            for (int j=0;j<rplanes.size();++j) {
                rplanes[j].move();
                if(rplanes[j].outOfWindow(WINDOWHEIGHT, WINDOWWIDTH)){
                    checkr[j] = 0;
                }
            }
            for (int j = 0; j < debris.size(); ++j) {
                debris[j].move();
                if (debris[j].outOfWindow(yend, xend)) {
                    checkd[j] = 0;
                }
            }
            for (MoveableObject &bullet : bullets) {
                bullet.move();
            }
            for (int j = 0; j < missile.size(); ++j) {
                missile[j].move();
                if (missile[j].outOfWindow(yend, xend)) {
                    checkm[j] = 0;
                    if (missile[j].getRefPos().getY() >= yend) {
                        if(!lifes.empty()){
                            lifes.pop_back();
                        }
                    }
                }
            }
            for (int j = 0; j < wheeldown.size(); ++j) {
                wheeldown[j].first.move();
                if (wheeldown[j].first.outOfWindow(yend, xend)) {
                    checkwd[j] = 0;
                }
                if(wheeldown[j].first.getRefPos().getY() + wheeldown[j].first.getHeight() >= yend){
                    checkwd[j] = 0;
                    float maxheight = (yend - wheeldown[j].second - wheeldown[j].first.getHeight())*0.6f;
                    if(maxheight < 1){
                        wheeldown[j].first.setVector(wheeldown[j].first.getDx(), 0);
                        tmpwc.push_back(wheeldown[j].first);
                    }
                    else{
                        wheeldown[j].first.setVector(wheeldown[j].first.getDx(), -wheeldown[j].first.getDy());
                        tmpwu.push_back({wheeldown[j].first, yend - maxheight - wheeldown[j].first.getHeight()});
                    }
                }
            }
            for (int j = 0; j < wheelup.size(); ++j) {
                wheelup[j].first.move();
                if (wheelup[j].first.outOfWindow(yend, xend)) {
                    checkwu[j] = 0;
                }
                if(wheelup[j].first.getRefPos().getY() <= wheelup[j].second){
                    checkwu[j] = 0;
                    wheelup[j].first.setVector(wheelup[j].first.getDx(), -wheelup[j].first.getDy());
                    tmpwd.push_back(wheelup[j]);
                }
            }
            for (int j = 0; j < wheelconst.size(); ++j) {
                wheelconst[j].move();
                if (wheelconst[j].outOfWindow(yend, xend)) {
                    checkwc[j] = 0;
                }
            }

            // very slow shit
            for (const MoveableObject &objb : bullets) {
                bool bisa = true;
                for (int j = 0; j < planes.size(); ++j) {
                    if (overlap(planes[j], objb)) {
                        // isi pecahan
                        MoveableObject sp = ledakan;
                        sp.setPos(planes[j].getRefPos().getX() + 100,
                                planes[j].getRefPos().getY());
                        explosion.emplace_back(sp, explosiontime);
                        sp = puing2;
                        sp.setPos(planes[j].getRefPos().getX() + 50,
                                planes[j].getRefPos().getY() + 75);
                        sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                                     sin(45 * PI / 180),
                                     cos(45 * PI / 180));
                        sp.setSpeed(2);
                        tmpd.push_back(sp);
                        sp = puing1;
                        sp.setPos(planes[j].getRefPos().getX(),
                                planes[j].getRefPos().getY() + 25);
                        sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                                     sin(60 * PI / 180),
                                     cos(60 * PI / 180));
                        sp.setSpeed(2);
                        tmpd.push_back(sp);
                        sp = puing3;
                        sp.setPos(planes[j].getRefPos().getX() + 50,
                                planes[j].getRefPos().getY());
                        sp.setVector((planes[j].getDx() < 0 ? -1 : 1) *
                                     sin(30 * PI / 180),
                                     cos(30 * PI / 180));
                        sp.setSpeed(2);
                        tmpd.push_back(sp);
                        sp = wheel;
                        sp.setPos(xend - (planes[j].getRefPos().getX() - planes[j].getWidth())/2.0f,
                                planes[j].getRefPos().getY() + planes[j].getHeight() - wheel.getHeight()/2.0f);
                        sp.setVector(-1, 2);
                        sp.setSpeed(2);
                        tmpwd.push_back({sp, sp.getRefPos().getY()});
                        checkp[j] = 0;
                        bisa = false;
                    }
                }
                for (int j = 0; j < rplanes.size(); ++j) {
                    if (overlap(rplanes[j], objb)) {
                        // isi pecahan
                        MoveableObject sp = ledakan;
                        sp.setPos(rplanes[j].getRefPos().getX() + 100,
                                rplanes[j].getRefPos().getY());
                        explosion.emplace_back(sp, explosiontime);
                        sp = revpuing2;
                        sp.setPos(rplanes[j].getRefPos().getX() + 50,
                                  rplanes[j].getRefPos().getY() + 75);
                        sp.setVector((rplanes[j].getDx() < 0 ? -1 : 1) *
                                     sin(45 * PI / 180),
                                     cos(45 * PI / 180));
                        sp.setSpeed(2);
                        tmpd.push_back(sp);
                        sp = revpuing1;
                        sp.setPos(rplanes[j].getRefPos().getX() + 100,
                                rplanes[j].getRefPos().getY() + 25);
                        sp.setVector((rplanes[j].getDx() < 0 ? -1 : 1) *
                                     sin(60 * PI / 180),
                                     cos(60 * PI / 180));
                        sp.setSpeed(2);
                        tmpd.push_back(sp);
                        sp = revpuing3;
                        sp.setPos(rplanes[j].getRefPos().getX(),
                                rplanes[j].getRefPos().getY());
                        sp.setVector((rplanes[j].getDx() < 0 ? -1 : 1) *
                                     sin(30 * PI / 180),
                                     cos(30 * PI / 180));
                        sp.setSpeed(2);
                        tmpd.push_back(sp);
                        sp = wheel;
                        sp.setPos(xend - (planes[j].getRefPos().getX() - planes[j].getWidth())/2.0f,
                                  planes[j].getRefPos().getY() + planes[j].getHeight()  + wheel.getHeight()/2.0f);
                        sp.setVector(1, 2);
                        sp.setSpeed(2);
                        tmpwd.push_back({sp, sp.getRefPos().getY()});
                        checkr[j] = 0;
                        bisa = false;
                    }
                }
                for (int j = 0; j < debris.size(); ++j) {
                    if (overlap(debris[j], objb)) {
                        checkd[j] = 0;
                        bisa = false;
                    }
                }
                for (int j = 0; j < missile.size(); ++j) {
                    if (overlap(missile[j], objb)) {
                        checkm[j] = 0;
                        bisa = false;
                    }
                }
                if (bisa && !objb.outOfWindow(yend, xend)) {
                    tmpb.push_back(objb);
                }
            }
            for (int j = 0; j < planes.size(); ++j) {
                if (checkp[j])
                    tmpp.push_back(planes[j]);
            }
            for (int j = 0; j < rplanes.size(); ++j) {
                if (checkr[j])
                    tmpr.push_back(rplanes[j]);
            }
            for (int j = 0; j < debris.size(); ++j) {
                if (checkd[j])
                    tmpd.push_back(debris[j]);
            }
            for (int j = 0; j < missile.size(); ++j) {
                if (checkm[j]) {
                    tmpm.push_back(missile[j]);
                }
            }
            for (int j = 0; j < wheeldown.size(); ++j) {
                if (checkwd[j]) {
                    tmpwd.push_back(wheeldown[j]);
                }
            }
            for (int j = 0; j < wheelup.size(); ++j) {
                if (checkwu[j]) {
                    tmpwu.push_back(wheelup[j]);
                }
            }
            for (int j = 0; j < wheelconst.size(); ++j) {
                if (checkwc[j]) {
                    tmpwc.push_back(wheelconst[j]);
                }
            }
            rplanes = tmpr;
            planes = tmpp;
            bullets = tmpb;
            debris = tmpd;
            missile = tmpm;
            wheeldown = tmpwd;
            wheelup = tmpwu;
            wheelconst = tmpwc;

            /* Spawn Section */
            if (shoot > 0) {
                MoveableObject tmp = MoveableObject(peluru);
                tmp.setSpeed(2);
                tmp.selfRotate(titik_acuan_x, titik_acuan_y, sudut_meriam);
                tmp.setVector(sin(sudut_meriam * PI / 180),
                              -cos(sudut_meriam * PI / 180));
                bullets.push_back(tmp);
                shoot--;
            }
            if (i == 0) {
                planes.push_back(MoveableObject(-3, 1, 1, pesawat));
            }
            else if(i == 400){
                rplanes.push_back(MoveableObject(3, 1, 1, revpesawat));
            }
            for (const MoveableObject &obj : planes) {
                if (rand() % selectedtime == 0) {
                    MoveableObject tmp = MoveableObject(0, 1, 1, misil);
                    tmp.setPos(obj.getRefPos().getX() + (obj.getWidth() - tmp.getWidth()) / 2.0f, obj.getRefPos().getY() + 80);
                    missile.push_back(tmp);
                }
            }
            for (const MoveableObject &obj : rplanes) {
                if (rand() % selectedtime == 0) {
                    MoveableObject tmp = MoveableObject(0, 1, 1, misil);
                    tmp.setPos(obj.getRefPos().getX() + (obj.getWidth() - tmp.getWidth()) / 2.0f, obj.getRefPos().getY() + 80);
                    missile.push_back(tmp);
                }
            }

            if (lifes.empty()) {
                break;
            }

            flush();
            usleep(5000);
        }
        puts("GAME OVER!! YOU LOSE");
    }

    bool overlap(const Object &p, const Object &q) {
        int a, b, c, d, e, f, g, h;
        a = p.getRefPos().getX();
        b = p.getRefPos().getY();
        c = a + p.getLowerRight().getX();
        d = b + p.getLowerRight().getY();
        e = q.getRefPos().getX();
        f = q.getRefPos().getY();
        g = e + q.getLowerRight().getX();
        h = f + q.getLowerRight().getY();
        if (a > g || e > c)
            return false;
        if (b > h || f > d)
            return false;
        return true;
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