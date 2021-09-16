#include "Triangle.h"


Triangle::Triangle()
{
}


Triangle::~Triangle()
{
}


Triangle::Triangle(Point2D p0, Point2D p1, Point2D p2, Color c)
{
	vertices[0] = p0;
	vertices[1] = p1;
	vertices[2] = p2;
	color = c;
}


BoundingBox Triangle::get_bouding_box()
{
	BoundingBox bb;
    bb.left = std::min({vertices[0].x, vertices[1].x, vertices[2].x});
    bb.right = std::max({vertices[0].x, vertices[1].x, vertices[2].x});

    bb.bottom = std::min({vertices[0].y, vertices[1].y, vertices[2].y});
    bb.top = std::max({vertices[0].y, vertices[1].y, vertices[2].y});

	return bb;
}


double Triangle::get_area()
{
    return 0.5 * (-vertices[1].y * vertices[2].x +
                  vertices[0].y * (-vertices[1].x + vertices[2].x) +
                  vertices[0].x * (vertices[1].y - vertices[2].y) +
                  vertices[1].x * vertices[2].y);
}


bool Triangle::is_inside(Point2D p)
{

    double area = get_area();
    double s = 1 / (2 * area) *
               (vertices[0].y * vertices[2].x - vertices[0].x * vertices[2].y +
                (vertices[2].y - vertices[0].y) * p.x +
                (vertices[0].x - vertices[2].x) * p.y);
    double t = 1 / (2 * area) *
               (vertices[0].x * vertices[1].y - vertices[0].y * vertices[1].x +
                (vertices[0].y - vertices[1].y) * p.x +
                (vertices[1].x - vertices[0].x) * p.y);

    return (s >= 0 && t >= 0 && 1 - s - t >= 0);
}




bool Triangle::is_split()
{
    if (fabs(vertices[0].x - vertices[1].x) >= 0.5 ||
        fabs(vertices[0].x - vertices[2].x) >= 0.5)
		return true;
	return false;
}
