/**
 * map.c -- Simple fixed-length map used by the RTX
 */

#include "../rtx.h"

#ifndef MAP_H
#define MAP_H
#define MAX_MAP_VALUE_SIZE 3
#define MAX_MAP_BLOCKS 10

typedef struct MapNode {
    void *key;
    int value[MAX_MAP_VALUE_SIZE];
} MapNode;

#define MAX_MAP_ELEMENTS_PER_BLOCK (BLOCK_SIZE/sizeof(MapNode))

typedef struct Map {
    MapNode* blocks[MAX_MAP_BLOCKS];
    int is_kernel;
} Map;

void map_init(Map *map);

/**
 * Invariant: key must be a key that was sent to reserve.
 */
void *map_get(Map *map, void *key);

/**
 * Returns '1' if key is in map, else '0'.
 */
int map_is_in(Map *map, void *key);

/**
 * Invariant: less than MAX_MAP_ELEMENTS must be currently reserved.
 */
void *map_reserve(Map *map, void *key);

/**
 * Invariant: if key is reserved on input, key is not reserved on output.
 */
void map_remove(Map *map, void *key);

#endif // MAP_H
