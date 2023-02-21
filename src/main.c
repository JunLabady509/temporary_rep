#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "geometry.h"
#include "particle.h"

// Nombre de particules dans la simulation
#define N_PARTICLE 5

// Dimensions de la fenêtre
#define WINDOW_DIM 480

// Microsecondes entre chaque frame
#define FRAME_DELAY 16667

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);

    printf("Creating the window...");

    // create the window
    SDL_Window *main_window = SDL_CreateWindow(
        "Physics Sim in pure C (press X to close)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_DIM,
        WINDOW_DIM,
        0);

    // create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        main_window,
        0,
        0);

    // Null pointer check
    if (main_window == NULL || renderer == NULL)
    {
        printf("\nError! Something went horribly wrong!\n");
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    printf(" done.\n");

    int window_open = 1; // will be set to 0 when it's time to close the window
    SDL_Event event;     // used to check for keyboard input

    // Stock la position de la souris à chaque frame
    // (pour déterminer la direction du mouvement)
    int mouse_x, mouse_y;
    int prev_mouse_x, prev_mouse_y;
    Particle *mouse_particle = create_particle(0, 0, 0, 0);

    // Creer les particules
    Particle **particles = (Particle **)malloc(N_PARTICLE * sizeof(Particle *));
    int i;
    for (i = 0; i < N_PARTICLE; i++)
    {
        particles[i] = create_particle(
            // Les Particules commencent dans un cercle
            // près du centre de l'écran
            (WINDOW_DIM / 2) + 20 * sin(i),
            (WINDOW_DIM / 2) + 20 * cos(i),
            0,
            0);
    }

    // Créer un tableau de particules pour stocker l'état des particules
    // à la frame précédente
    Particle **prev_frame_particles = (Particle **)malloc(N_PARTICLE * sizeof(Particle *));
    for (i = 0; i < N_PARTICLE; i++)
    {
        prev_frame_particles[i] = create_particle(0, 0, 0, 0);
    }

    printf("Press the X key to exit\n");

    while (window_open)
    {
        usleep(FRAME_DELAY);

        // Stock la position de la souris à la frame précédente
        prev_mouse_x = mouse_x;
        prev_mouse_y = mouse_y;

        // Récupère la position de la souris
        SDL_GetMouseState(
            &mouse_x,
            &mouse_y);

        // Gère les événements (clavier, souris, etc.)
        mouse_particle->x_pos = (float)mouse_x;
        mouse_particle->y_pos = (float)mouse_y;
        mouse_particle->x_momentum = (float)mouse_x - prev_mouse_x;
        mouse_particle->y_momentum = (float)mouse_y - prev_mouse_y;

        // Verifie si le curseur de la souris est dans la fenêtre ou non (pour cacher le curseur)

        if (
            (mouse_x > 0 && mouse_x < WINDOW_DIM) &&
            (mouse_y > 0 && mouse_y < WINDOW_DIM))
        {
            SDL_ShowCursor(SDL_DISABLE);
        }
        else
        {
            SDL_ShowCursor(SDL_ENABLE);
        }

        // D'abord, on copie les particules de la frame précédente
        for (i = 0; i < N_PARTICLE; i++)
        {
            duplicate_particle(prev_frame_particles[i], *particles[i]);
        }
        // Ensuite, on vérifie les collisions
        int j, bounce_direction = 0;
        for (i = 0; i < N_PARTICLE; i++)
        {
            if (bounce_direction > 3)
                bounce_direction = 0;
            for (j = 0; j < N_PARTICLE; j++)
            {
                if (bounce_direction > 3)
                    bounce_direction = 0;
                if (i != j)
                {
                    // Les particules se collent si elles sont à moins de 1.5 pixels l'une de l'autre
                    // (le 1.5 est un facteur de correction pour rendre la détection de collision
                    // plus précise)
                    if (
                        is_distance(
                            (int)particles[i]->x_pos,
                            (int)particles[i]->y_pos,
                            (int)prev_frame_particles[j]->x_pos,
                            (int)prev_frame_particles[j]->y_pos,
                            1.5))
                    {
                        collide(
                            particles[i],
                            *prev_frame_particles[j],
                            bounce_direction);
                        bounce_direction++;
                        /* Look in the definition of collide() in particles.c
                        to understand the purpose of `bounce_direction`. */
                    }
                }
            }

            // Collide avec la particule de la souris, aussi
            if (
                is_distance(
                    (int)particles[i]->x_pos,
                    (int)particles[i]->y_pos,
                    mouse_x,
                    mouse_y,
                    1.5))
            {
                collide(
                    particles[i],
                    *mouse_particle,
                    bounce_direction);
                bounce_direction++;
            }

            // Met à jour les propriétés de chaque particule
            // (position, vitesse, etc.)
            update_particle(particles[i], mouse_x, mouse_y);
        }

        // Background noir
        // (c'est la couleur par défaut, mais on la met quand même)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);

        // Couleur verte pour les particules
        SDL_SetRenderDrawColor(renderer, 0, 255, 50, 255);

        // Dessine les particules
        for (i = 0; i < N_PARTICLE; i++)
        {
            SDL_RenderDrawPoint(
                renderer,
                particles[i]->x_pos,
                particles[i]->y_pos);
        }

        // Affiche le rendu
        SDL_RenderPresent(renderer);

        // Un cercle de couleur rouge pour la particule de la souris
        SDL_RenderFillCircle(renderer, mouse_x, mouse_y, 10);
    
        // SDL_SetRenderDrawColor(renderer, 255, 10, 0, 255);
        // and a little bigger

        // Dessine la particule de la souris
        // (c'est une particule, mais elle est plus grosse)
        // (et elle est rouge)
        SDL_RenderDrawPoint(renderer, mouse_x, mouse_y);
        SDL_RenderDrawPoint(renderer, mouse_x - 1, mouse_y);
        SDL_RenderDrawPoint(renderer, mouse_x + 1, mouse_y);
        SDL_RenderDrawPoint(renderer, mouse_x, mouse_y - 1);
        SDL_RenderDrawPoint(renderer, mouse_x, mouse_y + 1);

        SDL_RenderPresent(renderer);

        // On vérifie si le bouton de fermeture de la fenêtre ou la touche X ont été appuyées
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            window_open = 0;
            break;
        case SDL_KEYUP:
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_x:
                printf("Closing the program.\n");
                window_open = 0;
                break;
            }
            break;
        }
    }

    // Now it's time to clean everything up
    for (i = 0; i < N_PARTICLE; i++)
    {
        free_particle(particles[i]);
    }

    free_particle(mouse_particle);
    free(particles);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(main_window);
    SDL_Quit();
    return 0;
}