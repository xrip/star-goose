/* Движение игрока и кнопочные автоматы
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * update_player  |  CS:1108
 * Назначение: Обновляет скорость, камеру, горизонтальную инерцию, рельеф, столкновения и оружие игрока.
 * Вход: Глобальная запись player; маска управления.
 * Эффект/ограничения: Инерция ограничена [-24,24]; dx=(v+3) SAR 2; x ограничен [20,236]. Боковые действия — автоматы 0/1/2/3.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_player, @function
update_player:

    # Скорость прокрутки и положение камеры обновляются ПЕРЕД боковым движением игрока.
    call update_scroll_speed                                         # CS:1108
    call advance_surface_scroll                                      # CS:110B
    mov bx, WORD PTR ds:[D_control_bits]                             # CS:110E
    mov ax, ds:[D_player_x_velocity]                                 # CS:1112

    # LEFT: ускорение -1, быстрая смена направления через +4, нижний предел -24.
    test bl, CONTROL_LEFT                                                     # CS:1115
    je L_player_check_right                                          # CS:1118
    ACC16 sub, 0x1                                                   # CS:111A
    js L_update_player_1127                                          # CS:111D
    ACC16 cmp, 0x4                                                   # CS:111F
    jle L_update_player_1127                                         # CS:1122
    mov ax, 0x4                                                      # CS:1124
L_update_player_1127:
    ACC16 cmp, 0xffe8                                                # CS:1127
    jge L_update_player_112f                                         # CS:112A
    mov ax, 0xffe8                                                   # CS:112C
L_update_player_112f:
    jmp L_player_integrate_x                                         # CS:112F
L_player_check_right:

    # RIGHT: зеркальная ветвь с порогом -4 и верхним пределом +24.
    test bl, CONTROL_RIGHT                                                     # CS:1131
    je L_player_apply_drag                                           # CS:1134
    ACC16 add, 0x1                                                   # CS:1136
    jns L_update_player_1143                                         # CS:1139
    ACC16 cmp, 0xfffc                                                # CS:113B
    jge L_update_player_1143                                         # CS:113E
    mov ax, 0xfffc                                                   # CS:1140
L_update_player_1143:
    ACC16 cmp, 0x18                                                  # CS:1143
    jle L_update_player_114b                                         # CS:1146
    mov ax, 0x18                                                     # CS:1148
L_update_player_114b:
    jmp L_player_integrate_x                                         # CS:114B
L_player_apply_drag:

    # Без направления скорость затухает к нулю на одну единицу за кадр.
    ACC16 cmp, 0x0                                                   # CS:114D
    je L_player_integrate_x                                          # CS:1150
    jle L_update_player_1157                                         # CS:1152
    dec ax                                                           # CS:1154
    jmp L_player_integrate_x                                         # CS:1155
L_update_player_1157:
    inc ax                                                           # CS:1157
L_player_integrate_x:
    mov ds:[D_player_x_velocity], ax                                 # CS:1158

    # dx = arithmetic_shift_right(v+3,2), НЕ (v+3)/4 с C-округлением к нулю.
    ACC16 add, 0x3                                                   # CS:115B
    sar ax, 1                                                        # CS:115E
    sar ax, 1                                                        # CS:1160
    add ax, WORD PTR ds:[D_player_x]                                 # CS:1162

    # X ограничен20..236. При упоре в край боковая скорость сбрасывается.
    ACC16 cmp, 0x14                                                  # CS:1166
    jge L_update_player_1174                                         # CS:1169
    mov ax, 0x14                                                     # CS:116B
    mov WORD PTR ds:[D_player_x_velocity], 0x0                       # CS:116E
L_update_player_1174:
    ACC16 cmp, 0xec                                                  # CS:1174
    jle L_player_update_terrain                                      # CS:1177
    mov ax, 0xec                                                     # CS:1179
    mov WORD PTR ds:[D_player_x_velocity], 0x0                       # CS:117C
L_player_update_terrain:
    mov ds:[D_player_x], ax                                          # CS:1182
    mov al, ds:[D_player_frame]                                      # CS:1185
    cmp al, 0x0                                                      # CS:1188
    je L_update_player_118f                                          # CS:118A
    call sync_player_to_terrain                                      # CS:118C
L_update_player_118f:
    call push_player_history                                         # CS:118F
    call query_player_map_collision                                  # CS:1192
    ACC16 cmp, 0x0                                                   # CS:1195
    je L_update_player_119f                                          # CS:1198
    call apply_player_map_hit                                        # CS:119A
    jmp L_update_player_11bc                                         # CS:119D
L_update_player_119f:

    # Столкновение дополнительно проверяет соседнюю строку карты; затем восстанавливает player_map_row.
    inc BYTE PTR ds:[D_player_map_row]                               # CS:119F
    and BYTE PTR ds:[D_player_map_row], 0x3f                         # CS:11A3
    call query_player_map_collision                                  # CS:11A8
    ACC16 cmp, 0x0                                                   # CS:11AB
    je L_update_player_11b3                                          # CS:11AE
    call apply_player_map_hit                                        # CS:11B0
L_update_player_11b3:
    dec BYTE PTR ds:[D_player_map_row]                               # CS:11B3
    and BYTE PTR ds:[D_player_map_row], 0x3f                         # CS:11B7
L_update_player_11bc:
    mov ax, ds:[D_shield_units]                                      # CS:11BC
    ACC16 cmp, 0x0                                                   # CS:11BF
    jg L_player_check_void                                           # CS:11C2
    call begin_player_death                                          # CS:11C4
    ret                                                              # CS:11C7
L_player_check_void:
    mov al, ds:[D_player_frame]                                      # CS:11C8
    cmp al, 0x0                                                      # CS:11CB
    jne L_player_weapons                                             # CS:11CD
    inc BYTE PTR ds:[D_player_zero_frame_age]                        # CS:11CF
    mov al, ds:[D_player_zero_frame_age]                             # CS:11D3
    cmp al, 0x4                                                      # CS:11D6
    jb L_update_player_11dd                                          # CS:11D8
    call begin_player_death                                          # CS:11DA
L_update_player_11dd:
    ret                                                              # CS:11DD
L_player_weapons:
    mov ax, ds:[D_ammo_units]                                        # CS:11DE
    ACC16 cmp, 0x0                                                   # CS:11E1
    jle L_player_left_device                                         # CS:11E4
    mov ax, ds:[D_control_bits]                                      # CS:11E6
    test al, 0x80                                                    # CS:11E9
    je L_player_left_device                                          # CS:11EB
    call fire_player_weapons                                         # CS:11ED
L_player_left_device:

    # Левое устройство: press0→1, release1→2, press2→3, release3→0; это не обычный bool.
    mov ax, ds:[D_control_bits]                                      # CS:11F0
    mov bl, BYTE PTR ds:[D_left_device_state]                        # CS:11F3
    test al, 0x40                                                    # CS:11F7
    je L_player_left_release                                         # CS:11F9
    cmp bl, 0x0                                                      # CS:11FB
    jne L_update_player_1224                                         # CS:11FE
    mov ax, ds:[D_side_charge_units]                                 # CS:1200
    ACC16 cmp, 0x0                                                   # CS:1203
    jbe L_update_player_1222                                         # CS:1206
    mov BYTE PTR ds:[D_left_device_state], 0x1                       # CS:1208
    mov si, OFFSET D_template_side_device                            # CS:120D
    and BYTE PTR [si+E_FLAGS], 0xdf                                  # CS:1210
    call position_side_device                                        # CS:1213
    call allocate_entity_copy                                        # CS:1216
    dec WORD PTR ds:[D_side_charge_delta]                            # CS:1219
    mov al, 0x1                                                      # CS:121D
    call sound_trigger                                               # CS:121F
L_update_player_1222:
    jmp L_update_player_1233                                         # CS:1222
L_update_player_1224:
    cmp bl, 0x2                                                      # CS:1224
    jne L_update_player_1233                                         # CS:1227
    mov BYTE PTR ds:[D_left_device_state], 0x3                       # CS:1229
    mov al, 0x2                                                      # CS:122E
    call sound_trigger                                               # CS:1230
L_update_player_1233:
    jmp L_player_right_device                                        # CS:1233
L_player_left_release:
    cmp bl, 0x1                                                      # CS:1235
    jne L_update_player_1241                                         # CS:1238
    mov BYTE PTR ds:[D_left_device_state], 0x2                       # CS:123A
    jmp L_player_right_device                                        # CS:123F
L_update_player_1241:
    cmp bl, 0x3                                                      # CS:1241
    jne L_player_right_device                                        # CS:1244
    mov BYTE PTR ds:[D_left_device_state], 0x0                       # CS:1246
L_player_right_device:

    # Правое устройство имеет отдельный такой же автомат; оба влияют на общий запас зарядов.
    mov ax, ds:[D_control_bits]                                      # CS:124B
    mov bl, BYTE PTR ds:[D_right_device_state]                       # CS:124E
    test al, 0x20                                                    # CS:1252
    je L_player_right_release                                        # CS:1254
    cmp bl, 0x0                                                      # CS:1256
    jne L_update_player_127f                                         # CS:1259
    mov ax, ds:[D_side_charge_units]                                 # CS:125B
    ACC16 cmp, 0x0                                                   # CS:125E
    jbe L_update_player_127d                                         # CS:1261
    mov BYTE PTR ds:[D_right_device_state], 0x1                      # CS:1263
    mov si, OFFSET D_template_side_device                            # CS:1268
    or BYTE PTR [si+E_FLAGS], 0x20                                   # CS:126B
    call position_side_device                                        # CS:126E
    call allocate_entity_copy                                        # CS:1271
    dec WORD PTR ds:[D_side_charge_delta]                            # CS:1274
    mov al, 0x1                                                      # CS:1278
    call sound_trigger                                               # CS:127A
L_update_player_127d:
    jmp L_update_player_128e                                         # CS:127D
L_update_player_127f:
    cmp bl, 0x2                                                      # CS:127F
    jne L_update_player_128e                                         # CS:1282
    mov BYTE PTR ds:[D_right_device_state], 0x3                      # CS:1284
    mov al, 0x2                                                      # CS:1289
    call sound_trigger                                               # CS:128B
L_update_player_128e:
    jmp L_update_player_12a6                                         # CS:128E
L_player_right_release:
    cmp bl, 0x1                                                      # CS:1290
    jne L_update_player_129c                                         # CS:1293
    mov BYTE PTR ds:[D_right_device_state], 0x2                      # CS:1295
    jmp L_update_player_12a6                                         # CS:129A
L_update_player_129c:
    cmp bl, 0x3                                                      # CS:129C
    jne L_update_player_12a6                                         # CS:129F
    mov BYTE PTR ds:[D_right_device_state], 0x0                      # CS:12A1
L_update_player_12a6:
    ret                                                              # CS:12A6
