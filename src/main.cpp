#include <iostream>
#include <stdio.h>
#include <string>
#include "cppargs/cppargs.h"

#include "Mesh.h"
#include "Texture.h"
#include "Triangle.h"

using std::cout;
using std::endl;


std::string remove_extension(std::string filename);


int main(int argc, char *argv[])
{
    Parser argparser;
    argparser.add_description(
        "Uncertainty textures for asterod models. Program takse a SHP model "
        "and a file with per vertex errors (v err_minus err_plus) as input. "
        "Returns textures for positive and negative uncertainties and OBJ "
        "model file with texture coordinates.");

    argparser.add_option('m', "model", "shp model file", true, "");
    argparser.add_option('e', "errors", "errors file", true, "");
    argparser.add_option('o', "out",
                         "output textures name prefix. You can prepend catalog "
                         "name in front, e.g. path/to/folder/texture",
                         false, "texture");
    argparser.add_option('H', "height", "texture height, default=600px", false,
                         "600");
    argparser.add_option('W', "width", "texture width, default=600px", false,
                         "600");

    Args args = argparser.parse_args(argc, argv);

    Mesh mesh(args.get_value<std::string>("model"),
              args.get_value<std::string>("errors"));

    std::vector<Triangle> triangles_plus =
        mesh.get_triangles(ErrorType::POSITIVE);
    std::vector<Triangle> triangles_minus =
        mesh.get_triangles(ErrorType::NEGATIVE);

    Texture texture(args.get_value<int>("width"),
                    args.get_value<int>("height"));
    texture.render_triangles(triangles_plus);
    texture.save(args.get_value<std::string>("out") + "_plus.png");

    texture.render_triangles(triangles_minus);
    texture.save(args.get_value<std::string>("out") + "_minus.png");

    // save obj file
    mesh.save_obj(remove_extension(args.get_value<std::string>("model")) +
                  ".obj");

    return 0;
}


std::string remove_extension(std::string filename)
{
	int dot_pos = filename.rfind(".");
	int slash_pos = filename.rfind("/");
	if (slash_pos > dot_pos)
		return filename;
	else
		return filename.substr(0, dot_pos);
}
