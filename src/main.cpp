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
std::string file_base(string filename);


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
    argparser.add_option(
        "obj-out", "base name of out OBJ file, default: input model basename",
        false, "");
    argparser.add_option('H', "height", "texture height, default=600px", false,
                         "600");
    argparser.add_option('W', "width", "texture width, default=600px", false,
                         "600");
    argparser.add_option('r', "range",
                         "Specify the range of value. Values obove will have "
                         "the value of 'range' (cutoff)",
                         false, "0.1");
    argparser.add_option(
        'b', "blur", "kernel size of the gausian blur for texture", false, "");
    argparser.add_flag('i', "info", "display info and exit");
    argparser.add_flag('a', "auto-range", "auto max range");

    Args args = argparser.parse_args(argc, argv);

    Mesh mesh(args.get_value<std::string>("model"),
              args.get_value<std::string>("errors"));


    Range range_plus = mesh.get_error_ranges(ErrorType::POSITIVE);
    Range range_minus = mesh.get_error_ranges(ErrorType::NEGATIVE);

    double range;
    if (args["auto-range"])
    {
        range = std::max(range_plus.max, -range_minus.max);
    }
    else
        range = args.get_value<double>("range");


    if (args["info"])
    {
        cout << "INFO:" << endl;

        cout << "num vertices: " << mesh.vertices.size() << endl;
        cout << "num faces " << mesh.faces.size() << endl;
        cout << "range: " << range << endl;
        cout << "(" << range_minus.max << ") --- (" << range_minus.min
             << ") -- 0 -- (" << range_plus.min << ") --- (" << range_plus.max
             << ")\n";
        return 0;
    }

    std::vector<Triangle> triangles_plus =
        mesh.get_triangles(ErrorType::POSITIVE, range);
    std::vector<Triangle> triangles_minus =
        mesh.get_triangles(ErrorType::NEGATIVE, range);

    Texture texture(args.get_value<int>("width"),
                    args.get_value<int>("height"));
    texture.render_triangles(triangles_plus);
    if (args["blur"])
        texture.blur(args.get_value<int>("blur"));
    texture.save(args.get_value<std::string>("out") + "_plus.png");

    texture.render_triangles(triangles_minus);
    if (args["blur"])
        texture.blur(args.get_value<int>("blur"));
    texture.save(args.get_value<std::string>("out") + "_minus.png");

    // save obj file
    std::string obj_filename;
    if (args["obj-out"])
        obj_filename = args.get_value<std::string>("obj-out") + ".obj";
    else
    {
        obj_filename = file_base(args.get_value<std::string>("model"));
        obj_filename = remove_extension(obj_filename) + ".obj";
    }
    mesh.save_obj(obj_filename);


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


std::string file_base(string filename)
{
	int slash_pos = filename.rfind("/");
	return filename.substr(slash_pos + 1, filename.size() - slash_pos);
}
