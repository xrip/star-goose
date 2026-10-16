/* Шкалы ресурсов и индикаторы
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * init_hud  |  CS:21DC
 * Назначение: Рисует общие элементы HUD, ресурсы, счёт, collectibles и жизни.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Рисование HUD идёт сразу в обе видеостраницы.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_hud, @function
init_hud:

    # HUD применяет накопленные изменения ресурсов, затем рисует шкалы и индикаторы.
    mov ax, 0x403c                                                   # CS:21DC
    push ax                                                          # CS:21DF
    mov ax, 0x1e11                                                   # CS:21E0
    push ax                                                          # CS:21E3
    mov ax, 0x38                                                     # CS:21E4
    push ax                                                          # CS:21E7
    mov ax, 0x7                                                      # CS:21E8
    push ax                                                          # CS:21EB
    mov ax, 0x2                                                      # CS:21EC
    push ax                                                          # CS:21EF
    call blit_hud_sprite_both_pages                                  # CS:21F0
    call reset_hud_resources                                         # CS:21F3
    call reset_score                                                 # CS:21F6
    call reset_collectible_hud                                       # CS:21F9
    call init_lives                                                  # CS:21FC
    ret                                                              # CS:21FF

/* ========================================================================== 
 * reset_hud_resources  |  CS:2200
 * Назначение: Восстанавливает три шкалы и 6 зарядов боковых устройств.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Не является полным reset всей игры.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_hud_resources, @function
reset_hud_resources:
    call init_fuel_gauge                                             # CS:2200
    call init_ammo_gauge                                             # CS:2203
    call init_shield_gauge                                           # CS:2206
    call init_side_charges                                           # CS:2209
    ret                                                              # CS:220C

/* ========================================================================== 
 * update_hud  |  CS:220D
 * Назначение: Каждый четвёртый кадр начисляет очки за этап; применяет отложенные изменения шкал и зарядов.
 * Вход: Накопленные *_delta.
 * Эффект/ограничения: Fuel/ammo при отрицательной delta уменьшаются ровно на 1, shield/charge — на модуль delta.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type update_hud, @function
update_hud:
    inc BYTE PTR ds:[D_hud_frame_counter]                            # CS:220D
    mov al, ds:[D_hud_frame_counter]                                 # CS:2211
    and al, 0x3                                                      # CS:2214
    jne L_update_hud_2228                                            # CS:2216
    mov ax, 0x0                                                      # CS:2218
    push ax                                                          # CS:221B
    push ax                                                          # CS:221C
    push ax                                                          # CS:221D
    mov al, ds:[D_stage_counter]                                     # CS:221E
    push ax                                                          # CS:2221
    call score_add_digits                                            # CS:2222
    call draw_current_score                                          # CS:2225
L_update_hud_2228:
    mov ax, ds:[D_fuel_delta]                                        # CS:2228
    ACC16 cmp, 0x0                                                   # CS:222B
    je L_update_hud_2246                                             # CS:222E
    jns L_update_hud_2237                                            # CS:2230
    call decrease_fuel                                               # CS:2232
    jmp L_update_hud_2240                                            # CS:2235
L_update_hud_2237:
    {load} mov cx, ax                                                # CS:2237
L_update_hud_2239:
    push cx                                                          # CS:2239
    call increase_fuel                                               # CS:223A
    pop cx                                                           # CS:223D
    loop L_update_hud_2239                                           # CS:223E
L_update_hud_2240:
    mov WORD PTR ds:[D_fuel_delta], 0x0                              # CS:2240
L_update_hud_2246:
    mov ax, ds:[D_ammo_delta]                                        # CS:2246
    ACC16 cmp, 0x0                                                   # CS:2249
    je L_update_hud_2264                                             # CS:224C
    jns L_update_hud_2255                                            # CS:224E
    call decrease_ammo                                               # CS:2250
    jmp L_update_hud_225e                                            # CS:2253
L_update_hud_2255:
    {load} mov cx, ax                                                # CS:2255
L_update_hud_2257:
    push cx                                                          # CS:2257
    call increase_ammo                                               # CS:2258
    pop cx                                                           # CS:225B
    loop L_update_hud_2257                                           # CS:225C
L_update_hud_225e:
    mov WORD PTR ds:[D_ammo_delta], 0x0                              # CS:225E
L_update_hud_2264:
    mov ax, ds:[D_shield_delta]                                      # CS:2264
    ACC16 cmp, 0x0                                                   # CS:2267
    je L_update_hud_228a                                             # CS:226A
    jns L_update_hud_227b                                            # CS:226C
    neg ax                                                           # CS:226E
    {load} mov cx, ax                                                # CS:2270
L_update_hud_2272:
    push cx                                                          # CS:2272
    call decrease_shield                                             # CS:2273
    pop cx                                                           # CS:2276
    loop L_update_hud_2272                                           # CS:2277
    jmp L_update_hud_2284                                            # CS:2279
L_update_hud_227b:
    {load} mov cx, ax                                                # CS:227B
L_update_hud_227d:
    push cx                                                          # CS:227D
    call increase_shield                                             # CS:227E
    pop cx                                                           # CS:2281
    loop L_update_hud_227d                                           # CS:2282
L_update_hud_2284:
    mov WORD PTR ds:[D_shield_delta], 0x0                            # CS:2284
L_update_hud_228a:
    mov ax, ds:[D_side_charge_delta]                                 # CS:228A
    ACC16 cmp, 0x0                                                   # CS:228D
    je L_update_hud_22b0                                             # CS:2290
    jns L_update_hud_22a1                                            # CS:2292
    neg ax                                                           # CS:2294
    {load} mov cx, ax                                                # CS:2296
L_update_hud_2298:
    push cx                                                          # CS:2298
    call remove_side_charge                                          # CS:2299
    pop cx                                                           # CS:229C
    loop L_update_hud_2298                                           # CS:229D
    jmp L_update_hud_22aa                                            # CS:229F
L_update_hud_22a1:
    {load} mov cx, ax                                                # CS:22A1
L_update_hud_22a3:
    push cx                                                          # CS:22A3
    call add_side_charge                                             # CS:22A4
    pop cx                                                           # CS:22A7
    loop L_update_hud_22a3                                           # CS:22A8
L_update_hud_22aa:
    mov WORD PTR ds:[D_side_charge_delta], 0x0                       # CS:22AA
L_update_hud_22b0:
    ret                                                              # CS:22B0

/* ========================================================================== 
 * init_lives  |  CS:22B1
 * Назначение: Задаёт 3 жизни и рисует три значка.
 * Вход: Нет аргументов.
 * Эффект/ограничения: life_count=3.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_lives, @function
init_lives:
    mov WORD PTR ds:[D_life_count], 0x3                              # CS:22B1
    call draw_life_1                                                 # CS:22B7
    call draw_life_2                                                 # CS:22BA
    call draw_life_3                                                 # CS:22BD
    ret                                                              # CS:22C0

/* ========================================================================== 
 * add_life  |  CS:22C1
 * Назначение: Увеличивает число жизней с максимумом 3 и дорисовывает значок.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Переходы в draw_life_* — хвостовые, без нового return address.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type add_life, @function
add_life:
    inc WORD PTR ds:[D_life_count]                                   # CS:22C1
    mov ax, ds:[D_life_count]                                        # CS:22C5
    ACC16 cmp, 0x3                                                   # CS:22C8
    jbe L_add_life_22d4                                              # CS:22CB
    mov WORD PTR ds:[D_life_count], 0x3                              # CS:22CD
    ret                                                              # CS:22D3
L_add_life_22d4:
    je draw_life_3                                                   # CS:22D4
    ACC16 cmp, 0x2                                                   # CS:22D6
    je draw_life_2                                                   # CS:22D9
    ACC16 cmp, 0x1                                                   # CS:22DB
    je draw_life_1                                                   # CS:22DE
    ret                                                              # CS:22E0

/* ========================================================================== 
 * draw_life_1  |  CS:22E1
 * Назначение: Рисует первый значок жизни в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Хвостовая цель add_life и отдельная CALL-процедура.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_life_1, @function
draw_life_1:
    mov ax, 0x43e6                                                   # CS:22E1
    push ax                                                          # CS:22E4
    mov ax, 0x1b90                                                   # CS:22E5
    push ax                                                          # CS:22E8
    mov ax, 0x10                                                     # CS:22E9
    push ax                                                          # CS:22EC
    mov ax, 0xf                                                      # CS:22ED
    push ax                                                          # CS:22F0
    mov ax, 0x2                                                      # CS:22F1
    push ax                                                          # CS:22F4
    call blit_hud_sprite_both_pages                                  # CS:22F5
    ret                                                              # CS:22F8

/* ========================================================================== 
 * draw_life_2  |  CS:22F9
 * Назначение: Рисует второй значок жизни в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Хвостовая цель add_life.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_life_2, @function
draw_life_2:
    mov ax, 0x43e6                                                   # CS:22F9
    push ax                                                          # CS:22FC
    mov ax, 0x1b93                                                   # CS:22FD
    push ax                                                          # CS:2300
    mov ax, 0x10                                                     # CS:2301
    push ax                                                          # CS:2304
    mov ax, 0xf                                                      # CS:2305
    push ax                                                          # CS:2308
    mov ax, 0x2                                                      # CS:2309
    push ax                                                          # CS:230C
    call blit_hud_sprite_both_pages                                  # CS:230D
    ret                                                              # CS:2310

/* ========================================================================== 
 * draw_life_3  |  CS:2311
 * Назначение: Рисует третий значок жизни в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Хвостовая цель add_life.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_life_3, @function
draw_life_3:
    mov ax, 0x43e6                                                   # CS:2311
    push ax                                                          # CS:2314
    mov ax, 0x1b96                                                   # CS:2315
    push ax                                                          # CS:2318
    mov ax, 0x10                                                     # CS:2319
    push ax                                                          # CS:231C
    mov ax, 0xf                                                      # CS:231D
    push ax                                                          # CS:2320
    mov ax, 0x2                                                      # CS:2321
    push ax                                                          # CS:2324
    call blit_hud_sprite_both_pages                                  # CS:2325
    ret                                                              # CS:2328

/* ========================================================================== 
 * remove_life  |  CS:2329
 * Назначение: Уменьшает life_count и стирает соответствующий значок.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Сам не насыщает снизу; предшествующая проверка сделана в handle_player_respawn.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type remove_life, @function
remove_life:
    dec WORD PTR ds:[D_life_count]                                   # CS:2329
    mov ax, ds:[D_life_count]                                        # CS:232D
    ACC16 cmp, 0x2                                                   # CS:2330
    je L_remove_life_2368                                            # CS:2333
    ACC16 cmp, 0x1                                                   # CS:2335
    je L_remove_life_2354                                            # CS:2338
    ACC16 cmp, 0x0                                                   # CS:233A
    je L_remove_life_2340                                            # CS:233D
    ret                                                              # CS:233F
L_remove_life_2340:
    mov ax, 0x1b90                                                   # CS:2340
    push ax                                                          # CS:2343
    mov ax, 0x2                                                      # CS:2344
    push ax                                                          # CS:2347
    mov ax, 0xf                                                      # CS:2348
    push ax                                                          # CS:234B
    mov ax, 0x0                                                      # CS:234C
    push ax                                                          # CS:234F
    call fill_hud_rect_both_pages                                    # CS:2350
    ret                                                              # CS:2353
L_remove_life_2354:
    mov ax, 0x1b93                                                   # CS:2354
    push ax                                                          # CS:2357
    mov ax, 0x2                                                      # CS:2358
    push ax                                                          # CS:235B
    mov ax, 0xf                                                      # CS:235C
    push ax                                                          # CS:235F
    mov ax, 0x0                                                      # CS:2360
    push ax                                                          # CS:2363
    call fill_hud_rect_both_pages                                    # CS:2364
    ret                                                              # CS:2367
L_remove_life_2368:
    mov ax, 0x1b96                                                   # CS:2368
    push ax                                                          # CS:236B
    mov ax, 0x2                                                      # CS:236C
    push ax                                                          # CS:236F
    mov ax, 0xf                                                      # CS:2370
    push ax                                                          # CS:2373
    mov ax, 0x0                                                      # CS:2374
    push ax                                                          # CS:2377
    call fill_hud_rect_both_pages                                    # CS:2378
    ret                                                              # CS:237B

/* ========================================================================== 
 * init_side_charges  |  CS:237C
 * Назначение: Устанавливает 6 зарядов и рисует все шесть индикаторов.
 * Вход: Нет аргументов.
 * Эффект/ограничения: side_charge_delta=0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_side_charges, @function
init_side_charges:
    mov WORD PTR ds:[D_side_charge_units], 0x6                       # CS:237C
    mov WORD PTR ds:[D_side_charge_delta], 0x0                       # CS:2382
    call draw_charge_1                                               # CS:2388
    call draw_charge_2                                               # CS:238B
    call draw_charge_3                                               # CS:238E
    call draw_charge_4                                               # CS:2391
    call draw_charge_5                                               # CS:2394
    call draw_charge_6                                               # CS:2397
    ret                                                              # CS:239A

/* ========================================================================== 
 * add_side_charge  |  CS:239B
 * Назначение: Добавляет заряд до 6, проигрывает звук и рисует соответствующий индикатор.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Вызовы и хвостовые переходы используют одни и те же draw_charge_*.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type add_side_charge, @function
add_side_charge:
    inc WORD PTR ds:[D_side_charge_units]                            # CS:239B
    mov ax, ds:[D_side_charge_units]                                 # CS:239F
    ACC16 cmp, 0x6                                                   # CS:23A2
    jbe L_add_side_charge_23ae                                       # CS:23A5
    mov WORD PTR ds:[D_side_charge_units], 0x6                       # CS:23A7
    ret                                                              # CS:23AD
L_add_side_charge_23ae:
    push ax                                                          # CS:23AE
    mov al, 0x8                                                      # CS:23AF
    call sound_trigger                                               # CS:23B1
    pop ax                                                           # CS:23B4
    ACC16 cmp, 0x1                                                   # CS:23B5
    je draw_charge_1                                                 # CS:23B8
    ACC16 cmp, 0x2                                                   # CS:23BA
    je draw_charge_2                                                 # CS:23BD
    ACC16 cmp, 0x3                                                   # CS:23BF
    je draw_charge_3                                                 # CS:23C2
    ACC16 cmp, 0x4                                                   # CS:23C4
    je draw_charge_4                                                 # CS:23C7
    ACC16 cmp, 0x5                                                   # CS:23C9
    je draw_charge_5                                                 # CS:23CC
    ACC16 cmp, 0x6                                                   # CS:23CE
    je draw_charge_6                                                 # CS:23D1
    ret                                                              # CS:23D3

/* ========================================================================== 
 * draw_charge_1  |  CS:23D4
 * Назначение: Рисует индикатор заряда 1 в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные адреса HUD — не адреса DS.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_charge_1, @function
draw_charge_1:
    mov ax, 0x43ee                                                   # CS:23D4
    push ax                                                          # CS:23D7
    mov ax, 0x1b99                                                   # CS:23D8
    push ax                                                          # CS:23DB
    mov ax, 0x10                                                     # CS:23DC
    push ax                                                          # CS:23DF
    mov ax, 0xf                                                      # CS:23E0
    push ax                                                          # CS:23E3
    mov ax, 0x2                                                      # CS:23E4
    push ax                                                          # CS:23E7
    call blit_hud_sprite_both_pages                                  # CS:23E8
    ret                                                              # CS:23EB

/* ========================================================================== 
 * draw_charge_2  |  CS:23EC
 * Назначение: Рисует индикатор заряда 2 в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные адреса HUD — не адреса DS.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_charge_2, @function
draw_charge_2:
    mov ax, 0x43ee                                                   # CS:23EC
    push ax                                                          # CS:23EF
    mov ax, 0x1b9b                                                   # CS:23F0
    push ax                                                          # CS:23F3
    mov ax, 0x10                                                     # CS:23F4
    push ax                                                          # CS:23F7
    mov ax, 0xf                                                      # CS:23F8
    push ax                                                          # CS:23FB
    mov ax, 0x2                                                      # CS:23FC
    push ax                                                          # CS:23FF
    call blit_hud_sprite_both_pages                                  # CS:2400
    ret                                                              # CS:2403

/* ========================================================================== 
 * draw_charge_3  |  CS:2404
 * Назначение: Рисует индикатор заряда 3 в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные адреса HUD — не адреса DS.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_charge_3, @function
draw_charge_3:
    mov ax, 0x43ee                                                   # CS:2404
    push ax                                                          # CS:2407
    mov ax, 0x1b9d                                                   # CS:2408
    push ax                                                          # CS:240B
    mov ax, 0x10                                                     # CS:240C
    push ax                                                          # CS:240F
    mov ax, 0xf                                                      # CS:2410
    push ax                                                          # CS:2413
    mov ax, 0x2                                                      # CS:2414
    push ax                                                          # CS:2417
    call blit_hud_sprite_both_pages                                  # CS:2418
    ret                                                              # CS:241B

/* ========================================================================== 
 * draw_charge_4  |  CS:241C
 * Назначение: Рисует индикатор заряда 4 в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные адреса HUD — не адреса DS.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_charge_4, @function
draw_charge_4:
    mov ax, 0x43ee                                                   # CS:241C
    push ax                                                          # CS:241F
    mov ax, 0x1b9f                                                   # CS:2420
    push ax                                                          # CS:2423
    mov ax, 0x10                                                     # CS:2424
    push ax                                                          # CS:2427
    mov ax, 0xf                                                      # CS:2428
    push ax                                                          # CS:242B
    mov ax, 0x2                                                      # CS:242C
    push ax                                                          # CS:242F
    call blit_hud_sprite_both_pages                                  # CS:2430
    ret                                                              # CS:2433

/* ========================================================================== 
 * draw_charge_5  |  CS:2434
 * Назначение: Рисует индикатор заряда 5 в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные адреса HUD — не адреса DS.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_charge_5, @function
draw_charge_5:
    mov ax, 0x43ee                                                   # CS:2434
    push ax                                                          # CS:2437
    mov ax, 0x1ba1                                                   # CS:2438
    push ax                                                          # CS:243B
    mov ax, 0x10                                                     # CS:243C
    push ax                                                          # CS:243F
    mov ax, 0xf                                                      # CS:2440
    push ax                                                          # CS:2443
    mov ax, 0x2                                                      # CS:2444
    push ax                                                          # CS:2447
    call blit_hud_sprite_both_pages                                  # CS:2448
    ret                                                              # CS:244B

/* ========================================================================== 
 * draw_charge_6  |  CS:244C
 * Назначение: Рисует индикатор заряда 6 в обеих страницах.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные адреса HUD — не адреса DS.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_charge_6, @function
draw_charge_6:
    mov ax, 0x43ee                                                   # CS:244C
    push ax                                                          # CS:244F
    mov ax, 0x1ba3                                                   # CS:2450
    push ax                                                          # CS:2453
    mov ax, 0x10                                                     # CS:2454
    push ax                                                          # CS:2457
    mov ax, 0xf                                                      # CS:2458
    push ax                                                          # CS:245B
    mov ax, 0x2                                                      # CS:245C
    push ax                                                          # CS:245F
    call blit_hud_sprite_both_pages                                  # CS:2460
    ret                                                              # CS:2463

/* ========================================================================== 
 * remove_side_charge  |  CS:2464
 * Назначение: Уменьшает число зарядов до минимума 0 и стирает прежний крайний индикатор.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Выбор значка сделан по значению ДО декремента.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type remove_side_charge, @function
remove_side_charge:
    mov ax, ds:[D_side_charge_units]                                 # CS:2464
    dec WORD PTR ds:[D_side_charge_units]                            # CS:2467
    jns L_remove_side_charge_2474                                    # CS:246B
    mov WORD PTR ds:[D_side_charge_units], 0x0                       # CS:246D
    ret                                                              # CS:2473
L_remove_side_charge_2474:
    ACC16 cmp, 0x1                                                   # CS:2474
    je L_remove_side_charge_2493                                     # CS:2477
    ACC16 cmp, 0x2                                                   # CS:2479
    je L_remove_side_charge_24a7                                     # CS:247C
    ACC16 cmp, 0x3                                                   # CS:247E
    je L_remove_side_charge_24bb                                     # CS:2481
    ACC16 cmp, 0x4                                                   # CS:2483
    je L_remove_side_charge_24cf                                     # CS:2486
    ACC16 cmp, 0x5                                                   # CS:2488
    je L_remove_side_charge_24e3                                     # CS:248B
    ACC16 cmp, 0x6                                                   # CS:248D
    je L_remove_side_charge_24f7                                     # CS:2490
    ret                                                              # CS:2492
L_remove_side_charge_2493:
    mov ax, 0x1b99                                                   # CS:2493
    push ax                                                          # CS:2496
    mov ax, 0x2                                                      # CS:2497
    push ax                                                          # CS:249A
    mov ax, 0xf                                                      # CS:249B
    push ax                                                          # CS:249E
    mov ax, 0x0                                                      # CS:249F
    push ax                                                          # CS:24A2
    call fill_hud_rect_both_pages                                    # CS:24A3
    ret                                                              # CS:24A6
L_remove_side_charge_24a7:
    mov ax, 0x1b9b                                                   # CS:24A7
    push ax                                                          # CS:24AA
    mov ax, 0x2                                                      # CS:24AB
    push ax                                                          # CS:24AE
    mov ax, 0xf                                                      # CS:24AF
    push ax                                                          # CS:24B2
    mov ax, 0x0                                                      # CS:24B3
    push ax                                                          # CS:24B6
    call fill_hud_rect_both_pages                                    # CS:24B7
    ret                                                              # CS:24BA
L_remove_side_charge_24bb:
    mov ax, 0x1b9d                                                   # CS:24BB
    push ax                                                          # CS:24BE
    mov ax, 0x2                                                      # CS:24BF
    push ax                                                          # CS:24C2
    mov ax, 0xf                                                      # CS:24C3
    push ax                                                          # CS:24C6
    mov ax, 0x0                                                      # CS:24C7
    push ax                                                          # CS:24CA
    call fill_hud_rect_both_pages                                    # CS:24CB
    ret                                                              # CS:24CE
L_remove_side_charge_24cf:
    mov ax, 0x1b9f                                                   # CS:24CF
    push ax                                                          # CS:24D2
    mov ax, 0x2                                                      # CS:24D3
    push ax                                                          # CS:24D6
    mov ax, 0xf                                                      # CS:24D7
    push ax                                                          # CS:24DA
    mov ax, 0x0                                                      # CS:24DB
    push ax                                                          # CS:24DE
    call fill_hud_rect_both_pages                                    # CS:24DF
    ret                                                              # CS:24E2
L_remove_side_charge_24e3:
    mov ax, 0x1ba1                                                   # CS:24E3
    push ax                                                          # CS:24E6
    mov ax, 0x2                                                      # CS:24E7
    push ax                                                          # CS:24EA
    mov ax, 0xf                                                      # CS:24EB
    push ax                                                          # CS:24EE
    mov ax, 0x0                                                      # CS:24EF
    push ax                                                          # CS:24F2
    call fill_hud_rect_both_pages                                    # CS:24F3
    ret                                                              # CS:24F6
L_remove_side_charge_24f7:
    mov ax, 0x1ba3                                                   # CS:24F7
    push ax                                                          # CS:24FA
    mov ax, 0x2                                                      # CS:24FB
    push ax                                                          # CS:24FE
    mov ax, 0xf                                                      # CS:24FF
    push ax                                                          # CS:2502
    mov ax, 0x0                                                      # CS:2503
    push ax                                                          # CS:2506
    call fill_hud_rect_both_pages                                    # CS:2507
    ret                                                              # CS:250A

/* ========================================================================== 
 * init_fuel_gauge  |  CS:250B
 * Назначение: Инициализирует шкалу, расходуемую движением (вероятно топливо), значением 44.
 * Вход: Нет аргументов.
 * Эффект/ограничения: fuel_delta=0; имя fuel выведено по использованию, не по авторским символам.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_fuel_gauge, @function
init_fuel_gauge:
    mov WORD PTR ds:[D_fuel_units], 0x2c                             # CS:250B
    mov WORD PTR ds:[D_fuel_delta], 0x0                              # CS:2511
    mov ax, 0x48f6                                                   # CS:2517
    push ax                                                          # CS:251A
    mov ax, 0x1b88                                                   # CS:251B
    push ax                                                          # CS:251E
    mov ax, 0x2c                                                     # CS:251F
    push ax                                                          # CS:2522
    mov ax, 0x7                                                      # CS:2523
    push ax                                                          # CS:2526
    mov ax, 0x2                                                      # CS:2527
    push ax                                                          # CS:252A
    call blit_hud_sprite_both_pages                                  # CS:252B
    ret                                                              # CS:252E

/* ========================================================================== 
 * decrease_fuel  |  CS:252F
 * Назначение: Уменьшает fuel_units, если результат неотрицателен.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Хвостовой переход в store_fuel_gauge; отрицательная ветка возвращает без записи.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type decrease_fuel, @function
decrease_fuel:
    mov ax, ds:[D_fuel_units]                                        # CS:252F
    dec ax                                                           # CS:2532
    jns store_fuel_gauge                                             # CS:2533
    ret                                                              # CS:2535

/* ========================================================================== 
 * store_fuel_gauge  |  CS:2536
 * Назначение: Записывает fuel_units и обновляет столбец шкалы.
 * Вход: AX = новое/текущее значение шкалы.
 * Эффект/ограничения: Общий хвост decrease_fuel; add_fuel вызывает его отдельно.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type store_fuel_gauge, @function
store_fuel_gauge:
    mov ds:[D_fuel_units], ax                                        # CS:2536
    push ax                                                          # CS:2539
    mov ax, 0x1b88                                                   # CS:253A
    push ax                                                          # CS:253D
    call toggle_hud_gauge_column                                     # CS:253E
    ret                                                              # CS:2541

/* ========================================================================== 
 * increase_fuel  |  CS:2542
 * Назначение: При значении<44 обновляет столбец и увеличивает fuel_units.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Отрисовка использует старое значение, инкремент сделан после неё.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type increase_fuel, @function
increase_fuel:
    mov ax, ds:[D_fuel_units]                                        # CS:2542
    ACC16 cmp, 0x2c                                                  # CS:2545
    jb L_increase_fuel_254b                                          # CS:2548
    ret                                                              # CS:254A
L_increase_fuel_254b:
    call store_fuel_gauge                                            # CS:254B
    inc WORD PTR ds:[D_fuel_units]                                   # CS:254E
    ret                                                              # CS:2552

/* ========================================================================== 
 * init_ammo_gauge  |  CS:2553
 * Назначение: Инициализирует шкалу, расходуемую стрельбой, значением 48.
 * Вход: Нет аргументов.
 * Эффект/ограничения: ammo_delta=0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_ammo_gauge, @function
init_ammo_gauge:
    mov WORD PTR ds:[D_ammo_units], 0x30                             # CS:2553
    mov WORD PTR ds:[D_ammo_delta], 0x0                              # CS:2559
    mov ax, 0x2ce6                                                   # CS:255F
    push ax                                                          # CS:2562
    mov ax, 0x1cc8                                                   # CS:2563
    push ax                                                          # CS:2566
    mov ax, 0x30                                                     # CS:2567
    push ax                                                          # CS:256A
    mov ax, 0x7                                                      # CS:256B
    push ax                                                          # CS:256E
    mov ax, 0x2                                                      # CS:256F
    push ax                                                          # CS:2572
    call blit_hud_sprite_both_pages                                  # CS:2573
    ret                                                              # CS:2576

/* ========================================================================== 
 * decrease_ammo  |  CS:2577
 * Назначение: Уменьшает ammo_units без ухода ниже нуля.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Хвостовой переход в store_ammo_gauge.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type decrease_ammo, @function
decrease_ammo:
    mov ax, ds:[D_ammo_units]                                        # CS:2577
    dec ax                                                           # CS:257A
    jns store_ammo_gauge                                             # CS:257B
    ret                                                              # CS:257D

/* ========================================================================== 
 * store_ammo_gauge  |  CS:257E
 * Назначение: Записывает ammo_units и обновляет столбец шкалы.
 * Вход: AX = значение.
 * Эффект/ограничения: Общий хвост, также самостоятельная CALL-цель.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type store_ammo_gauge, @function
store_ammo_gauge:
    mov ds:[D_ammo_units], ax                                        # CS:257E
    push ax                                                          # CS:2581
    mov ax, 0x1cc8                                                   # CS:2582
    push ax                                                          # CS:2585
    call toggle_hud_gauge_column                                     # CS:2586
    ret                                                              # CS:2589

/* ========================================================================== 
 * increase_ammo  |  CS:258A
 * Назначение: При значении<48 обновляет столбец и увеличивает ammo_units.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Отрисовка до инкремента.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type increase_ammo, @function
increase_ammo:
    mov ax, ds:[D_ammo_units]                                        # CS:258A
    ACC16 cmp, 0x30                                                  # CS:258D
    jb L_increase_ammo_2593                                          # CS:2590
    ret                                                              # CS:2592
L_increase_ammo_2593:
    call store_ammo_gauge                                            # CS:2593
    inc WORD PTR ds:[D_ammo_units]                                   # CS:2596
    ret                                                              # CS:259A

/* ========================================================================== 
 * init_shield_gauge  |  CS:259B
 * Назначение: Инициализирует шкалу прочности значением 56.
 * Вход: Нет аргументов.
 * Эффект/ограничения: shield_delta=0; имя shield условное по применению урона.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_shield_gauge, @function
init_shield_gauge:
    mov WORD PTR ds:[D_shield_units], 0x38                           # CS:259B
    mov WORD PTR ds:[D_shield_delta], 0x0                            # CS:25A1
    mov ax, 0x4496                                                   # CS:25A7
    push ax                                                          # CS:25AA
    mov ax, 0x1e08                                                   # CS:25AB
    push ax                                                          # CS:25AE
    mov ax, 0x38                                                     # CS:25AF
    push ax                                                          # CS:25B2
    mov ax, 0x7                                                      # CS:25B3
    push ax                                                          # CS:25B6
    mov ax, 0x2                                                      # CS:25B7
    push ax                                                          # CS:25BA
    call blit_hud_sprite_both_pages                                  # CS:25BB
    ret                                                              # CS:25BE

/* ========================================================================== 
 * decrease_shield  |  CS:25BF
 * Назначение: Уменьшает прочность без ухода ниже нуля.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Хвостовой переход в store_shield_gauge.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type decrease_shield, @function
decrease_shield:
    mov ax, ds:[D_shield_units]                                      # CS:25BF
    dec ax                                                           # CS:25C2
    jns store_shield_gauge                                           # CS:25C3
    ret                                                              # CS:25C5

/* ========================================================================== 
 * store_shield_gauge  |  CS:25C6
 * Назначение: Записывает shield_units и обновляет столбец шкалы.
 * Вход: AX = значение.
 * Эффект/ограничения: Общий хвост, также CALL-цель.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type store_shield_gauge, @function
store_shield_gauge:
    mov ds:[D_shield_units], ax                                      # CS:25C6
    push ax                                                          # CS:25C9
    mov ax, 0x1e08                                                   # CS:25CA
    push ax                                                          # CS:25CD
    call toggle_hud_gauge_column                                     # CS:25CE
    ret                                                              # CS:25D1

/* ========================================================================== 
 * increase_shield  |  CS:25D2
 * Назначение: При значении<56 обновляет столбец и увеличивает прочность.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Отрисовка до инкремента.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type increase_shield, @function
increase_shield:
    mov ax, ds:[D_shield_units]                                      # CS:25D2
    ACC16 cmp, 0x38                                                  # CS:25D5
    jb L_increase_shield_25db                                        # CS:25D8
    ret                                                              # CS:25DA
L_increase_shield_25db:
    call store_shield_gauge                                          # CS:25DB
    inc WORD PTR ds:[D_shield_units]                                 # CS:25DE
    ret                                                              # CS:25E2

/* ========================================================================== 
 * toggle_hud_gauge_column  |  CS:25E3
 * Назначение: Меняет выбранный бит столбца высотой 7 строк в обеих видеостраницах с графической логической операцией.
 * Вход: [BP+4]=video offset; [BP+6]=column pixel.
 * Эффект/ограничения: RET 4; это не простая запись цвета в packed-массив, действует состояние EGA.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type toggle_hud_gauge_column, @function
toggle_hud_gauge_column:
    push bp                                                          # CS:25E3
    {load} mov bp, sp                                                # CS:25E4
    mov ax, 0xa000                                                   # CS:25E6
    mov es, ax                                                       # CS:25E9
    mov ax, WORD PTR [bp+0x6]                                        # CS:25EB
    shr ax, 1                                                        # CS:25EE
    shr ax, 1                                                        # CS:25F0
    shr ax, 1                                                        # CS:25F2
    add ax, WORD PTR [bp+0x4]                                        # CS:25F4
    {load} mov di, ax                                                # CS:25F7
    mov dx, 0x3ce                                                    # CS:25F9
    mov ax, 0x805                                                    # CS:25FC
    out dx, ax                                                       # CS:25FF
    mov cx, WORD PTR [bp+0x6]                                        # CS:2600
    and cx, 0x7                                                      # CS:2603
    mov ah, 0x80                                                     # CS:2606
    shr ah, cl                                                       # CS:2608
    mov al, 0x8                                                      # CS:260A
    out dx, ax                                                       # CS:260C
    {load} mov bl, ah                                                # CS:260D
    mov ax, 0x1803                                                   # CS:260F
    out dx, ax                                                       # CS:2612
    mov ax, 0xf01                                                    # CS:2613
    out dx, ax                                                       # CS:2616
    mov ax, 0x700                                                    # CS:2617
    out dx, ax                                                       # CS:261A
    mov cx, 0x7                                                      # CS:261B
L_toggle_hud_gauge_column_261e:
    mov al, BYTE PTR es:[di]                                         # CS:261E
    test bl, al                                                      # CS:2621
    jne L_toggle_hud_gauge_column_262d                               # CS:2623
    mov BYTE PTR es:[di], bl                                         # CS:2625
    mov BYTE PTR es:[di+0x2000], bl                                  # CS:2628
L_toggle_hud_gauge_column_262d:
    add di, 0x28                                                     # CS:262D
    loop L_toggle_hud_gauge_column_261e                              # CS:2630
    mov ax, 0x5                                                      # CS:2632
    out dx, ax                                                       # CS:2635
    mov ax, 0xff08                                                   # CS:2636
    out dx, ax                                                       # CS:2639
    mov ax, 0x3                                                      # CS:263A
    out dx, ax                                                       # CS:263D
    mov ax, 0x1                                                      # CS:263E
    out dx, ax                                                       # CS:2641
    mov ax, 0x0                                                      # CS:2642
    out dx, ax                                                       # CS:2645
    pop bp                                                           # CS:2646
    ret 0x4                                                          # CS:2647
