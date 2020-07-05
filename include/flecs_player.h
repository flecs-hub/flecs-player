#ifndef FLECS_PLAYER_H
#define FLECS_PLAYER_H

/* This generated file contains includes for project dependencies */
#include "flecs-player/bake_config.h"

#ifdef __cplusplus
extern "C" {
#endif

ECS_ENUM(EcsPlayerState, {
    EcsPlayerPlay,
    EcsPlayerPause,
    EcsPlayerStop
});

ECS_STRUCT(EcsPlayer, {
    EcsPlayerState state;
    EcsPlayerState prev_state;
    float time_scale;
    ecs_entity_t play_pipeline;
    ecs_entity_t stop_pipeline;

ECS_NON_SERIALIZABLE
    ecs_snapshot_t *snapshot;
});

typedef struct FlecsPlayer {
    ECS_DECLARE_COMPONENT(EcsPlayer);
} FlecsPlayer;

void FlecsPlayerImport(
    ecs_world_t *world,
    int flags);

#define FlecsPlayerImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, EcsPlayer); 

#ifdef __cplusplus
}
#endif

#endif