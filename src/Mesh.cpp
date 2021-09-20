#include "Mesh.h"
#include <cstdio>
#include <string>

using namespace std;


Mesh::Mesh(string model_file, string errors_file)
{
    std::ifstream shape_model(model_file);

    if (!shape_model.is_open())
    {
        cout << "Error opening model file" << endl;
        exit(1);
    }

    string line;
    // read num of vertices and faces
    getline(shape_model, line);
    int num_faces, num_vertices;
    sscanf(line.c_str(), "%d %d", &num_vertices, &num_faces);

    vertices.reserve(num_vertices);
    faces.reserve(num_faces);

    for (int i = 0; i < num_vertices; i++)
    {
        getline(shape_model, line);
        vertices.emplace_back(MeshVertex());
        sscanf(line.c_str(), "%lf %lf %lf", &vertices.back().x,
               &vertices.back().y, &vertices.back().z);
    }

    for (int i = 0; i < num_faces; i++)
    {
        getline(shape_model, line);
		faces.emplace_back(Face());
        sscanf(line.c_str(), "%d %d %d", &faces.back().vertex_idx[0],
               &faces.back().vertex_idx[1], &faces.back().vertex_idx[2]);
    }

	// to 0-base indexing
	for (Face &face: faces)
		for (int i = 0; i < 3; i++)
			face.vertex_idx[i] -= 1;

	shape_model.close();

	calculate_texture_coords();
	load_errors(errors_file);
}


Mesh::~Mesh()
{
}


void Mesh::calculate_texture_coords()
{
    for (MeshVertex &vertex : vertices)
    {
        double longitude = atan2(vertex.y, vertex.x);

        double lenght = sqrt(vertex.x * vertex.x +
                             vertex.y * vertex.y +
                             vertex.z * vertex.z);
        double latitude = asin(vertex.z / lenght);

		// u,v coordinates [0, 1) range; (0,0) at left-bottom
		vertex.u = (longitude / (2.* M_PI));
// 		vertex.u += 0.5;
		if (vertex.u < 0)
			vertex.u += 1;
		vertex.v = (latitude / M_PI) + 0.5;
    }
}


void Mesh::load_errors(string filename)
{
	ifstream errors(filename);

    if (!errors.is_open())
    {
        cout << "Error opening uncertainty file" << endl;
        exit(1);
    }

	string line;
	while (errors)
	{
		getline(errors, line);
		int vid;
		double ep, em;
		sscanf(line.c_str(), "%d %lf %lf", &vid, &em, &ep);

		vertices[vid - 1].error_minus = em;
		vertices[vid - 1].error_plus = ep;
	}
}


Range Mesh::get_error_ranges(ErrorType et)
{
    double max_error, min_error;

    if (et == ErrorType::POSITIVE)
    {
        max_error = vertices[0].error_plus;
        min_error = vertices[0].error_plus;

        for (MeshVertex &v : vertices)
        {
            if (v.error_plus > max_error)
                max_error = v.error_plus;
            if (v.error_plus < min_error)
                min_error = v.error_plus;
        }
    }
    else if (et == ErrorType::NEGATIVE)
    {
        max_error = vertices[0].error_minus;
        min_error = vertices[0].error_minus;

        for (MeshVertex &v : vertices)
        {
            if (v.error_minus < max_error)
                max_error = v.error_minus;
            if (v.error_minus > min_error)
                min_error = v.error_minus;
        }
    }

    return Range{min_error, max_error};
}


Color Mesh::get_face_color(ErrorType error_type, const Face& face, double range)
{
    double error;
    Color color;

    for (int i = 0; i < 3; i++)
    {
        if (error_type == POSITIVE)
            error += vertices[face.vertex_idx[i]].error_plus;
        else if (error_type == NEGATIVE)
            error += vertices[face.vertex_idx[i]].error_minus;
    }
    error /= 3 * range;

    if (error_type == POSITIVE)
    {
    	error = std::min(error, 1.0);
        color.r = error;
        color.g = 1. - error;
        color.b = 0.;
    }
    else if (error_type == NEGATIVE)
    {
    	error = std::max(error, -1.0);
        color.r = 0;
        color.g = 1 + error;
        color.b = -error;
    }

    return color;
}


std::vector<Triangle> Mesh::get_triangles(ErrorType error_type, double range)
{
    vector<Triangle> triangles;
    int num_faces = faces.size();
    triangles.reserve(num_faces);

    for (int i = 0; i < num_faces; i++)
    {
        Face &face = faces[i];
        MeshVertex &mv0 = vertices[face.vertex_idx[0]];
        MeshVertex &mv1 = vertices[face.vertex_idx[1]];
        MeshVertex &mv2 = vertices[face.vertex_idx[2]];

        Triangle triangle(Point2D(mv0.u, mv0.v), Point2D(mv1.u, mv1.v),
                          Point2D(mv2.u, mv2.v),
                          get_face_color(error_type, face, range));

        if (triangle.is_split())
        {
            Triangle copy_triangle = triangle;

            for (int i = 0; i < 3; i++)
            {
                if (triangle.vertices[i].x >= 0.5)
                    triangle.vertices[i].x -= 1;
            }

            for (int i = 0; i < 3; i++)
            {
                if (copy_triangle.vertices[i].x < 0.5)
                    copy_triangle.vertices[i].x += 1;
            }
            triangles.push_back(copy_triangle);
        }

        triangles.emplace_back(triangle);
    }

    return triangles;
}


void Mesh::save_obj(std::string filename, double z_rotation)
{
    FILE *out = fopen(filename.c_str(), "w");

    if (!out)
        return;

    for (MeshVertex &mv : vertices)
	{
        if (z_rotation == 0.0)
        {
            fprintf(out, "v %.10lf %.10lf %.10lf\n", mv.x, mv.y, mv.z);
        }
        else
        {
            // Rotate about z axis
            double x = mv.x * cos(z_rotation) - mv.y * sin(z_rotation);
            double y = mv.x * sin(z_rotation) + mv.y * cos(z_rotation);
            fprintf(out, "v %.10lf %.10lf %.10lf\n", x, y, mv.z);
        }
	}

    for (MeshVertex &mv : vertices)
        fprintf(out, "vt %.10lf %.10lf\n", mv.u, mv.v);

    for (Face &face : faces)
    {
        fprintf(out, "f");
        for (int i = 0; i < 3; i++)
        {
            fprintf(out, " %d/%d", face.vertex_idx[i] + 1,
                    face.vertex_idx[i] + 1);
        }
        fprintf(out, "\n");
    }

    fclose(out);
}
