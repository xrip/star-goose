/* Таблица35 обработчиков и culling
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * update_entities  |  CS:0FDA
 * Назначение: Обходит пул по 23 байта и вызывает handler_id-1 для записей с UPDATE; обрабатывает смерть и смену режима.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: SI сохраняется вокруг handler. После смены режима обход начинается заново; handler_id=0 пропускается.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_entities, @function
update_entities:

    # Проход может быть начат заново после смерти или смены surface/tunnel режима.
    mov al, ds:[D_tunnel_mode]                                       # CS:0FDA
    mov ds:[D_dispatch_mode_snapshot], al                            # CS:0FDD
    mov si, OFFSET D_entity_records                                  # CS:0FE0
L_update_check_world_transition:
    mov al, ds:[D_player_respawn_pending]                            # CS:0FE3
    cmp al, 0x0                                                      # CS:0FE6
    je L_update_check_mode_change                                    # CS:0FE8
    call handle_player_respawn                                       # CS:0FEA
    mov al, ds:[D_game_loop_end_flag]                                # CS:0FED
    cmp al, 0x0                                                      # CS:0FF0
    je update_entities                                               # CS:0FF2
    ret                                                              # CS:0FF4
L_update_check_mode_change:
    mov al, ds:[D_tunnel_mode]                                       # CS:0FF5
    cmp al, BYTE PTR ds:[D_dispatch_mode_snapshot]                   # CS:0FF8
    je L_update_dispatch_slot                                        # CS:0FFC
    cmp al, 0x1                                                      # CS:0FFE
    jne L_update_entities_100e                                       # CS:1000
    mov al, 0xb                                                      # CS:1002
    call sound_trigger                                               # CS:1004
    call enter_tunnel_mode                                           # CS:1007
    jmp update_entities                                              # CS:100A
    jmp L_update_dispatch_slot                                       # CS:100C | Не достигнуто текущим статическим CFG; не удалять.
L_update_entities_100e:
    mov al, 0xb                                                      # CS:100E
    call sound_trigger                                               # CS:1010
    mov al, ds:[D_tunnel_reward_kind]                                # CS:1013
    cmp al, 0xf0                                                     # CS:1016
    jne L_update_entities_101f                                       # CS:1018
    call advance_level                                               # CS:101A
    jmp L_update_entities_1034                                       # CS:101D
L_update_entities_101f:
    mov al, ds:[D_player_angle_or_age]                               # CS:101F
    and al, 0x7f                                                     # CS:1022
    cmp al, 0x28                                                     # CS:1024
    jbe L_update_entities_1031                                       # CS:1026
    cmp al, 0x58                                                     # CS:1028
    jae L_update_entities_1031                                       # CS:102A
    mov BYTE PTR ds:[D_player_forced_stop], 0x1                      # CS:102C
L_update_entities_1031:
    call rebuild_surface_entities                                    # CS:1031
L_update_entities_1034:
    jmp update_entities                                              # CS:1034
L_update_dispatch_slot:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1036
    test al, EF_SENTINEL                                                    # CS:1038
    jne L_update_done                                                # CS:103A
    test al, EF_UPDATE                                                     # CS:103C
    je L_update_next_slot                                            # CS:103E

    # Сохраняем SI текущего слота: handler может создавать объекты и возвращать другой SI.
    push si                                                          # CS:1040
    mov bl, BYTE PTR [si+E_HANDLER]                                  # CS:1041
    cmp bl, 0x0                                                      # CS:1044
    je L_update_restore_slot                                         # CS:1047
    mov bh, 0x0                                                      # CS:1049
    dec bx                                                           # CS:104B
    shl bx, 1                                                        # CS:104C

    # Косвенный near CALL: id1..35 → table[id-1]. ID0 уже отсеян; верхней проверки здесь нет.
    call WORD PTR cs:[bx+CS_entity_handlers]                         # CS:104E | 35-entry near-call table; entity handler id is converted to a word index
L_update_restore_slot:
    pop si                                                           # CS:1053
L_update_next_slot:

    # Созданный далее по пулу объект может обновиться в этом же кадре; предшествующий слот — нет.
    add si, ENTITY_SIZE                                              # CS:1054
    jmp L_update_check_world_transition                              # CS:1057
L_update_done:
    ret                                                              # CS:1059

/* CS:105A: 35 near offsets selected by CALL CS:[BX+105Ah]. */
entity_handlers:
    .word update_player - image_start    # handler id  1; original CS:1108
    .word update_looping_anim4 - image_start    # handler id  2; original CS:176C
    .word update_pingpong_animation - image_start    # handler id  3; original CS:1783
    .word cull_entity_after_view - image_start    # handler id  4; original CS:10C6
    .word update_aimed_emitter_a - image_start    # handler id  5; original CS:17AC
    .word update_aimed_emitter_b - image_start    # handler id  6; original CS:17D6
    .word update_aimed_emitter_c - image_start    # handler id  7; original CS:18A9
    .word update_refill_gate - image_start    # handler id  8; original CS:1A9E
    .word cull_entity_after_view - image_start    # handler id  9; original CS:10C6
    .word update_tunnel_entrance_a - image_start    # handler id 10; original CS:1993
    .word update_tunnel_entrance_b - image_start    # handler id 11; original CS:19C2
    .word update_tunnel_player - image_start    # handler id 12; original CS:2E02
    .word cull_entity_after_view - image_start    # handler id 13; original CS:10C6
    .word update_tunnel_exit_marker - image_start    # handler id 14; original CS:2FE8
    .word update_tunnel_pickup - image_start    # handler id 15; original CS:300A
    .word update_five_frame_explosion - image_start    # handler id 16; original CS:16FE
    .word update_player_death - image_start    # handler id 17; original CS:12CB
    .word cull_entity_after_view - image_start    # handler id 18; original CS:10C6
    .word update_player_projectile - image_start    # handler id 19; original CS:12EB
    .word update_enemy_projectile - image_start    # handler id 20; original CS:15FC
    .word update_four_frame_impact - image_start    # handler id 21; original CS:16DF
    .word update_countdown_effect - image_start    # handler id 22; original CS:16D6
    .word update_side_impact - image_start    # handler id 23; original CS:1740
    .word update_side_device_follow - image_start    # handler id 24; original CS:1384
    .word update_ground_projectile - image_start    # handler id 25; original CS:13A8
    .word update_mobile_enemy - image_start    # handler id 26; original CS:14F0
    .word update_accelerating_effect - image_start    # handler id 27; original CS:10F3
    .word update_level_exit_gate - image_start    # handler id 28; original CS:18C7
    .word update_ground_enemy_shot - image_start    # handler id 29; original CS:1429
    .word update_arcing_projectile - image_start    # handler id 30; original CS:17F4
    .word update_six_frame_effect - image_start    # handler id 31; original CS:171F
    .word update_collectible - image_start    # handler id 32; original CS:1B09
    .word update_drifting_effect - image_start    # handler id 33; original CS:1B4A
    .word update_tunnel_side_device - image_start    # handler id 34; original CS:2F50
    .word update_player_forced_stop - image_start    # handler id 35; original CS:12A7

/* ========================================================================== 
 * cull_entity_before_camera  |  CS:10A0
 * Назначение: Удаляет сущность, если нижний край попал в невидимую половину кольца относительно камеры.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Работает с кольцом 1024 и порогом 512, а не с простым signed y<0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type cull_entity_before_camera, @function
cull_entity_before_camera:

    # Culling снизу: сравнение JBE (включая512) отличается от второго culling helper.
    mov al, BYTE PTR [si+E_HEIGHT]                                   # CS:10A0
    sub al, BYTE PTR [si+E_Z]                                        # CS:10A3
    sub al, BYTE PTR [si+E_ANCHOR_Y]                                 # CS:10A6
    cbw                                                              # CS:10A9
    add ax, WORD PTR [si+E_Y]                                        # CS:10AA
    ACC16 and, WORLD_Y_MASK                                          # CS:10AD
    mov bx, WORD PTR ds:[D_camera_y]                                 # CS:10B0
    {load} cmp ax, bx                                                # CS:10B4
    jae L_cull_entity_before_camera_10bb                             # CS:10B6
    ACC16 add, 0x400                                                 # CS:10B8
L_cull_entity_before_camera_10bb:
    {load} sub ax, bx                                                # CS:10BB
    ACC16 cmp, 0x200                                                 # CS:10BD
    jbe L_cull_entity_before_camera_10c5                             # CS:10C0
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:10C2
L_cull_entity_before_camera_10c5:
    ret                                                              # CS:10C5

/* ========================================================================== 
 * cull_entity_after_view  |  CS:10C6
 * Назначение: Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Общий обработчик ID 4,9,13,18; это не четыре разных алгоритма.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type cull_entity_after_view, @function
cull_entity_after_view:

    # Culling сверху: используется JAE. Не объединять обе границы в симметричный тест без проверки.
    mov al, BYTE PTR [si+E_Z]                                        # CS:10C6
    add al, BYTE PTR [si+E_ANCHOR_Y]                                 # CS:10C9
    cbw                                                              # CS:10CC
    neg ax                                                           # CS:10CD
    add ax, WORD PTR [si+E_Y]                                        # CS:10CF
    ACC16 and, WORLD_Y_MASK                                          # CS:10D2
    mov bx, WORD PTR ds:[D_camera_y]                                 # CS:10D5
    add bx, 0xa0                                                     # CS:10D9
    and bx, 0x3ff                                                    # CS:10DD
    {load} cmp ax, bx                                                # CS:10E1
    jae L_cull_entity_after_view_10e8                                # CS:10E3
    ACC16 add, 0x400                                                 # CS:10E5
L_cull_entity_after_view_10e8:
    {load} sub ax, bx                                                # CS:10E8
    ACC16 cmp, 0x200                                                 # CS:10EA
    jae L_cull_entity_after_view_10f2                                # CS:10ED
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:10EF
L_cull_entity_after_view_10f2:
    ret                                                              # CS:10F2

/* ========================================================================== 
 * update_accelerating_effect  |  CS:10F3
 * Назначение: Увеличивает state_16 и двигает y назад на min(counter,8), затем проверяет видимость.
 * Вход: DS:SI = сущность; handler ID 27.
 * Эффект/ограничения: Счётчик байтовый, проверка unsigned; его wrap не заменять бесконечным saturating счётчиком.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_accelerating_effect, @function
update_accelerating_effect:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:10F3
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:10F6
    cmp al, 0x8                                                      # CS:10F9
    jbe L_update_accelerating_effect_10ff                            # CS:10FB
    mov al, 0x8                                                      # CS:10FD
L_update_accelerating_effect_10ff:
    mov ah, 0x0                                                      # CS:10FF
    sub WORD PTR [si+E_Y], ax                                        # CS:1101
    call cull_entity_before_camera                                   # CS:1104
    ret                                                              # CS:1107
