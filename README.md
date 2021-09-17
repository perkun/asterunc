asterunc
---

# Requirements

* [meshview](https://github.com/perkun/svarog) program installed in the system for projections' rendering
* convert (ImageMagick)

# Installation

## Meshview

Install `Svarog` engine together with `meshview` program by following the
instructions on Svarog's [GitHub project page](https://github.com/perkun/svarog).

## asterunc

Just run `./install` script.

# Running

To produce publication-ready projection you need to run a `shaperr` bash script
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


