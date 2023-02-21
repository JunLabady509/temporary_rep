#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "particle.h"

// dimensions de la fenêtre
#define WINDOW_DIM 480

// Quantité d'énergie que les particules conservent lorsqu'elles heurtent les murs.
#define WALL_BOUNCINESS 0.4

// Acceleration des particules vers les sources de gravité.
// (Plus elles sont proches, plus la gravité est forte)
#define GRAVITY 0.2

// Quantité d'élan que les particules perdent à chaque frame.
#define SPACE_DRAG 0.002

Particle *create_particle(
    float x_pos,
    float y_pos,
    float x_momentum,
    float y_momentum)
{
    Particle *result = (Particle *)malloc(sizeof(Particle));
    result->x_pos = x_pos;
    result->y_pos = y_pos;
    result->x_momentum = x_momentum;
    result->y_momentum = y_momentum;
    return result;
}

void duplicate_particle(Particle *new_particle, Particle source_particle)
{
    new_particle->x_pos = source_particle.x_pos;
    new_particle->y_pos = source_particle.y_pos;
    new_particle->x_momentum = source_particle.x_momentum;
    new_particle->y_momentum = source_particle.y_momentum;
    return;
}

void free_particle(Particle *particle)
{
    free(particle);
    return;
}

void update_particle(
    Particle *particle,
    int gravity_x,
    int gravity_y)
{
    particle->x_pos += particle->x_momentum;
    particle->y_pos += particle->y_momentum;

    particle->x_momentum *= (1.0 - SPACE_DRAG);
    particle->y_momentum *= (1.0 - SPACE_DRAG);

    if (particle->x_pos < 0)
    {
        particle->x_pos = 0;
        particle->x_momentum *= -WALL_BOUNCINESS; // lose a little energy
    }

    if (particle->x_pos > WINDOW_DIM)
    {
        particle->x_pos = WINDOW_DIM;
        particle->x_momentum *= -WALL_BOUNCINESS;
    }

    if (particle->y_pos < 0)
    {
        particle->y_pos = 0;
        particle->y_momentum *= -WALL_BOUNCINESS;
    }

    if (particle->y_pos > WINDOW_DIM)
    {
        particle->y_pos = WINDOW_DIM;
        particle->y_momentum *= -WALL_BOUNCINESS;
    }

    // Peu importe où se trouve la source de gravité,
    // les choses devraient s'accélérer vers elle.

    float y_diff = particle->y_pos - gravity_y;
    float x_diff = particle->x_pos - gravity_x;
    float hypot = sqrt(pow(y_diff, 2) + pow(x_diff, 2));
    float x_component = GRAVITY * (x_diff / hypot);
    float y_component = GRAVITY * (y_diff / hypot);
    particle->x_momentum -= x_component;
    particle->y_momentum -= y_component;
}

// bounce_direction: 0 = gauche, 1 = bas, 2 = droite, 3 = haut
void collide(Particle *a, Particle b, int bounce_direction)
{
    a->x_momentum = b.y_momentum;
    a->y_momentum = b.x_momentum;

    switch (bounce_direction)
    {
    case 0:
        a->x_pos -= 1;
        break;
    case 1:
        a->y_pos += 1;
        break;
    case 2:
        a->x_pos += 1;
        break;
    case 3:
        a->y_pos -= 1;
        break;
    }
}