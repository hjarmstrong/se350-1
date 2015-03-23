#include "map.h"

void map_init(Map *map) {
    int i;
    for (i = 0; i < MAX_MAP_BLOCKS; ++i) {
        map->blocks[i] = NULL;
    }
}

void *map_get(Map *map, void *key) {
    int i, j;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_BLOCKS; ++i) {
        if (!map->blocks[i]) {
            continue;
        }
        for (j = 0; j < MAX_MAP_ELEMENTS_PER_BLOCK; ++j) {
            if (map->blocks[i][j].key == key) {
                return map->blocks[i][j].value;
            }
        }
    }
    ASSERT(0) // map_get must be passed a valid key
}

int map_is_in(Map *map, void *key) {
    int i, j;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_BLOCKS; ++i) {
        if (!map->blocks[i]) {
            continue;
        }
        for (j = 0; j < MAX_MAP_ELEMENTS_PER_BLOCK; ++j) {
            if (map->blocks[i][j].key == key) {
                return 1;
            }
        }
    }
    return 0;
}

void *map_reserve(Map *map, void *key) {
    int i, j;
    int first_free_i = -1;
    int first_free_j = -1;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_BLOCKS; ++i) {
        if (!map->blocks[i]) {
            map->blocks[i] = map->is_kernel ? k_request_memory_block() : request_memory_block();
            for (j = 0; j < MAX_MAP_ELEMENTS_PER_BLOCK; ++j) {
                map->blocks[i][j].key = 0;
            }
        }

        for (j = 0; j < MAX_MAP_ELEMENTS_PER_BLOCK; ++j) {
            if (map->blocks[i][j].key == key) {
                return map->blocks[i][j].value;
            } else if (first_free_i == -1 && !map->blocks[i][j].key) {
                first_free_i = i;
                first_free_j = j;
                break;
            }
        }
    }
    ASSERT(first_free_i != -1) // We're full. Increase MAX_MAP_BLOCKS
    for (i = 0; i < MAX_MAP_VALUE_SIZE; ++i) {
        map->blocks[first_free_i][first_free_j].value[i] = 0;
    }
    map->blocks[first_free_i][first_free_j].key = key;
    return map->blocks[first_free_i][first_free_j].value;
}

void map_remove(Map *map, void *key) {
    int i, j;
    for (i = 0; i < MAX_MAP_BLOCKS; ++i) {
        if (!map->blocks[i]) {
            continue;
        }
        for (j = 0; j < MAX_MAP_ELEMENTS_PER_BLOCK; ++j) {
            if (map->blocks[i][j].key == key) {
                map->blocks[i][j].key = NULL;
                return;
            }
        }
    }
}
