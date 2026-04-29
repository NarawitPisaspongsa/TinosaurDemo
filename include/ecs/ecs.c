#include <ecs/ecs.h>

void ecs_init(Registry* reg) {
    reg->entity_count = 0;
    memset(reg->signatures, 0, sizeof(reg->signatures));
    memset(reg->pools, 0, sizeof(reg->pools));
}

Entity ecs_create_entity(Registry* reg) {
    Entity e = reg->entity_count++;
    reg->signatures[e] = 0;
    return e;
}

void ecs_register_component(Registry* reg, int comp_id, size_t element_size) {
    reg->pools[comp_id].element_size = element_size;
    reg->pools[comp_id].count = 0;
    reg->pools[comp_id].dense_data = malloc(MAX_ENTITIES * element_size);
}

void* ecs_add_component(Registry* reg, Entity e, int comp_id) {
    ComponentPool* pool = &reg->pools[comp_id];
    
    // Add bit to signature
    reg->signatures[e] |= (1ULL << comp_id);

    // Add to sparse set
    uint32_t dense_index = pool->count;
    pool->sparse[e] = dense_index;
    pool->dense_to_entity[dense_index] = e;
    pool->count++;

    // Return pointer to the new component in the dense array
    return (char*)pool->dense_data + (dense_index * pool->element_size);
}

void* ecs_get_component(Registry* reg, Entity e, int comp_id) {
    if (!ecs_has_component(reg, e, comp_id)) return NULL;
    
    ComponentPool* pool = &reg->pools[comp_id];
    uint32_t dense_index = pool->sparse[e];
    
    return (char*)pool->dense_data + (dense_index * pool->element_size);
}

bool ecs_has_component(Registry* reg, Entity e, int comp_id) {
    return (reg->signatures[e] & (1ULL << comp_id)) != 0;
}
