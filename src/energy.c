#include "energy.h"

#include <stdlib.h>
#include <string.h>

#include "indexing.h"
#include "util.h"

uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }

uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

/**
 * Calculate the difference of two color values @p a and @p b.
 * The result is the sum of the squares of the differences of the three (red,
 * green and blue) color components.
 * */
inline uint32_t diff_color(struct pixel const a, struct pixel const b) {
    // TODO implement (assignment 3.2)
    uint32_t diff = (((a.r - b.r) * (a.r - b.r))) +
                    (((a.g - b.g) * (a.g - b.g))) +
                    (((a.b - b.b) * (a.b - b.b)));
    return diff;
}

/**
 * Calculate the total energy at every pixel of the image @p `img`,
 * but only considering columns with index less than @p `w`.
 * To this end, first calculate the local energy and use it to calculate the
 * total energy.
 * @p `energy` is expected to have allocated enough space
 * to represent the energy for every pixel of the whole image @p `img.
 * @p `w` is the width up to (excluding) which column in the image the energy
 * should be calculated. The energy is expected to be stored exactly analogous
 * to the image, i.e. you should be able to access the energy of a pixel with
 * the same array index.
 */
void calculate_energy(uint32_t* const energy, struct image* const img,
                      int const w) {
    // TODO implement (assignment 3.2)
    // calculate locale energy
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < img->h; j++) {
            if (i == 0 && j == 0)
                energy[0] = 0;
            else if (j == 0)
                energy[yx_index(j, i, img->w)] =
                    diff_color(img->pixels[yx_index(j, i, img->w)],
                               img->pixels[yx_index(j, i - 1, img->w)]);
            else if (i == 0)
                energy[yx_index(j, i, img->w)] =
                    diff_color(img->pixels[yx_index(j, i, img->w)],
                               img->pixels[yx_index(j - 1, i, img->w)]);
            else
                energy[yx_index(j, i, img->w)] =
                    diff_color(img->pixels[yx_index(j, i, img->w)],
                               img->pixels[yx_index(j - 1, i, img->w)]) +
                    diff_color(img->pixels[yx_index(j, i, img->w)],
                               img->pixels[yx_index(j, i - 1, img->w)]);
        }
    }

    // calculate total energy
    for (int j = 1; j < img->h; j++) {
        for (int i = 0; i < w; i++) {
            if (i == 0)
                energy[yx_index(j, i, img->w)] =
                    min(energy[yx_index(j - 1, i, img->w)],
                        energy[yx_index(j - 1, i + 1, img->w)]) +
                    energy[yx_index(j, i, img->w)];
            else if (i == (w - 1))
                energy[yx_index(j, i, img->w)] =
                    min(energy[yx_index(j - 1, i, img->w)],
                        energy[yx_index(j - 1, i - 1, img->w)]) +
                    energy[yx_index(j, i, img->w)];
            else
                energy[yx_index(j, i, img->w)] =
                    min(min(energy[yx_index(j - 1, i, img->w)],
                            energy[yx_index(j - 1, i + 1, img->w)]),
                        energy[yx_index(j - 1, i - 1, img->w)]) +
                    energy[yx_index(j, i, img->w)];
        }
    }
}

/**
 * Calculate the index of the column with the least energy in bottom row.
 * Expects that @p `energy` holds the energy of every pixel of @p `img` up to
 * column (excluding) @p `w`. Columns with index `>= w` are not considered as
 * part of the image.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
int calculate_min_energy_column(uint32_t const* const energy, int const w0,
                                int const w, int const h) {
    // TODO implement (assignment 3.2)
    uint32_t mi = energy[yx_index(h - 1, 0, w0)];
    int index = 0;
    for (int i = 0; i < w; i++) {
        if (mi > energy[yx_index(h - 1, i, w0)]) {
            mi = energy[yx_index(h - 1, i, w0)];
            index = i;
        }
    }
    return index;
}

/**
 * Calculate the optimal path (i.e. least energy), according to the energy
 * entries in @p `energy` up to (excluding) column @p `w`. The path is stored in
 * @p `seam`. Columns with index `>= w` are not considered as part of the image.
 * @p `x` is the index in the bottom row where the seam starts.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
void calculate_optimal_path(uint32_t const* const energy, int const w0,
                            int const w, int const h, int x,
                            uint32_t* const seam) {
    // TODO implement (assignment 3.2)
    seam[h - 1] = x;
    for (int j = (h - 2); j >= 0; j--) {
        if (((x + 1) < w) && ((x - 1) >= 0)) {
            uint32_t a = energy[yx_index(j, x, w0)];
            uint32_t b = energy[yx_index(j, x - 1, w0)];
            uint32_t c = energy[yx_index(j, x + 1, w0)];
            if (a <= min(b, c))
                seam[j] = x;
            else if (b <= min(a, c))
                seam[j] = x - 1;
            else
                seam[j] = x + 1;
        } else if ((x - 1) < 0) {
            uint32_t a = energy[yx_index(j, x, w0)];
            uint32_t b = energy[yx_index(j, x + 1, w0)];
            if (a <= b)
                seam[j] = x;
            else
                seam[j] = x + 1;
        } else if (((x + 1) > w) && ((x - 1) < 0))
            seam[j] = x;
        else {
            uint32_t a = energy[yx_index(j, x, w0)];
            uint32_t b = energy[yx_index(j, x - 1, w0)];
            if (a <= b)
                seam[j] = x;
            else
                seam[j] = x - 1;
        }
        x = seam[j];
    }
}
