#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTITIES 10000
#define MAX_COMPONENTS 64

typedef uint32_t Entity;
typedef uint64_t Signature; // Bitmask for 64 possible component types

/* * SPARSE SET: The secret sauce of ECS.
 * 'sparse' allows O(1) lookups: dense_index = sparse[entity_id]
 * 'dense' keeps data packed tightly for the CPU cache.
 */
typedef struct {
    size_t element_size;
    uint32_t count;
    Entity sparse[MAX_ENTITIES];
    Entity dense_to_entity[MAX_ENTITIES]; // Maps dense index back to entity
    void* dense_data; // The actual packed component structs
} ComponentPool;

/* THE REGISTRY: Holds everything in the world */
typedef struct {
    uint32_t entity_count;
    Signature signatures[MAX_ENTITIES];
    ComponentPool pools[MAX_COMPONENTS];
} Registry;

// Core API
void ecs_init(Registry* reg);
Entity ecs_create_entity(Registry* reg);
void ecs_destroy_entity(Registry* reg, Entity e);

// Pool/Component API
void ecs_register_component(Registry* reg, int comp_id, size_t element_size);
void* ecs_add_component(Registry* reg, Entity e, int comp_id);
void* ecs_get_component(Registry* reg, Entity e, int comp_id);
bool ecs_has_component(Registry* reg, Entity e, int comp_id);

#endif
