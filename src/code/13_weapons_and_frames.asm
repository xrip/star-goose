/* Оружие, столкновения сущностей, кадры и прицеливание
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * fire_player_weapons  |  CS:1D1E
 * Назначение: Создаёт пару эффектов стволов и пару выстрелов; использует циклическую таблицу вариаций.
 * Вход: Глобальная запись player.
 * Эффект/ограничения: Расход ammo накапливается отдельно; сохранять наблюдаемую зависимость от BX после вызовов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type fire_player_weapons, @function
fire_player_weapons:

    # Выстрелы используют изменяемые шаблоны и циклическую таблицу вариаций, а не генератор случайных чисел.
    mov si, OFFSET D_template_player_shot                            # CS:1D1E
    and BYTE PTR [si+E_FLAGS], 0xdf                                  # CS:1D21
    mov bl, BYTE PTR ds:[D_player_frame]                             # CS:1D24
    mov bh, 0x0                                                      # CS:1D28
    mov al, BYTE PTR [bx+D_player_shot_frame_map]                    # CS:1D2A
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1D2E
    mov al, BYTE PTR [bx+D_player_left_muzzle_dx]                    # CS:1D31
    cbw                                                              # CS:1D35
    add ax, WORD PTR ds:[D_player_x]                                 # CS:1D36
    mov WORD PTR [si+E_X], ax                                        # CS:1D3A
    mov ax, ds:[D_player_y]                                          # CS:1D3D
    ACC16 sub, 0x4                                                   # CS:1D40
    ACC16 and, WORLD_Y_MASK                                          # CS:1D43
    mov WORD PTR [si+E_Y], ax                                        # CS:1D46
    mov al, ds:[D_player_z]                                          # CS:1D49
    add al, BYTE PTR [bx+D_player_left_muzzle_dz]                    # CS:1D4C
    mov BYTE PTR [si+E_Z], al                                        # CS:1D50
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1D53
    mov bh, 0x0                                                      # CS:1D56
    mov al, BYTE PTR [bx+D_shot_variant_heights]                     # CS:1D58
    mov BYTE PTR [si+E_HEIGHT], al                                   # CS:1D5C
    shl bx, 1                                                        # CS:1D5F
    mov ax, WORD PTR [bx+D_player_shot_sprite_offsets]               # CS:1D61
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:1D65
    call allocate_entity_copy                                        # CS:1D68
    mov si, OFFSET D_template_muzzle_effect                          # CS:1D6B
    mov ax, ds:[D_player_shot_template_x]                            # CS:1D6E
    mov WORD PTR [si+E_X], ax                                        # CS:1D71
    mov ax, ds:[D_player_shot_template_y]                            # CS:1D74
    mov WORD PTR [si+E_Y], ax                                        # CS:1D77
    mov al, ds:[D_player_shot_template_z]                            # CS:1D7A
    mov BYTE PTR [si+E_Z], al                                        # CS:1D7D
    inc BYTE PTR ds:[D_shot_variation_index]                         # CS:1D80
    mov bl, BYTE PTR ds:[D_shot_variation_index]                     # CS:1D84
    and bx, 0xf                                                      # CS:1D88
    mov al, BYTE PTR [bx+D_shot_variation_sequence]                  # CS:1D8B
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1D8F
    mov bx, OFFSET D_sprite_frames_100e                              # CS:1D92
    call apply_sprite_frame                                          # CS:1D95
    call allocate_entity_copy                                        # CS:1D98
    mov si, OFFSET D_template_player_shot                            # CS:1D9B
    or BYTE PTR [si+E_FLAGS], 0x20                                   # CS:1D9E
    mov bl, BYTE PTR ds:[D_player_frame]                             # CS:1DA1
    mov bh, 0x0                                                      # CS:1DA5
    mov al, BYTE PTR [bx+D_player_right_muzzle_dx]                   # CS:1DA7
    cbw                                                              # CS:1DAB
    add ax, WORD PTR ds:[D_player_x]                                 # CS:1DAC
    mov WORD PTR [si+E_X], ax                                        # CS:1DB0
    mov al, ds:[D_player_z]                                          # CS:1DB3
    add al, BYTE PTR [bx+D_player_right_muzzle_dz]                   # CS:1DB6
    mov BYTE PTR [si+E_Z], al                                        # CS:1DBA
    call allocate_entity_copy                                        # CS:1DBD
    mov si, OFFSET D_template_muzzle_effect                          # CS:1DC0
    mov ax, ds:[D_player_shot_template_x]                            # CS:1DC3
    mov WORD PTR [si+E_X], ax                                        # CS:1DC6
    mov ax, ds:[D_player_shot_template_y]                            # CS:1DC9
    mov WORD PTR [si+E_Y], ax                                        # CS:1DCC
    mov al, ds:[D_player_shot_template_z]                            # CS:1DCF
    mov BYTE PTR [si+E_Z], al                                        # CS:1DD2
    call allocate_entity_copy                                        # CS:1DD5
    mov ax, ds:[D_ammo_use_accumulator]                              # CS:1DD8
    {load} add ax, bx                                                # CS:1DDB
    ACC16 cmp, 0x10                                                  # CS:1DDD
    jbe L_fire_player_weapons_1deb                                   # CS:1DE0
    mov ax, 0x0                                                      # CS:1DE2
    mov WORD PTR ds:[D_ammo_delta], 0xffff                           # CS:1DE5
L_fire_player_weapons_1deb:
    mov ds:[D_ammo_use_accumulator], ax                              # CS:1DEB
    ret                                                              # CS:1DEE

/* ========================================================================== 
 * fire_mobile_enemy_pattern  |  CS:1DEF
 * Назначение: Выбирает 1,2 или 3 точки выстрела по param и frame врага.
 * Вход: DS:SI = emitter.
 * Эффект/ограничения: Сам вызывает spawn_enemy_shot_at_offset с разными dx/dy/dz.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type fire_mobile_enemy_pattern, @function
fire_mobile_enemy_pattern:
    inc BYTE PTR ds:[D_shot_variation_index]                         # CS:1DEF
    mov bl, BYTE PTR ds:[D_shot_variation_index]                     # CS:1DF3
    and bx, 0xf                                                      # CS:1DF7
    mov al, BYTE PTR [bx+D_shot_variation_sequence]                  # CS:1DFA
    mov ds:[D_shot_effect_template_frame], al                        # CS:1DFE
    mov al, BYTE PTR [si+E_PARAM]                                    # CS:1E01
    cmp al, 0x0                                                      # CS:1E04
    jne L_fire_mobile_enemy_pattern_1e2e                             # CS:1E06
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1E08
    mov bh, 0x0                                                      # CS:1E0B
    mov cl, BYTE PTR [bx+D_enemy_muzzle_a_left_dz]                   # CS:1E0D
    mov ax, 0xfffd                                                   # CS:1E11
    mov bx, 0x8                                                      # CS:1E14
    call spawn_enemy_shot_at_offset                                  # CS:1E17
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1E1A
    mov bh, 0x0                                                      # CS:1E1D
    mov cl, BYTE PTR [bx+D_enemy_muzzle_a_right_dz]                  # CS:1E1F
    mov ax, 0x3                                                      # CS:1E23
    mov bx, 0x8                                                      # CS:1E26
    call spawn_enemy_shot_at_offset                                  # CS:1E29
    jmp L_fire_mobile_enemy_pattern_1e7c                             # CS:1E2C
L_fire_mobile_enemy_pattern_1e2e:
    cmp al, 0x1                                                      # CS:1E2E
    jne L_fire_mobile_enemy_pattern_1e46                             # CS:1E30
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1E32
    mov bh, 0x0                                                      # CS:1E35
    mov cl, BYTE PTR [bx+D_enemy_muzzle_b_dz]                        # CS:1E37
    mov ax, 0x0                                                      # CS:1E3B
    mov bx, 0x8                                                      # CS:1E3E
    call spawn_enemy_shot_at_offset                                  # CS:1E41
    jmp L_fire_mobile_enemy_pattern_1e7c                             # CS:1E44
L_fire_mobile_enemy_pattern_1e46:
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1E46
    mov bh, 0x0                                                      # CS:1E49
    mov cl, BYTE PTR [bx+D_enemy_muzzle_c_left_dz]                   # CS:1E4B
    mov ax, 0xfff9                                                   # CS:1E4F
    mov bx, 0x8                                                      # CS:1E52
    call spawn_enemy_shot_at_offset                                  # CS:1E55
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1E58
    mov bh, 0x0                                                      # CS:1E5B
    mov cl, BYTE PTR [bx+D_enemy_muzzle_c_center_dz]                 # CS:1E5D
    mov ax, 0x0                                                      # CS:1E61
    mov bx, 0xb                                                      # CS:1E64
    call spawn_enemy_shot_at_offset                                  # CS:1E67
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1E6A
    mov bh, 0x0                                                      # CS:1E6D
    mov cl, BYTE PTR [bx+D_enemy_muzzle_c_right_dz]                  # CS:1E6F
    mov ax, 0x7                                                      # CS:1E73
    mov bx, 0x8                                                      # CS:1E76
    call spawn_enemy_shot_at_offset                                  # CS:1E79
L_fire_mobile_enemy_pattern_1e7c:
    ret                                                              # CS:1E7C

/* ========================================================================== 
 * spawn_enemy_shot_at_offset  |  CS:1E7D
 * Назначение: Создаёт эффект и, при gate, снаряд с заданным смещением от emitter.
 * Вход: DS:SI=emitter; AX=dx; BX=dy; CL=dz.
 * Эффект/ограничения: Сохраняет SI. Если новый слот расположен раньше emitter, сразу вызывает его update.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_enemy_shot_at_offset, @function
spawn_enemy_shot_at_offset:
    push si                                                          # CS:1E7D
    mov di, 0xff7                                                    # CS:1E7E
    add ax, WORD PTR [si+E_X]                                        # CS:1E81
    mov WORD PTR [di+E_X], ax                                        # CS:1E84
    add bx, WORD PTR [si+E_Y]                                        # CS:1E87
    and bx, 0x3ff                                                    # CS:1E8A
    mov WORD PTR [di+E_Y], bx                                        # CS:1E8E
    add cl, BYTE PTR [si+E_Z]                                        # CS:1E91
    mov BYTE PTR [di+E_Z], cl                                        # CS:1E94
    {load} mov si, di                                                # CS:1E97
    mov bx, OFFSET D_sprite_frames_1039                              # CS:1E99
    call apply_sprite_frame                                          # CS:1E9C
    call allocate_entity_copy                                        # CS:1E9F
    cmp si, 0x0                                                      # CS:1EA2
    je L_spawn_enemy_shot_at_offset_1eb0                             # CS:1EA5
    pop ax                                                           # CS:1EA7
    push ax                                                          # CS:1EA8
    {load} cmp ax, si                                                # CS:1EA9
    jbe L_spawn_enemy_shot_at_offset_1eb0                            # CS:1EAB
    call update_countdown_effect                                     # CS:1EAD
L_spawn_enemy_shot_at_offset_1eb0:
    pop si                                                           # CS:1EB0
    mov al, ds:[D_spawn_projectile_gate]                             # CS:1EB1
    cmp al, 0x0                                                      # CS:1EB4
    jne L_spawn_enemy_shot_at_offset_1eb9                            # CS:1EB6
    ret                                                              # CS:1EB8
L_spawn_enemy_shot_at_offset_1eb9:
    push si                                                          # CS:1EB9
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1EBA
    mov bh, 0x0                                                      # CS:1EBD
    mov si, OFFSET D_template_enemy_shot                             # CS:1EBF
    mov al, BYTE PTR [bx+D_player_shot_frame_map]                    # CS:1EC2
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1EC6
    mov ax, ds:[D_shot_effect_template_x]                            # CS:1EC9
    mov WORD PTR [si+E_X], ax                                        # CS:1ECC
    mov ax, ds:[D_shot_effect_template_y]                            # CS:1ECF
    mov WORD PTR [si+E_Y], ax                                        # CS:1ED2
    mov al, ds:[D_shot_effect_template_z]                            # CS:1ED5
    mov BYTE PTR [si+E_Z], al                                        # CS:1ED8
    mov BYTE PTR [si+E_PARAM], 0x2                                   # CS:1EDB
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1EDF
    mov bh, 0x0                                                      # CS:1EE2
    mov al, BYTE PTR [bx+D_shot_variant_heights]                     # CS:1EE4
    mov BYTE PTR [si+E_HEIGHT], al                                   # CS:1EE8
    mov BYTE PTR [si+E_WIDTH], 0x4                                   # CS:1EEB
    shl bx, 1                                                        # CS:1EEF
    mov ax, WORD PTR [bx+D_enemy_shot_sprite_offsets]                # CS:1EF1
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:1EF5
    call allocate_entity_copy                                        # CS:1EF8
    cmp si, 0x0                                                      # CS:1EFB
    je L_spawn_enemy_shot_at_offset_1f09                             # CS:1EFE
    pop ax                                                           # CS:1F00
    push ax                                                          # CS:1F01
    {load} cmp ax, si                                                # CS:1F02
    jbe L_spawn_enemy_shot_at_offset_1f09                            # CS:1F04
    call update_enemy_projectile                                     # CS:1F06
L_spawn_enemy_shot_at_offset_1f09:
    pop si                                                           # CS:1F09
    ret                                                              # CS:1F0A

/* ========================================================================== 
 * spawn_aimed_shot  |  CS:1F0B
 * Назначение: Создаёт эффект/снаряд в выбранном направлении, беря смещения и размеры из таблиц.
 * Вход: DS:SI = emitter; frame = направление.
 * Эффект/ограничения: Тот же компенсирующий update для слота раньше текущего.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_aimed_shot, @function
spawn_aimed_shot:
    push si                                                          # CS:1F0B
    inc BYTE PTR ds:[D_shot_variation_index]                         # CS:1F0C
    mov bl, BYTE PTR ds:[D_shot_variation_index]                     # CS:1F10
    and bx, 0xf                                                      # CS:1F14
    mov al, BYTE PTR [bx+D_shot_variation_sequence]                  # CS:1F17
    mov ds:[D_shot_effect_template_frame], al                        # CS:1F1B
    mov di, 0xff7                                                    # CS:1F1E
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1F21
    mov bh, 0x0                                                      # CS:1F24
    mov al, BYTE PTR [bx+D_aimed_muzzle_dx]                          # CS:1F26
    cbw                                                              # CS:1F2A
    add ax, WORD PTR [si+E_X]                                        # CS:1F2B
    mov WORD PTR [di+E_X], ax                                        # CS:1F2E
    mov al, BYTE PTR [bx+D_aimed_muzzle_dy]                          # CS:1F31
    cbw                                                              # CS:1F35
    add ax, WORD PTR [si+E_Y]                                        # CS:1F36
    ACC16 and, WORLD_Y_MASK                                          # CS:1F39
    mov WORD PTR [di+E_Y], ax                                        # CS:1F3C
    mov al, BYTE PTR [si+E_Z]                                        # CS:1F3F
    add al, 0x4                                                      # CS:1F42
    mov BYTE PTR [di+E_Z], al                                        # CS:1F44
    shl bx, 1                                                        # CS:1F47
    mov bx, WORD PTR [bx+D_aimed_effect_descriptors]                 # CS:1F49
    {load} mov si, di                                                # CS:1F4D
    call apply_sprite_frame                                          # CS:1F4F
    call allocate_entity_copy                                        # CS:1F52
    cmp si, 0x0                                                      # CS:1F55
    je L_spawn_aimed_shot_1f63                                       # CS:1F58
    pop ax                                                           # CS:1F5A
    push ax                                                          # CS:1F5B
    {load} cmp ax, si                                                # CS:1F5C
    jbe L_spawn_aimed_shot_1f63                                      # CS:1F5E
    call update_countdown_effect                                     # CS:1F60
L_spawn_aimed_shot_1f63:
    pop si                                                           # CS:1F63
    mov al, ds:[D_spawn_projectile_gate]                             # CS:1F64
    cmp al, 0x0                                                      # CS:1F67
    jne L_spawn_aimed_shot_1f6c                                      # CS:1F69
    ret                                                              # CS:1F6B
L_spawn_aimed_shot_1f6c:
    push si                                                          # CS:1F6C
    mov di, 0x13b0                                                   # CS:1F6D
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1F70
    mov bh, 0x0                                                      # CS:1F73
    mov BYTE PTR [di+E_PARAM], bl                                    # CS:1F75
    mov ax, ds:[D_shot_effect_template_x]                            # CS:1F78
    mov WORD PTR [di+E_X], ax                                        # CS:1F7B
    mov ax, ds:[D_shot_effect_template_y]                            # CS:1F7E
    mov WORD PTR [di+E_Y], ax                                        # CS:1F81
    mov al, ds:[D_shot_effect_template_z]                            # CS:1F84
    mov BYTE PTR [di+E_Z], al                                        # CS:1F87
    mov al, BYTE PTR [bx+D_enemy_shot_widths]                        # CS:1F8A
    mov BYTE PTR [di+E_WIDTH], al                                    # CS:1F8E
    mov al, BYTE PTR [bx+D_enemy_shot_heights]                       # CS:1F91
    mov BYTE PTR [di+E_HEIGHT], al                                   # CS:1F95
    mov BYTE PTR [di+E_FRAME], 0x0                                   # CS:1F98
    shl bx, 1                                                        # CS:1F9C
    mov ax, WORD PTR [bx+D_enemy_aimed_shot_sprites]                 # CS:1F9E
    mov WORD PTR [di+E_SPRITE], ax                                   # CS:1FA2
    {load} mov si, di                                                # CS:1FA5
    call allocate_entity_copy                                        # CS:1FA7
    cmp si, 0x0                                                      # CS:1FAA
    je L_spawn_aimed_shot_1fb8                                       # CS:1FAD
    pop ax                                                           # CS:1FAF
    push ax                                                          # CS:1FB0
    {load} cmp ax, si                                                # CS:1FB1
    jbe L_spawn_aimed_shot_1fb8                                      # CS:1FB3
    call update_enemy_projectile                                     # CS:1FB5
L_spawn_aimed_shot_1fb8:
    pop si                                                           # CS:1FB8
    ret                                                              # CS:1FB9

/* ========================================================================== 
 * spawn_ground_enemy_shot  |  CS:1FBA
 * Назначение: Порождает handler29 из шаблона DS:10E8 и таблиц смещений.
 * Вход: DS:SI = emitter.
 * Эффект/ограничения: SI сохраняется; новый ранний слот получает один немедленный update.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_ground_enemy_shot, @function
spawn_ground_enemy_shot:
    push si                                                          # CS:1FBA
    mov di, 0x10e8                                                   # CS:1FBB
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1FBE
    mov BYTE PTR [di+E_PARAM], bl                                    # CS:1FC1
    mov bh, 0x0                                                      # CS:1FC4
    mov al, BYTE PTR [bx+D_ground_shot_dx]                           # CS:1FC6
    cbw                                                              # CS:1FCA
    add ax, WORD PTR [si+E_X]                                        # CS:1FCB
    mov WORD PTR [di+E_X], ax                                        # CS:1FCE
    mov al, BYTE PTR [bx+D_ground_shot_dy]                           # CS:1FD1
    cbw                                                              # CS:1FD5
    add ax, WORD PTR [si+E_Y]                                        # CS:1FD6
    ACC16 and, WORLD_Y_MASK                                          # CS:1FD9
    mov WORD PTR [di+E_Y], ax                                        # CS:1FDC
    mov al, BYTE PTR [si+E_Z]                                        # CS:1FDF
    add al, 0x8                                                      # CS:1FE2
    mov BYTE PTR [di+E_Z], al                                        # CS:1FE4
    shl bx, 1                                                        # CS:1FE7
    mov bx, WORD PTR [bx+D_ground_shot_descriptors]                  # CS:1FE9
    {load} mov si, di                                                # CS:1FED
    call apply_sprite_frame                                          # CS:1FEF
    call allocate_entity_copy                                        # CS:1FF2
    cmp si, 0x0                                                      # CS:1FF5
    je L_spawn_ground_enemy_shot_2003                                # CS:1FF8
    pop ax                                                           # CS:1FFA
    push ax                                                          # CS:1FFB
    {load} cmp ax, si                                                # CS:1FFC
    jbe L_spawn_ground_enemy_shot_2003                               # CS:1FFE
    call update_ground_enemy_shot                                    # CS:2000
L_spawn_ground_enemy_shot_2003:
    pop si                                                           # CS:2003
    ret                                                              # CS:2004

/* ========================================================================== 
 * spawn_arcing_projectile  |  CS:2005
 * Назначение: Порождает handler30 из шаблона DS:1446 и таблиц смещений.
 * Вход: DS:SI = emitter.
 * Эффект/ограничения: SI сохраняется; новый ранний слот получает один немедленный update.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_arcing_projectile, @function
spawn_arcing_projectile:
    push si                                                          # CS:2005
    mov di, 0x1446                                                   # CS:2006
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:2009
    mov BYTE PTR [di+E_PARAM], bl                                    # CS:200C
    mov bh, 0x0                                                      # CS:200F
    mov al, BYTE PTR [bx+D_arc_spawn_dx]                             # CS:2011
    cbw                                                              # CS:2015
    add ax, WORD PTR [si+E_X]                                        # CS:2016
    mov WORD PTR [di+E_X], ax                                        # CS:2019
    mov al, BYTE PTR [bx+D_arc_spawn_dy]                             # CS:201C
    cbw                                                              # CS:2020
    add ax, WORD PTR [si+E_Y]                                        # CS:2021
    ACC16 and, WORLD_Y_MASK                                          # CS:2024
    mov WORD PTR [di+E_Y], ax                                        # CS:2027
    mov al, BYTE PTR [si+E_Z]                                        # CS:202A
    add al, 0x8                                                      # CS:202D
    mov BYTE PTR [di+E_Z], al                                        # CS:202F
    {load} mov si, di                                                # CS:2032
    call allocate_entity_copy                                        # CS:2034
    cmp si, 0x0                                                      # CS:2037
    je L_spawn_arcing_projectile_2045                                # CS:203A
    pop ax                                                           # CS:203C
    push ax                                                          # CS:203D
    {load} cmp ax, si                                                # CS:203E
    jbe L_spawn_arcing_projectile_2045                               # CS:2040
    call update_arcing_projectile                                    # CS:2042
L_spawn_arcing_projectile_2045:
    pop si                                                           # CS:2045
    ret                                                              # CS:2046

/* ========================================================================== 
 * replace_with_side_impact  |  CS:2047
 * Назначение: Заменяет выстрел эффектом, если соответствующий side_impact_mask свободен; иначе удаляет.
 * Вход: DS:SI = выстрел.
 * Эффект/ограничения: Отдельные биты 01h/80h ограничивают одновременные эффекты двух сторон.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type replace_with_side_impact, @function
replace_with_side_impact:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:2047
    mov bl, BYTE PTR ds:[D_side_impact_mask]                         # CS:2049
    test al, 0x20                                                    # CS:204D
    je L_replace_with_side_impact_2072                               # CS:204F
    test bl, 0x80                                                    # CS:2051
    jne L_replace_with_side_impact_206d                              # CS:2054
    or BYTE PTR ds:[D_side_impact_mask], 0x80                        # CS:2056
    or BYTE PTR ds:[D_side_impact_template], 0x20                    # CS:205B
    mov di, 0x1326                                                   # CS:2060
    call apply_entity_state_template                                 # CS:2063
    mov al, 0x3                                                      # CS:2066
    call sound_trigger                                               # CS:2068
    jmp L_replace_with_side_impact_2070                              # CS:206B
L_replace_with_side_impact_206d:
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:206D
L_replace_with_side_impact_2070:
    jmp L_replace_with_side_impact_2091                              # CS:2070
L_replace_with_side_impact_2072:
    test bl, 0x1                                                     # CS:2072
    jne L_replace_with_side_impact_208e                              # CS:2075
    or BYTE PTR ds:[D_side_impact_mask], 0x1                         # CS:2077
    and BYTE PTR ds:[D_side_impact_template], 0xdf                   # CS:207C
    mov di, 0x1326                                                   # CS:2081
    call apply_entity_state_template                                 # CS:2084
    mov al, 0x3                                                      # CS:2087
    call sound_trigger                                               # CS:2089
    jmp L_replace_with_side_impact_2091                              # CS:208C
L_replace_with_side_impact_208e:
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:208E
L_replace_with_side_impact_2091:
    ret                                                              # CS:2091

/* ========================================================================== 
 * replace_with_impact  |  CS:2092
 * Назначение: Применяет шаблон эффекта DS:1427 к текущей записи.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Координаты не копируются; изменяются только поля replacement-процедуры.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type replace_with_impact, @function
replace_with_impact:
    mov di, 0x1427                                                   # CS:2092
    call apply_entity_state_template                                 # CS:2095
    ret                                                              # CS:2098

/* ========================================================================== 
 * replace_with_arc_effect  |  CS:2099
 * Назначение: Применяет шаблон DS:12DE к текущей записи.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Общий механизм apply_entity_state_template.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type replace_with_arc_effect, @function
replace_with_arc_effect:
    mov di, 0x12de                                                   # CS:2099
    call apply_entity_state_template                                 # CS:209C
    ret                                                              # CS:209F

/* ========================================================================== 
 * apply_entity_state_template  |  CS:20A0
 * Назначение: Копирует flags, layer, age, frame, размеры, якорь, sprite и handler из DI в SI.
 * Вход: DS:SI = объект; DS:DI = шаблон.
 * Эффект/ограничения: НЕ копирует param, x/y/z, bank, transparent и state13..15.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type apply_entity_state_template, @function
apply_entity_state_template:
    mov al, BYTE PTR [di+E_FLAGS]                                    # CS:20A0
    mov BYTE PTR [si+E_FLAGS], al                                    # CS:20A2
    mov al, BYTE PTR [di+E_LAYER]                                    # CS:20A4
    mov BYTE PTR [si+E_LAYER], al                                    # CS:20A7
    mov al, BYTE PTR [di+E_STATE_16]                                 # CS:20AA
    mov BYTE PTR [si+E_STATE_16], al                                 # CS:20AD
    mov al, BYTE PTR [di+E_FRAME]                                    # CS:20B0
    mov BYTE PTR [si+E_FRAME], al                                    # CS:20B3
    mov al, BYTE PTR [di+E_WIDTH]                                    # CS:20B6
    mov BYTE PTR [si+E_WIDTH], al                                    # CS:20B9
    mov al, BYTE PTR [di+E_HEIGHT]                                   # CS:20BC
    mov BYTE PTR [si+E_HEIGHT], al                                   # CS:20BF
    mov al, BYTE PTR [di+E_ANCHOR_X]                                 # CS:20C2
    mov BYTE PTR [si+E_ANCHOR_X], al                                 # CS:20C5
    mov al, BYTE PTR [di+E_ANCHOR_Y]                                 # CS:20C8
    mov BYTE PTR [si+E_ANCHOR_Y], al                                 # CS:20CB
    mov ax, WORD PTR [di+E_SPRITE]                                   # CS:20CE
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:20D1
    mov al, BYTE PTR [di+E_HANDLER]                                  # CS:20D4
    mov BYTE PTR [si+E_HANDLER], al                                  # CS:20D7
    ret                                                              # CS:20DA

/* ========================================================================== 
 * destroy_map_entity  |  CS:20DB
 * Назначение: Начисляет 150 очков, создаёт взрыв и помечает объект карты использованным.
 * Вход: DS:SI = сущность карты.
 * Эффект/ограничения: Исходная запись преобразуется, а не обязательно сразу очищается.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type destroy_map_entity, @function
destroy_map_entity:
    call score_add_150                                               # CS:20DB
    call spawn_explosion_for_entity                                  # CS:20DE
    push si                                                          # CS:20E1
    mov al, BYTE PTR [si+E_STATE_13]                                 # CS:20E2
    mov ah, 0x0                                                      # CS:20E5
    push ax                                                          # CS:20E7
    mov al, BYTE PTR [si+E_STATE_14]                                 # CS:20E8
    push ax                                                          # CS:20EB
    call consume_map_object                                          # CS:20EC
    ret                                                              # CS:20EF

/* ========================================================================== 
 * spawn_explosion_for_entity  |  CS:20F0
 * Назначение: Создаёт explosion из координат текущей записи, при bit10h корректирует якорь.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Сохраняет SI; шаблон DS:129E изменяется перед копированием.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_explosion_for_entity, @function
spawn_explosion_for_entity:
    push si                                                          # CS:20F0
    mov ax, WORD PTR [si+E_X]                                        # CS:20F1
    mov ds:[D_explosion_template_x], ax                              # CS:20F4
    mov ax, WORD PTR [si+E_Y]                                        # CS:20F7
    mov ds:[D_explosion_template_y], ax                              # CS:20FA
    mov al, BYTE PTR [si+E_Z]                                        # CS:20FD
    mov ds:[D_explosion_template_z], al                              # CS:2100
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:2103
    test al, 0x10                                                    # CS:2105
    je L_spawn_explosion_for_entity_2119                             # CS:2107
    sub WORD PTR ds:[D_explosion_template_x], 0xc                    # CS:2109
    sub WORD PTR ds:[D_explosion_template_y], 0x8                    # CS:210E
    and WORD PTR ds:[D_explosion_template_y], 0x3ff                  # CS:2113
L_spawn_explosion_for_entity_2119:
    mov si, OFFSET D_template_explosion                              # CS:2119
    call allocate_entity_copy                                        # CS:211C
    pop si                                                           # CS:211F
    ret                                                              # CS:2120

/* ========================================================================== 
 * apply_sprite_frame  |  CS:2121
 * Назначение: Читает count и SoA-массивы width/height/anchor_x/anchor_y, затем пары bank/offset.
 * Вход: DS:SI = Entity; DS:BX = descriptor; Entity.frame = индекс.
 * Эффект/ограничения: Обновляет поля 09..10h, не flags/координаты/transparent; границы frame здесь не проверяются.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type apply_sprite_frame, @function
apply_sprite_frame:

    # FrameSet хранит массивы полей (SoA), НЕ массив C-структур Frame. Индекс — E_FRAME.
    mov cl, BYTE PTR [bx]                                            # CS:2121
    mov ch, 0x0                                                      # CS:2123
    inc bx                                                           # CS:2125
    mov al, BYTE PTR [si+E_FRAME]                                    # CS:2126
    mov ah, 0x0                                                      # CS:2129
    {load} mov di, ax                                                # CS:212B
    mov al, BYTE PTR [bx+di]                                         # CS:212D
    mov BYTE PTR [si+E_WIDTH], al                                    # CS:212F
    {load} add bx, cx                                                # CS:2132
    mov al, BYTE PTR [bx+di]                                         # CS:2134
    mov BYTE PTR [si+E_HEIGHT], al                                   # CS:2136
    {load} add bx, cx                                                # CS:2139
    mov al, BYTE PTR [bx+di]                                         # CS:213B
    mov BYTE PTR [si+E_ANCHOR_X], al                                 # CS:213D
    {load} add bx, cx                                                # CS:2140
    mov al, BYTE PTR [bx+di]                                         # CS:2142
    mov BYTE PTR [si+E_ANCHOR_Y], al                                 # CS:2144
    {load} add bx, cx                                                # CS:2147
    shl di, 1                                                        # CS:2149
    shl di, 1                                                        # CS:214B
    mov ax, WORD PTR [bx+di]                                         # CS:214D
    mov WORD PTR [si+E_BANK], ax                                     # CS:214F
    add bx, 0x2                                                      # CS:2152
    mov ax, WORD PTR [bx+di]                                         # CS:2155
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:2157
    ret                                                              # CS:215A

/* ========================================================================== 
 * aim_entity_at_player  |  CS:215B
 * Назначение: Выбирает одно из 5 направлений, либо качает направление вне зоны прицеливания.
 * Вход: DS:SI = emitter; положение player.
 * Эффект/ограничения: CX=1 разрешает стрельбу, CX=0 запрещает; frame = направление 0..4.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type aim_entity_at_player, @function
aim_entity_at_player:

    # Преобразование векторов прицеливания целочисленное; знаки/округление нужны для эталонного порта.
    mov cx, 0x1                                                      # CS:215B
    mov ax, ds:[D_player_y]                                          # CS:215E
    sub ax, WORD PTR [si+0x7]                                        # CS:2161
    ACC16 cmp, 0x8c                                                  # CS:2164
    jle L_aim_entity_at_player_2171                                  # CS:2167
    mov BYTE PTR [si+0x4], 0x2                                       # CS:2169
    mov cx, 0x0                                                      # CS:216D
    ret                                                              # CS:2170
L_aim_entity_at_player_2171:
    ACC16 cmp, 0xfff0                                                # CS:2171
    jge L_aim_entity_at_player_21a0                                  # CS:2174
    mov ah, BYTE PTR [si+0x4]                                        # CS:2176
    mov al, BYTE PTR [si]                                            # CS:2179
    test al, 0x20                                                    # CS:217B
    je L_aim_entity_at_player_218d                                   # CS:217D
    inc ah                                                           # CS:217F
    cmp ah, 0x4                                                      # CS:2181
    jle L_aim_entity_at_player_218b                                  # CS:2184
    and BYTE PTR [si], 0xdf                                          # CS:2186
    mov ah, 0x4                                                      # CS:2189
L_aim_entity_at_player_218b:
    jmp L_aim_entity_at_player_2199                                  # CS:218B
L_aim_entity_at_player_218d:
    dec ah                                                           # CS:218D
    cmp ah, 0x0                                                      # CS:218F
    jge L_aim_entity_at_player_2199                                  # CS:2192
    or BYTE PTR [si], 0x20                                           # CS:2194
    mov ah, 0x0                                                      # CS:2197
L_aim_entity_at_player_2199:
    mov BYTE PTR [si+0x4], ah                                        # CS:2199
    mov cx, 0x0                                                      # CS:219C
    ret                                                              # CS:219F
L_aim_entity_at_player_21a0:
    mov bx, WORD PTR ds:[D_player_x]                                 # CS:21A0
    sub bx, WORD PTR [si+0x5]                                        # CS:21A4
    jns L_aim_entity_at_player_21c2                                  # CS:21A7
    cmp bx, 0xffe0                                                   # CS:21A9
    jle L_aim_entity_at_player_21b3                                  # CS:21AC
    mov BYTE PTR [si+0x4], 0x2                                       # CS:21AE
    ret                                                              # CS:21B2
L_aim_entity_at_player_21b3:
    ACC16 cmp, 0x20                                                  # CS:21B3
    jle L_aim_entity_at_player_21bd                                  # CS:21B6
    mov BYTE PTR [si+0x4], 0x1                                       # CS:21B8
    ret                                                              # CS:21BC
L_aim_entity_at_player_21bd:
    mov BYTE PTR [si+0x4], 0x0                                       # CS:21BD
    ret                                                              # CS:21C1
L_aim_entity_at_player_21c2:
    cmp bx, 0x20                                                     # CS:21C2
    jge L_aim_entity_at_player_21cc                                  # CS:21C5
    mov BYTE PTR [si+0x4], 0x2                                       # CS:21C7
    ret                                                              # CS:21CB
L_aim_entity_at_player_21cc:
    ACC16 cmp, 0x20                                                  # CS:21CC
    jle L_aim_entity_at_player_21d6                                  # CS:21CF
    mov BYTE PTR [si+0x4], 0x3                                       # CS:21D1
    ret                                                              # CS:21D5
L_aim_entity_at_player_21d6:
    mov BYTE PTR [si+0x4], 0x4                                       # CS:21D6
    ret                                                              # CS:21DA

/* CS:21DB: Inter-procedure alignment byte, not an instruction. */
padding_21db:
    .byte 0x00
