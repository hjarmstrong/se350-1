/**
 * map.c -- Simple fixed-length map used by the RTX
 */

#ifndef MAP_H
#define MAP_H
#define MAX_MAP_ELEMENTS 64
#define MAX_MAP_VALUE_SIZE 32

/**
 * Invariant: key must be a key that was sent to reserve.
 */
void *map_get(void *key);

/**
 * Invariant: less than MAX_MAP_ELEMENTS must be currently reserved.
 */
void *map_reserve(void *key);

/**
 * Invariant: if key is reserved on input, key is not reserved on output.
 */
void map_remove(void *key);

#endif // MAP_H
