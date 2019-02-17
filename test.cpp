#include <iostream>
#include "Master.hpp"
#include "MoveableObject.hpp"
#include "Object.hpp"

using namespace std;

class Runner : public Master {
protected:
    Object plane, part1, part2, part3;
public:
    Runner(int h = 700, int w = 1000) : Master(h, w) {
        plane = Object(0, 0, "Asset/object_plane3d_right.txt");
        part1 = Object(100, 200 + 25, "Asset/object_plane3d_right_part_front.txt");
        part2 = Object(50, 200 + 75, "Asset/object_plane3d_right_part_wing.txt");
        part3 = Object(0, 200, "Asset/object_plane3d_right_part_back.txt");
    }

    void start() {
        clearWindow();
        drawObject(plane);
        drawSolidObject(plane);
        drawObject(part2);
        drawSolidObject(part2);
        drawObject(part1);
        drawSolidObject(part1);
        drawObject(part3);
        drawSolidObject(part3);
        flush();
    }
};

int main() {
    Runner run;
    run.start();
    return 0;
}