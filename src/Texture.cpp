#include "Texture.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;


Texture::Texture(int w, int h): width(w), height(h)
{
    canvas_r = new double[width * height];
    canvas_g = new double[width * height];
    canvas_b = new double[width * height];

}

void Texture::clear()
{
    for (int i = 0; i < width * height; i++)
    {
		canvas_r[i] = canvas_g[i] = canvas_b[i] = 0.;
    }
}


Texture::~Texture()
{
    delete[] canvas_r;
    delete[] canvas_g;
    delete[] canvas_b;
}


double frac(double x)
{
	return x - floor(x);
}


void Texture::render_triangles(const vector<Triangle> &triangles)
{
	clear();

	for (Triangle triangle: triangles)
	{
		// to canvas space
		for (int i = 0; i < 3; i++)
		{
			triangle.vertices[i].x *= width;
			triangle.vertices[i].y *= height;
		}

		BoundingBox bb = triangle.get_bouding_box();

		for (int y = floor(bb.bottom) - 1; y <= ceil(bb.top) + 1; y++)
			for (int x = floor(bb.left) -1; x <= ceil(bb.right) + 1; x++)
			{
                if (x < 0. || x >= width || y >= height || y < 0)
                    continue;

				if (!triangle.is_inside(Point2D(x, y)))
					continue;
				int idx = y * width + x;
				canvas_r[idx] = triangle.color.r;
				canvas_g[idx] = triangle.color.g;
				canvas_b[idx] = triangle.color.b;

			}
	}

	fill_empty_spots();
}


void Texture::fill_empty_spots()
{
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;

            if (canvas_r[idx] == 0.0 && canvas_g[idx] == 0.0 &&
                canvas_b[idx] == 0.0)
            {
                int direction;
                y < height / 2 ? direction = 1 : direction = -1;

                int tmp_y = y + direction;
                while (tmp_y > 0 && tmp_y < height)
                {
                    int tmp_idx = tmp_y * width + x;
                    if (canvas_r[tmp_idx] == 0.0 && canvas_g[tmp_idx] == 0.0 &&
                        canvas_b[tmp_idx] == 0.0)
                    {
                        tmp_y += direction;
                        continue;
                    }

                    canvas_r[idx] = canvas_r[tmp_idx];
                    canvas_g[idx] = canvas_g[tmp_idx];
                    canvas_b[idx] = canvas_b[tmp_idx];
                    break;
                }
            }
        }
}


void Texture::save(std::string filename)
{
    stbi_flip_vertically_on_write(1);
    int channels = 3;

    unsigned char *png_data =
        new unsigned char[width * height * channels];

    for (int i = 0; i < width * height; i++)
    {
        png_data[i*channels + 0] = (unsigned char)(canvas_r[i] * 255);
        png_data[i*channels + 1] = (unsigned char)(canvas_g[i] * 255);
        png_data[i*channels + 2] = (unsigned char)(canvas_b[i] * 255);
    }

    stbi_write_png(filename.c_str(), width, height, channels, png_data,
                   width * channels);

    delete[] png_data;
}


double gauss(double x, double mi, double sigma)
{
	// not normalized gauss;

    return exp(-0.5 * (x - mi) * (x - mi) / (sigma * sigma));
}


void Texture::blur(int kernel_size)
{
    double kernel[kernel_size][kernel_size];

    double mi = 0;
    double sigma = (double)kernel_size / 4.;

    // fill the kernel
    double middle = (double)kernel_size / 2 - 0.5;
	double sum = 0;
    for (int y = 0; y < kernel_size; y++)
        for (int x = 0; x < kernel_size; x++)
        {
            kernel[y][x] = gauss(
                   sqrt((x - middle) * (x - middle) + (y - middle) *
                   (y - middle)), 0., sigma);
			sum += kernel[y][x];
        }

	// normalize kernel
    for (int y = 0; y < kernel_size; y++)
        for (int x = 0; x < kernel_size; x++)
			kernel[y][x] /= sum;


    double *tmp_canvas_r = new double[width * height];
    double *tmp_canvas_g = new double[width * height];
    double *tmp_canvas_b = new double[width * height];

    for (int canvas_y = kernel_size/2; canvas_y < height - kernel_size/2; canvas_y++)
        for (int canvas_x = 0; canvas_x < width; canvas_x++)
        {
            double value_r = 0;
            double value_g = 0;
            double value_b = 0;
            for (int kernel_y = 0; kernel_y < kernel_size; kernel_y++)
                for (int kernel_x = 0; kernel_x < kernel_size; kernel_x++)
                {
					int x = (canvas_x + kernel_x - kernel_size / 2);
					int y = (canvas_y + kernel_y - kernel_size / 2);

					// edge wrapping in x
					if (x < 0)
						x += width;
					if (x >= width)
						x -= width;

					int idx = y * width + x;


                    value_r += canvas_r[idx] * kernel[kernel_y][kernel_x];
                    value_g += canvas_g[idx] * kernel[kernel_y][kernel_x];
                    value_b += canvas_b[idx] * kernel[kernel_y][kernel_x];
                }
            tmp_canvas_r[canvas_y * width + canvas_x] = value_r;
            tmp_canvas_g[canvas_y * width + canvas_x] = value_g;
            tmp_canvas_b[canvas_y * width + canvas_x] = value_b;
        }


    for (int canvas_y = 0; canvas_y < kernel_size/2; canvas_y++)
        for (int canvas_x = 0; canvas_x < width; canvas_x++)
		{
			int idx = canvas_y * width + canvas_x;
            tmp_canvas_r[idx] = canvas_r[idx];
            tmp_canvas_g[idx] = canvas_g[idx];
            tmp_canvas_b[idx] = canvas_b[idx];
		}
    for (int canvas_y = height - kernel_size/2; canvas_y < height; canvas_y++)
        for (int canvas_x = 0; canvas_x < width; canvas_x++)
		{
			int idx = canvas_y * width + canvas_x;
            tmp_canvas_r[idx] = canvas_r[idx];
            tmp_canvas_g[idx] = canvas_g[idx];
            tmp_canvas_b[idx] = canvas_b[idx];
		}


    delete[] canvas_r;
    delete[] canvas_g;
    delete[] canvas_b;
	canvas_r = tmp_canvas_r;
	canvas_g = tmp_canvas_g;
	canvas_b = tmp_canvas_b;
}
