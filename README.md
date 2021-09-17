asterunc
---

Program for generating textures with asteroid shape uncertainties, and making
projections, e.g. for publications.

# Requirements

* [meshview](https://github.com/perkun/svarog) program installed in the system for projections' rendering
* convert (ImageMagick)
* make
* premake5
* python3

# Installation

## Meshview

Install `Svarog` engine together with `meshview` program by following the
instructions on Svarog's [GitHub project page](https://github.com/perkun/svarog).

## asterunc

Just run `./install` script. It `make`s `asterunc` program, then copies
executable to `/usr/local/bin` together with `shaperr` script.

# Running

To produce publication-ready projection you need to run a `shaperr` python script
(which gets installed with `asterunc` program), that will generate uncertainty
textures, make projections and then combine everything in one `.png` file. You
need:

* model file in SHP format,
* file with per vertex uncertainties

```txt
vertex_id err_minus err_plus
...
```

You will get:

* textures with positive and negative uncertainties,
* OBJ model file,
* projections of the model with uncertainty maps


