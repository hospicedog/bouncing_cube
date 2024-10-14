#include <SDL2/SDL.h>
#include <math.h>
#include "cube_model.h"
#define _USE_MATH_DEFINES

struct object {
    int   n_vertices;
    float (*vertices)[4];
    float (*to_world_vert)[4];
};

// A = A * B
extern void m_mul(float A[4][4], float B[4][4]);

// v = M * v
extern void t_vec(float M[4][4], float v[4]);

void set_as_id(float input[4][4]) {
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++) {
            input[row][col] = 0;
            if (row == col)
                input[row][col] = 1;
        }
}
// Rotates d_angle degrees around the x axis.
void rotate_x(float input[4][4], float d_angle) {
    float a = remainderf(d_angle, 360) * M_PI / 180;
    float rotation_matrix[4][4] = {
        {1, 0,       0,      0},
        {0, cos(a), -sin(a), 0},
        {0, sin(a),  cos(a), 0},
        {0, 0,       0,      1}
    };

    m_mul(input, rotation_matrix);
}

// Rotates d_angle degrees around the y axis.
void rotate_y(float input[4][4], float d_angle) {
    float a = remainderf(d_angle, 360) * M_PI / 180;
    float rotation_matrix[4][4] = {
        {cos(a), 0, -sin(a), 0},
        {0,      1,  0,      0},
        {sin(a), 0,  cos(a), 0},
        {0,      0,  0,      1}
    };

    m_mul(input, rotation_matrix);
}

// Rotates d_angle degrees around the z axis.
void rotate_z(float input[4][4], float d_angle) {
    float a = remainderf(d_angle, 360) * M_PI / 180;
    float rotation_matrix[4][4] = {
        {cos(a), -sin(a), 0, 0},
        {sin(a),  cos(a), 0, 0},
        {0,       0,      1, 0},
        {0,       0,      0, 1}
    };

    m_mul(input, rotation_matrix);
}

// Scales the object.
void scale(float input[4][4], float scale_factor) {
    float scale_matrix[4][4] = {
        {scale_factor, 0, 0, 0},
        {0, scale_factor, 0, 0},
        {0, 0, scale_factor, 0},
        {0, 0, 0, 1}
    };

    m_mul(input, scale_matrix);
}

// Moves the object around the space.
void translate(float input[4][4], float x, float y, float z) {
    float translation_matrix[4][4] = {
        {1, 0, 0, x},
        {0, 1, 0, y},
        {0, 0, 1, z},
        {0, 0, 0, 1}
    };

    m_mul(input, translation_matrix);
}

// Proyects a 3D object onto the screen that captures fov degrees of the scene.
void get_proyection(struct object* obj, float translation[4][4], float fov) {
    // The near and far values are at 0 and -1 respectively. The camera, set at 1,
    // is at a distance of 1 and 2, which are the values used in the matrix.
    float s = atan(fov * M_PI / 180);
    float proyection_matrix[4][4] = {
        {s,  0,  0,  0},
        {0,  s,  0,  0},
        {0,  0, -2, -1},
        {0,  0, -4,  0}
    };
    
    m_mul(proyection_matrix, translation);

    for (int v = 0; v < obj->n_vertices; v++) {
        for (int i = 0; i < 4; i++)
            obj->to_world_vert[v][i] = obj->vertices[v][i];
        t_vec(proyection_matrix, obj->to_world_vert[v]);
    }
}

void get_outer_coordinates(struct object* obj, float* left, float* right, float* top, float* bottom) {
    *left = obj->to_world_vert[0][0]; *right  = obj->to_world_vert[0][0];
    *top  = obj->to_world_vert[0][1]; *bottom = obj->to_world_vert[0][1];

    for (int v = 0; v < obj->n_vertices; v++) {
        *left   = *left   > obj->to_world_vert[v][0] ? obj->to_world_vert[v][0] : *left;
        *right  = *right  < obj->to_world_vert[v][0] ? obj->to_world_vert[v][0] : *right;
        *top    = *top    > obj->to_world_vert[v][1] ? obj->to_world_vert[v][1] : *top;
        *bottom = *bottom < obj->to_world_vert[v][1] ? obj->to_world_vert[v][1] : *bottom;
    }
}

void transform(struct object* obj, float transformation[4][4]) {
    for (int v = 0; v < obj->n_vertices; v++)
        t_vec(transformation, obj->vertices[v]);
}

int main () {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;
    int w_width, w_height;

	SDL_CreateWindowAndRenderer(500, 500, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetWindowTitle(window, "cube");

    float vertices[N_VERTICES_CUBE][4],
     vert_to_world[N_VERTICES_CUBE][4];
    struct object cube = { N_VERTICES_CUBE, vertices, vert_to_world };

    for (int v = 0; v < cube.n_vertices; v++) {
        int i = 0;
        for (; i < 3; i++)
            cube.vertices[v][i] = CUBE_VERTICES[v][i];
        cube.vertices[v][i] = 1;
    }

    float initial_transform[4][4];
    set_as_id(initial_transform);

    // Cube is defined from (-1, -1, -1) to (1, 1, 1).
    float cube_size = 75, cube_diag = cube_size * 4;
    scale(initial_transform, cube_size);
    transform(&cube, initial_transform);

    float x = 500, y = 500, x_mov = 3, y_mov = 1;
    float left, right, top, bottom,
          transformation[4][4], to_world[4][4];

	while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
            break;
        if (event.type == SDL_WINDOWEVENT)
            SDL_GetWindowSize(window, &w_width, &w_height);

        // Transformations are applied from the bottom to the top.
        set_as_id(transformation);
        rotate_x(transformation, 0.5 * x_mov / fabsf(x_mov));
        rotate_y(transformation, 0.5 * y_mov / fabsf(y_mov));
        transform(&cube, transformation);

        set_as_id(to_world);
        translate(to_world, x, y, 0); 
        get_proyection(&cube, to_world, 90);

        if (x > (float) w_width - cube_diag || x < cube_diag) {
            get_outer_coordinates(&cube, &left, &right, &top, &bottom);
            if (right + x_mov >= (float) w_width || left + x_mov <= 0)
                x_mov = -x_mov;
        }

        if (y > (float) w_height - cube_diag || y < cube_diag) {
            get_outer_coordinates(&cube, &left, &right, &top, &bottom);
            if (bottom + y_mov >= (float) w_height || top + y_mov <= 0)
                y_mov = -y_mov;
        }

        x += x_mov;
        y += y_mov;

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

        for (int e = 0; e < N_EDGES_CUBE; e++)
            SDL_RenderDrawLine(renderer,
                (int) cube.to_world_vert[CUBE_EDGES[e][0]][0], (int) cube.to_world_vert[CUBE_EDGES[e][0]][1],
                (int) cube.to_world_vert[CUBE_EDGES[e][1]][0], (int) cube.to_world_vert[CUBE_EDGES[e][1]][1]);
        
        SDL_RenderPresent(renderer);
		SDL_Delay(16);
    }

	SDL_Quit();
	return 0;
}

