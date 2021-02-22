#ifndef FLECS_PLAYER_H
#define FLECS_PLAYER_H

/* This generated file contains includes for project dependencies */
#include "flecs-player/bake_config.h"

#ifndef FLECS_LEGACY

ECS_ENUM(EcsPlayerSnapshotMode, {
    EcsPlayerSnapshotModeIgnore,
    EcsPlayerSnapshotModeTake,
    EcsPlayerSnapshotModeRestore
});

ECS_STRUCT(EcsPlayerMode, {
    float time_scale;
    ecs_entity_t pipeline;
    ecs_entity_t scope;
    EcsPlayerSnapshotMode snapshot_mode;
});

ECS_STRUCT(EcsPlayer, {
    ecs_entity_t mode;
    ecs_entity_t prev_mode;
ECS_PRIVATE
    ecs_snapshot_t *snapshot;
});

ECS_STRUCT(EcsTargetFps, {
    float target_fps;
});

#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FlecsPlayer {
    ECS_DECLARE_COMPONENT(EcsPlayer);
    ECS_DECLARE_COMPONENT(EcsPlayerMode);
    ECS_DECLARE_COMPONENT(EcsTargetFps);
} FlecsPlayer;

FLECS_PLAYER_API
void FlecsPlayerImport(
    ecs_world_t *world);

#define FlecsPlayerImportHandles(handles)\
    ECS_IMPORT_COMPONENT(handles, EcsPlayer); 

#ifdef __cplusplus
}
#endif

#endif
