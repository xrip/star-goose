/* Смена уровней, respawn, вход/выход туннеля
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * reset_attract_entities  |  CS:0D98
 * Назначение: Сбрасывает камеру и флаг смерти, очищает пул для заставки.
 * Вход: Нет аргументов.
 * Эффект/ограничения: camera_y=26; не загружает новый уровень.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_attract_entities, @function
reset_attract_entities:
    mov WORD PTR ds:[D_camera_y], 0x1a                               # CS:0D98
    mov BYTE PTR ds:[D_player_respawn_pending], 0x0                  # CS:0D9E
    call clear_entities                                              # CS:0DA3
    ret                                                              # CS:0DA6

/* ========================================================================== 
 * start_level_state  |  CS:0DA7
 * Назначение: Сбрасывает маски объектов, готовит карту и состояние игрока.
 * Вход: level_index/part уже выбраны.
 * Эффект/ограничения: Обнуляет запросы восстановления/боковых устройств.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type start_level_state, @function
start_level_state:
    call reset_map_object_masks                                      # CS:0DA7
    call prepare_level_maps                                          # CS:0DAA
    mov BYTE PTR ds:[D_respawn_effect_pending], 0x0                  # CS:0DAD
    mov BYTE PTR ds:[D_left_device_state], 0x0                       # CS:0DB2
    mov BYTE PTR ds:[D_right_device_state], 0x0                      # CS:0DB7
    call reset_player_motion                                         # CS:0DBC
    ret                                                              # CS:0DBF

/* ========================================================================== 
 * advance_level  |  CS:0DC0
 * Назначение: Увеличивает номер пройденного этапа и базовую скорость, циклически переключает level_index 1..7.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Сброс масок, новая карта, игрок, шесть collectible-индикаторов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type advance_level, @function
advance_level:
    inc BYTE PTR ds:[D_stage_counter]                                # CS:0DC0
    mov al, ds:[D_stage_counter]                                     # CS:0DC4
    cmp al, 0x6                                                      # CS:0DC7
    jb L_advance_level_0dd3                                          # CS:0DC9
    mov WORD PTR ds:[D_base_scroll_speed], 0x30                      # CS:0DCB
    jmp L_advance_level_0de5                                         # CS:0DD1
L_advance_level_0dd3:
    cmp al, 0x3                                                      # CS:0DD3
    jb L_advance_level_0ddf                                          # CS:0DD5
    mov WORD PTR ds:[D_base_scroll_speed], 0x20                      # CS:0DD7
    jmp L_advance_level_0de5                                         # CS:0DDD
L_advance_level_0ddf:
    mov WORD PTR ds:[D_base_scroll_speed], 0x10                      # CS:0DDF
L_advance_level_0de5:
    inc BYTE PTR ds:[D_level_index]                                  # CS:0DE5
    mov al, ds:[D_level_index]                                       # CS:0DE9
    cmp al, 0x7                                                      # CS:0DEC
    jbe L_advance_level_0df5                                         # CS:0DEE
    mov BYTE PTR ds:[D_level_index], 0x1                             # CS:0DF0
L_advance_level_0df5:
    call reset_map_object_masks                                      # CS:0DF5
    call prepare_level_maps                                          # CS:0DF8
    mov BYTE PTR ds:[D_respawn_effect_pending], 0x0                  # CS:0DFB
    call reset_player_motion                                         # CS:0E00
    call reset_collectible_hud                                       # CS:0E03
    ret                                                              # CS:0E06

/* ========================================================================== 
 * handle_player_respawn  |  CS:0E07
 * Назначение: Завершает игру при нуле жизней; иначе расходует жизнь и восстанавливает игрока.
 * Вход: player_respawn_pending != 0.
 * Эффект/ограничения: При возврате к part 0 заново готовит карту и HUD-ресурсы.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type handle_player_respawn, @function
handle_player_respawn:
    mov ax, ds:[D_life_count]                                        # CS:0E07
    ACC16 cmp, 0x0                                                   # CS:0E0A
    jne L_handle_player_respawn_0e15                                 # CS:0E0D
    mov BYTE PTR ds:[D_game_loop_end_flag], 0x1                      # CS:0E0F
    ret                                                              # CS:0E14
L_handle_player_respawn_0e15:
    call remove_life                                                 # CS:0E15
    mov al, ds:[D_level_part]                                        # CS:0E18
    cmp al, 0x0                                                      # CS:0E1B
    je L_handle_player_respawn_0e27                                  # CS:0E1D
    mov BYTE PTR ds:[D_level_part], 0x0                              # CS:0E1F
    call prepare_level_maps                                          # CS:0E24
L_handle_player_respawn_0e27:
    call reset_hud_resources                                         # CS:0E27
    mov BYTE PTR ds:[D_respawn_effect_pending], 0x1                  # CS:0E2A
    mov BYTE PTR ds:[D_left_device_state], 0x0                       # CS:0E2F
    mov BYTE PTR ds:[D_right_device_state], 0x0                      # CS:0E34
    call reset_player_motion                                         # CS:0E39
    ret                                                              # CS:0E3C

/* ========================================================================== 
 * reset_player_motion  |  CS:0E3D
 * Назначение: Задаёт камеру из таблицы старта, скорость и исходные счётчики игрока.
 * Вход: level_index, base_scroll_speed.
 * Эффект/ограничения: Затем заново формирует пул видимой области.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_player_motion, @function
reset_player_motion:
    mov bl, BYTE PTR ds:[D_level_index]                              # CS:0E3D
    mov bh, 0x0                                                      # CS:0E41
    shl bx, 1                                                        # CS:0E43
    mov ax, WORD PTR [bx+D_level_start_y]                            # CS:0E45
    ACC16 and, WORLD_Y_MASK                                          # CS:0E49
    mov ds:[D_camera_y], ax                                          # CS:0E4C
    mov ax, ds:[D_base_scroll_speed]                                 # CS:0E4F
    mov ds:[D_scroll_speed], ax                                      # CS:0E52
    mov WORD PTR ds:[D_surface_resume_x], 0x80                       # CS:0E55
    mov BYTE PTR ds:[D_map_reload_pending], 0x0                      # CS:0E5B
    mov WORD PTR ds:[D_fuel_distance_accumulator], 0x0               # CS:0E60
    mov WORD PTR ds:[D_ammo_use_accumulator], 0x0                    # CS:0E66
    mov BYTE PTR ds:[D_player_respawn_pending], 0x0                  # CS:0E6C
    mov BYTE PTR ds:[D_player_zero_frame_age], 0x0                   # CS:0E71
    mov BYTE PTR ds:[D_side_impact_mask], 0x0                        # CS:0E76
    mov BYTE PTR ds:[D_player_forced_stop], 0x0                      # CS:0E7B
    call rebuild_surface_entities                                    # CS:0E80
    ret                                                              # CS:0E83

/* ========================================================================== 
 * rebuild_surface_entities  |  CS:0E84
 * Назначение: Пересоздаёт игрока и 15 строк объектов, затем боковые устройства и специальные состояния.
 * Вход: camera_y, respawn/side flags.
 * Эффект/ограничения: Пул очищается; игрок занимает первый слот. Некоторые маршруты возвращаются раньше общей инициализации.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type rebuild_surface_entities, @function
rebuild_surface_entities:
    mov al, ds:[D_map_reload_pending]                                # CS:0E84
    cmp al, 0x0                                                      # CS:0E87
    je L_rebuild_surface_entities_0e93                               # CS:0E89
    mov BYTE PTR ds:[D_map_reload_pending], 0x0                      # CS:0E8B
    call prepare_level_maps                                          # CS:0E90
L_rebuild_surface_entities_0e93:
    mov WORD PTR ds:[D_player_x_velocity], 0x0                       # CS:0E93
    call clear_entities                                              # CS:0E99
    mov si, OFFSET D_template_surface_player                         # CS:0E9C
    call allocate_entity_copy                                        # CS:0E9F
    mov ax, ds:[D_surface_resume_x]                                  # CS:0EA2
    mov ds:[D_player_x], ax                                          # CS:0EA5
    call sync_player_to_terrain                                      # CS:0EA8
    mov ax, ds:[D_camera_y]                                          # CS:0EAB
    shr ax, 1                                                        # CS:0EAE
    shr ax, 1                                                        # CS:0EB0
    shr ax, 1                                                        # CS:0EB2
    shr ax, 1                                                        # CS:0EB4
    ACC16 sub, 0x3                                                   # CS:0EB6
    ACC16 and, 0x3f                                                  # CS:0EB9
    mov ds:[D_last_spawn_row], ax                                    # CS:0EBC
    mov cx, 0xf                                                      # CS:0EBF
L_rebuild_surface_entities_0ec2:
    push cx                                                          # CS:0EC2
    push ax                                                          # CS:0EC3
    push ax                                                          # CS:0EC4
    call spawn_map_row                                               # CS:0EC5
    pop ax                                                           # CS:0EC8
    pop cx                                                           # CS:0EC9
    inc ax                                                           # CS:0ECA
    ACC16 and, 0x3f                                                  # CS:0ECB
    loop L_rebuild_surface_entities_0ec2                             # CS:0ECE
    mov al, ds:[D_player_forced_stop]                                # CS:0ED0
    cmp al, 0x0                                                      # CS:0ED3
    je L_rebuild_surface_entities_0eea                               # CS:0ED5
    mov si, OFFSET D_entity_records                                  # CS:0ED7
    mov bx, OFFSET D_sprite_frames_0d49                              # CS:0EDA
    mov BYTE PTR [si+E_FRAME], 0xa                                   # CS:0EDD
    call apply_sprite_frame                                          # CS:0EE1
    mov BYTE PTR ds:[D_player_handler], 0x23                         # CS:0EE4
    ret                                                              # CS:0EE9
L_rebuild_surface_entities_0eea:
    mov al, ds:[D_respawn_effect_pending]                            # CS:0EEA
    cmp al, 0x0                                                      # CS:0EED
    je L_rebuild_surface_entities_0f08                               # CS:0EEF
    mov BYTE PTR ds:[D_respawn_effect_pending], 0x0                  # CS:0EF1
    mov si, OFFSET D_template_respawn_effect                         # CS:0EF6
    mov ax, ds:[D_player_y]                                          # CS:0EF9
    mov WORD PTR [si+E_Y], ax                                        # CS:0EFC
    mov al, ds:[D_player_z]                                          # CS:0EFF
    mov BYTE PTR [si+E_Z], al                                        # CS:0F02
    call allocate_entity_copy                                        # CS:0F05
L_rebuild_surface_entities_0f08:
    call reset_player_history                                        # CS:0F08
    mov al, ds:[D_left_device_state]                                 # CS:0F0B
    cmp al, 0x0                                                      # CS:0F0E
    je L_rebuild_surface_entities_0f23                               # CS:0F10
    mov BYTE PTR ds:[D_left_device_state], 0x1                       # CS:0F12
    mov si, OFFSET D_template_side_device                            # CS:0F17
    and BYTE PTR [si+E_FLAGS], 0xdf                                  # CS:0F1A
    call position_side_device                                        # CS:0F1D
    call allocate_entity_copy                                        # CS:0F20
L_rebuild_surface_entities_0f23:
    mov al, ds:[D_right_device_state]                                # CS:0F23
    cmp al, 0x0                                                      # CS:0F26
    je L_rebuild_surface_entities_0f3b                               # CS:0F28
    mov BYTE PTR ds:[D_right_device_state], 0x1                      # CS:0F2A
    mov si, OFFSET D_template_side_device                            # CS:0F2F
    or BYTE PTR [si+E_FLAGS], 0x20                                   # CS:0F32
    call position_side_device                                        # CS:0F35
    call allocate_entity_copy                                        # CS:0F38
L_rebuild_surface_entities_0f3b:
    ret                                                              # CS:0F3B

/* ========================================================================== 
 * enter_tunnel_mode  |  CS:0F3C
 * Назначение: Инициализирует туннель, дистанцию, угловую динамику и новый пул сущностей.
 * Вход: tunnel mode выбран вызывающей стороной.
 * Эффект/ограничения: Использует другие шаблоны игрока и боковых устройств.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type enter_tunnel_mode, @function
enter_tunnel_mode:
    mov BYTE PTR ds:[D_tunnel_flash_state], 0x0                      # CS:0F3C
    call select_tunnel_pattern                                       # CS:0F41
    mov WORD PTR ds:[D_camera_y], 0x1a                               # CS:0F44
    mov WORD PTR ds:[D_tunnel_distance], 0x0                         # CS:0F4A
    mov WORD PTR ds:[D_tunnel_angular_velocity], 0x0                 # CS:0F50
    mov BYTE PTR ds:[D_tunnel_exit_spawned], 0x0                     # CS:0F56
    mov BYTE PTR ds:[D_tunnel_min_radius], 0x3                       # CS:0F5B
    call clear_entities                                              # CS:0F60
    mov si, OFFSET D_template_tunnel_player                          # CS:0F63
    call allocate_entity_copy                                        # CS:0F66
    call reset_tunnel_angle_history                                  # CS:0F69
    mov al, ds:[D_left_device_state]                                 # CS:0F6C
    cmp al, 0x0                                                      # CS:0F6F
    je L_enter_tunnel_mode_0f84                                      # CS:0F71
    mov BYTE PTR ds:[D_left_device_state], 0x1                       # CS:0F73
    mov si, OFFSET D_template_tunnel_side                            # CS:0F78
    and BYTE PTR [si+E_FLAGS], 0xdf                                  # CS:0F7B
    call update_tunnel_side_device                                   # CS:0F7E
    call allocate_entity_copy                                        # CS:0F81
L_enter_tunnel_mode_0f84:
    mov al, ds:[D_right_device_state]                                # CS:0F84
    cmp al, 0x0                                                      # CS:0F87
    je L_enter_tunnel_mode_0f9c                                      # CS:0F89
    mov BYTE PTR ds:[D_right_device_state], 0x1                      # CS:0F8B
    mov si, OFFSET D_template_tunnel_side                            # CS:0F90
    or BYTE PTR [si+E_FLAGS], 0x20                                   # CS:0F93
    call update_tunnel_side_device                                   # CS:0F96
    call allocate_entity_copy                                        # CS:0F99
L_enter_tunnel_mode_0f9c:
    ret                                                              # CS:0F9C

/* ========================================================================== 
 * reset_player_history  |  CS:0F9D
 * Назначение: Заполняет четыре сохранённых позиции/кадра/высоты текущими значениями игрока.
 * Вход: Глобальная запись player.
 * Эффект/ограничения: История используется боковыми устройствами с задержкой.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_player_history, @function
reset_player_history:
    mov ax, ds:[D_player_x]                                          # CS:0F9D
    mov ds:[D_player_history_x0], ax                                 # CS:0FA0
    mov ds:[D_player_history_x1], ax                                 # CS:0FA3
    mov ds:[D_player_history_x2], ax                                 # CS:0FA6
    mov ds:[D_player_history_x3], ax                                 # CS:0FA9
    mov ax, ds:[D_player_y]                                          # CS:0FAC
    mov ds:[D_player_history_y0], ax                                 # CS:0FAF
    mov ds:[D_player_history_y1], ax                                 # CS:0FB2
    mov ds:[D_player_history_y2], ax                                 # CS:0FB5
    mov ds:[D_player_history_y3], ax                                 # CS:0FB8
    mov al, ds:[D_player_frame]                                      # CS:0FBB
    mov ds:[D_player_history_frame0], al                             # CS:0FBE
    mov ds:[D_player_history_frame1], al                             # CS:0FC1
    mov ds:[D_player_history_frame2], al                             # CS:0FC4
    mov ds:[D_player_history_frame3], al                             # CS:0FC7
    mov al, ds:[D_player_z]                                          # CS:0FCA
    mov ds:[D_player_history_z0], al                                 # CS:0FCD
    mov ds:[D_player_history_z1], al                                 # CS:0FD0
    mov ds:[D_player_history_z2], al                                 # CS:0FD3
    mov ds:[D_player_history_z3], al                                 # CS:0FD6
    ret                                                              # CS:0FD9
