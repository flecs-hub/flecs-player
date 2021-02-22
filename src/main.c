#include <flecs_player.h>

static 
void player_enable_scope(
    ecs_world_t* world,
    ecs_entity_t scope,
    bool enabled)
{

}

static 
void player_init_mode(
    ecs_world_t *world,
    void* ctx)
{
    ecs_ref_t *ref = ctx;
    
    EcsPlayerMode *mode = (EcsPlayerMode*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(mode != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (mode->scope) {
        player_enable_scope(world, mode->scope, false);
    }

    ecs_os_free(ref);
}

static
void player_unset_mode(
    ecs_world_t* world,
    void* ctx) 
{
    ecs_ref_t *ref = ctx;
    
    EcsPlayerMode *mode = (EcsPlayerMode*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(mode != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (mode->scope) {
        player_enable_scope(world, mode->scope, false);
    }
    
    ecs_os_free(ref);
}

static 
void player_set_mode(
    ecs_world_t *world,
    void *ctx)
{
    ecs_ref_t *ref = ctx;
    
    EcsPlayerMode *mode = (EcsPlayerMode*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(mode != NULL, ECS_INTERNAL_ERROR, NULL);

    if (mode->time_scale >= 0.0) {
        ecs_set_time_scale(world, mode->time_scale);
    }

    if (mode->pipeline) {
        ecs_set_pipeline(world, mode->pipeline);
    }

    if (mode->scope) {
        player_enable_scope(world, mode->scope, true);
    }
    
    ecs_os_free(ref);
}

static
void player_take_snapshot(
    ecs_world_t *world,
    void* ctx)
{
    ecs_ref_t *ref = ctx;
    EcsPlayer *player = (EcsPlayer*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(player != NULL, ECS_INTERNAL_ERROR, NULL);
    
    player->snapshot = ecs_snapshot_take(world);

    ecs_os_free(ref);
}

static
void player_restore_snapshot(
    ecs_world_t *world,
    void* ctx)
{
    ecs_ref_t *ref = ctx;
    EcsPlayer *player = (EcsPlayer*)ecs_get_ref_w_entity(world, ref, 0, 0);
    ecs_assert(player != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (player->snapshot) {
        ecs_snapshot_restore(world, player->snapshot);

        player = (EcsPlayer*)ecs_get_ref_w_entity(world, ref, 0, 0);
        ecs_reset_clock(world);
    }

    player->snapshot = NULL;
    
    ecs_os_free(ref);
}

static
void EcsSetPlayerMode(ecs_iter_t* it) {
    ecs_world_t *world = it->world;

    EcsPlayerMode *mode = ecs_column(it, EcsPlayerMode, 1);
    ecs_entity_t ecs_typeid(EcsPlayerMode) = ecs_column_entity(it, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_ref_t *ref = ecs_os_calloc(sizeof(ecs_ref_t));

        ecs_get_ref(world, ref, it->entities[i], EcsPlayerMode);

        ecs_run_post_frame(it->world, player_init_mode, ref);
    }
}

static
void EcsSetPlayer(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    EcsPlayer *player = ecs_column(it, EcsPlayer, 1);
    ecs_entity_t ecs_typeid(EcsPlayer) = ecs_column_entity(it, 1);
    ecs_entity_t ecs_typeid(EcsPlayerMode) = ecs_column_entity(it, 2);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e_mode = player[i].mode;
        ecs_entity_t e_prev_mode = player[i].prev_mode;

        if (e_prev_mode != e_mode) {
            player[i].prev_mode = e_mode; 
            
            ecs_ref_t *mode_ref = ecs_os_calloc(sizeof(ecs_ref_t));
            ecs_ref_t *prev_mode_ref = ecs_os_calloc(sizeof(ecs_ref_t));
            
            const EcsPlayerMode* mode = ecs_get_ref(world, mode_ref, e_mode, EcsPlayerMode);
            const EcsPlayerMode* prev_mode = ecs_get_ref(world, prev_mode_ref, e_prev_mode, EcsPlayerMode);

            ecs_fini_action_t player_action = NULL;

            if (prev_mode == NULL || prev_mode->snapshot_mode != EcsPlayerSnapshotModeIgnore) {
                switch (mode->snapshot_mode) {
                case EcsPlayerSnapshotModeTake:
                    player_action = player_take_snapshot;
                    break;

                case EcsPlayerSnapshotModeRestore:
                    player_action = player_restore_snapshot;
                    break;

                default:
                    break;
                }
            }

            if (player_action) {
                ecs_ref_t *player_ref = ecs_os_calloc(sizeof(ecs_ref_t));

                ecs_get_ref(world, player_ref, it->entities[i], EcsPlayer);

                ecs_run_post_frame(it->world, player_action, player_ref);
            }

            ecs_run_post_frame(it->world, player_unset_mode, prev_mode_ref);
            ecs_run_post_frame(it->world, player_set_mode, mode_ref);
        }
    }
}

void FlecsPlayerImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPlayer);

    ECS_IMPORT(world, FlecsMeta);

    ecs_set_name_prefix(world, "Ecs");

    ecs_set_ptr(world, ecs_set(world, 0,
        EcsName, {"EcsPlayerSnapshotMode", NULL, NULL}),
        EcsMetaType, &__EcsPlayerSnapshotMode__);

    ECS_META(world, EcsPlayer);
    ECS_META(world, EcsPlayerMode);
    ECS_META(world, EcsTargetFps);

    ECS_PIPELINE(world, StopPipeline, 
        flecs.pipeline.PreFrame, 
        flecs.pipeline.OnLoad, 
        flecs.pipeline.PostLoad, 
        flecs.pipeline.PreStore, 
        flecs.pipeline.OnStore, 
        flecs.pipeline.PostFrame);

    ECS_SYSTEM(world, EcsSetPlayer, EcsOnSet, EcsPlayer, :EcsPlayerMode);
    ECS_SYSTEM(world, EcsSetPlayerMode, EcsOnSet, EcsPlayerMode);

    ECS_ENTITY(world, mode, 0);

    ecs_set_scope(world, mode);

    ECS_ENTITY(world, play, 0);
    ECS_ENTITY(world, pause, 0);
    ECS_ENTITY(world, stop, 0);

    ecs_set(world, play, EcsPlayerMode, {
        .time_scale = 1,
        .pipeline = ecs_get_pipeline(world),
        .scope = 0,
        .snapshot_mode = EcsPlayerSnapshotModeTake
    });

    ecs_set(world, pause, EcsPlayerMode, {
        .time_scale = 0,
        .pipeline = 0,
        .scope = 0,
        .snapshot_mode = EcsPlayerSnapshotModeIgnore
    });

    ecs_set(world, stop, EcsPlayerMode, {
        .time_scale = 0,
        .pipeline = StopPipeline,
        .scope = 0,
        .snapshot_mode = EcsPlayerSnapshotModeRestore
    });

    ecs_set(world, EcsWorld, EcsPlayer, {
        .mode = play,
        .prev_mode = stop
    });

    ECS_EXPORT_COMPONENT(EcsPlayer);
    ECS_EXPORT_COMPONENT(EcsPlayerMode);
    ECS_EXPORT_COMPONENT(EcsTargetFps);
}
