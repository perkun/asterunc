#ifndef MESH_H_
#define MESH_H_

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <vector>
#include <math.h>

#include "Triangle.h"

enum ErrorType
{
	POSITIVE,
	NEGATIVE
};


struct MeshVertex
{
	double x, y, z, u ,v;
	double error_plus, error_minus;
};


struct Face
{
	int vertex_idx[3];
};


struct Range
{
	double min, max;
};


class Mesh
{
public:
    Mesh(std::string model_file, std::string errors_file);
    ~Mesh();

	std::vector<MeshVertex> vertices;
	std::vector<Face> faces;

	void calculate_texture_coords();
	void load_errors(std::string filename);
	std::vector<Triangle> get_triangles(ErrorType et);
	void save_obj(std::string filename);

private:
	Range get_plus_error_ranges();
	Range get_minus_error_ranges();
	Color get_face_color(ErrorType et, const Face& face, double range = 0.1);
};

#endif /* MESH_H_ */
