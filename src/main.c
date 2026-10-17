#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "argparser.h"
#include "energy.h"
#include "image.h"
#include "util.h"

/**
 * Compute the brightness and print the statistics of @p `img`,
 * i.e. width, height & brightness.
 */
void statistics(struct image* img) {
    // TODO implement (assignment 3.1)
    /* implement and use the function:
     * `image_brightness`
     */
    printf("width: %u\n", img->w);
    printf("height: %u\n", img->h);
    printf("brightness: %u\n", image_brightness(img));
}

/**
 * Find & print the minimal path of @p `img`.
 */
void find_print_min_path(struct image* img) {
    // TODO implement (assignment 3.2)
    /* implement and use the functions:
     * - `calculate_energy`
     * - `calculate_min_energy_column`
     * - `calculate_optimal_path`
     * in `energy.c`
     */
    if (img->w != 0 && img->h != 0) {
        uint32_t* energy = malloc(sizeof(uint32_t) * ((img->h) * (img->w)));
        calculate_energy(energy, img, img->w);
        int ind = calculate_min_energy_column(energy, img->w, img->w, img->h);
        uint32_t* seam = malloc(sizeof(uint32_t) * img->h);
        calculate_optimal_path(energy, img->w, img->w, img->h, ind, seam);
        for (int i = 0; i < img->h; i++) printf("%u\n", seam[i]);
        free(seam);
        free(energy);
    }
}

/**
 * Find & carve out @p `n` minimal paths in @p `img`.
 * The image size stays the same, instead for every carved out path there is a
 * column of black pixels appended to the right.
 */
void find_and_carve_path(struct image* const img, int n) {
    // TODO implement (assignment 3.3)
    /* implement and use the functions from assignment 3.2 and:
     * - `carve_path`
     * - `image_write_to_file`
     * in `image.c`.
     */
    int w = img->w;
    uint32_t* energy = malloc(sizeof(uint32_t) * ((img->h) * (img->w)));
    uint32_t* seam = malloc(sizeof(uint32_t) * img->h);
    if (n >= img->w) n = img->w;
    for (int i = 0; i < n; i++, w--) {
        calculate_energy(energy, img, w);
        int ind = calculate_min_energy_column(energy, img->w, w, img->h);
        calculate_optimal_path(energy, img->w, w, img->h, ind, seam);
        carve_path(img, w, seam);
    }

    image_write_to_file(img, "out.ppm");
    free(seam);
    free(energy);
}

/**
 * Parse the arguments and call the appropriate functions as specified by the
 * arguments.
 */
int main(int const argc, char** const argv) {
    // DO NOT EDIT
    bool show_min_path = false;
    bool show_statistics = false;
    int n_steps = -1;

    char const* const filename =
        parse_arguments(argc, argv, &show_min_path, &show_statistics, &n_steps);
    if (!filename) return EXIT_FAILURE;

    struct image* img = image_read_from_file(filename);

    if (show_statistics) {
        statistics(img);
        image_destroy(img);
        return EXIT_SUCCESS;
    }

    if (show_min_path) {
        find_print_min_path(img);
    } else {
        if (n_steps < 0 || n_steps > img->w) n_steps = img->w;

        find_and_carve_path(img, n_steps);
    }

    image_destroy(img);
    return EXIT_SUCCESS;
}
