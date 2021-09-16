#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <math.h>

struct Point2D
{
    Point2D() {}
    Point2D(double x, double y) : x(x), y(y) {}
    double x, y;
};

struct BoundingBox
{
	double left, right, top, bottom;
};

struct Color
{
    double r, g, b;
};

class Triangle
{
public:
    Triangle();
    Triangle(Point2D p0, Point2D p1, Point2D p2, Color c);
    ~Triangle();


	BoundingBox get_bouding_box();
	double get_area();
	bool is_inside(Point2D p);
	bool is_split();

    Point2D vertices[3];
    Color color;


};

#endif /* TRIANGLE_H_ */
