#ifndef PLANE_HPP
#define PLANE_HPP

#include <vector>

#include "Line.hpp"
using namespace std;

class Plane
{
  protected:
    Point position;
    vector<Line> lines;

    float xMin, yMin, xMax, yMax;

    unsigned int color;
    int priority;

  public:
    Plane(const vector<Line> &lines, int color, int priority);

    void reverseHorizontal();

    void setColor(unsigned int color);
    void setPriority(int priority);
    void calculate();

    unsigned int getColor() const;
    int getPriority() const;
    int getWidth() const;
    int getHeight() const;
    Point getLowerRight() const;
    Point getUpperLeft() const;

    void setPos(Point position);
    void setPos(float x, float y);

    Point getPos() const;
    const Point & getRefPos() const;

    vector<Line> getLines() const;
    vector<Line> & getRefLines();
    const vector<Line> &getConstRefLines() const;

    /* For MovableObject polymorphism purpose */
    virtual void selfRotate(float pivotX, float pivotY, float theta);
    virtual void selfDilate(float pivotX, float pivotY, float scalingConstant);
};

#endif