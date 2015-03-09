/**
 * map.c -- Simple fixed-length map used by the RTX
 */

#ifndef MAP_H
#define MAP_H
#define MAX_MAP_ELEMENTS 64
#define MAX_MAP_VALUE_SIZE 32

typedef struct MapNode {
    void *key;
    char *value[MAX_MAP_VALUE_SIZE];
} MapNode;

typedef struct Map {
    MapNode nodes[MAX_MAP_ELEMENTS];
} Map;

void map_clear(Map *map);

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
