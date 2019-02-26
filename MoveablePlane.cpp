#include <iostream>
#include "MoveablePlane.hpp"

MoveablePlane::MoveablePlane(float x, float y, const vector<Line> &lines, int color, int priority) : Plane(lines, color, priority)
{
    this->position = Point(x, y);
}

MoveablePlane::MoveablePlane(float x, float y, const Plane &plane) : Plane(plane)
{
    this->position = Point(x, y);
}

void MoveablePlane::selfRotate(float pivotX, float pivotY, float theta)
{
    pivotX -= position.getX();
    pivotY -= position.getY();

    for (Line &line : this->lines)
    {
        line.setStartPixel(line.getStartPixel().rotation(pivotX, pivotY, theta));
        line.setEndPixel(line.getEndPixel().rotation(pivotX, pivotY, theta));
    }

    calculate();
}

void MoveablePlane::selfDilate(float pivotX, float pivotY, float scalingConstant)
{
    pivotX -= position.getX();
    pivotY -= position.getY();

    for (Line &line : this->lines){
        line.setStartPixel(line.getStartPixel().dilated(pivotX, pivotY, scalingConstant));
        line.setEndPixel(line.getEndPixel().dilated(pivotX, pivotY, scalingConstant));
    }

    calculate();
}

