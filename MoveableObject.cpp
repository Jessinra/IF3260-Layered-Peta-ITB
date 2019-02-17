
#include "MoveableObject.hpp"

MoveableObject::MoveableObject(float x, float y, const std::string &filename) : Object(x, y, filename)
{
    this->dx = 0;
    this->dy = 0;
    this->speed = 1;
    calculate();
}

MoveableObject::MoveableObject(float x, float y, float dx, float dy, float speed, const std::string &filename) : Object(x, y, filename)
{
    float distance = sqrt(dx * dx + dy * dy);
    this->dx = 0;
    this->dy = 0;

    if (distance > 0)
    {
        this->dx = dx / distance;
        this->dy = dy / distance;
    }

    this->speed = speed;

    calculate();
}

MoveableObject::MoveableObject(const Object &obj) : Object(obj)
{
    this->dx = 0;
    this->dy = 0;
    this->speed = 1;
}

MoveableObject::MoveableObject(float dx, float dy, float speed, const Object &obj) : Object(obj)
{
    float distance = sqrt(dx * dx + dy * dy);
    this->dx = 0;
    this->dy = 0;

    if (distance > 0)
    {
        this->dx = dx / distance;
        this->dy = dy / distance;
    }

    this->speed = speed;
}

void MoveableObject::setVector(float dx, float dy)
{
    float distance = sqrt(dx * dx + dy * dy);
    if (distance > 0)
    {
        this->dx = dx / distance;
        this->dy = dy / distance;
    }
}

void MoveableObject::setSpeed(float speed)
{
    this->speed = speed;
}

float MoveableObject::getDx() const
{
    return this->dx;
}

float MoveableObject::getDy() const
{
    return this->dy;
}

float MoveableObject::getSpeed() const
{
    return this->speed;
}

void MoveableObject::move()
{
    this->position.setPoint(this->position.getX() + this->speed*this->dx, this->position.getY() + this->speed*this->dy);
}

void MoveableObject::selfRotate(float pivotX, float pivotY, float theta)
{
    pivotX -= this->position.getX();
    pivotY -= this->position.getY();
    for (Plane &plane : this->planes)
    {
        plane.selfRotate(pivotX, pivotY, theta);
    }

    calculate();
}

void MoveableObject::selfDilate(float pivotX, float pivotY, float scalingConstant)
{
    for (Plane &plane : this->planes)
    {
        plane.selfDilate(pivotX, pivotY, scalingConstant);
    }

    calculate();
}

void MoveableObject::calculate() {
    if(planes.empty()) return;

    xMin = planes[0].getUpperLeft().getX() + planes[0].getRefPos().getX();
    yMin = planes[0].getUpperLeft().getY() + planes[0].getRefPos().getY();
    xMax = planes[0].getLowerRight().getX() + planes[0].getRefPos().getX();
    yMax = planes[0].getLowerRight().getY() + planes[0].getRefPos().getY();

    for(int i=1;i<planes.size();++i){
        xMin = min(xMin, planes[i].getUpperLeft().getX() + planes[i].getRefPos().getX());
        yMin = min(yMin, planes[i].getUpperLeft().getY() + planes[i].getRefPos().getY());
        xMax = max(xMax, planes[i].getLowerRight().getX() + planes[i].getRefPos().getX());
        yMax = max(yMax, planes[i].getLowerRight().getY() + planes[i].getRefPos().getY());
    }

    xMax -= xMin;
    yMax -= yMin;

    for(MoveablePlane &plane : planes){
        plane.setPos(plane.getRefPos().getX() - xMin, plane.getRefPos().getY() - yMin);
    }

    position.setPoint(position.getX() + xMin, position.getY() + yMin);

    xMin = yMin = 0;
}
