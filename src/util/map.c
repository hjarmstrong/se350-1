#include "map.h"
#include "../rtx.h"

void map_clear(Map *map) {
    int i;
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        map->nodes[i].key = 0;
    }
}

void *map_get(Map *map, void *key) {
    int i;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (map->nodes[i].key == key) {
            return map->nodes[i].value;
        }
    }
    ASSERT(0) // map_get must be passed a valid key
}

int map_is_in(Map *map, void *key) {
    int i;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (map->nodes[i].key == key) {
            return 1;
        }
    }
    return 0;
}

void *map_reserve(Map *map, void *key) {
    int i;
    int first_free = -1;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (map->nodes[i].key == key) {
            return map->nodes[i].value;
        } else if (first_free == -1 && !map->nodes[i].key) {
            first_free = i;
        }
    }
    ASSERT(first_free != -1) // We're full. Increase MAX_MAP_ELEMENTS
    for (i = 0; i < MAX_MAP_VALUE_SIZE; ++i) {
        map->nodes[first_free].value[i] = 0;
    }
    map->nodes[first_free].key = key;
    return map->nodes[first_free].value;
}

void map_remove(Map *map, void *key) {
    int i;
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (map->nodes[i].key == key) {
            map->nodes[i].key = NULL;
            return;
        }
    }
}
