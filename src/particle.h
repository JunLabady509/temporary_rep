// Structure d'une Particule (Particle)
typedef struct Particle Particle;

struct Particle {
    float x_pos;
    float y_pos;
    float x_momentum;
    float y_momentum;
};

// Fonction pour allouer une nouvelle Particule (Particle) sur le tas.
Particle *create_particle(
    float x_pos,
    float y_pos,
    float x_momentum,
    float y_momentum
);

// Fonction pour dupliquer une Particule (Particle) en fonction d'une autre.
void duplicate_particle(Particle *new_particle, Particle source_particle);

// Fonction pour libérer une Particule (Particle) de la mémoire.
void free_particle(Particle *particle);

// Fonction pour mettre à jour une Particule (Particle).
void update_particle(Particle *particle, int gravity_x, int gravity_y);

// Fonction pour gérer une collision entre deux Particules (Particle).
// Et décide la direction dans laquelle la particule doit se déplacer après la collision.
void collide(Particle *a, Particle b, int bounce_direction);
