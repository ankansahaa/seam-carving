# Seam Carving

Content-aware image resizing in C. Instead of scaling an image uniformly, seam carving
repeatedly removes the vertical path of pixels ("seam") that carries the least visual
information, so important content keeps its proportions.

Written for the Programming 2 course (2025), project 2.

## How it works

1. **Energy** – for every pixel, the local energy is the squared color difference to its
   neighbours; total energy is accumulated top to bottom (dynamic programming).
2. **Minimal seam** – pick the bottom-row column with the least total energy and trace the
   cheapest path back up to the top row.
3. **Carve** – remove that path and shift the rest of each row left. Repeat *n* times.
   The output keeps the original size, with one black column appended on the right per
   carved seam.

| File | Contents |
|---|---|
| `src/image.c` | PPM (P3) reading/writing, brightness, carving a path |
| `src/energy.c` | Energy map, minimal column, optimal path |
| `src/indexing.c` | Row/column index helpers |
| `src/argparser.c` | Command-line parsing |
| `src/unit_tests.c` | Unit tests |

## Build

Requires a C compiler, `make` and `clang-format` (the build formats sources).

```bash
make
```

This produces `bin/carve_opt` (optimised), `bin/carve_debug` (ASan/UBSan) and `bin/testrunner`.

## Usage

```
bin/carve_opt [-n <count>] [-p] [-s] <image.ppm>
```

| Flag | Effect |
|---|---|
| `-s` | Print statistics: width, height, average brightness |
| `-p` | Print the column index of the minimal seam for each row |
| `-n <count>` | Carve out `count` seams and write the result to `out.ppm` |

To work with JPG/PNG directly, `run.sh` converts via ImageMagick, carves, crops the black
columns and converts back:

```bash
./run.sh input.jpg output.jpg -n 100
```

`cutup.sh` generates a series of cut vs. plainly scaled versions of an image for comparison.

## Tests

```bash
make check
```
