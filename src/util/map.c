#include "map.h"
#include "../rtx.h"

typedef struct MapNode {
    void *key;
    char *value[MAX_MAP_VALUE_SIZE];
} MapNode;

static MapNode mapNodes[MAX_MAP_ELEMENTS];

void map_clear() {
    int i;
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        mapNodes[i].key = NULL;
    }
}

void *map_get(void *key) {
    int i;
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (mapNodes[i].key == key) {
            return mapNodes[i].value;
        }
    }
    ASSERT(0) // map_get must be passed a valid key
}

void *map_reserve(void *key) {
    int i;
    int first_free = -1;
    ASSERT(!!key)
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (mapNodes[i].key == key) {
            return mapNodes[i].value;
        } else if (first_free == -1 && !mapNodes[i].key) {
            first_free = i;
        }
    }
    ASSERT(first_free != -1) // We're full. Increase MAX_MAP_VALUE_SIZE
    mapNodes[first_free].key = key;
    return mapNodes[first_free].value;
}

void map_remove(void *key) {
    int i;
    for (i = 0; i < MAX_MAP_ELEMENTS; ++i) {
        if (mapNodes[i].key == key) {
            mapNodes[i].key = NULL;
            return;
        }
    }
}
