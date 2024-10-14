#define N_VERTICES_CUBE 8
#define N_EDGES_CUBE   12

const float CUBE_VERTICES[N_VERTICES_CUBE][3] = {
    {-1,  1,  1},
    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    {-1,  1, -1},
    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1}
};

const int CUBE_EDGES[N_EDGES_CUBE][2] = {
    {0, 1}, {0, 3}, {0, 4},
    {2, 1}, {2, 3}, {2, 6},
    {5, 1}, {5, 4}, {5, 6},
    {7, 3}, {7, 4}, {7, 6}
};

