/* Обработчики врагов, снарядов и эффектов
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * update_player_forced_stop  |  CS:12A7
 * Назначение: Уменьшает скорость на 3, при отрицательном результате запускает смерть; удерживает кадр 10.
 * Вход: Глобальная запись игрока; handler ID 35.
 * Эффект/ограничения: Не обычная физика игрока; отдельное состояние после перехода.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_player_forced_stop, @function
update_player_forced_stop:
    mov ax, ds:[D_scroll_speed]                                      # CS:12A7
    ACC16 sub, 0x3                                                   # CS:12AA
    js L_update_player_forced_stop_12b4                              # CS:12AD
    mov ds:[D_scroll_speed], ax                                      # CS:12AF
    jmp L_update_player_forced_stop_12b7                             # CS:12B2
L_update_player_forced_stop_12b4:
    call begin_player_death                                          # CS:12B4
L_update_player_forced_stop_12b7:
    call advance_surface_scroll                                      # CS:12B7
    call sync_player_to_terrain                                      # CS:12BA
    mov si, OFFSET D_entity_records                                  # CS:12BD
    mov bx, OFFSET D_sprite_frames_0d49                              # CS:12C0
    mov BYTE PTR [si+E_FRAME], 0xa                                   # CS:12C3
    call apply_sprite_frame                                          # CS:12C7
    ret                                                              # CS:12CA

/* ========================================================================== 
 * update_player_death  |  CS:12CB
 * Назначение: Замедляет прокрутку, обновляет положение и по достижении 40 тиков запрашивает respawn.
 * Вход: Глобальные счётчики; handler ID 17.
 * Эффект/ограничения: Смерть не означает немедленное удаление первого слота.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_player_death, @function
update_player_death:
    mov ax, ds:[D_scroll_speed]                                      # CS:12CB
    dec ax                                                           # CS:12CE
    js L_update_player_death_12d4                                    # CS:12CF
    mov ds:[D_scroll_speed], ax                                      # CS:12D1
L_update_player_death_12d4:
    call advance_surface_scroll                                      # CS:12D4
    call sync_player_to_terrain                                      # CS:12D7
    inc BYTE PTR ds:[D_player_death_age]                             # CS:12DA
    mov al, ds:[D_player_death_age]                                  # CS:12DE
    cmp al, 0x28                                                     # CS:12E1
    jb L_update_player_death_12ea                                    # CS:12E3
    mov BYTE PTR ds:[D_player_respawn_pending], 0x1                  # CS:12E5
L_update_player_death_12ea:
    ret                                                              # CS:12EA

/* ========================================================================== 
 * update_player_projectile  |  CS:12EB
 * Назначение: Двигает выстрел по y на -16, меняет высоту, проверяет рельеф и объект строки.
 * Вход: DS:SI = сущность; handler ID 19.
 * Эффект/ограничения: Может изменить цель, счёт, звук и заменить выстрел эффектом.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_player_projectile, @function
update_player_projectile:
    call cull_entity_before_camera                                   # CS:12EB
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:12EE
    cmp al, 0x0                                                      # CS:12F0
    jne L_update_player_projectile_12f5                              # CS:12F2
    ret                                                              # CS:12F4
L_update_player_projectile_12f5:
    sub WORD PTR [si+E_Y], 0x10                                      # CS:12F5
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:12F9
    mov al, BYTE PTR [si+E_FRAME]                                    # CS:12FE
    cmp al, 0x1                                                      # CS:1301
    jne L_update_player_projectile_1312                              # CS:1303
    mov al, BYTE PTR [si+E_Z]                                        # CS:1305
    cmp al, 0x9c                                                     # CS:1308
    jle L_update_player_projectile_1310                              # CS:130A
    sub BYTE PTR [si+E_Z], 0x8                                       # CS:130C
L_update_player_projectile_1310:
    jmp L_update_player_projectile_1321                              # CS:1310
L_update_player_projectile_1312:
    cmp al, 0x2                                                      # CS:1312
    jne L_update_player_projectile_1321                              # CS:1314
    mov al, BYTE PTR [si+E_Z]                                        # CS:1316
    cmp al, 0x64                                                     # CS:1319
    jge L_update_player_projectile_1321                              # CS:131B
    add BYTE PTR [si+E_Z], 0x8                                       # CS:131D
L_update_player_projectile_1321:
    push si                                                          # CS:1321
    mov ax, WORD PTR [si+E_X]                                        # CS:1322
    push ax                                                          # CS:1325
    mov ax, WORD PTR [si+E_Y]                                        # CS:1326
    push ax                                                          # CS:1329
    call sample_terrain                                              # CS:132A
    pop si                                                           # CS:132D
    cmp al, BYTE PTR [si+E_Z]                                        # CS:132E
    jl L_update_player_projectile_1338                               # CS:1331
    call replace_with_impact                                         # CS:1333
    jmp L_update_player_projectile_1383                              # CS:1336
L_update_player_projectile_1338:
    push si                                                          # CS:1338
    mov ax, WORD PTR [si+E_X]                                        # CS:1339
    push ax                                                          # CS:133C
    mov BYTE PTR [si+E_STATE_14], cl                                 # CS:133D
    push cx                                                          # CS:1340
    call query_map_collision_at                                      # CS:1341
    pop si                                                           # CS:1344
    ACC16 cmp, 0x0                                                   # CS:1345
    je L_update_player_projectile_1383                               # CS:1348
    push si                                                          # CS:134A
    mov al, BYTE PTR [si+E_STATE_14]                                 # CS:134B
    mov ah, 0x0                                                      # CS:134E
    push bx                                                          # CS:1350
    push ax                                                          # CS:1351
    call find_map_entity                                             # CS:1352
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1355
    test al, 0x80                                                    # CS:1357
    jne L_update_player_projectile_1382                              # CS:1359
    pop bx                                                           # CS:135B
    push bx                                                          # CS:135C
    mov al, BYTE PTR [si+E_Z]                                        # CS:135D
    sub al, BYTE PTR [bx+0x3]                                        # CS:1360
    jns L_update_player_projectile_1367                              # CS:1363
    neg al                                                           # CS:1365
L_update_player_projectile_1367:
    cmp al, 0x10                                                     # CS:1367
    jae L_update_player_projectile_1382                              # CS:1369
    dec BYTE PTR [si+E_STATE_15]                                     # CS:136B
    jns L_update_player_projectile_137a                              # CS:136E
    call destroy_map_entity                                          # CS:1370
    mov al, 0x6                                                      # CS:1373
    call sound_trigger                                               # CS:1375
    jmp L_update_player_projectile_137d                              # CS:1378
L_update_player_projectile_137a:
    call score_add_10                                                # CS:137A
L_update_player_projectile_137d:
    pop si                                                           # CS:137D
    push si                                                          # CS:137E
    call replace_with_side_impact                                    # CS:137F
L_update_player_projectile_1382:
    pop si                                                           # CS:1382
L_update_player_projectile_1383:
    ret                                                              # CS:1383

/* ========================================================================== 
 * update_side_device_follow  |  CS:1384
 * Назначение: Следует истории игрока; при состоянии кнопки 3 переключается на автономный handler 25.
 * Вход: DS:SI = сущность; handler ID 24.
 * Эффект/ограничения: Бит 20h выбирает левое/правое устройство; при смерти игрока отключается.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_side_device_follow, @function
update_side_device_follow:
    mov al, ds:[D_player_handler]                                    # CS:1384
    cmp al, 0x11                                                     # CS:1387
    jne L_update_side_device_follow_138e                             # CS:1389
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:138B
L_update_side_device_follow_138e:
    call position_side_device                                        # CS:138E
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1391
    test al, 0x20                                                    # CS:1393
    jne L_update_side_device_follow_139c                             # CS:1395
    mov al, ds:[D_left_device_state]                                 # CS:1397
    jmp L_update_side_device_follow_139f                             # CS:139A
L_update_side_device_follow_139c:
    mov al, ds:[D_right_device_state]                                # CS:139C
L_update_side_device_follow_139f:
    cmp al, 0x3                                                      # CS:139F
    jne L_update_side_device_follow_13a7                             # CS:13A1
    mov BYTE PTR [si+E_HANDLER], 0x19                                # CS:13A3
L_update_side_device_follow_13a7:
    ret                                                              # CS:13A7

/* ========================================================================== 
 * update_ground_projectile  |  CS:13A8
 * Назначение: Продвигает объект на -6 по y, привязывает к рельефу, проверяет препятствия и срок жизни.
 * Вход: DS:SI = сущность; handler ID 25.
 * Эффект/ограничения: В состоянии frame=0 корректирует y обратно на +4 и уменьшает state_15.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_ground_projectile, @function
update_ground_projectile:
    call cull_entity_before_camera                                   # CS:13A8
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:13AB
    cmp al, 0x0                                                      # CS:13AD
    jne L_update_ground_projectile_13b2                              # CS:13AF
    ret                                                              # CS:13B1
L_update_ground_projectile_13b2:
    sub WORD PTR [si+E_Y], 0x6                                       # CS:13B2
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:13B6
    push si                                                          # CS:13BB
    mov ax, WORD PTR [si+E_X]                                        # CS:13BC
    push ax                                                          # CS:13BF
    mov ax, WORD PTR [si+E_Y]                                        # CS:13C0
    push ax                                                          # CS:13C3
    call sample_terrain                                              # CS:13C4
    pop si                                                           # CS:13C7
    add al, 0x4                                                      # CS:13C8
    mov BYTE PTR [si+E_Z], al                                        # CS:13CA
    mov BYTE PTR [si+E_STATE_14], cl                                 # CS:13CD
    mov BYTE PTR [si+E_FRAME], bl                                    # CS:13D0
    mov bx, OFFSET D_sprite_frames_0dda                              # CS:13D3
    call apply_sprite_frame                                          # CS:13D6
    mov al, BYTE PTR [si+E_FRAME]                                    # CS:13D9
    cmp al, 0x0                                                      # CS:13DC
    jne L_update_ground_projectile_13f5                              # CS:13DE
    add WORD PTR [si+E_Y], 0x4                                       # CS:13E0
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:13E4
    dec BYTE PTR [si+E_STATE_15]                                     # CS:13E9
    jns L_update_ground_projectile_13f5                              # CS:13EC
    call spawn_explosion_for_entity                                  # CS:13EE
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:13F1
    ret                                                              # CS:13F4
L_update_ground_projectile_13f5:
    push si                                                          # CS:13F5
    mov ax, WORD PTR [si+E_X]                                        # CS:13F6
    push ax                                                          # CS:13F9
    mov cl, BYTE PTR [si+E_STATE_14]                                 # CS:13FA
    mov ch, 0x0                                                      # CS:13FD
    push cx                                                          # CS:13FF
    call query_map_collision_at                                      # CS:1400
    pop si                                                           # CS:1403
    ACC16 cmp, 0x0                                                   # CS:1404
    je L_update_ground_projectile_1428                               # CS:1407
    push si                                                          # CS:1409
    mov al, BYTE PTR [si+E_STATE_14]                                 # CS:140A
    mov ah, 0x0                                                      # CS:140D
    push bx                                                          # CS:140F
    push ax                                                          # CS:1410
    call find_map_entity                                             # CS:1411
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1414
    test al, 0x80                                                    # CS:1416
    jne L_update_ground_projectile_1427                              # CS:1418
    call destroy_map_entity                                          # CS:141A
    mov al, 0x6                                                      # CS:141D
    call sound_trigger                                               # CS:141F
    pop si                                                           # CS:1422
    push si                                                          # CS:1423
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:1424
L_update_ground_projectile_1427:
    pop si                                                           # CS:1427
L_update_ground_projectile_1428:
    ret                                                              # CS:1428

/* ========================================================================== 
 * update_ground_enemy_shot  |  CS:1429
 * Назначение: Двигает наземный выстрел в одном из 5 направлений и проверяет близость игрока.
 * Вход: DS:SI = сущность; handler ID 29.
 * Эффект/ограничения: Направление в param; y modulo 1024; попадание даёт shield_delta -=100.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_ground_enemy_shot, @function
update_ground_enemy_shot:
    call cull_entity_after_view                                      # CS:1429
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:142C
    cmp al, 0x0                                                      # CS:142E
    jne L_update_ground_enemy_shot_1433                              # CS:1430
    ret                                                              # CS:1432
L_update_ground_enemy_shot_1433:
    mov al, BYTE PTR [si+E_PARAM]                                    # CS:1433
    cmp al, 0x2                                                      # CS:1436
    jne L_update_ground_enemy_shot_1440                              # CS:1438
    add WORD PTR [si+E_Y], 0x5                                       # CS:143A
    jmp L_update_ground_enemy_shot_146a                              # CS:143E
L_update_ground_enemy_shot_1440:
    cmp al, 0x0                                                      # CS:1440
    jne L_update_ground_enemy_shot_144a                              # CS:1442
    sub WORD PTR [si+E_X], 0x5                                       # CS:1444
    jmp L_update_ground_enemy_shot_146a                              # CS:1448
L_update_ground_enemy_shot_144a:
    cmp al, 0x1                                                      # CS:144A
    jne L_update_ground_enemy_shot_1458                              # CS:144C
    sub WORD PTR [si+E_X], 0x3                                       # CS:144E
    add WORD PTR [si+E_Y], 0x2                                       # CS:1452
    jmp L_update_ground_enemy_shot_146a                              # CS:1456
L_update_ground_enemy_shot_1458:
    cmp al, 0x3                                                      # CS:1458
    jne L_update_ground_enemy_shot_1466                              # CS:145A
    add WORD PTR [si+E_X], 0x3                                       # CS:145C
    add WORD PTR [si+E_Y], 0x2                                       # CS:1460
    jmp L_update_ground_enemy_shot_146a                              # CS:1464
L_update_ground_enemy_shot_1466:
    add WORD PTR [si+E_X], 0x5                                       # CS:1466
L_update_ground_enemy_shot_146a:
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:146A
    mov ax, WORD PTR [si+E_X]                                        # CS:146F
    ACC16 cmp, 0xc                                                   # CS:1472
    jge L_update_ground_enemy_shot_147b                              # CS:1475
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:1477
    ret                                                              # CS:147A
L_update_ground_enemy_shot_147b:
    ACC16 cmp, 0xf4                                                  # CS:147B
    jle L_update_ground_enemy_shot_1484                              # CS:147E
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:1480
    ret                                                              # CS:1483
L_update_ground_enemy_shot_1484:
    push si                                                          # CS:1484
    mov ax, WORD PTR [si+E_X]                                        # CS:1485
    push ax                                                          # CS:1488
    mov ax, WORD PTR [si+E_Y]                                        # CS:1489
    push ax                                                          # CS:148C
    call sample_terrain                                              # CS:148D
    pop si                                                           # CS:1490
    add al, 0x8                                                      # CS:1491
    mov BYTE PTR [si+E_Z], al                                        # CS:1493
    mov BYTE PTR [si+E_STATE_14], cl                                 # CS:1496
    mov BYTE PTR [si+E_FRAME], bl                                    # CS:1499
    mov bl, BYTE PTR [si+E_PARAM]                                    # CS:149C
    mov bh, 0x0                                                      # CS:149F
    shl bx, 1                                                        # CS:14A1
    mov bx, WORD PTR [bx+D_ground_shot_descriptors]                  # CS:14A3
    call apply_sprite_frame                                          # CS:14A7
    mov al, ds:[D_player_handler]                                    # CS:14AA
    cmp al, 0x11                                                     # CS:14AD
    jne L_update_ground_enemy_shot_14b2                              # CS:14AF
    ret                                                              # CS:14B1
L_update_ground_enemy_shot_14b2:
    mov ax, WORD PTR [si+E_X]                                        # CS:14B2
    sub ax, WORD PTR ds:[D_player_x]                                 # CS:14B5
    jns L_update_ground_enemy_shot_14bd                              # CS:14B9
    neg ax                                                           # CS:14BB
L_update_ground_enemy_shot_14bd:
    ACC16 cmp, 0x10                                                  # CS:14BD
    jae L_update_ground_enemy_shot_14ef                              # CS:14C0
    mov ax, WORD PTR [si+E_Y]                                        # CS:14C2
    sub ax, WORD PTR ds:[D_player_y]                                 # CS:14C5
    ACC16 sub, 0x8                                                   # CS:14C9
    jns L_update_ground_enemy_shot_14d0                              # CS:14CC
    neg ax                                                           # CS:14CE
L_update_ground_enemy_shot_14d0:
    ACC16 cmp, 0x10                                                  # CS:14D0
    jae L_update_ground_enemy_shot_14ef                              # CS:14D3
    mov al, 0x5                                                      # CS:14D5
    call sound_trigger                                               # CS:14D7
    sub WORD PTR ds:[D_shield_delta], 0x64                           # CS:14DA
    mov BYTE PTR ds:[D_explosion_template_layer], 0x1                # CS:14DF
    call spawn_explosion_for_entity                                  # CS:14E4
    mov BYTE PTR ds:[D_explosion_template_layer], 0x5                # CS:14E7
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:14EC
L_update_ground_enemy_shot_14ef:
    ret                                                              # CS:14EF

/* ========================================================================== 
 * update_mobile_enemy  |  CS:14F0
 * Назначение: Двигает сущность на +1 по y, привязывает к рельефу, проверяет игрока и его снаряды, стреляет.
 * Вход: DS:SI = сущность; handler ID 26.
 * Эффект/ограничения: state_15 — знаковый счётчик попаданий; bit40h других сущностей определяет кандидатов столкновения.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_mobile_enemy, @function
update_mobile_enemy:
    add WORD PTR [si+E_Y], 0x1                                       # CS:14F0
    push si                                                          # CS:14F4
    mov ax, WORD PTR [si+E_X]                                        # CS:14F5
    push ax                                                          # CS:14F8
    mov ax, WORD PTR [si+E_Y]                                        # CS:14F9
    push ax                                                          # CS:14FC
    call sample_terrain                                              # CS:14FD
    pop si                                                           # CS:1500
    mov BYTE PTR [si+E_Z], al                                        # CS:1501
    mov BYTE PTR [si+E_STATE_14], cl                                 # CS:1504
    mov BYTE PTR [si+E_FRAME], bl                                    # CS:1507
    mov bl, BYTE PTR [si+E_PARAM]                                    # CS:150A
    mov bh, 0x0                                                      # CS:150D
    shl bx, 1                                                        # CS:150F
    mov bx, WORD PTR [bx+D_mobile_enemy_descriptors]                 # CS:1511
    call apply_sprite_frame                                          # CS:1515
    mov al, ds:[D_player_handler]                                    # CS:1518
    cmp al, 0x11                                                     # CS:151B
    je L_update_mobile_enemy_1548                                    # CS:151D
    mov ax, WORD PTR [si+E_X]                                        # CS:151F
    sub ax, WORD PTR ds:[D_player_x]                                 # CS:1522
    jns L_update_mobile_enemy_152a                                   # CS:1526
    neg ax                                                           # CS:1528
L_update_mobile_enemy_152a:
    ACC16 cmp, 0x1a                                                  # CS:152A
    jae L_update_mobile_enemy_1548                                   # CS:152D
    mov ax, WORD PTR [si+E_Y]                                        # CS:152F
    sub ax, WORD PTR ds:[D_player_y]                                 # CS:1532
    jns L_update_mobile_enemy_153a                                   # CS:1536
    neg ax                                                           # CS:1538
L_update_mobile_enemy_153a:
    ACC16 cmp, 0x12                                                  # CS:153A
    jae L_update_mobile_enemy_1548                                   # CS:153D
    sub WORD PTR ds:[D_shield_delta], 0xc                            # CS:153F
    mov BYTE PTR [si+E_STATE_15], 0xff                               # CS:1544
L_update_mobile_enemy_1548:
    mov di, 0x1ba1                                                   # CS:1548
L_update_mobile_enemy_154b:
    mov al, BYTE PTR [si+E_STATE_15]                                 # CS:154B
    cmp al, 0x0                                                      # CS:154E
    jge L_update_mobile_enemy_1561                                   # CS:1550
    mov al, 0x7                                                      # CS:1552
    call sound_trigger                                               # CS:1554
    call spawn_explosion_for_entity                                  # CS:1557
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:155A
    call score_add_150                                               # CS:155D
    ret                                                              # CS:1560
L_update_mobile_enemy_1561:
    mov al, BYTE PTR [di]                                            # CS:1561
    test al, 0x80                                                    # CS:1563
    jne L_update_mobile_enemy_15c0                                   # CS:1565
    test al, 0x40                                                    # CS:1567
    je L_update_mobile_enemy_15bb                                    # CS:1569
    push di                                                          # CS:156B
    mov ax, WORD PTR [di+0x5]                                        # CS:156C
    sub ax, WORD PTR [si+E_X]                                        # CS:156F
    jns L_update_mobile_enemy_1576                                   # CS:1572
    neg ax                                                           # CS:1574
L_update_mobile_enemy_1576:
    ACC16 cmp, 0x14                                                  # CS:1576
    jae L_update_mobile_enemy_15ba                                   # CS:1579
    mov ax, WORD PTR [di+0x7]                                        # CS:157B
    sub ax, WORD PTR [si+E_Y]                                        # CS:157E
    jns L_update_mobile_enemy_1585                                   # CS:1581
    neg ax                                                           # CS:1583
L_update_mobile_enemy_1585:
    ACC16 cmp, 0x10                                                  # CS:1585
    jae L_update_mobile_enemy_15ba                                   # CS:1588
    mov al, BYTE PTR [di+0x12]                                       # CS:158A
    cmp al, 0x19                                                     # CS:158D
    jne L_update_mobile_enemy_159a                                   # CS:158F
    mov BYTE PTR [si+E_STATE_15], 0xff                               # CS:1591
    mov BYTE PTR [di], 0x0                                           # CS:1595
    jmp L_update_mobile_enemy_15ba                                   # CS:1598
L_update_mobile_enemy_159a:
    mov al, BYTE PTR [di+0x3]                                        # CS:159A
    sub al, BYTE PTR [si+E_Z]                                        # CS:159D
    jns L_update_mobile_enemy_15a4                                   # CS:15A0
    neg al                                                           # CS:15A2
L_update_mobile_enemy_15a4:
    cmp al, 0x10                                                     # CS:15A4
    jae L_update_mobile_enemy_15ba                                   # CS:15A6
    dec BYTE PTR [si+E_STATE_15]                                     # CS:15A8
    push si                                                          # CS:15AB
    {load} mov si, di                                                # CS:15AC
    call replace_with_impact                                         # CS:15AE
    mov al, 0x3                                                      # CS:15B1
    call sound_trigger                                               # CS:15B3
    call score_add_10                                                # CS:15B6
    pop si                                                           # CS:15B9
L_update_mobile_enemy_15ba:
    pop di                                                           # CS:15BA
L_update_mobile_enemy_15bb:
    add di, ENTITY_SIZE                                              # CS:15BB
    jmp L_update_mobile_enemy_154b                                   # CS:15BE
L_update_mobile_enemy_15c0:
    mov al, ds:[D_player_handler]                                    # CS:15C0
    cmp al, 0x11                                                     # CS:15C3
    je L_update_mobile_enemy_15f8                                    # CS:15C5
    mov ax, ds:[D_player_y]                                          # CS:15C7
    cmp ax, WORD PTR [si+E_Y]                                        # CS:15CA
    jbe L_update_mobile_enemy_15f8                                   # CS:15CD
    mov ax, ds:[D_player_x]                                          # CS:15CF
    sub ax, WORD PTR [si+E_X]                                        # CS:15D2
    jns L_update_mobile_enemy_15d9                                   # CS:15D5
    neg ax                                                           # CS:15D7
L_update_mobile_enemy_15d9:
    ACC16 cmp, 0x20                                                  # CS:15D9
    jae L_update_mobile_enemy_15f8                                   # CS:15DC
    dec BYTE PTR [si+E_STATE_16]                                     # CS:15DE
    jns L_update_mobile_enemy_15ee                                   # CS:15E1
    mov BYTE PTR [si+E_STATE_16], 0x7                                # CS:15E3
    mov BYTE PTR ds:[D_spawn_projectile_gate], 0x1                   # CS:15E7
    jmp L_update_mobile_enemy_15f3                                   # CS:15EC
L_update_mobile_enemy_15ee:
    mov BYTE PTR ds:[D_spawn_projectile_gate], 0x0                   # CS:15EE
L_update_mobile_enemy_15f3:
    push si                                                          # CS:15F3
    call fire_mobile_enemy_pattern                                   # CS:15F4
    pop si                                                           # CS:15F7
L_update_mobile_enemy_15f8:
    call cull_entity_after_view                                      # CS:15F8
    ret                                                              # CS:15FB

/* ========================================================================== 
 * update_enemy_projectile  |  CS:15FC
 * Назначение: Движение в пяти направлениях, коррекция высоты, столкновение с рельефом и игроком.
 * Вход: DS:SI = сущность; handler ID 20.
 * Эффект/ограничения: При попадании shield_delta -=1; направления используют разные dx/dy, не тригонометрию.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_enemy_projectile, @function
update_enemy_projectile:
    call cull_entity_after_view                                      # CS:15FC
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:15FF
    cmp al, 0x0                                                      # CS:1601
    jne L_update_enemy_projectile_1606                               # CS:1603
    ret                                                              # CS:1605
L_update_enemy_projectile_1606:
    mov al, BYTE PTR [si+E_PARAM]                                    # CS:1606
    cmp al, 0x2                                                      # CS:1609
    jne L_update_enemy_projectile_1613                               # CS:160B
    add WORD PTR [si+E_Y], 0x8                                       # CS:160D
    jmp L_update_enemy_projectile_163d                               # CS:1611
L_update_enemy_projectile_1613:
    cmp al, 0x0                                                      # CS:1613
    jne L_update_enemy_projectile_161d                               # CS:1615
    sub WORD PTR [si+E_X], 0x8                                       # CS:1617
    jmp L_update_enemy_projectile_163d                               # CS:161B
L_update_enemy_projectile_161d:
    cmp al, 0x1                                                      # CS:161D
    jne L_update_enemy_projectile_162b                               # CS:161F
    sub WORD PTR [si+E_X], 0x6                                       # CS:1621
    add WORD PTR [si+E_Y], 0x4                                       # CS:1625
    jmp L_update_enemy_projectile_163d                               # CS:1629
L_update_enemy_projectile_162b:
    cmp al, 0x3                                                      # CS:162B
    jne L_update_enemy_projectile_1639                               # CS:162D
    add WORD PTR [si+E_X], 0x6                                       # CS:162F
    add WORD PTR [si+E_Y], 0x4                                       # CS:1633
    jmp L_update_enemy_projectile_163d                               # CS:1637
L_update_enemy_projectile_1639:
    add WORD PTR [si+E_X], 0x8                                       # CS:1639
L_update_enemy_projectile_163d:
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:163D
    mov ax, WORD PTR [si+E_X]                                        # CS:1642
    ACC16 cmp, 0x4                                                   # CS:1645
    jge L_update_enemy_projectile_164e                               # CS:1648
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:164A
    ret                                                              # CS:164D
L_update_enemy_projectile_164e:
    ACC16 cmp, 0xfc                                                  # CS:164E
    jle L_update_enemy_projectile_1657                               # CS:1651
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:1653
    ret                                                              # CS:1656
L_update_enemy_projectile_1657:
    mov al, BYTE PTR [si+E_FRAME]                                    # CS:1657
    cmp al, 0x1                                                      # CS:165A
    jne L_update_enemy_projectile_166b                               # CS:165C
    mov al, BYTE PTR [si+E_Z]                                        # CS:165E
    cmp al, 0x64                                                     # CS:1661
    jge L_update_enemy_projectile_1669                               # CS:1663
    add BYTE PTR [si+E_Z], 0x4                                       # CS:1665
L_update_enemy_projectile_1669:
    jmp L_update_enemy_projectile_167a                               # CS:1669
L_update_enemy_projectile_166b:
    cmp al, 0x2                                                      # CS:166B
    jne L_update_enemy_projectile_167a                               # CS:166D
    mov al, BYTE PTR [si+E_Z]                                        # CS:166F
    cmp al, 0x9c                                                     # CS:1672
    jle L_update_enemy_projectile_167a                               # CS:1674
    sub BYTE PTR [si+E_Z], 0x4                                       # CS:1676
L_update_enemy_projectile_167a:
    push si                                                          # CS:167A
    mov ax, WORD PTR [si+E_X]                                        # CS:167B
    push ax                                                          # CS:167E
    mov ax, WORD PTR [si+E_Y]                                        # CS:167F
    push ax                                                          # CS:1682
    call sample_terrain                                              # CS:1683
    pop si                                                           # CS:1686
    cmp al, BYTE PTR [si+E_Z]                                        # CS:1687
    jl L_update_enemy_projectile_1691                                # CS:168A
    call replace_with_impact                                         # CS:168C
    jmp L_update_enemy_projectile_16d5                               # CS:168F
L_update_enemy_projectile_1691:
    mov al, ds:[D_player_handler]                                    # CS:1691
    cmp al, 0x11                                                     # CS:1694
    jne L_update_enemy_projectile_1699                               # CS:1696
    ret                                                              # CS:1698
L_update_enemy_projectile_1699:
    mov ax, WORD PTR [si+E_X]                                        # CS:1699
    sub ax, WORD PTR ds:[D_player_x]                                 # CS:169C
    jns L_update_enemy_projectile_16a4                               # CS:16A0
    neg ax                                                           # CS:16A2
L_update_enemy_projectile_16a4:
    ACC16 cmp, 0xc                                                   # CS:16A4
    jae L_update_enemy_projectile_16d5                               # CS:16A7
    mov ax, WORD PTR [si+E_Y]                                        # CS:16A9
    sub ax, WORD PTR ds:[D_player_y]                                 # CS:16AC
    jns L_update_enemy_projectile_16b4                               # CS:16B0
    neg ax                                                           # CS:16B2
L_update_enemy_projectile_16b4:
    ACC16 cmp, 0x8                                                   # CS:16B4
    jae L_update_enemy_projectile_16d5                               # CS:16B7
    mov al, BYTE PTR [si+E_Z]                                        # CS:16B9
    sub al, BYTE PTR ds:[D_player_z]                                 # CS:16BC
    jns L_update_enemy_projectile_16c4                               # CS:16C0
    neg al                                                           # CS:16C2
L_update_enemy_projectile_16c4:
    cmp al, 0x10                                                     # CS:16C4
    jae L_update_enemy_projectile_16d5                               # CS:16C6
    sub WORD PTR ds:[D_shield_delta], 0x1                            # CS:16C8
    call replace_with_impact                                         # CS:16CD
    mov al, 0x4                                                      # CS:16D0
    call sound_trigger                                               # CS:16D2
L_update_enemy_projectile_16d5:
    ret                                                              # CS:16D5

/* ========================================================================== 
 * update_countdown_effect  |  CS:16D6
 * Назначение: Декрементирует frame/state и удаляет объект при установленном SF.
 * Вход: DS:SI = сущность; handler ID 22.
 * Эффект/ограничения: Знаковый 8-битный результат после DEC; не unsigned frame<0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_countdown_effect, @function
update_countdown_effect:
    dec BYTE PTR [si+E_FRAME]                                        # CS:16D6
    jns L_update_countdown_effect_16de                               # CS:16D9
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:16DB
L_update_countdown_effect_16de:
    ret                                                              # CS:16DE

/* ========================================================================== 
 * update_four_frame_impact  |  CS:16DF
 * Назначение: Проигрывает четыре sprite-offset из таблицы; затем очищает flags.
 * Вход: DS:SI = сущность; handler ID 21.
 * Эффект/ограничения: Сравнивает старое frame, инкремент сделан до проверки.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_four_frame_impact, @function
update_four_frame_impact:
    mov al, BYTE PTR [si+E_FRAME]                                    # CS:16DF
    inc BYTE PTR [si+E_FRAME]                                        # CS:16E2
    cmp al, 0x4                                                      # CS:16E5
    jb L_update_four_frame_impact_16ed                               # CS:16E7
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:16E9
    ret                                                              # CS:16EC
L_update_four_frame_impact_16ed:
    shl al, 1                                                        # CS:16ED
    {load} mov bl, al                                                # CS:16EF
    mov bh, 0x0                                                      # CS:16F1
    mov ax, WORD PTR [bx+D_impact_frame_sprites]                     # CS:16F3
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:16F7
    call cull_entity_after_view                                      # CS:16FA
    ret                                                              # CS:16FD

/* ========================================================================== 
 * update_five_frame_explosion  |  CS:16FE
 * Назначение: Выбирает frame=(++age>>2)&7; кадры 0..4, затем удаление.
 * Вход: DS:SI = сущность; handler ID 16.
 * Эффект/ограничения: Применяет descriptor DS:12B5 и culling.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_five_frame_explosion, @function
update_five_frame_explosion:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:16FE
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:1701
    shr al, 1                                                        # CS:1704
    shr al, 1                                                        # CS:1706
    and al, 0x7                                                      # CS:1708
    cmp al, 0x5                                                      # CS:170A
    jl L_update_five_frame_explosion_1712                            # CS:170C
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:170E
    ret                                                              # CS:1711
L_update_five_frame_explosion_1712:
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1712
    mov bx, OFFSET D_sprite_frames_12b5                              # CS:1715
    call apply_sprite_frame                                          # CS:1718
    call cull_entity_after_view                                      # CS:171B
    ret                                                              # CS:171E

/* ========================================================================== 
 * update_six_frame_effect  |  CS:171F
 * Назначение: Выбирает frame=(++age>>2)&7; кадры 0..5, затем удаление.
 * Вход: DS:SI = сущность; handler ID 31.
 * Эффект/ограничения: Применяет descriptor DS:12F5 и culling.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_six_frame_effect, @function
update_six_frame_effect:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:171F
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:1722
    shr al, 1                                                        # CS:1725
    shr al, 1                                                        # CS:1727
    and al, 0x7                                                      # CS:1729
    cmp al, 0x6                                                      # CS:172B
    jl L_update_six_frame_effect_1733                                # CS:172D
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:172F
    ret                                                              # CS:1732
L_update_six_frame_effect_1733:
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1733
    mov bx, OFFSET D_sprite_frames_12f5                              # CS:1736
    call apply_sprite_frame                                          # CS:1739
    call cull_entity_after_view                                      # CS:173C
    ret                                                              # CS:173F

/* ========================================================================== 
 * update_side_impact  |  CS:1740
 * Назначение: Четырёхкадровый эффект; по окончании освобождает соответствующий бит side_impact_mask.
 * Вход: DS:SI = сущность; handler ID 23.
 * Эффект/ограничения: Бит стороны 20h; mask использует 01h/80h.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_side_impact, @function
update_side_impact:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:1740
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:1743
    and al, 0x7                                                      # CS:1746
    cmp al, 0x4                                                      # CS:1748
    jl L_update_side_impact_1762                                     # CS:174A
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:174C
    test al, 0x20                                                    # CS:174E
    je L_update_side_impact_1759                                     # CS:1750
    and BYTE PTR ds:[D_side_impact_mask], 0x7f                       # CS:1752
    jmp L_update_side_impact_175e                                    # CS:1757
L_update_side_impact_1759:
    and BYTE PTR ds:[D_side_impact_mask], 0xfe                       # CS:1759
L_update_side_impact_175e:
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:175E
    ret                                                              # CS:1761
L_update_side_impact_1762:
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1762
    mov bx, OFFSET D_sprite_frames_133d                              # CS:1765
    call apply_sprite_frame                                          # CS:1768
    ret                                                              # CS:176B

/* ========================================================================== 
 * update_looping_anim4  |  CS:176C
 * Назначение: Циклическая анимация frame=(++age>>1)&3 и culling.
 * Вход: DS:SI = сущность; handler ID 2.
 * Эффект/ограничения: Descriptor DS:0E2B; игровое имя объекта не установлено.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_looping_anim4, @function
update_looping_anim4:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:176C
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:176F
    shr al, 1                                                        # CS:1772
    and al, 0x3                                                      # CS:1774
    mov BYTE PTR [si+E_FRAME], al                                    # CS:1776
    mov bx, OFFSET D_sprite_frames_0e2b                              # CS:1779
    call apply_sprite_frame                                          # CS:177C
    call cull_entity_after_view                                      # CS:177F
    ret                                                              # CS:1782

/* ========================================================================== 
 * update_pingpong_animation  |  CS:1783
 * Назначение: Анимация через шестизначную таблицу кадров; после фазы 5 сбрасывает счётчик.
 * Вход: DS:SI = сущность; handler ID 3.
 * Эффект/ограничения: Descriptor DS:0E52; визуальная роль отдельно не подтверждена.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_pingpong_animation, @function
update_pingpong_animation:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:1783
    mov al, BYTE PTR [si+E_STATE_16]                                 # CS:1786
    shr al, 1                                                        # CS:1789
    and al, 0x7                                                      # CS:178B
    cmp al, 0x6                                                      # CS:178D
    jb L_update_pingpong_animation_1797                              # CS:178F
    mov al, 0x0                                                      # CS:1791
    mov BYTE PTR [si+E_STATE_16], 0x0                                # CS:1793
L_update_pingpong_animation_1797:
    {load} mov bl, al                                                # CS:1797
    mov bh, 0x0                                                      # CS:1799
    mov al, BYTE PTR [bx+D_pingpong_frame_sequence]                  # CS:179B
    mov BYTE PTR [si+E_FRAME], al                                    # CS:179F
    mov bx, OFFSET D_sprite_frames_0e52                              # CS:17A2
    call apply_sprite_frame                                          # CS:17A5
    call cull_entity_after_view                                      # CS:17A8
    ret                                                              # CS:17AB

/* ========================================================================== 
 * update_aimed_emitter_a  |  CS:17AC
 * Назначение: Выбирает направление на игрока и вызывает emitter; перезарядка через знаковый DEC с периодом сброса 8.
 * Вход: DS:SI = сущность; handler ID 5.
 * Эффект/ограничения: spawn_projectile_gate включает настоящий снаряд только на выбранных кадрах; эффекты создаются чаще.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_aimed_emitter_a, @function
update_aimed_emitter_a:
    call aim_entity_at_player                                        # CS:17AC
    cmp cx, 0x0                                                      # CS:17AF
    je L_update_aimed_emitter_a_17cc                                 # CS:17B2
    dec BYTE PTR [si+E_STATE_16]                                     # CS:17B4
    jns L_update_aimed_emitter_a_17c4                                # CS:17B7
    mov BYTE PTR [si+E_STATE_16], 0x8                                # CS:17B9
    mov BYTE PTR ds:[D_spawn_projectile_gate], 0x1                   # CS:17BD
    jmp L_update_aimed_emitter_a_17c9                                # CS:17C2
L_update_aimed_emitter_a_17c4:
    mov BYTE PTR ds:[D_spawn_projectile_gate], 0x0                   # CS:17C4
L_update_aimed_emitter_a_17c9:
    call spawn_aimed_shot                                            # CS:17C9
L_update_aimed_emitter_a_17cc:
    mov bx, OFFSET D_sprite_frames_0e73                              # CS:17CC
    call apply_sprite_frame                                          # CS:17CF
    call cull_entity_after_view                                      # CS:17D2
    ret                                                              # CS:17D5

/* ========================================================================== 
 * update_aimed_emitter_b  |  CS:17D6
 * Назначение: Целится в игрока; по окончании счётчика создаёт навесной снаряд и ставит счётчик 18.
 * Вход: DS:SI = сущность; handler ID 6.
 * Эффект/ограничения: Descriptor DS:0E9C; общий aiming возвращает CX=0 вне зоны стрельбы.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_aimed_emitter_b, @function
update_aimed_emitter_b:
    call aim_entity_at_player                                        # CS:17D6
    cmp cx, 0x0                                                      # CS:17D9
    je L_update_aimed_emitter_b_17ea                                 # CS:17DC
    dec BYTE PTR [si+E_STATE_16]                                     # CS:17DE
    jns L_update_aimed_emitter_b_17ea                                # CS:17E1
    mov BYTE PTR [si+E_STATE_16], 0x12                               # CS:17E3
    call spawn_arcing_projectile                                     # CS:17E7
L_update_aimed_emitter_b_17ea:
    mov bx, OFFSET D_sprite_frames_0e9c                              # CS:17EA
    call apply_sprite_frame                                          # CS:17ED
    call cull_entity_after_view                                      # CS:17F0
    ret                                                              # CS:17F3

/* ========================================================================== 
 * update_arcing_projectile  |  CS:17F4
 * Назначение: Движение в 5 направлениях и изменение высоты по 16-байтовой таблице; затем эффект.
 * Вход: DS:SI = сущность; handler ID 30.
 * Эффект/ограничения: Проверяет 3D-близость к игроку; shield_delta -=10.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_arcing_projectile, @function
update_arcing_projectile:
    call cull_entity_after_view                                      # CS:17F4
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:17F7
    cmp al, 0x0                                                      # CS:17F9
    jne L_update_arcing_projectile_17fe                              # CS:17FB
    ret                                                              # CS:17FD
L_update_arcing_projectile_17fe:
    mov al, BYTE PTR [si+E_PARAM]                                    # CS:17FE
    cmp al, 0x2                                                      # CS:1801
    jne L_update_arcing_projectile_180b                              # CS:1803
    add WORD PTR [si+E_Y], 0x3                                       # CS:1805
    jmp L_update_arcing_projectile_1835                              # CS:1809
L_update_arcing_projectile_180b:
    cmp al, 0x0                                                      # CS:180B
    jne L_update_arcing_projectile_1815                              # CS:180D
    sub WORD PTR [si+E_X], 0x4                                       # CS:180F
    jmp L_update_arcing_projectile_1835                              # CS:1813
L_update_arcing_projectile_1815:
    cmp al, 0x1                                                      # CS:1815
    jne L_update_arcing_projectile_1823                              # CS:1817
    sub WORD PTR [si+E_X], 0x4                                       # CS:1819
    add WORD PTR [si+E_Y], 0x3                                       # CS:181D
    jmp L_update_arcing_projectile_1835                              # CS:1821
L_update_arcing_projectile_1823:
    cmp al, 0x3                                                      # CS:1823
    jne L_update_arcing_projectile_1831                              # CS:1825
    add WORD PTR [si+E_X], 0x4                                       # CS:1827
    add WORD PTR [si+E_Y], 0x3                                       # CS:182B
    jmp L_update_arcing_projectile_1835                              # CS:182F
L_update_arcing_projectile_1831:
    add WORD PTR [si+E_X], 0x4                                       # CS:1831
L_update_arcing_projectile_1835:
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:1835
    mov ax, WORD PTR [si+E_X]                                        # CS:183A
    ACC16 cmp, 0x8                                                   # CS:183D
    jge L_update_arcing_projectile_1846                              # CS:1840
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:1842
    ret                                                              # CS:1845
L_update_arcing_projectile_1846:
    ACC16 cmp, 0xf8                                                  # CS:1846
    jle L_update_arcing_projectile_184f                              # CS:1849
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:184B
    ret                                                              # CS:184E
L_update_arcing_projectile_184f:
    inc BYTE PTR [si+E_STATE_16]                                     # CS:184F
    mov bl, BYTE PTR [si+E_STATE_16]                                 # CS:1852
    cmp bl, 0x10                                                     # CS:1855
    jb L_update_arcing_projectile_185e                               # CS:1858
    call replace_with_arc_effect                                     # CS:185A
    ret                                                              # CS:185D
L_update_arcing_projectile_185e:
    mov bh, 0x0                                                      # CS:185E
    mov al, BYTE PTR [bx+D_arc_height_deltas]                        # CS:1860
    add BYTE PTR [si+E_Z], al                                        # CS:1864
    mov al, ds:[D_player_handler]                                    # CS:1867
    cmp al, 0x11                                                     # CS:186A
    jne L_update_arcing_projectile_186f                              # CS:186C
    ret                                                              # CS:186E
L_update_arcing_projectile_186f:
    mov ax, WORD PTR [si+E_X]                                        # CS:186F
    sub ax, WORD PTR ds:[D_player_x]                                 # CS:1872
    jns L_update_arcing_projectile_187a                              # CS:1876
    neg ax                                                           # CS:1878
L_update_arcing_projectile_187a:
    ACC16 cmp, 0xe                                                   # CS:187A
    jae L_update_arcing_projectile_18a8                              # CS:187D
    mov ax, WORD PTR [si+E_Y]                                        # CS:187F
    sub ax, WORD PTR ds:[D_player_y]                                 # CS:1882
    jns L_update_arcing_projectile_188a                              # CS:1886
    neg ax                                                           # CS:1888
L_update_arcing_projectile_188a:
    ACC16 cmp, 0xa                                                   # CS:188A
    jae L_update_arcing_projectile_18a8                              # CS:188D
    mov al, BYTE PTR [si+E_Z]                                        # CS:188F
    sub al, BYTE PTR ds:[D_player_z]                                 # CS:1892
    jns L_update_arcing_projectile_189a                              # CS:1896
    neg al                                                           # CS:1898
L_update_arcing_projectile_189a:
    cmp al, 0x10                                                     # CS:189A
    jae L_update_arcing_projectile_18a8                              # CS:189C
    sub WORD PTR ds:[D_shield_delta], 0xa                            # CS:189E
    mov al, 0x5                                                      # CS:18A3
    call sound_trigger                                               # CS:18A5
L_update_arcing_projectile_18a8:
    ret                                                              # CS:18A8

/* ========================================================================== 
 * update_aimed_emitter_c  |  CS:18A9
 * Назначение: Целится и порождает наземный снаряд при истечении счётчика, затем сброс 40.
 * Вход: DS:SI = сущность; handler ID 7.
 * Эффект/ограничения: Descriptor DS:0EC5; нельзя объединять период с другими emitter без параметров.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_aimed_emitter_c, @function
update_aimed_emitter_c:
    call aim_entity_at_player                                        # CS:18A9
    cmp cx, 0x0                                                      # CS:18AC
    je L_update_aimed_emitter_c_18bd                                 # CS:18AF
    dec BYTE PTR [si+E_STATE_16]                                     # CS:18B1
    jns L_update_aimed_emitter_c_18bd                                # CS:18B4
    mov BYTE PTR [si+E_STATE_16], 0x28                               # CS:18B6
    call spawn_ground_enemy_shot                                     # CS:18BA
L_update_aimed_emitter_c_18bd:
    mov bx, OFFSET D_sprite_frames_0ec5                              # CS:18BD
    call apply_sprite_frame                                          # CS:18C0
    call cull_entity_after_view                                      # CS:18C3
    ret                                                              # CS:18C6

/* ========================================================================== 
 * update_level_exit_gate  |  CS:18C7
 * Назначение: Открывает ворота лишь после всех 6 collectibles, подтягивает игрока к центру и запускает переход.
 * Вход: DS:SI = сущность; handler ID 28.
 * Эффект/ограничения: Открытие по Manhattan distance, затем отдельные пороги x/y; не круговой радиус.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_level_exit_gate, @function
update_level_exit_gate:
    mov al, ds:[D_collectible_0]                                     # CS:18C7
    and al, BYTE PTR ds:[D_collectible_1]                            # CS:18CA
    and al, BYTE PTR ds:[D_collectible_2]                            # CS:18CE
    and al, BYTE PTR ds:[D_collectible_3]                            # CS:18D2
    and al, BYTE PTR ds:[D_collectible_4]                            # CS:18D6
    and al, BYTE PTR ds:[D_collectible_5]                            # CS:18DA
    jne L_update_level_exit_gate_18e4                                # CS:18DE
    call cull_entity_after_view                                      # CS:18E0
    ret                                                              # CS:18E3
L_update_level_exit_gate_18e4:
    mov ax, ds:[D_player_x]                                          # CS:18E4
    sub ax, WORD PTR [si+E_X]                                        # CS:18E7
    ACC16 sub, 0xc                                                   # CS:18EA
    jns L_update_level_exit_gate_18f1                                # CS:18ED
    neg ax                                                           # CS:18EF
L_update_level_exit_gate_18f1:
    mov cx, WORD PTR ds:[D_player_y]                                 # CS:18F1
    sub cx, 0x8                                                      # CS:18F5
    sub cx, WORD PTR [si+E_Y]                                        # CS:18F8
    jns L_update_level_exit_gate_18ff                                # CS:18FB
    neg cx                                                           # CS:18FD
L_update_level_exit_gate_18ff:
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:18FF
    shr bl, 1                                                        # CS:1902
    {load} add ax, cx                                                # CS:1904
    ACC16 cmp, 0x40                                                  # CS:1906
    ja L_update_level_exit_gate_1915                                 # CS:1909
    cmp bl, 0x8                                                      # CS:190B
    je L_update_level_exit_gate_1913                                 # CS:190E
    inc BYTE PTR [si+E_FRAME]                                        # CS:1910
L_update_level_exit_gate_1913:
    jmp L_update_level_exit_gate_191d                                # CS:1913
L_update_level_exit_gate_1915:
    cmp bl, 0x0                                                      # CS:1915
    je L_update_level_exit_gate_191d                                 # CS:1918
    dec BYTE PTR [si+E_FRAME]                                        # CS:191A
L_update_level_exit_gate_191d:
    push bx                                                          # CS:191D
    mov bh, 0x0                                                      # CS:191E
    shl bx, 1                                                        # CS:1920
    mov ax, WORD PTR [bx+D_exit_gate_frame_sprites]                  # CS:1922
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:1926
    pop bx                                                           # CS:1929
    cmp bl, 0x2                                                      # CS:192A
    jb L_update_level_exit_gate_196b                                 # CS:192D
    mov ax, ds:[D_player_x]                                          # CS:192F
    sub ax, WORD PTR [si+E_X]                                        # CS:1932
    ACC16 sub, 0xc                                                   # CS:1935
    jns L_update_level_exit_gate_1947                                # CS:1938
    neg ax                                                           # CS:193A
    ACC16 cmp, 0x18                                                  # CS:193C
    ja L_update_level_exit_gate_1945                                 # CS:193F
    inc WORD PTR ds:[D_player_x]                                     # CS:1941
L_update_level_exit_gate_1945:
    jmp L_update_level_exit_gate_1952                                # CS:1945
L_update_level_exit_gate_1947:
    je L_update_level_exit_gate_1952                                 # CS:1947
    ACC16 cmp, 0x18                                                  # CS:1949
    ja L_update_level_exit_gate_1952                                 # CS:194C
    dec WORD PTR ds:[D_player_x]                                     # CS:194E
L_update_level_exit_gate_1952:
    cmp bl, 0x8                                                      # CS:1952
    jne L_update_level_exit_gate_196b                                # CS:1955
    cmp cx, 0x8                                                      # CS:1957
    ja L_update_level_exit_gate_196b                                 # CS:195A
    ACC16 cmp, 0x8                                                   # CS:195C
    ja L_update_level_exit_gate_196b                                 # CS:195F
    mov al, ds:[D_player_handler]                                    # CS:1961
    cmp al, 0x11                                                     # CS:1964
    je L_update_level_exit_gate_196b                                 # CS:1966
    call begin_level_exit                                            # CS:1968
L_update_level_exit_gate_196b:
    call cull_entity_after_view                                      # CS:196B
    ret                                                              # CS:196E

/* ========================================================================== 
 * begin_level_exit  |  CS:196F
 * Назначение: Если frame игрока=0, задаёт переход F0h/туннель; иначе сначала переводит игрока в frame=0.
 * Вход: DS:SI = ворота.
 * Эффект/ограничения: Смена режима отсрочена до выравнивания состояния игрока.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type begin_level_exit, @function
begin_level_exit:
    mov al, ds:[D_player_frame]                                      # CS:196F
    cmp al, 0x0                                                      # CS:1972
    jne L_begin_level_exit_1982                                      # CS:1974
    mov BYTE PTR ds:[D_tunnel_reward_kind], 0xf0                     # CS:1976
    mov BYTE PTR ds:[D_tunnel_mode], 0x1                             # CS:197B
    jmp L_begin_level_exit_1992                                      # CS:1980
L_begin_level_exit_1982:
    push si                                                          # CS:1982
    mov BYTE PTR ds:[D_player_frame], 0x0                            # CS:1983
    mov si, OFFSET D_entity_records                                  # CS:1988
    mov bx, OFFSET D_sprite_frames_0d49                              # CS:198B
    call apply_sprite_frame                                          # CS:198E
    pop si                                                           # CS:1991
L_begin_level_exit_1992:
    ret                                                              # CS:1992

/* ========================================================================== 
 * update_tunnel_entrance_a  |  CS:1993
 * Назначение: Общий вход в туннель плюс выбор destination/тип награды из таблицы A.
 * Вход: DS:SI = сущность; handler ID 10.
 * Эффект/ограничения: Табличное слово: нижние 10 бит=destination_y, старшие два бита=вид награды.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_entrance_a, @function
update_tunnel_entrance_a:
    call update_tunnel_gate_common                                   # CS:1993
    mov al, ds:[D_tunnel_mode]                                       # CS:1996
    cmp al, 0x1                                                      # CS:1999
    jne L_update_tunnel_entrance_a_19c1                              # CS:199B
    mov al, ds:[D_level_index]                                       # CS:199D
    {load} mov bl, al                                                # CS:19A0
    shl al, 1                                                        # CS:19A2
    {load} add bl, al                                                # CS:19A4
    add bl, BYTE PTR ds:[D_level_part]                               # CS:19A6
    mov bh, 0x0                                                      # CS:19AA
    shl bx, 1                                                        # CS:19AC
    mov ax, WORD PTR [bx+D_tunnel_destinations_a]                    # CS:19AE
    {load} mov bh, ah                                                # CS:19B2
    ACC16 and, WORLD_Y_MASK                                          # CS:19B4
    mov ds:[D_surface_resume_y], ax                                  # CS:19B7
    and bh, 0xc0                                                     # CS:19BA
    mov BYTE PTR ds:[D_tunnel_reward_kind], bh                       # CS:19BD
L_update_tunnel_entrance_a_19c1:
    ret                                                              # CS:19C1

/* ========================================================================== 
 * update_tunnel_entrance_b  |  CS:19C2
 * Назначение: Вход в туннель по таблице B; помечает перезагрузку карты и циклически меняет part 0..2.
 * Вход: DS:SI = сущность; handler ID 11.
 * Эффект/ограничения: Выбор таблицы делается до увеличения level_part.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_entrance_b, @function
update_tunnel_entrance_b:
    call update_tunnel_gate_common                                   # CS:19C2
    mov al, ds:[D_tunnel_mode]                                       # CS:19C5
    cmp al, 0x1                                                      # CS:19C8
    jne L_update_tunnel_entrance_b_1a03                              # CS:19CA
    mov al, ds:[D_level_index]                                       # CS:19CC
    {load} mov bl, al                                                # CS:19CF
    shl al, 1                                                        # CS:19D1
    {load} add bl, al                                                # CS:19D3
    add bl, BYTE PTR ds:[D_level_part]                               # CS:19D5
    mov bh, 0x0                                                      # CS:19D9
    shl bx, 1                                                        # CS:19DB
    mov ax, WORD PTR [bx+D_tunnel_destinations_b]                    # CS:19DD
    {load} mov bh, ah                                                # CS:19E1
    ACC16 and, WORLD_Y_MASK                                          # CS:19E3
    mov ds:[D_surface_resume_y], ax                                  # CS:19E6
    and bh, 0xc0                                                     # CS:19E9
    mov BYTE PTR ds:[D_tunnel_reward_kind], bh                       # CS:19EC
    mov BYTE PTR ds:[D_map_reload_pending], 0x1                      # CS:19F0
    mov al, ds:[D_level_part]                                        # CS:19F5
    inc al                                                           # CS:19F8
    cmp al, 0x2                                                      # CS:19FA
    jbe L_update_tunnel_entrance_b_1a00                              # CS:19FC
    mov al, 0x0                                                      # CS:19FE
L_update_tunnel_entrance_b_1a00:
    mov ds:[D_level_part], al                                        # CS:1A00
L_update_tunnel_entrance_b_1a03:
    ret                                                              # CS:1A03

/* ========================================================================== 
 * update_tunnel_gate_common  |  CS:1A04
 * Назначение: Открывает/закрывает створку по близости, скрывает открытый кадр, центрирует игрока и ставит tunnel_mode.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Сохраняет player_x для выхода; общая процедура у двух обработчиков входа.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_tunnel_gate_common, @function
update_tunnel_gate_common:
    mov ax, ds:[D_player_x]                                          # CS:1A04
    sub ax, WORD PTR [si+E_X]                                        # CS:1A07
    ACC16 sub, 0xc                                                   # CS:1A0A
    jns L_update_tunnel_gate_common_1a11                             # CS:1A0D
    neg ax                                                           # CS:1A0F
L_update_tunnel_gate_common_1a11:
    mov cx, WORD PTR ds:[D_player_y]                                 # CS:1A11
    sub cx, WORD PTR [si+E_Y]                                        # CS:1A15
    jns L_update_tunnel_gate_common_1a1c                             # CS:1A18
    neg cx                                                           # CS:1A1A
L_update_tunnel_gate_common_1a1c:
    mov bl, BYTE PTR [si+E_FRAME]                                    # CS:1A1C
    shr bl, 1                                                        # CS:1A1F
    {load} add ax, cx                                                # CS:1A21
    ACC16 cmp, 0x40                                                  # CS:1A23
    ja L_update_tunnel_gate_common_1a32                              # CS:1A26
    cmp bl, 0x6                                                      # CS:1A28
    je L_update_tunnel_gate_common_1a30                              # CS:1A2B
    inc BYTE PTR [si+E_FRAME]                                        # CS:1A2D
L_update_tunnel_gate_common_1a30:
    jmp L_update_tunnel_gate_common_1a3a                             # CS:1A30
L_update_tunnel_gate_common_1a32:
    cmp bl, 0x0                                                      # CS:1A32
    je L_update_tunnel_gate_common_1a3a                              # CS:1A35
    dec BYTE PTR [si+E_FRAME]                                        # CS:1A37
L_update_tunnel_gate_common_1a3a:
    cmp bl, 0x6                                                      # CS:1A3A
    jne L_update_tunnel_gate_common_1a45                             # CS:1A3D
    mov BYTE PTR [si+E_LAYER], 0x0                                   # CS:1A3F
    jmp L_update_tunnel_gate_common_1a56                             # CS:1A43
L_update_tunnel_gate_common_1a45:
    push bx                                                          # CS:1A45
    mov BYTE PTR [si+E_LAYER], 0x7                                   # CS:1A46
    mov bh, 0x0                                                      # CS:1A4A
    shl bx, 1                                                        # CS:1A4C
    mov ax, WORD PTR [bx+D_tunnel_gate_frame_sprites]                # CS:1A4E
    mov WORD PTR [si+E_SPRITE], ax                                   # CS:1A52
    pop bx                                                           # CS:1A55
L_update_tunnel_gate_common_1a56:
    cmp bl, 0x1                                                      # CS:1A56
    jb L_update_tunnel_gate_common_1a9a                              # CS:1A59
    mov ax, ds:[D_player_x]                                          # CS:1A5B
    sub ax, WORD PTR [si+E_X]                                        # CS:1A5E
    ACC16 sub, 0xc                                                   # CS:1A61
    jns L_update_tunnel_gate_common_1a73                             # CS:1A64
    neg ax                                                           # CS:1A66
    ACC16 cmp, 0x18                                                  # CS:1A68
    ja L_update_tunnel_gate_common_1a71                              # CS:1A6B
    inc WORD PTR ds:[D_player_x]                                     # CS:1A6D
L_update_tunnel_gate_common_1a71:
    jmp L_update_tunnel_gate_common_1a7e                             # CS:1A71
L_update_tunnel_gate_common_1a73:
    je L_update_tunnel_gate_common_1a7e                              # CS:1A73
    ACC16 cmp, 0x18                                                  # CS:1A75
    ja L_update_tunnel_gate_common_1a7e                              # CS:1A78
    dec WORD PTR ds:[D_player_x]                                     # CS:1A7A
L_update_tunnel_gate_common_1a7e:
    cmp cx, 0x8                                                      # CS:1A7E
    ja L_update_tunnel_gate_common_1a9a                              # CS:1A81
    ACC16 cmp, 0x8                                                   # CS:1A83
    ja L_update_tunnel_gate_common_1a9a                              # CS:1A86
    mov al, ds:[D_player_handler]                                    # CS:1A88
    cmp al, 0x11                                                     # CS:1A8B
    je L_update_tunnel_gate_common_1a9a                              # CS:1A8D
    mov ax, ds:[D_player_x]                                          # CS:1A8F
    mov ds:[D_surface_resume_x], ax                                  # CS:1A92
    mov BYTE PTR ds:[D_tunnel_mode], 0x1                             # CS:1A95
L_update_tunnel_gate_common_1a9a:
    call cull_entity_after_view                                      # CS:1A9A
    ret                                                              # CS:1A9D

/* ========================================================================== 
 * update_refill_gate  |  CS:1A9E
 * Назначение: Центрирует игрока у объекта: узкая центральная зона добавляет side_charge_delta, соседняя обнуляет shield.
 * Вход: DS:SI = сущность; handler ID 8.
 * Эффект/ограничения: Предполагаемая роль — пополнение; точные проверки x/y в коде важнее названия.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_refill_gate, @function
update_refill_gate:
    call cull_entity_after_view                                      # CS:1A9E
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1AA1
    cmp al, 0x0                                                      # CS:1AA3
    jne L_update_refill_gate_1aa8                                    # CS:1AA5
    ret                                                              # CS:1AA7
L_update_refill_gate_1aa8:
    mov bx, WORD PTR ds:[D_player_y]                                 # CS:1AA8
    sub bx, WORD PTR [si+E_Y]                                        # CS:1AAC
    jns L_update_refill_gate_1abb                                    # CS:1AAF
    cmp bx, 0xfff0                                                   # CS:1AB1
    jge L_update_refill_gate_1aba                                    # CS:1AB4
    mov BYTE PTR [si+E_LAYER], 0x1                                   # CS:1AB6
L_update_refill_gate_1aba:
    ret                                                              # CS:1ABA
L_update_refill_gate_1abb:
    cmp bx, 0x20                                                     # CS:1ABB
    jle L_update_refill_gate_1ac1                                    # CS:1ABE
    ret                                                              # CS:1AC0
L_update_refill_gate_1ac1:
    mov ax, ds:[D_player_x]                                          # CS:1AC1
    sub ax, WORD PTR [si+E_X]                                        # CS:1AC4
    ACC16 sub, 0xc                                                   # CS:1AC7
    jns L_update_refill_gate_1ad9                                    # CS:1ACA
    neg ax                                                           # CS:1ACC
    ACC16 cmp, 0x18                                                  # CS:1ACE
    jae L_update_refill_gate_1ad7                                    # CS:1AD1
    inc WORD PTR ds:[D_player_x]                                     # CS:1AD3
L_update_refill_gate_1ad7:
    jmp L_update_refill_gate_1ae7                                    # CS:1AD7
L_update_refill_gate_1ad9:
    ACC16 cmp, 0x18                                                  # CS:1AD9
    jae L_update_refill_gate_1ae7                                    # CS:1ADC
    ACC16 cmp, 0x0                                                   # CS:1ADE
    je L_update_refill_gate_1ae7                                     # CS:1AE1
    dec WORD PTR ds:[D_player_x]                                     # CS:1AE3
L_update_refill_gate_1ae7:
    cmp bx, 0x8                                                      # CS:1AE7
    jbe L_update_refill_gate_1aed                                    # CS:1AEA
    ret                                                              # CS:1AEC
L_update_refill_gate_1aed:
    ACC16 cmp, 0x2                                                   # CS:1AED
    ja L_update_refill_gate_1af8                                     # CS:1AF0
    inc WORD PTR ds:[D_side_charge_delta]                            # CS:1AF2
    jmp L_update_refill_gate_1b08                                    # CS:1AF6
L_update_refill_gate_1af8:
    ACC16 cmp, 0x24                                                  # CS:1AF8
    jae L_update_refill_gate_1b08                                    # CS:1AFB
    mov WORD PTR ds:[D_shield_units], 0x0                            # CS:1AFD
    mov al, 0x6                                                      # CS:1B03
    call sound_trigger                                               # CS:1B05
L_update_refill_gate_1b08:
    ret                                                              # CS:1B08

/* ========================================================================== 
 * update_collectible  |  CS:1B09
 * Назначение: При близости игрока очищает flags, засчитывает collectible[param] и запускает звук.
 * Вход: DS:SI = сущность; handler ID 32.
 * Эффект/ограничения: Игрок в состоянии смерти не собирает объект.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_collectible, @function
update_collectible:
    call cull_entity_after_view                                      # CS:1B09
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:1B0C
    cmp al, 0x0                                                      # CS:1B0E
    jne L_update_collectible_1b13                                    # CS:1B10
    ret                                                              # CS:1B12
L_update_collectible_1b13:
    mov al, ds:[D_player_handler]                                    # CS:1B13
    cmp al, 0x11                                                     # CS:1B16
    jne L_update_collectible_1b1b                                    # CS:1B18
    ret                                                              # CS:1B1A
L_update_collectible_1b1b:
    mov ax, WORD PTR [si+E_X]                                        # CS:1B1B
    sub ax, WORD PTR ds:[D_player_x]                                 # CS:1B1E
    jns L_update_collectible_1b26                                    # CS:1B22
    neg ax                                                           # CS:1B24
L_update_collectible_1b26:
    ACC16 cmp, 0x12                                                  # CS:1B26
    jae L_update_collectible_1b49                                    # CS:1B29
    mov ax, WORD PTR [si+E_Y]                                        # CS:1B2B
    sub ax, WORD PTR ds:[D_player_y]                                 # CS:1B2E
    jns L_update_collectible_1b36                                    # CS:1B32
    neg ax                                                           # CS:1B34
L_update_collectible_1b36:
    ACC16 cmp, 0x10                                                  # CS:1B36
    jae L_update_collectible_1b49                                    # CS:1B39
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:1B3B
    mov al, BYTE PTR [si+E_PARAM]                                    # CS:1B3E
    call collect_item_and_update_hud                                 # CS:1B41
    mov al, 0xa                                                      # CS:1B44
    call sound_trigger                                               # CS:1B46
L_update_collectible_1b49:
    ret                                                              # CS:1B49

/* ========================================================================== 
 * update_drifting_effect  |  CS:1B4A
 * Назначение: Сдвигает y на -2 с маской 03FFh и проверяет видимость.
 * Вход: DS:SI = сущность; handler ID 33.
 * Эффект/ограничения: Визуальное название эффекта не установлено.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_drifting_effect, @function
update_drifting_effect:
    sub WORD PTR [si+E_Y], 0x2                                       # CS:1B4A
    and WORD PTR [si+E_Y], 0x3ff                                     # CS:1B4E
    call cull_entity_before_camera                                   # CS:1B53
    ret                                                              # CS:1B56
