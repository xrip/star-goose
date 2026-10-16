/* Камера, скорость, рельеф и смерть игрока
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * sync_player_to_terrain  |  CS:1B57
 * Назначение: Держит игрока на camera_y+112, запрашивает рельеф и применяет sprite descriptor.
 * Вход: Глобальные player_x/camera_y.
 * Эффект/ограничения: Записывает player_y, z, map_row, frame; SI возвращается на первую запись.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sync_player_to_terrain, @function
sync_player_to_terrain:

    # Координата игрока на поверхности связана с camera_y; не путать render Y и абсолютное world Y.
    mov ax, ds:[D_player_x]                                          # CS:1B57
    push ax                                                          # CS:1B5A
    mov ax, ds:[D_camera_y]                                          # CS:1B5B
    ACC16 add, 0x70                                                  # CS:1B5E
    ACC16 and, WORLD_Y_MASK                                          # CS:1B61
    mov ds:[D_player_y], ax                                          # CS:1B64
    push ax                                                          # CS:1B67
    call sample_terrain                                              # CS:1B68
    mov ds:[D_player_z], al                                          # CS:1B6B
    mov BYTE PTR ds:[D_player_map_row], cl                           # CS:1B6E
    mov BYTE PTR ds:[D_player_frame], bl                             # CS:1B72
    mov si, OFFSET D_entity_records                                  # CS:1B76
    mov bx, OFFSET D_sprite_frames_0d49                              # CS:1B79
    call apply_sprite_frame                                          # CS:1B7C
    ret                                                              # CS:1B7F

/* ========================================================================== 
 * update_scroll_speed  |  CS:1B80
 * Назначение: Кнопки UP/DOWN меняют скорость; при отсутствии топлива принудительно выбирается DOWN.
 * Вход: scroll_speed, base_scroll_speed, fuel_units, controls.
 * Эффект/ограничения: UP: +1 до 64; DOWN: -2 не ниже base_scroll_speed.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_scroll_speed, @function
update_scroll_speed:

    # UP ускоряет до64, DOWN уменьшает на2 до базовой скорости; пустое топливо принудительно даёт DOWN.
    mov bx, WORD PTR ds:[D_scroll_speed]                             # CS:1B80
    mov ax, ds:[D_fuel_units]                                        # CS:1B84
    ACC16 cmp, 0x0                                                   # CS:1B87
    jg L_update_scroll_speed_1b91                                    # CS:1B8A
    mov ax, 0x8                                                      # CS:1B8C
    jmp L_update_scroll_speed_1b94                                   # CS:1B8F
L_update_scroll_speed_1b91:
    mov ax, ds:[D_control_bits]                                      # CS:1B91
L_update_scroll_speed_1b94:
    test al, 0x4                                                     # CS:1B94
    je L_update_scroll_speed_1ba3                                    # CS:1B96
    inc bx                                                           # CS:1B98
    cmp bx, 0x40                                                     # CS:1B99
    jle L_update_scroll_speed_1ba1                                   # CS:1B9C
    mov bx, 0x40                                                     # CS:1B9E
L_update_scroll_speed_1ba1:
    jmp L_update_scroll_speed_1bb4                                   # CS:1BA1
L_update_scroll_speed_1ba3:
    test al, 0x8                                                     # CS:1BA3
    je L_update_scroll_speed_1bb4                                    # CS:1BA5
    sub bx, 0x2                                                      # CS:1BA7
    cmp bx, WORD PTR ds:[D_base_scroll_speed]                        # CS:1BAA
    jge L_update_scroll_speed_1bb4                                   # CS:1BAE
    mov bx, WORD PTR ds:[D_base_scroll_speed]                        # CS:1BB0
L_update_scroll_speed_1bb4:
    mov WORD PTR ds:[D_scroll_speed], bx                             # CS:1BB4
    ret                                                              # CS:1BB8

/* ========================================================================== 
 * advance_surface_scroll  |  CS:1BB9
 * Назначение: Обновляет счётчик расхода топлива, camera_y modulo1024 и подгружает новую строку объектов.
 * Вход: scroll_speed.
 * Эффект/ограничения: Шаг=((speed+15)>>3)>>1; порядок округления и порог 139 сохранены.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type advance_surface_scroll, @function
advance_surface_scroll:

    # Дискретное движение камеры и аккумулятор расхода топлива используют разные шаги округления.
    mov bx, WORD PTR ds:[D_scroll_speed]                             # CS:1BB9
    add bx, 0xf                                                      # CS:1BBD
    shr bx, 1                                                        # CS:1BC0
    shr bx, 1                                                        # CS:1BC2
    shr bx, 1                                                        # CS:1BC4
    {load} mov ax, bx                                                # CS:1BC6
    dec ax                                                           # CS:1BC8
    shr bx, 1                                                        # CS:1BC9
    add ax, WORD PTR ds:[D_fuel_distance_accumulator]                # CS:1BCB
    ACC16 cmp, 0x8b                                                  # CS:1BCF
    jbe L_advance_surface_scroll_1bdd                                # CS:1BD2
    mov ax, 0x0                                                      # CS:1BD4
    mov WORD PTR ds:[D_fuel_delta], 0xffff                           # CS:1BD7
L_advance_surface_scroll_1bdd:
    mov ds:[D_fuel_distance_accumulator], ax                         # CS:1BDD
    mov ax, ds:[D_camera_y]                                          # CS:1BE0
    {load} sub ax, bx                                                # CS:1BE3
    ACC16 and, WORLD_Y_MASK                                          # CS:1BE5
    mov ds:[D_camera_y], ax                                          # CS:1BE8
    mov ax, ds:[D_camera_y]                                          # CS:1BEB
    shr ax, 1                                                        # CS:1BEE
    shr ax, 1                                                        # CS:1BF0
    shr ax, 1                                                        # CS:1BF2
    shr ax, 1                                                        # CS:1BF4
    ACC16 sub, 0x3                                                   # CS:1BF6
    ACC16 and, 0x3f                                                  # CS:1BF9
    cmp ax, WORD PTR ds:[D_last_spawn_row]                           # CS:1BFC
    je L_advance_surface_scroll_1c09                                 # CS:1C00
    mov ds:[D_last_spawn_row], ax                                    # CS:1C02
    push ax                                                          # CS:1C05
    call spawn_map_row                                               # CS:1C06
L_advance_surface_scroll_1c09:
    ret                                                              # CS:1C09

/* ========================================================================== 
 * find_map_entity  |  CS:1C0A
 * Назначение: Ищет запись с флагом MAP_OBJECT и совпадающими row/column.
 * Вход: [BP+4]=row; [BP+6]=column.
 * Эффект/ограничения: SI=найденная запись или адрес sentinel, не NULL. RET 4.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type find_map_entity, @function
find_map_entity:
    push bp                                                          # CS:1C0A
    {load} mov bp, sp                                                # CS:1C0B
    mov bx, WORD PTR [bp+0x6]                                        # CS:1C0D
    mov cx, WORD PTR [bp+0x4]                                        # CS:1C10
    mov si, OFFSET D_entity_records                                  # CS:1C13
L_find_map_entity_1c16:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1C16
    test al, 0x80                                                    # CS:1C18
    jne L_find_map_entity_1c2f                                       # CS:1C1A
    test al, 0x8                                                     # CS:1C1C
    je L_find_map_entity_1c2a                                        # CS:1C1E
    cmp cl, BYTE PTR [si+E_STATE_14]                                 # CS:1C20
    jne L_find_map_entity_1c2a                                       # CS:1C23
    cmp bl, BYTE PTR [si+E_STATE_13]                                 # CS:1C25
    je L_find_map_entity_1c2f                                        # CS:1C28
L_find_map_entity_1c2a:
    add si, ENTITY_SIZE                                              # CS:1C2A
    jmp L_find_map_entity_1c16                                       # CS:1C2D
L_find_map_entity_1c2f:
    pop bp                                                           # CS:1C2F
    ret 0x4                                                          # CS:1C30

/* ========================================================================== 
 * begin_player_death  |  CS:1C33
 * Назначение: Создаёт взрыв, переключает игрока на handler17, скрывает слой и сбрасывает death_age.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Повторный вызов при handler17 ничего не делает.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type begin_player_death, @function
begin_player_death:
    mov al, ds:[D_player_handler]                                    # CS:1C33
    cmp al, 0x11                                                     # CS:1C36
    jne L_begin_player_death_1c3b                                    # CS:1C38
    ret                                                              # CS:1C3A
L_begin_player_death_1c3b:
    mov al, 0xc                                                      # CS:1C3B
    call sound_trigger                                               # CS:1C3D
    mov si, OFFSET D_entity_records                                  # CS:1C40
    call spawn_explosion_for_entity                                  # CS:1C43
    mov BYTE PTR ds:[D_player_handler], 0x11                         # CS:1C46
    mov BYTE PTR ds:[D_player_layer], 0x0                            # CS:1C4B
    mov BYTE PTR ds:[D_player_death_age], 0x0                        # CS:1C50
    ret                                                              # CS:1C55

/* ========================================================================== 
 * apply_player_map_hit  |  CS:1C56
 * Назначение: Записывает отрицательный урон и преобразует найденный объект карты в разрушенное состояние.
 * Вход: AX=damage/code; BX=column; player_map_row.
 * Эффект/ограничения: shield_delta присваивается, не уменьшается на AX; вызываемые процедуры меняют SI.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type apply_player_map_hit, @function
apply_player_map_hit:
    neg ax                                                           # CS:1C56
    mov ds:[D_shield_delta], ax                                      # CS:1C58
    push ax                                                          # CS:1C5B
    mov al, 0x6                                                      # CS:1C5C
    call sound_trigger                                               # CS:1C5E
    pop ax                                                           # CS:1C61
    {load} mov ax, bx                                                # CS:1C62
    push ax                                                          # CS:1C64
    mov al, ds:[D_player_map_row]                                    # CS:1C65
    mov ah, 0x0                                                      # CS:1C68
    push ax                                                          # CS:1C6A
    call find_map_entity                                             # CS:1C6B
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1C6E
    test al, 0x80                                                    # CS:1C70
    jne L_apply_player_map_hit_1c77                                  # CS:1C72
    call destroy_map_entity                                          # CS:1C74
L_apply_player_map_hit_1c77:
    ret                                                              # CS:1C77

/* ========================================================================== 
 * push_player_history  |  CS:1C78
 * Назначение: Сдвигает четыре сохранённых x/y/frame/z и вставляет текущего игрока.
 * Вход: Глобальная запись player.
 * Эффект/ограничения: Это четыре параллельных массива, не четыре последовательные Entity.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type push_player_history, @function
push_player_history:
    mov ax, ds:[D_player_history_x2]                                 # CS:1C78
    mov ds:[D_player_history_x3], ax                                 # CS:1C7B
    mov ax, ds:[D_player_history_x1]                                 # CS:1C7E
    mov ds:[D_player_history_x2], ax                                 # CS:1C81
    mov ax, ds:[D_player_history_x0]                                 # CS:1C84
    mov ds:[D_player_history_x1], ax                                 # CS:1C87
    mov ax, ds:[D_player_x]                                          # CS:1C8A
    mov ds:[D_player_history_x0], ax                                 # CS:1C8D
    mov ax, ds:[D_player_history_y2]                                 # CS:1C90
    mov ds:[D_player_history_y3], ax                                 # CS:1C93
    mov ax, ds:[D_player_history_y1]                                 # CS:1C96
    mov ds:[D_player_history_y2], ax                                 # CS:1C99
    mov ax, ds:[D_player_history_y0]                                 # CS:1C9C
    mov ds:[D_player_history_y1], ax                                 # CS:1C9F
    mov ax, ds:[D_player_y]                                          # CS:1CA2
    mov ds:[D_player_history_y0], ax                                 # CS:1CA5
    mov al, ds:[D_player_history_frame2]                             # CS:1CA8
    mov ds:[D_player_history_frame3], al                             # CS:1CAB
    mov al, ds:[D_player_history_frame1]                             # CS:1CAE
    mov ds:[D_player_history_frame2], al                             # CS:1CB1
    mov al, ds:[D_player_history_frame0]                             # CS:1CB4
    mov ds:[D_player_history_frame1], al                             # CS:1CB7
    mov al, ds:[D_player_frame]                                      # CS:1CBA
    mov ds:[D_player_history_frame0], al                             # CS:1CBD
    mov al, ds:[D_player_history_z2]                                 # CS:1CC0
    mov ds:[D_player_history_z3], al                                 # CS:1CC3
    mov al, ds:[D_player_history_z1]                                 # CS:1CC6
    mov ds:[D_player_history_z2], al                                 # CS:1CC9
    mov al, ds:[D_player_history_z0]                                 # CS:1CCC
    mov ds:[D_player_history_z1], al                                 # CS:1CCF
    mov al, ds:[D_player_z]                                          # CS:1CD2
    mov ds:[D_player_history_z0], al                                 # CS:1CD5
    ret                                                              # CS:1CD8

/* ========================================================================== 
 * position_side_device  |  CS:1CD9
 * Назначение: Берёт самую старую из 4 позиций, смещает на ±8 по x и +6 по y/z, применяет descriptor.
 * Вход: DS:SI = боковое устройство; flags20h = сторона.
 * Эффект/ограничения: Учитывает рельефный наклон по таблице.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type position_side_device, @function
position_side_device:
    mov ax, ds:[D_player_history_x3]                                 # CS:1CD9
    mov WORD PTR [si+E_X], ax                                        # CS:1CDC
    mov ax, ds:[D_player_history_y3]                                 # CS:1CDF
    ACC16 add, 0x6                                                   # CS:1CE2
    ACC16 and, WORLD_Y_MASK                                          # CS:1CE5
    mov WORD PTR [si+E_Y], ax                                        # CS:1CE8
    mov al, ds:[D_player_history_frame3]                             # CS:1CEB
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1CEE
    {load} mov bl, al                                                # CS:1CF1
    mov bh, 0x0                                                      # CS:1CF3
    mov cl, BYTE PTR [bx+D_side_device_z_offsets]                    # CS:1CF5
    mov al, ds:[D_player_history_z3]                                 # CS:1CF9
    add al, 0x6                                                      # CS:1CFC
    mov BYTE PTR [si+E_Z], al                                        # CS:1CFE
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1D01
    test al, 0x20                                                    # CS:1D03
    jne L_position_side_device_1d10                                  # CS:1D05
    sub WORD PTR [si+E_X], 0x8                                       # CS:1D07
    add BYTE PTR [si+E_Z], cl                                        # CS:1D0B
    jmp L_position_side_device_1d17                                  # CS:1D0E
L_position_side_device_1d10:
    add WORD PTR [si+E_X], 0x8                                       # CS:1D10
    sub BYTE PTR [si+E_Z], cl                                        # CS:1D14
L_position_side_device_1d17:
    mov bx, OFFSET D_sprite_frames_0dda                              # CS:1D17
    call apply_sprite_frame                                          # CS:1D1A
    ret                                                              # CS:1D1D
