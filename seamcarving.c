#include "seamcarving.h"
#include <stdio.h>
#include <math.h>

void calc_energy(struct rgb_img *im, struct rgb_img **grad) {
    int height = im->height;
    int width = im->width;

    // Create the output image to store gradient
    create_img(grad, height, width);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Wrap-around indices
            int left = (x - 1 + width) % width;
            int right = (x + 1) % width;
            int up = (y - 1 + height) % height;
            int down = (y + 1) % height;

            // Partial derivatives (x-direction)
            int rx = get_pixel(im, y, right, 0) - get_pixel(im, y, left, 0);
            int gx = get_pixel(im, y, right, 1) - get_pixel(im, y, left, 1);
            int bx = get_pixel(im, y, right, 2) - get_pixel(im, y, left, 2);

            // Partial derivatives (y-direction)
            int ry = get_pixel(im, down, x, 0) - get_pixel(im, up, x, 0);
            int gy = get_pixel(im, down, x, 1) - get_pixel(im, up, x, 1);
            int by = get_pixel(im, down, x, 2) - get_pixel(im, up, x, 2);

            // Compute energy
            int dx2 = rx * rx + gx * gx + bx * bx;
            int dy2 = ry * ry + gy * gy + by * by;

            int energy = (int)sqrt(dx2 + dy2);
            uint8_t val = (uint8_t)(energy / 10); // scale down and cast

            // Set the energy value (same in R, G, B)
            set_pixel(*grad, y, x, val, val, val);
        }
    }
}


// Returns the minimum of three doubles
double min_val(double a, double b, double c) {
    if(a <= b && a <= c)
        return a;
    else if(b <= a && b <= c)
        return b;
    else
        return c;
}

int min_val2(int a, int b, int c) {
    if(a <= b && a <= c)
        return a;
    else if(b <= a && b <= c)
        return b;
    else
        return c;
}

// Compute the cumulative energy (cost) array for seam carving
void dynamic_seam(struct rgb_img *grad, double **best_arr) {
    int height = grad->height;
    int width = grad->width;
    int index;

    // Allocate best_arr for height*width doubles
    *best_arr = malloc(sizeof(double) * height * width);
    if (!*best_arr) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Initialize the first row (j == 0) with the energy values from grad.
    // Since grad's R, G, and B channels are the same, we use channel 0.
    for (int i = 0; i < width; i++) {
        (*best_arr)[i] = grad->raster[3 * i];
    }

    // For each subsequent row, add the minimum of the three "parent" values.
    for (int j = 1; j < height; j++) {
        for (int i = 0; i < width; i++) {
            // For boundary pixels, use a large value (1e9) for out-of-bound neighbors.
            double left = (i == 0) ? 1e9 : (*best_arr)[(j - 1) * width + (i - 1)];
            double mid  = (*best_arr)[(j - 1) * width + i];
            double right = (i == width - 1) ? 1e9 : (*best_arr)[(j - 1) * width + (i + 1)];

            double best_prev = min_val(left, mid, right);
            index = j * width + i;
            // Get current pixel's energy from grad (channel 0)
            (*best_arr)[index] = grad->raster[3 * (j * width + i)] + best_prev;
        }
    }
}

void print_best_arr(double *best_arr, int height, int width) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            printf("%6.2f ", best_arr[j * width + i]);
        }
        printf("\n");
    }
}
void recover_path(double *best, int height, int width, int **path){
    *path = malloc(sizeof(int) * height);
    if (!*path) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Step 1: Find the minimum value in the bottom row
    int min_index = 0;
    double min_value = best[(height - 1) * width];
    for (int i = 1; i < width; i++) {
        double val = best[(height - 1) * width + i];
        if (val < min_value) {
            min_value = val;
            min_index = i;
        }
    }

    // Set last row path
    (*path)[height - 1] = min_index;

    // Step 2: Work upwards from bottom-1 to top
    for (int j = height - 2; j >= 0; j--) {
        int prev_idx = (*path)[j + 1];
        int best_idx = prev_idx;
        double best_cost = best[j * width + prev_idx];

        if (prev_idx > 0 && best[j * width + prev_idx - 1] < best_cost) {
    best_idx = prev_idx - 1;
    best_cost = best[j * width + prev_idx - 1];
}
        if (prev_idx < width - 1 && best[j * width + prev_idx + 1] < best_cost) {
            best_idx = prev_idx + 1;
            // best_cost updated if needed, though not strictly required afterwards
}


        (*path)[j] = best_idx;
    }
}



int main() {
    struct rgb_img *im;
    struct rgb_img *grad;

    printf("Reading image...\n");
    read_in_img(&im, "6x5.bin");  

    printf("Calculating energy...\n");
    calc_energy(im, &grad);

    printf("Printing gradient energy values:\n");
    print_grad(grad);

    double *best_arr = NULL;
    dynamic_seam(grad, &best_arr);

    printf("Printing dynamic seam cost array:\n");
    print_best_arr(best_arr, grad->height, grad->width);

    // === New code starts here ===
    int *path = NULL;
    recover_path(best_arr, grad->height, grad->width, &path);

    printf("Recovered seam path (column indices):\n[");
    for (int i = 0; i < grad->height; i++) {
        printf("%d", path[i]);
        if (i != grad->height - 1)
            printf(", ");
    }
    printf("]\n");

    // Optional: Check if it matches expected output
    int expected[] = {3, 4, 3, 2, 2};
    int correct = 1;
    for (int i = 0; i < grad->height; i++) {
        if (path[i] != expected[i]) {
            correct = 0;
            break;
        }
    }
    if (correct) {
        printf("Seam path matches expected result ✅\n");
    } else {
        printf("Seam path does NOT match expected result ❌\n");
    }

    free(path);
    // === New code ends here ===

    free(best_arr);
    destroy_image(im);
    destroy_image(grad);

    return 0;
}



