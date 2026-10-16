/* Физика туннеля, предметы, проекция
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * select_tunnel_pattern  |  CS:2DDC
 * Назначение: Циклически выбирает один из четырёх потоков туннельных объектов и сбрасывает номер предыдущей секции.
 * Вход: tunnel_pattern_index.
 * Эффект/ограничения: Индекс растёт байтово, таблица индексируется по младшим двум битам.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type select_tunnel_pattern, @function
select_tunnel_pattern:

    # Туннель — самостоятельная ветвь игровой логики с тем же пулом Entity.
    mov bl, BYTE PTR ds:[D_tunnel_pattern_index]                     # CS:2DDC
    and bx, 0x3                                                      # CS:2DE0
    shl bx, 1                                                        # CS:2DE3
    mov ax, WORD PTR [bx+D_tunnel_pattern_pointers]                  # CS:2DE5
    mov ds:[D_tunnel_pattern_pointer], ax                            # CS:2DE9
    inc BYTE PTR ds:[D_tunnel_pattern_index]                         # CS:2DEC
    mov BYTE PTR ds:[D_tunnel_last_section], 0xff                    # CS:2DF0
    ret                                                              # CS:2DF5

/* ========================================================================== 
 * reset_tunnel_angle_history  |  CS:2DF6
 * Назначение: Обнуляет историю трёх углов для боковых устройств.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Не обнуляет всю физику туннеля.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_tunnel_angle_history, @function
reset_tunnel_angle_history:
    mov al, 0x0                                                      # CS:2DF6
    mov ds:[D_tunnel_angle_history0], al                             # CS:2DF8
    mov ds:[D_tunnel_angle_history1], al                             # CS:2DFB
    mov ds:[D_tunnel_angle_history2], al                             # CS:2DFE
    ret                                                              # CS:2E01

/* ========================================================================== 
 * update_tunnel_player  |  CS:2E02
 * Назначение: Обновляет скорость/дистанцию, угловую инерцию, позицию по lookup-таблицам и боковые действия.
 * Вход: DS:SI = игрок туннеля; handler ID12.
 * Эффект/ограничения: Инерция ограничена ±64, угол modulo128, координаты рассчитаны целочисленно.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_player, @function
update_tunnel_player:
    push si                                                          # CS:2E02
    call update_scroll_speed                                         # CS:2E03
    call advance_tunnel_distance                                     # CS:2E06
    pop si                                                           # CS:2E09
    mov ax, ds:[D_scroll_speed]                                      # CS:2E0A
    shr ax, 1                                                        # CS:2E0D
    shr ax, 1                                                        # CS:2E0F
    shr ax, 1                                                        # CS:2E11
    mov bx, WORD PTR ds:[D_control_bits]                             # CS:2E13
    test bl, 0x1                                                     # CS:2E17
    je L_update_tunnel_player_2e23                                   # CS:2E1A
    neg ax                                                           # CS:2E1C
    ACC16 sub, 0x3                                                   # CS:2E1E
    jmp L_update_tunnel_player_2e30                                  # CS:2E21
L_update_tunnel_player_2e23:
    test bl, 0x2                                                     # CS:2E23
    je L_update_tunnel_player_2e2d                                   # CS:2E26
    ACC16 add, 0x3                                                   # CS:2E28
    jmp L_update_tunnel_player_2e30                                  # CS:2E2B
L_update_tunnel_player_2e2d:
    mov ax, 0x0                                                      # CS:2E2D
L_update_tunnel_player_2e30:
    mov ds:[D_tunnel_input_force], ax                                # CS:2E30
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:2E33
    ACC16 and, 0x7f                                                  # CS:2E36
    cmp al, 0x3f                                                     # CS:2E39
    jbe L_update_tunnel_player_2e48                                  # CS:2E3B
    neg al                                                           # CS:2E3D
    add al, 0x7f                                                     # CS:2E3F
    call tunnel_angle_force                                          # CS:2E41
    neg ax                                                           # CS:2E44
    jmp L_update_tunnel_player_2e4b                                  # CS:2E46
L_update_tunnel_player_2e48:
    call tunnel_angle_force                                          # CS:2E48
L_update_tunnel_player_2e4b:
    mov ds:[D_tunnel_passive_force], ax                              # CS:2E4B
    mov ax, ds:[D_tunnel_angular_velocity]                           # CS:2E4E
    add ax, WORD PTR ds:[D_tunnel_input_force]                       # CS:2E51
    add ax, WORD PTR ds:[D_tunnel_passive_force]                     # CS:2E55
    je L_update_tunnel_player_2e6a                                   # CS:2E59
    jns L_update_tunnel_player_2e63                                  # CS:2E5B
    inc ax                                                           # CS:2E5D
    jns L_update_tunnel_player_2e61                                  # CS:2E5E
    inc ax                                                           # CS:2E60
L_update_tunnel_player_2e61:
    jmp L_update_tunnel_player_2e6a                                  # CS:2E61
L_update_tunnel_player_2e63:
    dec ax                                                           # CS:2E63
    ACC16 cmp, 0x0                                                   # CS:2E64
    jle L_update_tunnel_player_2e6a                                  # CS:2E67
    dec ax                                                           # CS:2E69
L_update_tunnel_player_2e6a:
    ACC16 cmp, 0x40                                                  # CS:2E6A
    jle L_update_tunnel_player_2e74                                  # CS:2E6D
    mov ax, 0x40                                                     # CS:2E6F
    jmp L_update_tunnel_player_2e7c                                  # CS:2E72
L_update_tunnel_player_2e74:
    ACC16 cmp, 0xffc0                                                # CS:2E74
    jge L_update_tunnel_player_2e7c                                  # CS:2E77
    mov ax, 0xffc0                                                   # CS:2E79
L_update_tunnel_player_2e7c:
    mov ds:[D_tunnel_angular_velocity], ax                           # CS:2E7C
    sar ax, 1                                                        # CS:2E7F
    sar ax, 1                                                        # CS:2E81
    add BYTE PTR [si+E_STATE_16], al                                 # CS:2E83
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:2E86
    ACC16 and, 0x7f                                                  # CS:2E89
    {load} mov bx, ax                                                # CS:2E8C
    mov al, BYTE PTR [bx+D_tunnel_circle_x]                          # CS:2E8E
    cbw                                                              # CS:2E92
    shl ax, 1                                                        # CS:2E93
    ACC16 add, 0x80                                                  # CS:2E95
    mov WORD PTR [si+E_X], ax                                        # CS:2E98
    mov al, BYTE PTR [bx+D_tunnel_circle_y]                          # CS:2E9B
    cbw                                                              # CS:2E9F
    shl ax, 1                                                        # CS:2EA0
    ACC16 add, 0x80                                                  # CS:2EA2
    push bx                                                          # CS:2EA5
    ACC16 cmp, 0x0                                                   # CS:2EA6
    je L_update_tunnel_player_2ebe                                   # CS:2EA9
    {load} mov bx, ax                                                # CS:2EAB
    shl ax, 1                                                        # CS:2EAD
    shl ax, 1                                                        # CS:2EAF
    {load} add ax, bx                                                # CS:2EB1
    shl ax, 1                                                        # CS:2EB3
    ACC16 add, 0x5                                                   # CS:2EB5
    mov bl, 0xc                                                      # CS:2EB8
    div bl                                                           # CS:2EBA
    mov ah, 0x0                                                      # CS:2EBC
L_update_tunnel_player_2ebe:
    pop bx                                                           # CS:2EBE
    mov WORD PTR [si+E_Y], ax                                        # CS:2EBF
    mov al, BYTE PTR [bx+D_tunnel_player_frames]                     # CS:2EC2
    mov BYTE PTR [si+E_FRAME], al                                    # CS:2EC6
    mov bx, OFFSET D_sprite_frames_1594                              # CS:2EC9
    call apply_sprite_frame                                          # CS:2ECC
    mov al, ds:[D_tunnel_angle_history1]                             # CS:2ECF
    mov ds:[D_tunnel_angle_history2], al                             # CS:2ED2
    mov al, ds:[D_tunnel_angle_history0]                             # CS:2ED5
    mov ds:[D_tunnel_angle_history1], al                             # CS:2ED8
    mov al, ds:[D_player_angle_or_age]                               # CS:2EDB
    mov ds:[D_tunnel_angle_history0], al                             # CS:2EDE
    mov ax, ds:[D_control_bits]                                      # CS:2EE1
    mov bl, BYTE PTR ds:[D_left_device_state]                        # CS:2EE4
    test al, 0x40                                                    # CS:2EE8
    je L_update_tunnel_player_2f02                                   # CS:2EEA
    cmp bl, 0x2                                                      # CS:2EEC
    jne L_update_tunnel_player_2f00                                  # CS:2EEF
    mov BYTE PTR ds:[D_left_device_state], 0x3                       # CS:2EF1
    mov al, 0x2                                                      # CS:2EF6
    call sound_trigger                                               # CS:2EF8
    mov BYTE PTR ds:[D_tunnel_flash_state], 0x2                      # CS:2EFB
L_update_tunnel_player_2f00:
    jmp L_update_tunnel_player_2f18                                  # CS:2F00
L_update_tunnel_player_2f02:
    cmp bl, 0x1                                                      # CS:2F02
    jne L_update_tunnel_player_2f0e                                  # CS:2F05
    mov BYTE PTR ds:[D_left_device_state], 0x2                       # CS:2F07
    jmp L_update_tunnel_player_2f18                                  # CS:2F0C
L_update_tunnel_player_2f0e:
    cmp bl, 0x3                                                      # CS:2F0E
    jne L_update_tunnel_player_2f18                                  # CS:2F11
    mov BYTE PTR ds:[D_left_device_state], 0x0                       # CS:2F13
L_update_tunnel_player_2f18:
    mov ax, ds:[D_control_bits]                                      # CS:2F18
    mov bl, BYTE PTR ds:[D_right_device_state]                       # CS:2F1B
    test al, 0x20                                                    # CS:2F1F
    je L_update_tunnel_player_2f39                                   # CS:2F21
    cmp bl, 0x2                                                      # CS:2F23
    jne L_update_tunnel_player_2f37                                  # CS:2F26
    mov BYTE PTR ds:[D_right_device_state], 0x3                      # CS:2F28
    mov al, 0x2                                                      # CS:2F2D
    call sound_trigger                                               # CS:2F2F
    mov BYTE PTR ds:[D_tunnel_flash_state], 0x2                      # CS:2F32
L_update_tunnel_player_2f37:
    jmp L_update_tunnel_player_2f4f                                  # CS:2F37
L_update_tunnel_player_2f39:
    cmp bl, 0x1                                                      # CS:2F39
    jne L_update_tunnel_player_2f45                                  # CS:2F3C
    mov BYTE PTR ds:[D_right_device_state], 0x2                      # CS:2F3E
    jmp L_update_tunnel_player_2f4f                                  # CS:2F43
L_update_tunnel_player_2f45:
    cmp bl, 0x3                                                      # CS:2F45
    jne L_update_tunnel_player_2f4f                                  # CS:2F48
    mov BYTE PTR ds:[D_right_device_state], 0x0                      # CS:2F4A
L_update_tunnel_player_2f4f:
    ret                                                              # CS:2F4F

/* ========================================================================== 
 * update_tunnel_side_device  |  CS:2F50
 * Назначение: Следует задержанному углу; при выпуске/отключении исчезает и собирает все handler15 в пуле.
 * Вход: DS:SI = устройство; handler ID34.
 * Эффект/ограничения: SI на пути сбора перебирает пул; dispatcher обязан сохранять свой SI.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_side_device, @function
update_tunnel_side_device:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:2F50
    test al, 0x20                                                    # CS:2F52
    jne L_update_tunnel_side_device_2f5e                             # CS:2F54
    mov ah, BYTE PTR ds:[D_left_device_state]                        # CS:2F56
    mov al, 0xf8                                                     # CS:2F5A
    jmp L_update_tunnel_side_device_2f64                             # CS:2F5C
L_update_tunnel_side_device_2f5e:
    mov ah, BYTE PTR ds:[D_right_device_state]                       # CS:2F5E
    mov al, 0x8                                                      # CS:2F62
L_update_tunnel_side_device_2f64:
    cmp ah, 0x3                                                      # CS:2F64
    je L_update_tunnel_side_device_2f6e                              # CS:2F67
    cmp ah, 0x0                                                      # CS:2F69
    jne L_update_tunnel_side_device_2f90                             # CS:2F6C
L_update_tunnel_side_device_2f6e:
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:2F6E
    mov si, 0x1ba1                                                   # CS:2F71
L_update_tunnel_side_device_2f74:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:2F74
    test al, 0x80                                                    # CS:2F76
    jne L_update_tunnel_side_device_2f8f                             # CS:2F78
    test al, 0x1                                                     # CS:2F7A
    je L_update_tunnel_side_device_2f8a                              # CS:2F7C
    mov al, BYTE PTR [si+E_HANDLER]                                  # CS:2F7E
    cmp al, 0xf                                                      # CS:2F81
    jne L_update_tunnel_side_device_2f8a                             # CS:2F83
    push si                                                          # CS:2F85
    call collect_tunnel_pickup                                       # CS:2F86
    pop si                                                           # CS:2F89
L_update_tunnel_side_device_2f8a:
    add si, ENTITY_SIZE                                              # CS:2F8A
    jmp L_update_tunnel_side_device_2f74                             # CS:2F8D
L_update_tunnel_side_device_2f8f:
    ret                                                              # CS:2F8F
L_update_tunnel_side_device_2f90:
    add al, BYTE PTR ds:[D_tunnel_angle_history2]                    # CS:2F90
    ACC16 and, 0x7f                                                  # CS:2F94
    {load} mov bx, ax                                                # CS:2F97
    mov al, BYTE PTR [bx+D_tunnel_circle_x]                          # CS:2F99
    cbw                                                              # CS:2F9D
    {load} mov cx, ax                                                # CS:2F9E
    sar cx, 1                                                        # CS:2FA0
    {load} add ax, cx                                                # CS:2FA2
    ACC16 add, 0x80                                                  # CS:2FA4
    mov WORD PTR [si+E_X], ax                                        # CS:2FA7
    mov al, BYTE PTR [bx+D_tunnel_circle_y]                          # CS:2FAA
    cbw                                                              # CS:2FAE
    {load} mov cx, ax                                                # CS:2FAF
    sar cx, 1                                                        # CS:2FB1
    {load} add ax, cx                                                # CS:2FB3
    ACC16 add, 0x80                                                  # CS:2FB5
    ACC16 cmp, 0x0                                                   # CS:2FB8
    je L_update_tunnel_side_device_2fd0                              # CS:2FBB
    {load} mov bx, ax                                                # CS:2FBD
    shl ax, 1                                                        # CS:2FBF
    shl ax, 1                                                        # CS:2FC1
    {load} add ax, bx                                                # CS:2FC3
    shl ax, 1                                                        # CS:2FC5
    ACC16 add, 0x5                                                   # CS:2FC7
    mov bl, 0xc                                                      # CS:2FCA
    div bl                                                           # CS:2FCC
    mov ah, 0x0                                                      # CS:2FCE
L_update_tunnel_side_device_2fd0:
    mov bl, BYTE PTR ds:[D_player_angle_or_age]                      # CS:2FD0
    and bx, 0x7f                                                     # CS:2FD4
    mov WORD PTR [si+E_Y], ax                                        # CS:2FD7
    mov al, BYTE PTR [bx+D_tunnel_side_frames]                       # CS:2FDA
    mov BYTE PTR [si+E_FRAME], al                                    # CS:2FDE
    mov bx, OFFSET D_sprite_frames_1811                              # CS:2FE1
    call apply_sprite_frame                                          # CS:2FE4
    ret                                                              # CS:2FE7

/* ========================================================================== 
 * update_tunnel_exit_marker  |  CS:2FE8
 * Назначение: Выбирает размер кольца и кадр выходного маркера по оставшейся дистанции.
 * Вход: DS:SI = сущность; handler ID14.
 * Эффект/ограничения: Две таблицы по 32 элемента, индекс=(remaining>>3)&31.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_exit_marker, @function
update_tunnel_exit_marker:
    mov bx, WORD PTR ds:[D_tunnel_distance_remaining]                # CS:2FE8
    shr bx, 1                                                        # CS:2FEC
    shr bx, 1                                                        # CS:2FEE
    shr bx, 1                                                        # CS:2FF0
    and bx, 0x1f                                                     # CS:2FF2
    mov al, BYTE PTR [bx+D_tunnel_min_ring_radii]                    # CS:2FF5
    mov ds:[D_tunnel_min_radius], al                                 # CS:2FF9
    mov al, BYTE PTR [bx+D_tunnel_exit_frames]                       # CS:2FFC
    mov BYTE PTR [si+E_FRAME], al                                    # CS:3000
    mov bx, OFFSET D_sprite_frames_1929                              # CS:3003
    call apply_sprite_frame                                          # CS:3006
    ret                                                              # CS:3009

/* ========================================================================== 
 * update_tunnel_pickup  |  CS:300A
 * Назначение: По дистанции от появления вычисляет масштаб/позицию объекта; при совпадении сектора игрока собирает.
 * Вход: DS:SI = сущность; handler ID15.
 * Эффект/ограничения: state13..14 используются как WORD spawn_distance, не независимые row/column.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_pickup, @function
update_tunnel_pickup:
    mov ax, ds:[D_tunnel_distance]                                   # CS:300A
    sub ax, WORD PTR [si+E_STATE_13]                                 # CS:300D
    shr ax, 1                                                        # CS:3010
    shr ax, 1                                                        # CS:3012
    shr ax, 1                                                        # CS:3014
    ACC16 cmp, 0x2d                                                  # CS:3016
    jbe L_update_tunnel_pickup_301f                                  # CS:3019
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:301B
    ret                                                              # CS:301E
L_update_tunnel_pickup_301f:
    push ax                                                          # CS:301F
    call position_tunnel_pickup                                      # CS:3020
    pop ax                                                           # CS:3023
    shr ax, 1                                                        # CS:3024
    mov BYTE PTR [si+E_FRAME], al                                    # CS:3026
    {load} mov bx, ax                                                # CS:3029
    mov al, BYTE PTR [bx+D_tunnel_pickup_layers]                     # CS:302B
    mov BYTE PTR [si+E_LAYER], al                                    # CS:302F
    mov bx, OFFSET D_sprite_frames_1ac9                              # CS:3032
    call apply_sprite_frame                                          # CS:3035
    mov al, BYTE PTR [si+E_LAYER]                                    # CS:3038
    cmp al, 0x2                                                      # CS:303B
    je L_update_tunnel_pickup_3040                                   # CS:303D
    ret                                                              # CS:303F
L_update_tunnel_pickup_3040:
    mov bl, BYTE PTR ds:[D_player_frame]                             # CS:3040
    mov bh, 0x0                                                      # CS:3044
    mov al, BYTE PTR [bx+D_tunnel_frame_to_sector]                   # CS:3046
    cmp al, BYTE PTR [si+E_PARAM]                                    # CS:304A
    je L_update_tunnel_pickup_3050                                   # CS:304D
    ret                                                              # CS:304F
L_update_tunnel_pickup_3050:
    mov al, 0x9                                                      # CS:3050
    call sound_trigger                                               # CS:3052
    call collect_tunnel_pickup                                       # CS:3055
    ret                                                              # CS:3058

/* ========================================================================== 
 * collect_tunnel_pickup  |  CS:3059
 * Назначение: Удаляет объект и начисляет награду в зависимости от tunnel_reward_kind.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: 40h:+6 ammo, 80h:+4 fuel, C0h:+8 shield, иначе +2500 очков.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type collect_tunnel_pickup, @function
collect_tunnel_pickup:
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:3059
    mov al, ds:[D_tunnel_reward_kind]                                # CS:305C
    cmp al, 0x40                                                     # CS:305F
    jne L_collect_tunnel_pickup_306a                                 # CS:3061
    add WORD PTR ds:[D_ammo_delta], 0x6                              # CS:3063
    jmp L_collect_tunnel_pickup_3083                                 # CS:3068
L_collect_tunnel_pickup_306a:
    cmp al, 0x80                                                     # CS:306A
    jne L_collect_tunnel_pickup_3075                                 # CS:306C
    add WORD PTR ds:[D_fuel_delta], 0x4                              # CS:306E
    jmp L_collect_tunnel_pickup_3083                                 # CS:3073
L_collect_tunnel_pickup_3075:
    cmp al, 0xc0                                                     # CS:3075
    jne L_collect_tunnel_pickup_3080                                 # CS:3077
    add WORD PTR ds:[D_shield_delta], 0x8                            # CS:3079
    jmp L_collect_tunnel_pickup_3083                                 # CS:307E
L_collect_tunnel_pickup_3080:
    call score_add_2500                                              # CS:3080
L_collect_tunnel_pickup_3083:
    ret                                                              # CS:3083

/* ========================================================================== 
 * tunnel_angle_force  |  CS:3084
 * Назначение: Нелинейная добавка угловой скорости: после вычитания 5 проверяет SF, ограничивает до56, делит на2, возвращает -(x+3).
 * Вход: AL = нормализованный угол 0..63 у текущих caller.
 * Эффект/ограничения: AX = signed сила. Входы за пределами фактического домена требуют сохранения 8-битного wrap.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_angle_force, @function
tunnel_angle_force:

    # Форма силы использует 8-битный SUB и его SF. Для реальных входов0..63 выражение разъяснено в docs.
    sub al, 0x5                                                      # CS:3084
    jns L_tunnel_angle_force_308d                                    # CS:3086
    mov ax, 0x0                                                      # CS:3088
    jmp L_tunnel_angle_force_309c                                    # CS:308B
L_tunnel_angle_force_308d:
    cmp al, 0x38                                                     # CS:308D
    jbe L_tunnel_angle_force_3093                                    # CS:308F
    mov al, 0x38                                                     # CS:3091
L_tunnel_angle_force_3093:
    shr al, 1                                                        # CS:3093
    mov ah, 0x0                                                      # CS:3095
    neg ax                                                           # CS:3097
    ACC16 sub, 0x3                                                   # CS:3099
L_tunnel_angle_force_309c:
    ret                                                              # CS:309C

/* ========================================================================== 
 * advance_tunnel_distance  |  CS:309D
 * Назначение: Продвигает дистанцию, расходует топливо, создаёт объекты по секциям и возвращает на поверхность после 896.
 * Вход: scroll_speed; pattern stream.
 * Эффект/ограничения: Каждые 64 единицы новая секция; выходной маркер появляется в последних 256 единицах.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type advance_tunnel_distance, @function
advance_tunnel_distance:
    mov bx, WORD PTR ds:[D_scroll_speed]                             # CS:309D
    add bx, 0x7                                                      # CS:30A1
    shr bx, 1                                                        # CS:30A4
    shr bx, 1                                                        # CS:30A6
    shr bx, 1                                                        # CS:30A8
    add bx, 0x2                                                      # CS:30AA
    add WORD PTR ds:[D_tunnel_distance], bx                          # CS:30AD
    mov ax, ds:[D_fuel_distance_accumulator]                         # CS:30B1
    {load} add ax, bx                                                # CS:30B4
    ACC16 cmp, 0x2e8                                                 # CS:30B6
    jbe L_advance_tunnel_distance_30c4                               # CS:30B9
    mov ax, 0x0                                                      # CS:30BB
    mov WORD PTR ds:[D_fuel_delta], 0xffff                           # CS:30BE
L_advance_tunnel_distance_30c4:
    mov ds:[D_fuel_distance_accumulator], ax                         # CS:30C4
    mov ax, ds:[D_tunnel_distance]                                   # CS:30C7
    ACC16 sub, 0x380                                                 # CS:30CA
    jns L_advance_tunnel_distance_3124                               # CS:30CD
    neg ax                                                           # CS:30CF
    ACC16 cmp, 0x100                                                 # CS:30D1
    jae L_advance_tunnel_distance_30ed                               # CS:30D4
    mov ds:[D_tunnel_distance_remaining], ax                         # CS:30D6
    mov al, ds:[D_tunnel_exit_spawned]                               # CS:30D9
    cmp al, 0x0                                                      # CS:30DC
    jne L_advance_tunnel_distance_30eb                               # CS:30DE
    mov si, OFFSET D_template_tunnel_exit                            # CS:30E0
    call allocate_entity_copy                                        # CS:30E3
    mov BYTE PTR ds:[D_tunnel_exit_spawned], 0x1                     # CS:30E6
L_advance_tunnel_distance_30eb:
    jmp L_advance_tunnel_distance_3122                               # CS:30EB
L_advance_tunnel_distance_30ed:
    mov ax, ds:[D_tunnel_distance]                                   # CS:30ED
    shr ax, 1                                                        # CS:30F0
    shr ax, 1                                                        # CS:30F2
    shr ax, 1                                                        # CS:30F4
    shr ax, 1                                                        # CS:30F6
    shr ax, 1                                                        # CS:30F8
    shr ax, 1                                                        # CS:30FA
    cmp al, BYTE PTR ds:[D_tunnel_last_section]                      # CS:30FC
    je L_advance_tunnel_distance_3122                                # CS:3100
    mov ds:[D_tunnel_last_section], al                               # CS:3102
    {load} mov bl, al                                                # CS:3105
    mov bh, 0x0                                                      # CS:3107
    add bx, WORD PTR ds:[D_tunnel_pattern_pointer]                   # CS:3109
    mov al, BYTE PTR [bx]                                            # CS:310D
    cmp al, 0x0                                                      # CS:310F
    je L_advance_tunnel_distance_3122                                # CS:3111
    mov si, OFFSET D_template_tunnel_pickup                          # CS:3113
    mov BYTE PTR [si+0x1], al                                        # CS:3116
    mov ax, ds:[D_tunnel_distance]                                   # CS:3119
    mov WORD PTR [si+0x13], ax                                       # CS:311C
    call allocate_entity_copy                                        # CS:311F
L_advance_tunnel_distance_3122:
    jmp L_advance_tunnel_distance_312f                               # CS:3122
L_advance_tunnel_distance_3124:
    mov BYTE PTR ds:[D_tunnel_mode], 0x0                             # CS:3124
    mov ax, ds:[D_surface_resume_y]                                  # CS:3129
    mov ds:[D_camera_y], ax                                          # CS:312C
L_advance_tunnel_distance_312f:
    ret                                                              # CS:312F

/* ========================================================================== 
 * position_tunnel_pickup  |  CS:3130
 * Назначение: Берёт радиус по дистанции и выбирает один из 8 лучей вокруг центра.
 * Вход: AX = индекс масштаба 0..45; DS:SI = pickup, param=1..8.
 * Эффект/ограничения: Оси: центр x=128, y=106; диагонали не нормализованы через sqrt(2).
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type position_tunnel_pickup, @function
position_tunnel_pickup:
    {load} mov bl, al                                                # CS:3130
    mov bh, 0x0                                                      # CS:3132
    mov cl, BYTE PTR [bx+D_tunnel_pickup_radii]                      # CS:3134
    mov ch, 0x0                                                      # CS:3138
    mov al, BYTE PTR [si+0x1]                                        # CS:313A
    cmp al, 0x1                                                      # CS:313D
    je tunnel_set_north                                              # CS:313F
    cmp al, 0x2                                                      # CS:3141
    je L_tunnel_set_north_3167                                       # CS:3143
    cmp al, 0x3                                                      # CS:3145
    je tunnel_set_east                                               # CS:3147
    cmp al, 0x4                                                      # CS:3149
    je L_tunnel_set_east_3178                                        # CS:314B
    cmp al, 0x5                                                      # CS:314D
    je tunnel_set_south                                              # CS:314F
    cmp al, 0x6                                                      # CS:3151
    je L_tunnel_set_south_3188                                       # CS:3153
    cmp al, 0x7                                                      # CS:3155
    je tunnel_set_west                                               # CS:3157
    cmp al, 0x8                                                      # CS:3159
    je L_tunnel_set_west_319b                                        # CS:315B
    ret                                                              # CS:315D

/* ========================================================================== 
 * tunnel_set_north  |  CS:315E
 * Назначение: y=106-radius; также содержит диагональный хвост north-east.
 * Вход: DS:SI = pickup; CX = radius.
 * Эффект/ограничения: Дополнительные входы внутри позиционирования, не изолировать хвосты как независимый C-код.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_set_north, @function
tunnel_set_north:
    neg cx                                                           # CS:315E
    add cx, 0x6a                                                     # CS:3160
    mov WORD PTR [si+0x7], cx                                        # CS:3163
    ret                                                              # CS:3166
L_tunnel_set_north_3167:
    push cx                                                          # CS:3167
    call tunnel_set_north                                            # CS:3168
    pop cx                                                           # CS:316B
    call tunnel_set_east                                             # CS:316C
    ret                                                              # CS:316F

/* ========================================================================== 
 * tunnel_set_east  |  CS:3170
 * Назначение: x=128+radius; далее расположен хвост south-east.
 * Вход: DS:SI = pickup; CX = radius.
 * Эффект/ограничения: CX изменяется при записи; диагональные caller сохраняют исходный CX.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_set_east, @function
tunnel_set_east:
    add cx, 0x80                                                     # CS:3170
    mov WORD PTR [si+0x5], cx                                        # CS:3174
    ret                                                              # CS:3177
L_tunnel_set_east_3178:
    push cx                                                          # CS:3178
    call tunnel_set_east                                             # CS:3179
    pop cx                                                           # CS:317C
    call tunnel_set_south                                            # CS:317D
    ret                                                              # CS:3180

/* ========================================================================== 
 * tunnel_set_south  |  CS:3181
 * Назначение: y=106+radius; далее расположен хвост south-west.
 * Вход: DS:SI = pickup; CX = radius.
 * Эффект/ограничения: Общий набор позиционных помощников.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_set_south, @function
tunnel_set_south:
    add cx, 0x6a                                                     # CS:3181
    mov WORD PTR [si+0x7], cx                                        # CS:3184
    ret                                                              # CS:3187
L_tunnel_set_south_3188:
    push cx                                                          # CS:3188
    call tunnel_set_south                                            # CS:3189
    pop cx                                                           # CS:318C
    call tunnel_set_west                                             # CS:318D
    ret                                                              # CS:3190

/* ========================================================================== 
 * tunnel_set_west  |  CS:3191
 * Назначение: x=128-radius; далее расположен хвост north-west.
 * Вход: DS:SI = pickup; CX = radius.
 * Эффект/ограничения: Общий набор позиционных помощников.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_set_west, @function
tunnel_set_west:
    neg cx                                                           # CS:3191
    add cx, 0x80                                                     # CS:3193
    mov WORD PTR [si+0x5], cx                                        # CS:3197
    ret                                                              # CS:319A
L_tunnel_set_west_319b:
    push cx                                                          # CS:319B
    call tunnel_set_west                                             # CS:319C
    pop cx                                                           # CS:319F
    call tunnel_set_north                                            # CS:31A0
    ret                                                              # CS:31A3
