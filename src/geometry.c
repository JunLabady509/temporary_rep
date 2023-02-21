#include <math.h>

// Fonctions pour faire des calculs géométriques en C.
int is_distance(
    int position_x_0,
    int position_y_0,
    int position_x_1,
    int position_y_1,
    float distance
) {
    // Nous allons calculer la distance avec le théorème de Pythagore.
    int horiz_dist = position_x_1 - position_x_0;
    int vert_dist = position_y_1 - position_y_0;
    float points_dist = (
        pow((float)horiz_dist, 2.0)
        + pow((float)vert_dist, 2.0)
    );

    return (points_dist <= sqrt(distance));
}