#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "Triangle.h"

class Texture
{
public:
    Texture(int w, int h);
    ~Texture();

	int width, height;

	double *canvas_r, *canvas_g, *canvas_b;

	void render_triangles(const std::vector<Triangle> &triangles);
	void save(std::string filename);
	void clear();
private:
	void fill_empty_spots();


};

#endif /* TEXTURE_H_ */
