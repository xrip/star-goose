/* Абсолютные DS offsets — не flat-адреса процесса host ОС.
 * D_*: смещение. dat_*: физическая метка в образе EXE.
 * Смещение поля Entity не следует называть D_* даже при совпадении числа.
 * Подтверждённые типы — по ширине обращений; unknown остаются нерасшифрованными. */
.equ D_attract_seen                             , 0x0006  # u8: Первый attract уже завершён.
.equ D_msg_insufficient_ram                     , 0x0007  # DOS string: Сообщение о недостатке памяти, завершается долларом.
.equ D_msg_loading                              , 0x002a  # DOS string: Сообщение о загрузке, завершается долларом.
.equ D_key_to_character                         , 0x005a  # u8[128]: Таблица scan-code → символ для ввода имени.
.equ D_attract_script_scores                    , 0x00da  # byte stream: Сценарий после игровой сессии; первый читаемый элемент имеет индекс1.
.equ D_attract_script_intro                     , 0x00e9  # byte stream: Сценарий первого показа; первый читаемый элемент имеет индекс1.
.equ D_template_attract                         , 0x0109  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_sprite_frames_0120                       , 0x0120  # FrameSet[24]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_highscore_layout                         , 0x01b1  # records: Координаты/смещения для имён и результатов шести строк.
.equ D_highscore_name_destinations              , 0x01b3  # strided u16: Первое поле адреса имени; следующий элемент через8 байт.
.equ D_filename_bird1                           , 0x01e2  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_filename_bird2                           , 0x01ea  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_filename_blox                            , 0x01f2  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_filename_newbird                         , 0x01f9  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_filename_podz1                           , 0x0203  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_filename_intro                           , 0x020b  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_level_tile_streams                       , 0x0214  # u16[24]: 8 уровней ×3 части: DS offsets RLE-потоков 12×64.
.equ D_level_object_streams                     , 0x0244  # u16[24]: Потоки троек размещения объектов, терминатор FFh.
.equ D_template_map_tile                        , 0x0274  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_map_tile_template_z                      , 0x0277  # i8: Высотное поле шаблона DS:0274.
.equ D_map_tile_template_x                      , 0x0279  # u16: Поле x шаблона DS:0274.
.equ D_map_tile_template_y                      , 0x027b  # u16: Поле world_y шаблона DS:0274.
.equ D_map_tile_template_width                  , 0x027d  # u8: Поле width шаблона DS:0274.
.equ D_map_tile_template_height                 , 0x027e  # u8: Поле height шаблона DS:0274.
.equ D_map_tile_template_bank                   , 0x0281  # u16: Смещение в segment table, не номер файла.
.equ D_map_tile_template_sprite                 , 0x0283  # u16: Offset packed4bpp-изображения тайла.
.equ D_map_entity_templates                     , 0x028b  # u16[21]: Шаблон Entity по kind, извлечённому из маски строки.
.equ D_map_collision_damage                     , 0x02b5  # u8[21]: Код/урон препятствия; 0 означает отсутствие столкновения.
.equ D_map_collision_halfwidth                  , 0x02ca  # u16[21]: Полуширина области x для препятствия.
.equ D_map_collision_rightmost_x                , 0x02f4  # u16[21]: Базовая координата первого проверяемого из10 столбцов.
.equ D_entity_replacement_templates             , 0x031e  # u16[8]: Шаблон по прежнему handler_id; только подмножество ID используется.
.equ D_collectible_sprite_offsets               , 0x032e  # u16[6]: Шесть изображений коллекционных объектов.
.equ D_template_map_kind_00                     , 0x033a  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_01                     , 0x0351  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_02                     , 0x0368  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_03                     , 0x037f  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_04                     , 0x0396  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_05                     , 0x03ad  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_06                     , 0x03c4  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_10                     , 0x03db  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_08                     , 0x03f2  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_09                     , 0x0409  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_07                     , 0x0420  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_11                     , 0x0437  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_12                     , 0x044e  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_14                     , 0x0465  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_13                     , 0x047c  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_map_kind_15                     , 0x0493  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_collectible_00                  , 0x04aa  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_collectible_03                  , 0x04c1  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_collectible_04                  , 0x04d8  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_collectible_05                  , 0x04ef  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_collectible_06                  , 0x0506  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_tile_heights                             , 0x051d  # u8[42]: Высота изображения тайла в пикселях.
.equ D_tile_y_adjust                            , 0x0547  # i8[42]: Поправка y изображения тайла.
.equ D_tile_height_delta                        , 0x0571  # i8[42]: Перепад высоты от левого к правому краю тайла.
.equ D_tile_surface_variant_a                   , 0x059b  # u8[42]: Вариант frame/surface при очищенном бите80h sample.
.equ D_tile_surface_variant_b                   , 0x05c5  # u8[42]: Вариант при установленном бите80h sample.
.equ D_tile_sprite_offsets                      , 0x05ef  # u16[42]: Смещения packed-изображений тайлов в выбранном bank.
.equ D_tile_profile_offsets                     , 0x0643  # u16[42]: Указатели на 64-байтовые микропрофили 8×8.
.equ D_map_column_bits                          , 0x0cd7  # u16[10]: Биты столбцов: 0200h..0001h.
.equ D_map_column_clear_masks                   , 0x0ceb  # u16[12]: Инверсные маски для пометки использованного объекта; значения сохранить точно.
.equ D_template_tunnel_player                   , 0x0d04  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_tunnel_side                     , 0x0d1b  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_template_surface_player                  , 0x0d32  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_sprite_frames_0d49                       , 0x0d49  # FrameSet[11]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_template_respawn_effect                  , 0x0da2  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_side_device_z_offsets                    , 0x0db9  # i8 table: Поправка высоты бокового устройства по frame игрока.
.equ D_template_side_device                     , 0x0dc3  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_sprite_frames_0dda                       , 0x0dda  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0e2b                       , 0x0e2b  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_pingpong_frame_sequence                  , 0x0e4c  # u8[6]: Последовательность циклической анимации handler3.
.equ D_sprite_frames_0e52                       , 0x0e52  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0e73                       , 0x0e73  # FrameSet[5]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0e9c                       , 0x0e9c  # FrameSet[5]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0ec5                       , 0x0ec5  # FrameSet[5]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0eee                       , 0x0eee  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0f3f                       , 0x0f3f  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_0f90                       , 0x0f90  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_mobile_enemy_descriptors                 , 0x0fe1  # u16[3]: Таблица descriptor по param мобильного врага.
.equ D_shot_variation_sequence                  , 0x0fe7  # u8[16]: Детерминированная циклическая вариация; не PRNG.
.equ D_template_muzzle_effect                   , 0x0ff7  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_shot_effect_template_z                   , 0x0ffa  # i8: Высота шаблона DS:0FF7.
.equ D_shot_effect_template_frame               , 0x0ffb  # u8: Frame/state шаблона эффекта.
.equ D_shot_effect_template_x                   , 0x0ffc  # u16: Координата x шаблона эффекта.
.equ D_shot_effect_template_y                   , 0x0ffe  # u16: Координата y шаблона эффекта.
.equ D_sprite_frames_100e                       , 0x100e  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_aimed_effect_descriptors                 , 0x102f  # u16[5]: Sprite descriptors по направлению выстрела.
.equ D_sprite_frames_1039                       , 0x1039  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_105a                       , 0x105a  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_107b                       , 0x107b  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_109c                       , 0x109c  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_10bd                       , 0x10bd  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_ground_shot_dx                           , 0x10de  # i8[5]: Поправки spawn x по направлению.
.equ D_ground_shot_dy                           , 0x10e3  # i8[5]: Поправки spawn y по направлению.
.equ D_template_ground_shot                     , 0x10e8  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_ground_shot_descriptors                  , 0x10ff  # u16[5]: Sprite descriptors наземного снаряда.
.equ D_sprite_frames_1109                       , 0x1109  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_115a                       , 0x115a  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_11ab                       , 0x11ab  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_11fc                       , 0x11fc  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_sprite_frames_124d                       , 0x124d  # FrameSet[10]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_template_explosion                       , 0x129e  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_explosion_template_layer                 , 0x12a0  # u8: Layer шаблона взрыва DS:129E.
.equ D_explosion_template_z                     , 0x12a1  # i8: Z шаблона взрыва.
.equ D_explosion_template_x                     , 0x12a3  # u16: X шаблона взрыва.
.equ D_explosion_template_y                     , 0x12a5  # u16: Y шаблона взрыва.
.equ D_sprite_frames_12b5                       , 0x12b5  # FrameSet[5]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_template_arc_effect                      , 0x12de  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_sprite_frames_12f5                       , 0x12f5  # FrameSet[6]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_side_impact_template                     , 0x1326  # Entity: Шаблон эффектов двух сторон; flags20h меняется перед копированием.
.equ D_sprite_frames_133d                       , 0x133d  # FrameSet[4]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_template_player_shot                     , 0x135e  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_player_shot_template_z                   , 0x1361  # i8: Поле шаблона player shot DS:135E.
.equ D_player_shot_template_x                   , 0x1363  # u16: Поле шаблона player shot.
.equ D_player_shot_template_y                   , 0x1365  # u16: Поле шаблона player shot.
.equ D_player_left_muzzle_dx                    , 0x1375  # i8[10]: Смещения левого ствола по frame.
.equ D_player_right_muzzle_dx                   , 0x137f  # i8[10]: Смещения правого ствола по frame.
.equ D_player_left_muzzle_dz                    , 0x1389  # i8[10]: Поправка высоты левого ствола.
.equ D_player_right_muzzle_dz                   , 0x1393  # i8[10]: Поправка высоты правого ствола.
.equ D_player_shot_frame_map                    , 0x139d  # u8[10]: Выбор варианта выстрела по frame игрока.
.equ D_shot_variant_heights                     , 0x13a7  # u8[3]: Высоты трёх вариантов выстрела.
.equ D_player_shot_sprite_offsets               , 0x13aa  # u16[3]: Sprite offsets выстрела игрока.
.equ D_template_enemy_shot                      , 0x13b0  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_enemy_shot_sprite_offsets                , 0x13c7  # u16[3]: Sprite offsets выстрела противника.
.equ D_enemy_muzzle_a_left_dz                   , 0x13cd  # i8[10]: Таблица высот pattern0, левая точка.
.equ D_enemy_muzzle_a_right_dz                  , 0x13d7  # i8[10]: Таблица высот pattern0, правая точка.
.equ D_enemy_muzzle_b_dz                        , 0x13e1  # i8[10]: Таблица высот pattern1.
.equ D_enemy_muzzle_c_left_dz                   , 0x13eb  # i8[10]: Таблица высот pattern2, левая точка.
.equ D_enemy_muzzle_c_center_dz                 , 0x13f5  # i8[10]: Таблица высот pattern2, центральная точка.
.equ D_enemy_muzzle_c_right_dz                  , 0x13ff  # i8[10]: Таблица высот pattern2, правая точка.
.equ D_aimed_muzzle_dx                          , 0x1409  # i8[5]: X offset по направлению.
.equ D_aimed_muzzle_dy                          , 0x140e  # i8[5]: Y offset по направлению.
.equ D_enemy_shot_widths                        , 0x1413  # u8[5]: Ширина выстрела по направлению.
.equ D_enemy_shot_heights                       , 0x1418  # u8[5]: Высота выстрела по направлению.
.equ D_enemy_aimed_shot_sprites                 , 0x141d  # u16[5]: Sprite offsets направленных выстрелов.
.equ D_template_impact                          , 0x1427  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_impact_frame_sprites                     , 0x143e  # u16[4]: Четыре последовательных кадра попадания.
.equ D_template_arc_shot                        , 0x1446  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_arc_spawn_dx                             , 0x145d  # i8[5]: X offset навесного снаряда.
.equ D_arc_spawn_dy                             , 0x1462  # i8[5]: Y offset навесного снаряда.
.equ D_arc_height_deltas                        , 0x1467  # i8[16]: Вертикальные приращения по возрасту снаряда.
.equ D_tunnel_gate_frame_sprites                , 0x1477  # u16[6]: Кадры створок входа; fully-open скрывает layer.
.equ D_exit_gate_frame_sprites                  , 0x1483  # u16[9]: Кадры ворот выхода после6 collectibles.
.equ D_tunnel_destinations_a                    , 0x1495  # u16[24]: Destination_y в10 битах и reward kind в старших2.
.equ D_tunnel_destinations_b                    , 0x14c5  # u16[24]: Вторая таблица входов/выходов туннеля.
.equ D_level_start_y                            , 0x14f5  # u16[8]: Исходная позиция камеры по level_index.
.equ D_filename_hiscores                        , 0x1506  # ASCIIZ: Имя внешнего файла, нулевой терминатор.
.equ D_control_prompt_banks                     , 0x150f  # u16[8]: Bank offsets фрагментов запросов управления.
.equ D_control_prompt_sources                   , 0x151f  # u16[8]: Sprite offsets запросов управления.
.equ D_control_prompt_destinations              , 0x152f  # u16[8]: Адреса назначения в video RAM, не DS-указатели.
.equ D_control_prompt_widths                    , 0x153f  # u16[8]: Ширины фрагментов в пикселях.
.equ D_control_prompt_heights                   , 0x154f  # u16[8]: Высоты фрагментов.
.equ D_hud_digit_sprites                        , 0x155f  # u16[10]: Sprite offsets цифр HUD.
.equ D_hud_digit_destinations                   , 0x1573  # u16[6]: Адреса цифр в video RAM.
.equ D_highscore_digit_sprites                  , 0x157f  # u16[10]: Sprite offsets цифр таблицы рекордов.
.equ D_sprite_frames_1594                       , 0x1594  # FrameSet[28]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_tunnel_circle_x                          , 0x1675  # i8[128]: Готовые x координаты окружности; сохранить lookup, не заменять sin/cos.
.equ D_tunnel_circle_y                          , 0x16f5  # i8[128]: Готовые y координаты окружности.
.equ D_tunnel_player_frames                     , 0x1775  # u8[128]: Frame корабля по углу.
.equ D_tunnel_frame_to_sector                   , 0x17f5  # u8[28]: Сектор сбора предмета по frame игрока; фактический домен см. таблицу.
.equ D_sprite_frames_1811                       , 0x1811  # FrameSet[16]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_tunnel_side_frames                       , 0x1892  # u8[128]: Frame бокового устройства по углу игрока.
.equ D_template_tunnel_exit                     , 0x1912  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_sprite_frames_1929                       , 0x1929  # FrameSet[24]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_tunnel_exit_frames                       , 0x19ea  # u8[32]: Анимация выходного маркера по remaining distance.
.equ D_tunnel_min_ring_radii                    , 0x1a0a  # u8[32]: Минимальный радиус колец по дистанции.
.equ D_tunnel_pattern_pointers                  , 0x1a2a  # u16[4]: Четыре потока появления туннельных предметов.
.equ D_tunnel_pickup_radii                      , 0x1a6a  # u8[48]: Радиус изображения по дистанции от spawn.
.equ D_tunnel_pickup_layers                     , 0x1a9a  # u8[24]: Layer по кадру приближения предмета.
.equ D_template_tunnel_pickup                   , 0x1ab2  # Entity template: Изменяемый шаблон 23 байта; структура см. entity.inc.
.equ D_sprite_frames_1ac9                       , 0x1ac9  # FrameSet[24]: SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[].
.equ D_entity_records                           , 0x1b8a  # Entity[48]: Пул сущностей: 48 ×23 байта; первая запись — игрок.
.equ D_player_layer                             , 0x1b8c  # u8: Поле+02 первой записи пула.
.equ D_player_z                                 , 0x1b8d  # i8: Поле+03 первой записи; знаковая высота/проекционное смещение.
.equ D_player_frame                             , 0x1b8e  # u8: Поле+04 первой записи, также вариант рельефа.
.equ D_player_x                                 , 0x1b8f  # u16: Поле+05 первой записи.
.equ D_player_y                                 , 0x1b91  # u16: Поле+07 первой записи; modulo1024 на поверхности.
.equ D_player_handler                           , 0x1b9c  # u8: Поле+12 первой записи, состояние игрока.
.equ D_player_map_row                           , 0x1b9e  # u8: Поле+14 первой записи; в туннеле пересекается с иными state.
.equ D_player_angle_or_age                      , 0x1ba0  # u8: Поле+16: в туннеле угол, в иных состояниях счётчик.
.equ D_tunnel_ring_radius_curve                 , 0x1fdc  # u8[64]: Радиус по младшим6 битам дистанции.
.equ D_tunnel_edge_x_curve                      , 0x201c  # u8[13]: Дополнительные линии при phase<=12.
.equ D_tunnel_edge_y_curve                      , 0x2029  # u8[13]: Вторая таблица дополнительных линий.
.equ D_level_01_part_1_tiles                    , 0x62b6  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_01_part_2_tiles                    , 0x6445  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_01_part_3_tiles                    , 0x65d4  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_01_part_1_objects                  , 0x6755  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_01_part_2_objects                  , 0x678c  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_01_part_3_objects                  , 0x67c3  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_02_part_1_tiles                    , 0x67f7  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_02_part_2_tiles                    , 0x697f  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_02_part_3_tiles                    , 0x6b3b  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_02_part_1_objects                  , 0x6cac  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_02_part_2_objects                  , 0x6ce9  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_02_part_3_objects                  , 0x6d29  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_03_part_1_tiles                    , 0x6d5d  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_03_part_2_tiles                    , 0x6ed7  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_03_part_3_tiles                    , 0x7053  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_03_part_1_objects                  , 0x71dc  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_03_part_2_objects                  , 0x7228  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_03_part_3_objects                  , 0x725f  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_04_part_1_tiles                    , 0x7293  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_04_part_2_tiles                    , 0x7416  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_04_part_3_tiles                    , 0x75a8  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_04_part_1_objects                  , 0x7738  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_04_part_2_objects                  , 0x7781  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_04_part_3_objects                  , 0x77b8  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_05_part_1_tiles                    , 0x77fc  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_05_part_2_tiles                    , 0x7992  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_05_part_3_tiles                    , 0x7aee  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_05_part_1_objects                  , 0x7c86  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_05_part_2_objects                  , 0x7cb7  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_05_part_3_objects                  , 0x7cee  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_06_part_1_tiles                    , 0x7d2e  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_06_part_2_tiles                    , 0x7e7e  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_06_part_3_tiles                    , 0x7fbc  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_06_part_1_objects                  , 0x8108  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_06_part_2_objects                  , 0x815a  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_06_part_3_objects                  , 0x818b  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_07_part_1_tiles                    , 0x81e3  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_07_part_2_tiles                    , 0x832d  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_07_part_3_tiles                    , 0x8439  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_07_part_1_objects                  , 0x8596  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_07_part_2_objects                  , 0x85d9  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_07_part_3_objects                  , 0x8610  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_08_part_1_tiles                    , 0x8659  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_08_part_2_tiles                    , 0x87f7  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_08_part_3_tiles                    , 0x89ad  # RLE stream: Точные RLE-токены; декодирование даёт12×64 клетки.
.equ D_level_08_part_1_objects                  , 0x8b0f  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_08_part_2_objects                  , 0x8b5b  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_level_08_part_3_objects                  , 0x8ba4  # placement stream: Тройки kind,column,source_row; FFh завершает поток.
.equ D_sound_descriptor_pointers                , 0x8be4  # u16[15]: Таблица 6-байтовых звуковых descriptor.
.equ D_sound_descriptor_01                      , 0x8c02  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_02                      , 0x8c08  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_03                      , 0x8c0e  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_04                      , 0x8c14  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_05                      , 0x8c1a  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_06                      , 0x8c20  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_07                      , 0x8c26  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_08                      , 0x8c2c  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_charge_sound_divisor                     , 0x8c2e  # u16: Динамически меняется по side_charge_units.
.equ D_sound_descriptor_09                      , 0x8c32  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_angle_sound_divisor                      , 0x8c34  # u16: Динамически меняется по углу игрока.
.equ D_sound_descriptor_10                      , 0x8c38  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_11                      , 0x8c3e  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_12                      , 0x8c44  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_13                      , 0x8c4a  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_sound_descriptor_14                      , 0x8c50  # SoundDescriptor: modulation u8, duration u8, divisor u16, step i16; 6 bytes.
.equ D_music_descriptor_modulation              , 0x8c56  # u8: Первый байт изменяемого descriptor15.
.equ D_music_descriptor_duration                , 0x8c57  # u8: Время изменяемого descriptor15.
.equ D_music_descriptor_divisor                 , 0x8c58  # u16: PIT-divisor выбранной ноты.
.equ D_sound_modulation_pointers                , 0x8c5c  # u16[4]: Указатели на циклы модуляции по16 слов.
.equ D_sound_modulation_1                       , 0x8c64  # i16[16]: Циклическая добавка к делителю; индекс&15.
.equ D_sound_modulation_2                       , 0x8c84  # i16[16]: Циклическая добавка к делителю; индекс&15.
.equ D_sound_modulation_3                       , 0x8ca4  # i16[16]: Циклическая добавка к делителю; индекс&15.
.equ D_sound_modulation_4                       , 0x8cc4  # i16[16]: Циклическая добавка к делителю; индекс&15.
.equ D_note_divisors                            , 0x8ce4  # u16 table: Делитель PIT2 по note index.
.equ D_music_fill_notes                         , 0x8e5d  # u8[32]: Ноты специальной нулевой команды музыки.
.equ D_attract_script_pointer                   , 0x8e7e  # u16: DS offset сценария заставки.
.equ D_attract_step_counter                     , 0x8e80  # u16: Счётчик кадров сценария; команда каждую8-ю итерацию.
.equ D_old_keyboard_segment                     , 0x8e82  # u16: Сегмент старого INT09h.
.equ D_old_keyboard_offset                      , 0x8e84  # u16: Offset старого INT09h.
.equ D_old_timer_segment                        , 0x8e86  # u16: Сегмент старого INT1Ch.
.equ D_old_timer_offset                         , 0x8e88  # u16: Offset старого INT1Ch.
.equ D_keyboard_state                           , 0x8e8a  # u8[128]: 0 = отпущена, 1 = нажата; индекс — scan-code.
.equ D_mouse_present                            , 0x8f0a  # u8: Признак установленного драйвера мыши.
.equ D_last_key_pointer                         , 0x8f0b  # u16: DS offset последней клавиши при вводе имени.
.equ D_key_left                                 , 0x8f0d  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_right                                , 0x8f0f  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_up                                   , 0x8f11  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_down                                 , 0x8f13  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_fire                                 , 0x8f15  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_action_n                             , 0x8f17  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_action_m                             , 0x8f19  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_key_quit                                 , 0x8f1b  # u16: Настроенный scan-code; фактически используется младший байт.
.equ D_file_handle                              , 0x8f1e  # u16: DOS file handle.
.equ D_file_destination_segment                 , 0x8f20  # u16: Временный сегмент назначения DOS read.
.equ D_map_running_height                       , 0x8f22  # u8: Накопленная высота вдоль текущей строки тайлов.
.equ D_map_row_y                                , 0x8f23  # u16: Текущая y-строка карты при prerender.
.equ D_map_tile_cursor                          , 0x8f25  # u16: DS offset следующего декодированного тайла.
.equ D_decoded_map_tiles                        , 0x8f29  # u8[768]: 12 столбцов ×64 строки; хранение обратное world_y.
.equ D_map_spawn_rows                           , 0x9229  # u16[64]: 10 бит колонок +6 бит вида объекта строки.
.equ D_map_available_masks                      , 0x92a9  # u16[3][64]: Сброшенный бит означает использованный объект этой части уровня.
.equ D_collision_damage_tmp                     , 0x9429  # u8: Временный результат query_player_map_collision.
.equ D_collision_column_tmp                     , 0x942c  # u16: Текущий столбец поиска препятствия.
.equ D_sprite_dest_segment                      , 0x942e  # u16: EGA segment для следующего blit.
.equ D_sprite_clip_enabled                      , 0x9430  # u16: 1 для игрового экрана, 0 для prerender карты.
.equ D_player_forced_stop                       , 0x9432  # u8: Запрос отдельного состояния игрока после выхода из туннеля.
.equ D_player_history_x0                        , 0x9433  # u16: Новейшая из4 сохранённых координат x.
.equ D_player_history_x1                        , 0x9435  # u16: История x, задержка1.
.equ D_player_history_x2                        , 0x9437  # u16: История x, задержка2.
.equ D_player_history_x3                        , 0x9439  # u16: История x, задержка3.
.equ D_player_history_y0                        , 0x943b  # u16: Новейшая y.
.equ D_player_history_y1                        , 0x943d  # u16: История y1.
.equ D_player_history_y2                        , 0x943f  # u16: История y2.
.equ D_player_history_y3                        , 0x9441  # u16: История y3.
.equ D_player_history_frame0                    , 0x9443  # u8: Новейший frame.
.equ D_player_history_frame1                    , 0x9444  # u8: История frame1.
.equ D_player_history_frame2                    , 0x9445  # u8: История frame2.
.equ D_player_history_frame3                    , 0x9446  # u8: История frame3.
.equ D_player_history_z0                        , 0x9447  # i8: Новейшая высота.
.equ D_player_history_z1                        , 0x9448  # i8: История z1.
.equ D_player_history_z2                        , 0x9449  # i8: История z2.
.equ D_player_history_z3                        , 0x944a  # i8: История z3.
.equ D_hud_frame_counter                        , 0x944c  # u8: Очки за этап начисляются при (counter&3)==0.
.equ D_score_leading_zero                       , 0x944d  # u8: Флаг подавления ведущих нулей.
.equ D_text_draw_offset                         , 0x944e  # u16: Offset в PODZ1 для текста таблицы рекордов.
.equ D_pending_highscore_rank                   , 0x9450  # u8: Строка редактируемого имени.
.equ D_pending_name_pointer                     , 0x9451  # u16: DS offset 12-байтового имени.
.equ D_name_cursor                              , 0x9453  # u16: Позиция0..12 при редактировании имени.
.equ D_highscore_data                           , 0x9455  # u8[140]: HISCORES: 84 байта имён, 42 цифр, 7 scratch, 7 индексов.
.equ D_score_digit_100000                       , 0x94a9  # u8: Первая из6 цифр текущего счёта; начало7 результатов.
.equ D_score_digit_10000                        , 0x94aa  # u8: Десятичная цифра10000.
.equ D_score_digit_1000                         , 0x94ab  # u8: Десятичная цифра1000.
.equ D_score_digit_100                          , 0x94ac  # u8: Десятичная цифра100.
.equ D_score_digit_10                           , 0x94ad  # u8: Десятичная цифра10.
.equ D_score_digit_1                            , 0x94ae  # u8: Десятичная цифра1.
.equ D_highscore_sort_scratch0                  , 0x94d3  # u8: Первый из7 scratch индексов сортировки.
.equ D_highscore_sort_scratch1                  , 0x94d4  # u8: Scratch1.
.equ D_highscore_sort_scratch2                  , 0x94d5  # u8: Scratch2.
.equ D_highscore_sort_scratch3                  , 0x94d6  # u8: Scratch3.
.equ D_highscore_sort_scratch4                  , 0x94d7  # u8: Scratch4.
.equ D_highscore_sort_scratch5                  , 0x94d8  # u8: Scratch5.
.equ D_highscore_sort_scratch6                  , 0x94d9  # u8: Scratch6.
.equ D_highscore_order0                         , 0x94da  # u8: Первый из7 отсортированных индексов.
.equ D_highscore_order1                         , 0x94db  # u8: Order1.
.equ D_highscore_order2                         , 0x94dc  # u8: Order2.
.equ D_highscore_order3                         , 0x94dd  # u8: Order3.
.equ D_highscore_order4                         , 0x94de  # u8: Order4.
.equ D_highscore_order5                         , 0x94df  # u8: Order5.
.equ D_highscore_order6                         , 0x94e0  # u8: Последний индекс; 0 означает, что текущий результат не вошёл в6 лучших.
.equ D_tunnel_pattern_pointer                   , 0x94e2  # u16: DS offset потока объектов туннеля.
.equ D_tunnel_pattern_index                     , 0x94e4  # u8: Циклический выбор паттерна по &3.
.equ D_tunnel_angle_history0                    , 0x94e5  # u8: История угла0.
.equ D_tunnel_angle_history1                    , 0x94e6  # u8: История угла1.
.equ D_tunnel_angle_history2                    , 0x94e7  # u8: История угла2.
.equ D_segment_bird1                            , 0x94e8  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_bird2                            , 0x94ea  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_blox                             , 0x94ec  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_newbird                          , 0x94ee  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_podz1                            , 0x94f0  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_intro                            , 0x94f2  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_scratch0                         , 0x94f4  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_segment_scratch1                         , 0x94f6  # u16 segment: Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице.
.equ D_video_draw_segment                       , 0x94f8  # u16: Сегмент текущей экранной страницы для рисования.
.equ D_video_display_segment                    , 0x94fa  # u16: Сегмент второй видеостраницы.
.equ D_base_scroll_speed                        , 0x94fc  # u16/mixed: Базовая скорость; start_game_cycle пишет только младший байт.
.equ D_stage_counter                            , 0x94fe  # u8: Число этапов, влияет на базовую скорость и очки.
.equ D_tunnel_mode                              , 0x94ff  # u8: 0=поверхность,1=туннель.
.equ D_game_loop_end_flag                       , 0x9500  # u8: Запрос завершить game_loop.
.equ D_level_index                              , 0x9501  # u8: Индекс уровня 0..7.
.equ D_level_part                               , 0x9502  # u8: Часть уровня 0..2.
.equ D_attract_active                           , 0x9503  # u8: Маркер заставки; не подменять им состояние главного цикла.
.equ D_timer_ticks                              , 0x9504  # u8: Счётчик INT1Ch; переполнение modulo256.
.equ D_sound_toggle_state                       , 0x9505  # u8: Автомат кнопки звука0/1/2/3.
.equ D_sound_enabled                            , 0x9506  # u8: Разрешение звуков/музыки.
.equ D_last_spawn_row                           , 0x9507  # u16: Последняя обработанная строка перед камерой.
.equ D_dispatch_mode_snapshot                   , 0x9509  # u8: Режим на входе в проход update_entities.
.equ D_surface_resume_x                         , 0x950a  # u16: X для создания игрока после смены режима.
.equ D_surface_resume_y                         , 0x950c  # u16: Y камеры при выходе из туннеля.
.equ D_map_reload_pending                       , 0x950e  # u8: Запрос rebuild после перехода между частями.
.equ D_fuel_distance_accumulator                , 0x950f  # u16: Разные пороги расхода в surface/tunnel режимах.
.equ D_ammo_use_accumulator                     , 0x9511  # u16: Отложенный расход шкалы стрельбы.
.equ D_player_respawn_pending                   , 0x9513  # u8: Запрос смерти/respawn dispatcher.
.equ D_player_death_age                         , 0x9514  # u8: Таймер handler17.
.equ D_player_zero_frame_age                    , 0x9515  # u8: Срок пребывания в frame0 до смерти.
.equ D_shot_variation_index                     , 0x9516  # u8: Циклическая последовательность16 значений, не RNG.
.equ D_side_impact_mask                         , 0x9517  # u8: Занятые эффекты левого/правого ствола01h/80h.
.equ D_spawn_projectile_gate                    , 0x9518  # u8: Разрешает реальный снаряд вместе с эффектом.
.equ D_respawn_effect_pending                   , 0x9519  # u8: Создать эффект восстановления игрока.
.equ D_left_device_state                        , 0x951a  # u8: Автомат левой кнопки/устройства0..3.
.equ D_right_device_state                       , 0x951b  # u8: Автомат правой кнопки/устройства0..3.
.equ D_tunnel_flash_state                       , 0x951c  # u8: Вспышка:2→1→0.
.equ D_tunnel_reward_kind                       , 0x951d  # u8: 40 ammo,80 fuel,C0 shield,F0 переход уровня,прочее score.
.equ D_tunnel_angular_velocity                  , 0x951e  # i16: Инерция угла, clamp[-64,64].
.equ D_tunnel_passive_force                     , 0x9520  # i16: Пассивная добавка угловой скорости.
.equ D_tunnel_input_force                       , 0x9522  # i16: Добавка от LEFT/RIGHT.
.equ D_tunnel_exit_spawned                      , 0x9524  # u8: Выходной marker уже создан.
.equ D_tunnel_distance_remaining                , 0x9525  # u16: Дистанция до конца для маркера.
.equ D_tunnel_last_section                      , 0x9527  # u8: Последняя секция потока по distance>>6.
.equ D_camera_y                                 , 0x9528  # u16: Кольцевая координата поверхности modulo1024.
.equ D_scroll_speed                             , 0x952a  # u16: Скорость прокрутки/движения.
.equ D_player_x_velocity                        , 0x952c  # i16: Горизонтальная инерция [-24,24] на поверхности.
.equ D_collectible_0                            , 0x952e  # u8: Collected flag0; подряд6 байтов.
.equ D_collectible_1                            , 0x952f  # u8: Collected flag1.
.equ D_collectible_2                            , 0x9530  # u8: Collected flag2.
.equ D_collectible_3                            , 0x9531  # u8: Collected flag3.
.equ D_collectible_4                            , 0x9532  # u8: Collected flag4.
.equ D_collectible_5                            , 0x9533  # u8: Collected flag5.
.equ D_life_count                               , 0x9534  # u16: Жизни0..3 на штатных путях.
.equ D_side_charge_units                        , 0x9536  # u16: Заряды боковых устройств0..6.
.equ D_side_charge_delta                        , 0x9538  # i16: Накопленное изменение зарядов; применяется HUD.
.equ D_fuel_units                               , 0x953a  # u16: Шкала движения0..44; имя fuel — вывод по использованию.
.equ D_fuel_delta                               , 0x953c  # i16: Отложенное изменение топлива; отрицательное значит один decrement.
.equ D_ammo_units                               , 0x953e  # u16: Шкала стрельбы0..48.
.equ D_ammo_delta                               , 0x9540  # i16: Отложенное изменение ammo; отрицательное значит один decrement.
.equ D_shield_units                             , 0x9542  # u16: Шкала урона0..56; имя shield условное.
.equ D_shield_delta                             , 0x9544  # i16: Накопленное изменение прочности.
.equ D_pending_name_entry                       , 0x9546  # u8: Нужно ввести имя для рекорда; выбирает музыку заставки.
.equ D_tunnel_distance                          , 0x9547  # u16: Пройденная дистанция0..896 на штатном пути.
.equ D_tunnel_min_radius                        , 0x9549  # u8: Порог уменьшения колец.
.equ D_control_bits                             , 0x954c  # u16: Маска управления; смысл битов см. constants.inc.
.equ D_tunnel_ring_phase                        , 0x954e  # u16: distance&63 для построения bitmap.
.equ D_sound_active_id                          , 0x9550  # u8: Активный sound ID;0=нет.
.equ D_sound_pending_id                         , 0x9551  # u8: Первый ожидающий эффект.
.equ D_sound_priority_id                        , 0x9552  # u8: Второй ожидающий; хранит максимальный ID из запросов.
.equ D_sound_modulation_id                      , 0x9553  # u8: 0=без модуляции; иначе индекс таблицы+1.
.equ D_sound_modulation_phase                   , 0x9554  # u8: Индекс последовательности по &15.
.equ D_sound_modulation_pointer                 , 0x9555  # u16: DS offset16 слов модуляции.
.equ D_sound_ticks_left                         , 0x9557  # u8: Тики до завершения эффекта.
.equ D_sound_divisor                            , 0x9558  # u16: Основной делитель PIT2.
.equ D_sound_divisor_step                       , 0x955a  # i16: Приращение делителя на тик (16-битный wrap).
.equ D_sound_output_divisor                     , 0x955c  # u16: Сумма основного делителя и модуляции.
.equ D_music_active                             , 0x955e  # u8: Запрещает обычные sound_trigger пока активен.
.equ D_music_cursor                             , 0x955f  # u16: Позиция в потоке команд музыки.
.equ D_music_tick_divider                       , 0x9561  # u8: Шаг последовательности раз в8 IRQ-тиков.
.equ D_music_sequence_pointer                   , 0x9562  # u16: DS offset выбранной последовательности.

.set dat_attract_seen, image_start + DATA_BASE + D_attract_seen
.set dat_msg_insufficient_ram, image_start + DATA_BASE + D_msg_insufficient_ram
.set dat_msg_loading, image_start + DATA_BASE + D_msg_loading
.set dat_key_to_character, image_start + DATA_BASE + D_key_to_character
.set dat_attract_script_scores, image_start + DATA_BASE + D_attract_script_scores
.set dat_attract_script_intro, image_start + DATA_BASE + D_attract_script_intro
.set dat_template_attract, image_start + DATA_BASE + D_template_attract
.set dat_sprite_frames_0120, image_start + DATA_BASE + D_sprite_frames_0120
.set dat_highscore_layout, image_start + DATA_BASE + D_highscore_layout
.set dat_highscore_name_destinations, image_start + DATA_BASE + D_highscore_name_destinations
.set dat_filename_bird1, image_start + DATA_BASE + D_filename_bird1
.set dat_filename_bird2, image_start + DATA_BASE + D_filename_bird2
.set dat_filename_blox, image_start + DATA_BASE + D_filename_blox
.set dat_filename_newbird, image_start + DATA_BASE + D_filename_newbird
.set dat_filename_podz1, image_start + DATA_BASE + D_filename_podz1
.set dat_filename_intro, image_start + DATA_BASE + D_filename_intro
.set dat_level_tile_streams, image_start + DATA_BASE + D_level_tile_streams
.set dat_level_object_streams, image_start + DATA_BASE + D_level_object_streams
.set dat_template_map_tile, image_start + DATA_BASE + D_template_map_tile
.set dat_map_tile_template_z, image_start + DATA_BASE + D_map_tile_template_z
.set dat_map_tile_template_x, image_start + DATA_BASE + D_map_tile_template_x
.set dat_map_tile_template_y, image_start + DATA_BASE + D_map_tile_template_y
.set dat_map_tile_template_width, image_start + DATA_BASE + D_map_tile_template_width
.set dat_map_tile_template_height, image_start + DATA_BASE + D_map_tile_template_height
.set dat_map_tile_template_bank, image_start + DATA_BASE + D_map_tile_template_bank
.set dat_map_tile_template_sprite, image_start + DATA_BASE + D_map_tile_template_sprite
.set dat_map_entity_templates, image_start + DATA_BASE + D_map_entity_templates
.set dat_map_collision_damage, image_start + DATA_BASE + D_map_collision_damage
.set dat_map_collision_halfwidth, image_start + DATA_BASE + D_map_collision_halfwidth
.set dat_map_collision_rightmost_x, image_start + DATA_BASE + D_map_collision_rightmost_x
.set dat_entity_replacement_templates, image_start + DATA_BASE + D_entity_replacement_templates
.set dat_collectible_sprite_offsets, image_start + DATA_BASE + D_collectible_sprite_offsets
.set dat_template_map_kind_00, image_start + DATA_BASE + D_template_map_kind_00
.set dat_template_map_kind_01, image_start + DATA_BASE + D_template_map_kind_01
.set dat_template_map_kind_02, image_start + DATA_BASE + D_template_map_kind_02
.set dat_template_map_kind_03, image_start + DATA_BASE + D_template_map_kind_03
.set dat_template_map_kind_04, image_start + DATA_BASE + D_template_map_kind_04
.set dat_template_map_kind_05, image_start + DATA_BASE + D_template_map_kind_05
.set dat_template_map_kind_06, image_start + DATA_BASE + D_template_map_kind_06
.set dat_template_map_kind_10, image_start + DATA_BASE + D_template_map_kind_10
.set dat_template_map_kind_08, image_start + DATA_BASE + D_template_map_kind_08
.set dat_template_map_kind_09, image_start + DATA_BASE + D_template_map_kind_09
.set dat_template_map_kind_07, image_start + DATA_BASE + D_template_map_kind_07
.set dat_template_map_kind_11, image_start + DATA_BASE + D_template_map_kind_11
.set dat_template_map_kind_12, image_start + DATA_BASE + D_template_map_kind_12
.set dat_template_map_kind_14, image_start + DATA_BASE + D_template_map_kind_14
.set dat_template_map_kind_13, image_start + DATA_BASE + D_template_map_kind_13
.set dat_template_map_kind_15, image_start + DATA_BASE + D_template_map_kind_15
.set dat_template_collectible_00, image_start + DATA_BASE + D_template_collectible_00
.set dat_template_collectible_03, image_start + DATA_BASE + D_template_collectible_03
.set dat_template_collectible_04, image_start + DATA_BASE + D_template_collectible_04
.set dat_template_collectible_05, image_start + DATA_BASE + D_template_collectible_05
.set dat_template_collectible_06, image_start + DATA_BASE + D_template_collectible_06
.set dat_tile_heights, image_start + DATA_BASE + D_tile_heights
.set dat_tile_y_adjust, image_start + DATA_BASE + D_tile_y_adjust
.set dat_tile_height_delta, image_start + DATA_BASE + D_tile_height_delta
.set dat_tile_surface_variant_a, image_start + DATA_BASE + D_tile_surface_variant_a
.set dat_tile_surface_variant_b, image_start + DATA_BASE + D_tile_surface_variant_b
.set dat_tile_sprite_offsets, image_start + DATA_BASE + D_tile_sprite_offsets
.set dat_tile_profile_offsets, image_start + DATA_BASE + D_tile_profile_offsets
.set dat_map_column_bits, image_start + DATA_BASE + D_map_column_bits
.set dat_map_column_clear_masks, image_start + DATA_BASE + D_map_column_clear_masks
.set dat_template_tunnel_player, image_start + DATA_BASE + D_template_tunnel_player
.set dat_template_tunnel_side, image_start + DATA_BASE + D_template_tunnel_side
.set dat_template_surface_player, image_start + DATA_BASE + D_template_surface_player
.set dat_sprite_frames_0d49, image_start + DATA_BASE + D_sprite_frames_0d49
.set dat_template_respawn_effect, image_start + DATA_BASE + D_template_respawn_effect
.set dat_side_device_z_offsets, image_start + DATA_BASE + D_side_device_z_offsets
.set dat_template_side_device, image_start + DATA_BASE + D_template_side_device
.set dat_sprite_frames_0dda, image_start + DATA_BASE + D_sprite_frames_0dda
.set dat_sprite_frames_0e2b, image_start + DATA_BASE + D_sprite_frames_0e2b
.set dat_pingpong_frame_sequence, image_start + DATA_BASE + D_pingpong_frame_sequence
.set dat_sprite_frames_0e52, image_start + DATA_BASE + D_sprite_frames_0e52
.set dat_sprite_frames_0e73, image_start + DATA_BASE + D_sprite_frames_0e73
.set dat_sprite_frames_0e9c, image_start + DATA_BASE + D_sprite_frames_0e9c
.set dat_sprite_frames_0ec5, image_start + DATA_BASE + D_sprite_frames_0ec5
.set dat_sprite_frames_0eee, image_start + DATA_BASE + D_sprite_frames_0eee
.set dat_sprite_frames_0f3f, image_start + DATA_BASE + D_sprite_frames_0f3f
.set dat_sprite_frames_0f90, image_start + DATA_BASE + D_sprite_frames_0f90
.set dat_mobile_enemy_descriptors, image_start + DATA_BASE + D_mobile_enemy_descriptors
.set dat_shot_variation_sequence, image_start + DATA_BASE + D_shot_variation_sequence
.set dat_template_muzzle_effect, image_start + DATA_BASE + D_template_muzzle_effect
.set dat_shot_effect_template_z, image_start + DATA_BASE + D_shot_effect_template_z
.set dat_shot_effect_template_frame, image_start + DATA_BASE + D_shot_effect_template_frame
.set dat_shot_effect_template_x, image_start + DATA_BASE + D_shot_effect_template_x
.set dat_shot_effect_template_y, image_start + DATA_BASE + D_shot_effect_template_y
.set dat_sprite_frames_100e, image_start + DATA_BASE + D_sprite_frames_100e
.set dat_aimed_effect_descriptors, image_start + DATA_BASE + D_aimed_effect_descriptors
.set dat_sprite_frames_1039, image_start + DATA_BASE + D_sprite_frames_1039
.set dat_sprite_frames_105a, image_start + DATA_BASE + D_sprite_frames_105a
.set dat_sprite_frames_107b, image_start + DATA_BASE + D_sprite_frames_107b
.set dat_sprite_frames_109c, image_start + DATA_BASE + D_sprite_frames_109c
.set dat_sprite_frames_10bd, image_start + DATA_BASE + D_sprite_frames_10bd
.set dat_ground_shot_dx, image_start + DATA_BASE + D_ground_shot_dx
.set dat_ground_shot_dy, image_start + DATA_BASE + D_ground_shot_dy
.set dat_template_ground_shot, image_start + DATA_BASE + D_template_ground_shot
.set dat_ground_shot_descriptors, image_start + DATA_BASE + D_ground_shot_descriptors
.set dat_sprite_frames_1109, image_start + DATA_BASE + D_sprite_frames_1109
.set dat_sprite_frames_115a, image_start + DATA_BASE + D_sprite_frames_115a
.set dat_sprite_frames_11ab, image_start + DATA_BASE + D_sprite_frames_11ab
.set dat_sprite_frames_11fc, image_start + DATA_BASE + D_sprite_frames_11fc
.set dat_sprite_frames_124d, image_start + DATA_BASE + D_sprite_frames_124d
.set dat_template_explosion, image_start + DATA_BASE + D_template_explosion
.set dat_explosion_template_layer, image_start + DATA_BASE + D_explosion_template_layer
.set dat_explosion_template_z, image_start + DATA_BASE + D_explosion_template_z
.set dat_explosion_template_x, image_start + DATA_BASE + D_explosion_template_x
.set dat_explosion_template_y, image_start + DATA_BASE + D_explosion_template_y
.set dat_sprite_frames_12b5, image_start + DATA_BASE + D_sprite_frames_12b5
.set dat_template_arc_effect, image_start + DATA_BASE + D_template_arc_effect
.set dat_sprite_frames_12f5, image_start + DATA_BASE + D_sprite_frames_12f5
.set dat_side_impact_template, image_start + DATA_BASE + D_side_impact_template
.set dat_sprite_frames_133d, image_start + DATA_BASE + D_sprite_frames_133d
.set dat_template_player_shot, image_start + DATA_BASE + D_template_player_shot
.set dat_player_shot_template_z, image_start + DATA_BASE + D_player_shot_template_z
.set dat_player_shot_template_x, image_start + DATA_BASE + D_player_shot_template_x
.set dat_player_shot_template_y, image_start + DATA_BASE + D_player_shot_template_y
.set dat_player_left_muzzle_dx, image_start + DATA_BASE + D_player_left_muzzle_dx
.set dat_player_right_muzzle_dx, image_start + DATA_BASE + D_player_right_muzzle_dx
.set dat_player_left_muzzle_dz, image_start + DATA_BASE + D_player_left_muzzle_dz
.set dat_player_right_muzzle_dz, image_start + DATA_BASE + D_player_right_muzzle_dz
.set dat_player_shot_frame_map, image_start + DATA_BASE + D_player_shot_frame_map
.set dat_shot_variant_heights, image_start + DATA_BASE + D_shot_variant_heights
.set dat_player_shot_sprite_offsets, image_start + DATA_BASE + D_player_shot_sprite_offsets
.set dat_template_enemy_shot, image_start + DATA_BASE + D_template_enemy_shot
.set dat_enemy_shot_sprite_offsets, image_start + DATA_BASE + D_enemy_shot_sprite_offsets
.set dat_enemy_muzzle_a_left_dz, image_start + DATA_BASE + D_enemy_muzzle_a_left_dz
.set dat_enemy_muzzle_a_right_dz, image_start + DATA_BASE + D_enemy_muzzle_a_right_dz
.set dat_enemy_muzzle_b_dz, image_start + DATA_BASE + D_enemy_muzzle_b_dz
.set dat_enemy_muzzle_c_left_dz, image_start + DATA_BASE + D_enemy_muzzle_c_left_dz
.set dat_enemy_muzzle_c_center_dz, image_start + DATA_BASE + D_enemy_muzzle_c_center_dz
.set dat_enemy_muzzle_c_right_dz, image_start + DATA_BASE + D_enemy_muzzle_c_right_dz
.set dat_aimed_muzzle_dx, image_start + DATA_BASE + D_aimed_muzzle_dx
.set dat_aimed_muzzle_dy, image_start + DATA_BASE + D_aimed_muzzle_dy
.set dat_enemy_shot_widths, image_start + DATA_BASE + D_enemy_shot_widths
.set dat_enemy_shot_heights, image_start + DATA_BASE + D_enemy_shot_heights
.set dat_enemy_aimed_shot_sprites, image_start + DATA_BASE + D_enemy_aimed_shot_sprites
.set dat_template_impact, image_start + DATA_BASE + D_template_impact
.set dat_impact_frame_sprites, image_start + DATA_BASE + D_impact_frame_sprites
.set dat_template_arc_shot, image_start + DATA_BASE + D_template_arc_shot
.set dat_arc_spawn_dx, image_start + DATA_BASE + D_arc_spawn_dx
.set dat_arc_spawn_dy, image_start + DATA_BASE + D_arc_spawn_dy
.set dat_arc_height_deltas, image_start + DATA_BASE + D_arc_height_deltas
.set dat_tunnel_gate_frame_sprites, image_start + DATA_BASE + D_tunnel_gate_frame_sprites
.set dat_exit_gate_frame_sprites, image_start + DATA_BASE + D_exit_gate_frame_sprites
.set dat_tunnel_destinations_a, image_start + DATA_BASE + D_tunnel_destinations_a
.set dat_tunnel_destinations_b, image_start + DATA_BASE + D_tunnel_destinations_b
.set dat_level_start_y, image_start + DATA_BASE + D_level_start_y
.set dat_filename_hiscores, image_start + DATA_BASE + D_filename_hiscores
.set dat_control_prompt_banks, image_start + DATA_BASE + D_control_prompt_banks
.set dat_control_prompt_sources, image_start + DATA_BASE + D_control_prompt_sources
.set dat_control_prompt_destinations, image_start + DATA_BASE + D_control_prompt_destinations
.set dat_control_prompt_widths, image_start + DATA_BASE + D_control_prompt_widths
.set dat_control_prompt_heights, image_start + DATA_BASE + D_control_prompt_heights
.set dat_hud_digit_sprites, image_start + DATA_BASE + D_hud_digit_sprites
.set dat_hud_digit_destinations, image_start + DATA_BASE + D_hud_digit_destinations
.set dat_highscore_digit_sprites, image_start + DATA_BASE + D_highscore_digit_sprites
.set dat_sprite_frames_1594, image_start + DATA_BASE + D_sprite_frames_1594
.set dat_tunnel_circle_x, image_start + DATA_BASE + D_tunnel_circle_x
.set dat_tunnel_circle_y, image_start + DATA_BASE + D_tunnel_circle_y
.set dat_tunnel_player_frames, image_start + DATA_BASE + D_tunnel_player_frames
.set dat_tunnel_frame_to_sector, image_start + DATA_BASE + D_tunnel_frame_to_sector
.set dat_sprite_frames_1811, image_start + DATA_BASE + D_sprite_frames_1811
.set dat_tunnel_side_frames, image_start + DATA_BASE + D_tunnel_side_frames
.set dat_template_tunnel_exit, image_start + DATA_BASE + D_template_tunnel_exit
.set dat_sprite_frames_1929, image_start + DATA_BASE + D_sprite_frames_1929
.set dat_tunnel_exit_frames, image_start + DATA_BASE + D_tunnel_exit_frames
.set dat_tunnel_min_ring_radii, image_start + DATA_BASE + D_tunnel_min_ring_radii
.set dat_tunnel_pattern_pointers, image_start + DATA_BASE + D_tunnel_pattern_pointers
.set dat_tunnel_pickup_radii, image_start + DATA_BASE + D_tunnel_pickup_radii
.set dat_tunnel_pickup_layers, image_start + DATA_BASE + D_tunnel_pickup_layers
.set dat_template_tunnel_pickup, image_start + DATA_BASE + D_template_tunnel_pickup
.set dat_sprite_frames_1ac9, image_start + DATA_BASE + D_sprite_frames_1ac9
.set dat_entity_records, image_start + DATA_BASE + D_entity_records
.set dat_player_layer, image_start + DATA_BASE + D_player_layer
.set dat_player_z, image_start + DATA_BASE + D_player_z
.set dat_player_frame, image_start + DATA_BASE + D_player_frame
.set dat_player_x, image_start + DATA_BASE + D_player_x
.set dat_player_y, image_start + DATA_BASE + D_player_y
.set dat_player_handler, image_start + DATA_BASE + D_player_handler
.set dat_player_map_row, image_start + DATA_BASE + D_player_map_row
.set dat_player_angle_or_age, image_start + DATA_BASE + D_player_angle_or_age
.set dat_tunnel_ring_radius_curve, image_start + DATA_BASE + D_tunnel_ring_radius_curve
.set dat_tunnel_edge_x_curve, image_start + DATA_BASE + D_tunnel_edge_x_curve
.set dat_tunnel_edge_y_curve, image_start + DATA_BASE + D_tunnel_edge_y_curve
.set dat_level_01_part_1_tiles, image_start + DATA_BASE + D_level_01_part_1_tiles
.set dat_level_01_part_2_tiles, image_start + DATA_BASE + D_level_01_part_2_tiles
.set dat_level_01_part_3_tiles, image_start + DATA_BASE + D_level_01_part_3_tiles
.set dat_level_01_part_1_objects, image_start + DATA_BASE + D_level_01_part_1_objects
.set dat_level_01_part_2_objects, image_start + DATA_BASE + D_level_01_part_2_objects
.set dat_level_01_part_3_objects, image_start + DATA_BASE + D_level_01_part_3_objects
.set dat_level_02_part_1_tiles, image_start + DATA_BASE + D_level_02_part_1_tiles
.set dat_level_02_part_2_tiles, image_start + DATA_BASE + D_level_02_part_2_tiles
.set dat_level_02_part_3_tiles, image_start + DATA_BASE + D_level_02_part_3_tiles
.set dat_level_02_part_1_objects, image_start + DATA_BASE + D_level_02_part_1_objects
.set dat_level_02_part_2_objects, image_start + DATA_BASE + D_level_02_part_2_objects
.set dat_level_02_part_3_objects, image_start + DATA_BASE + D_level_02_part_3_objects
.set dat_level_03_part_1_tiles, image_start + DATA_BASE + D_level_03_part_1_tiles
.set dat_level_03_part_2_tiles, image_start + DATA_BASE + D_level_03_part_2_tiles
.set dat_level_03_part_3_tiles, image_start + DATA_BASE + D_level_03_part_3_tiles
.set dat_level_03_part_1_objects, image_start + DATA_BASE + D_level_03_part_1_objects
.set dat_level_03_part_2_objects, image_start + DATA_BASE + D_level_03_part_2_objects
.set dat_level_03_part_3_objects, image_start + DATA_BASE + D_level_03_part_3_objects
.set dat_level_04_part_1_tiles, image_start + DATA_BASE + D_level_04_part_1_tiles
.set dat_level_04_part_2_tiles, image_start + DATA_BASE + D_level_04_part_2_tiles
.set dat_level_04_part_3_tiles, image_start + DATA_BASE + D_level_04_part_3_tiles
.set dat_level_04_part_1_objects, image_start + DATA_BASE + D_level_04_part_1_objects
.set dat_level_04_part_2_objects, image_start + DATA_BASE + D_level_04_part_2_objects
.set dat_level_04_part_3_objects, image_start + DATA_BASE + D_level_04_part_3_objects
.set dat_level_05_part_1_tiles, image_start + DATA_BASE + D_level_05_part_1_tiles
.set dat_level_05_part_2_tiles, image_start + DATA_BASE + D_level_05_part_2_tiles
.set dat_level_05_part_3_tiles, image_start + DATA_BASE + D_level_05_part_3_tiles
.set dat_level_05_part_1_objects, image_start + DATA_BASE + D_level_05_part_1_objects
.set dat_level_05_part_2_objects, image_start + DATA_BASE + D_level_05_part_2_objects
.set dat_level_05_part_3_objects, image_start + DATA_BASE + D_level_05_part_3_objects
.set dat_level_06_part_1_tiles, image_start + DATA_BASE + D_level_06_part_1_tiles
.set dat_level_06_part_2_tiles, image_start + DATA_BASE + D_level_06_part_2_tiles
.set dat_level_06_part_3_tiles, image_start + DATA_BASE + D_level_06_part_3_tiles
.set dat_level_06_part_1_objects, image_start + DATA_BASE + D_level_06_part_1_objects
.set dat_level_06_part_2_objects, image_start + DATA_BASE + D_level_06_part_2_objects
.set dat_level_06_part_3_objects, image_start + DATA_BASE + D_level_06_part_3_objects
.set dat_level_07_part_1_tiles, image_start + DATA_BASE + D_level_07_part_1_tiles
.set dat_level_07_part_2_tiles, image_start + DATA_BASE + D_level_07_part_2_tiles
.set dat_level_07_part_3_tiles, image_start + DATA_BASE + D_level_07_part_3_tiles
.set dat_level_07_part_1_objects, image_start + DATA_BASE + D_level_07_part_1_objects
.set dat_level_07_part_2_objects, image_start + DATA_BASE + D_level_07_part_2_objects
.set dat_level_07_part_3_objects, image_start + DATA_BASE + D_level_07_part_3_objects
.set dat_level_08_part_1_tiles, image_start + DATA_BASE + D_level_08_part_1_tiles
.set dat_level_08_part_2_tiles, image_start + DATA_BASE + D_level_08_part_2_tiles
.set dat_level_08_part_3_tiles, image_start + DATA_BASE + D_level_08_part_3_tiles
.set dat_level_08_part_1_objects, image_start + DATA_BASE + D_level_08_part_1_objects
.set dat_level_08_part_2_objects, image_start + DATA_BASE + D_level_08_part_2_objects
.set dat_level_08_part_3_objects, image_start + DATA_BASE + D_level_08_part_3_objects
.set dat_sound_descriptor_pointers, image_start + DATA_BASE + D_sound_descriptor_pointers
.set dat_sound_descriptor_01, image_start + DATA_BASE + D_sound_descriptor_01
.set dat_sound_descriptor_02, image_start + DATA_BASE + D_sound_descriptor_02
.set dat_sound_descriptor_03, image_start + DATA_BASE + D_sound_descriptor_03
.set dat_sound_descriptor_04, image_start + DATA_BASE + D_sound_descriptor_04
.set dat_sound_descriptor_05, image_start + DATA_BASE + D_sound_descriptor_05
.set dat_sound_descriptor_06, image_start + DATA_BASE + D_sound_descriptor_06
.set dat_sound_descriptor_07, image_start + DATA_BASE + D_sound_descriptor_07
.set dat_sound_descriptor_08, image_start + DATA_BASE + D_sound_descriptor_08
.set dat_charge_sound_divisor, image_start + DATA_BASE + D_charge_sound_divisor
.set dat_sound_descriptor_09, image_start + DATA_BASE + D_sound_descriptor_09
.set dat_angle_sound_divisor, image_start + DATA_BASE + D_angle_sound_divisor
.set dat_sound_descriptor_10, image_start + DATA_BASE + D_sound_descriptor_10
.set dat_sound_descriptor_11, image_start + DATA_BASE + D_sound_descriptor_11
.set dat_sound_descriptor_12, image_start + DATA_BASE + D_sound_descriptor_12
.set dat_sound_descriptor_13, image_start + DATA_BASE + D_sound_descriptor_13
.set dat_sound_descriptor_14, image_start + DATA_BASE + D_sound_descriptor_14
.set dat_music_descriptor_modulation, image_start + DATA_BASE + D_music_descriptor_modulation
.set dat_music_descriptor_duration, image_start + DATA_BASE + D_music_descriptor_duration
.set dat_music_descriptor_divisor, image_start + DATA_BASE + D_music_descriptor_divisor
.set dat_sound_modulation_pointers, image_start + DATA_BASE + D_sound_modulation_pointers
.set dat_sound_modulation_1, image_start + DATA_BASE + D_sound_modulation_1
.set dat_sound_modulation_2, image_start + DATA_BASE + D_sound_modulation_2
.set dat_sound_modulation_3, image_start + DATA_BASE + D_sound_modulation_3
.set dat_sound_modulation_4, image_start + DATA_BASE + D_sound_modulation_4
.set dat_note_divisors, image_start + DATA_BASE + D_note_divisors
.set dat_music_fill_notes, image_start + DATA_BASE + D_music_fill_notes
.set dat_attract_script_pointer, image_start + DATA_BASE + D_attract_script_pointer
.set dat_attract_step_counter, image_start + DATA_BASE + D_attract_step_counter
.set dat_old_keyboard_segment, image_start + DATA_BASE + D_old_keyboard_segment
.set dat_old_keyboard_offset, image_start + DATA_BASE + D_old_keyboard_offset
.set dat_old_timer_segment, image_start + DATA_BASE + D_old_timer_segment
.set dat_old_timer_offset, image_start + DATA_BASE + D_old_timer_offset
.set dat_keyboard_state, image_start + DATA_BASE + D_keyboard_state
.set dat_mouse_present, image_start + DATA_BASE + D_mouse_present
.set dat_last_key_pointer, image_start + DATA_BASE + D_last_key_pointer
.set dat_key_left, image_start + DATA_BASE + D_key_left
.set dat_key_right, image_start + DATA_BASE + D_key_right
.set dat_key_up, image_start + DATA_BASE + D_key_up
.set dat_key_down, image_start + DATA_BASE + D_key_down
.set dat_key_fire, image_start + DATA_BASE + D_key_fire
.set dat_key_action_n, image_start + DATA_BASE + D_key_action_n
.set dat_key_action_m, image_start + DATA_BASE + D_key_action_m
.set dat_key_quit, image_start + DATA_BASE + D_key_quit
.set dat_file_handle, image_start + DATA_BASE + D_file_handle
.set dat_file_destination_segment, image_start + DATA_BASE + D_file_destination_segment
.set dat_map_running_height, image_start + DATA_BASE + D_map_running_height
.set dat_map_row_y, image_start + DATA_BASE + D_map_row_y
.set dat_map_tile_cursor, image_start + DATA_BASE + D_map_tile_cursor
.set dat_decoded_map_tiles, image_start + DATA_BASE + D_decoded_map_tiles
.set dat_map_spawn_rows, image_start + DATA_BASE + D_map_spawn_rows
.set dat_map_available_masks, image_start + DATA_BASE + D_map_available_masks
.set dat_collision_damage_tmp, image_start + DATA_BASE + D_collision_damage_tmp
.set dat_collision_column_tmp, image_start + DATA_BASE + D_collision_column_tmp
.set dat_sprite_dest_segment, image_start + DATA_BASE + D_sprite_dest_segment
.set dat_sprite_clip_enabled, image_start + DATA_BASE + D_sprite_clip_enabled
.set dat_player_forced_stop, image_start + DATA_BASE + D_player_forced_stop
.set dat_player_history_x0, image_start + DATA_BASE + D_player_history_x0
.set dat_player_history_x1, image_start + DATA_BASE + D_player_history_x1
.set dat_player_history_x2, image_start + DATA_BASE + D_player_history_x2
.set dat_player_history_x3, image_start + DATA_BASE + D_player_history_x3
.set dat_player_history_y0, image_start + DATA_BASE + D_player_history_y0
.set dat_player_history_y1, image_start + DATA_BASE + D_player_history_y1
.set dat_player_history_y2, image_start + DATA_BASE + D_player_history_y2
.set dat_player_history_y3, image_start + DATA_BASE + D_player_history_y3
.set dat_player_history_frame0, image_start + DATA_BASE + D_player_history_frame0
.set dat_player_history_frame1, image_start + DATA_BASE + D_player_history_frame1
.set dat_player_history_frame2, image_start + DATA_BASE + D_player_history_frame2
.set dat_player_history_frame3, image_start + DATA_BASE + D_player_history_frame3
.set dat_player_history_z0, image_start + DATA_BASE + D_player_history_z0
.set dat_player_history_z1, image_start + DATA_BASE + D_player_history_z1
.set dat_player_history_z2, image_start + DATA_BASE + D_player_history_z2
.set dat_player_history_z3, image_start + DATA_BASE + D_player_history_z3
.set dat_hud_frame_counter, image_start + DATA_BASE + D_hud_frame_counter
.set dat_score_leading_zero, image_start + DATA_BASE + D_score_leading_zero
.set dat_text_draw_offset, image_start + DATA_BASE + D_text_draw_offset
.set dat_pending_highscore_rank, image_start + DATA_BASE + D_pending_highscore_rank
.set dat_pending_name_pointer, image_start + DATA_BASE + D_pending_name_pointer
.set dat_name_cursor, image_start + DATA_BASE + D_name_cursor
.set dat_highscore_data, image_start + DATA_BASE + D_highscore_data
.set dat_score_digit_100000, image_start + DATA_BASE + D_score_digit_100000
.set dat_score_digit_10000, image_start + DATA_BASE + D_score_digit_10000
.set dat_score_digit_1000, image_start + DATA_BASE + D_score_digit_1000
.set dat_score_digit_100, image_start + DATA_BASE + D_score_digit_100
.set dat_score_digit_10, image_start + DATA_BASE + D_score_digit_10
.set dat_score_digit_1, image_start + DATA_BASE + D_score_digit_1
.set dat_highscore_sort_scratch0, image_start + DATA_BASE + D_highscore_sort_scratch0
.set dat_highscore_sort_scratch1, image_start + DATA_BASE + D_highscore_sort_scratch1
.set dat_highscore_sort_scratch2, image_start + DATA_BASE + D_highscore_sort_scratch2
.set dat_highscore_sort_scratch3, image_start + DATA_BASE + D_highscore_sort_scratch3
.set dat_highscore_sort_scratch4, image_start + DATA_BASE + D_highscore_sort_scratch4
.set dat_highscore_sort_scratch5, image_start + DATA_BASE + D_highscore_sort_scratch5
.set dat_highscore_sort_scratch6, image_start + DATA_BASE + D_highscore_sort_scratch6
.set dat_highscore_order0, image_start + DATA_BASE + D_highscore_order0
.set dat_highscore_order1, image_start + DATA_BASE + D_highscore_order1
.set dat_highscore_order2, image_start + DATA_BASE + D_highscore_order2
.set dat_highscore_order3, image_start + DATA_BASE + D_highscore_order3
.set dat_highscore_order4, image_start + DATA_BASE + D_highscore_order4
.set dat_highscore_order5, image_start + DATA_BASE + D_highscore_order5
.set dat_highscore_order6, image_start + DATA_BASE + D_highscore_order6
.set dat_tunnel_pattern_pointer, image_start + DATA_BASE + D_tunnel_pattern_pointer
.set dat_tunnel_pattern_index, image_start + DATA_BASE + D_tunnel_pattern_index
.set dat_tunnel_angle_history0, image_start + DATA_BASE + D_tunnel_angle_history0
.set dat_tunnel_angle_history1, image_start + DATA_BASE + D_tunnel_angle_history1
.set dat_tunnel_angle_history2, image_start + DATA_BASE + D_tunnel_angle_history2
.set dat_segment_bird1, image_start + DATA_BASE + D_segment_bird1
.set dat_segment_bird2, image_start + DATA_BASE + D_segment_bird2
.set dat_segment_blox, image_start + DATA_BASE + D_segment_blox
.set dat_segment_newbird, image_start + DATA_BASE + D_segment_newbird
.set dat_segment_podz1, image_start + DATA_BASE + D_segment_podz1
.set dat_segment_intro, image_start + DATA_BASE + D_segment_intro
.set dat_segment_scratch0, image_start + DATA_BASE + D_segment_scratch0
.set dat_segment_scratch1, image_start + DATA_BASE + D_segment_scratch1
.set dat_video_draw_segment, image_start + DATA_BASE + D_video_draw_segment
.set dat_video_display_segment, image_start + DATA_BASE + D_video_display_segment
.set dat_base_scroll_speed, image_start + DATA_BASE + D_base_scroll_speed
.set dat_stage_counter, image_start + DATA_BASE + D_stage_counter
.set dat_tunnel_mode, image_start + DATA_BASE + D_tunnel_mode
.set dat_game_loop_end_flag, image_start + DATA_BASE + D_game_loop_end_flag
.set dat_level_index, image_start + DATA_BASE + D_level_index
.set dat_level_part, image_start + DATA_BASE + D_level_part
.set dat_attract_active, image_start + DATA_BASE + D_attract_active
.set dat_timer_ticks, image_start + DATA_BASE + D_timer_ticks
.set dat_sound_toggle_state, image_start + DATA_BASE + D_sound_toggle_state
.set dat_sound_enabled, image_start + DATA_BASE + D_sound_enabled
.set dat_last_spawn_row, image_start + DATA_BASE + D_last_spawn_row
.set dat_dispatch_mode_snapshot, image_start + DATA_BASE + D_dispatch_mode_snapshot
.set dat_surface_resume_x, image_start + DATA_BASE + D_surface_resume_x
.set dat_surface_resume_y, image_start + DATA_BASE + D_surface_resume_y
.set dat_map_reload_pending, image_start + DATA_BASE + D_map_reload_pending
.set dat_fuel_distance_accumulator, image_start + DATA_BASE + D_fuel_distance_accumulator
.set dat_ammo_use_accumulator, image_start + DATA_BASE + D_ammo_use_accumulator
.set dat_player_respawn_pending, image_start + DATA_BASE + D_player_respawn_pending
.set dat_player_death_age, image_start + DATA_BASE + D_player_death_age
.set dat_player_zero_frame_age, image_start + DATA_BASE + D_player_zero_frame_age
.set dat_shot_variation_index, image_start + DATA_BASE + D_shot_variation_index
.set dat_side_impact_mask, image_start + DATA_BASE + D_side_impact_mask
.set dat_spawn_projectile_gate, image_start + DATA_BASE + D_spawn_projectile_gate
.set dat_respawn_effect_pending, image_start + DATA_BASE + D_respawn_effect_pending
.set dat_left_device_state, image_start + DATA_BASE + D_left_device_state
.set dat_right_device_state, image_start + DATA_BASE + D_right_device_state
.set dat_tunnel_flash_state, image_start + DATA_BASE + D_tunnel_flash_state
.set dat_tunnel_reward_kind, image_start + DATA_BASE + D_tunnel_reward_kind
.set dat_tunnel_angular_velocity, image_start + DATA_BASE + D_tunnel_angular_velocity
.set dat_tunnel_passive_force, image_start + DATA_BASE + D_tunnel_passive_force
.set dat_tunnel_input_force, image_start + DATA_BASE + D_tunnel_input_force
.set dat_tunnel_exit_spawned, image_start + DATA_BASE + D_tunnel_exit_spawned
.set dat_tunnel_distance_remaining, image_start + DATA_BASE + D_tunnel_distance_remaining
.set dat_tunnel_last_section, image_start + DATA_BASE + D_tunnel_last_section
.set dat_camera_y, image_start + DATA_BASE + D_camera_y
.set dat_scroll_speed, image_start + DATA_BASE + D_scroll_speed
.set dat_player_x_velocity, image_start + DATA_BASE + D_player_x_velocity
.set dat_collectible_0, image_start + DATA_BASE + D_collectible_0
.set dat_collectible_1, image_start + DATA_BASE + D_collectible_1
.set dat_collectible_2, image_start + DATA_BASE + D_collectible_2
.set dat_collectible_3, image_start + DATA_BASE + D_collectible_3
.set dat_collectible_4, image_start + DATA_BASE + D_collectible_4
.set dat_collectible_5, image_start + DATA_BASE + D_collectible_5
.set dat_life_count, image_start + DATA_BASE + D_life_count
.set dat_side_charge_units, image_start + DATA_BASE + D_side_charge_units
.set dat_side_charge_delta, image_start + DATA_BASE + D_side_charge_delta
.set dat_fuel_units, image_start + DATA_BASE + D_fuel_units
.set dat_fuel_delta, image_start + DATA_BASE + D_fuel_delta
.set dat_ammo_units, image_start + DATA_BASE + D_ammo_units
.set dat_ammo_delta, image_start + DATA_BASE + D_ammo_delta
.set dat_shield_units, image_start + DATA_BASE + D_shield_units
.set dat_shield_delta, image_start + DATA_BASE + D_shield_delta
.set dat_pending_name_entry, image_start + DATA_BASE + D_pending_name_entry
.set dat_tunnel_distance, image_start + DATA_BASE + D_tunnel_distance
.set dat_tunnel_min_radius, image_start + DATA_BASE + D_tunnel_min_radius
.set dat_control_bits, image_start + DATA_BASE + D_control_bits
.set dat_tunnel_ring_phase, image_start + DATA_BASE + D_tunnel_ring_phase
.set dat_sound_active_id, image_start + DATA_BASE + D_sound_active_id
.set dat_sound_pending_id, image_start + DATA_BASE + D_sound_pending_id
.set dat_sound_priority_id, image_start + DATA_BASE + D_sound_priority_id
.set dat_sound_modulation_id, image_start + DATA_BASE + D_sound_modulation_id
.set dat_sound_modulation_phase, image_start + DATA_BASE + D_sound_modulation_phase
.set dat_sound_modulation_pointer, image_start + DATA_BASE + D_sound_modulation_pointer
.set dat_sound_ticks_left, image_start + DATA_BASE + D_sound_ticks_left
.set dat_sound_divisor, image_start + DATA_BASE + D_sound_divisor
.set dat_sound_divisor_step, image_start + DATA_BASE + D_sound_divisor_step
.set dat_sound_output_divisor, image_start + DATA_BASE + D_sound_output_divisor
.set dat_music_active, image_start + DATA_BASE + D_music_active
.set dat_music_cursor, image_start + DATA_BASE + D_music_cursor
.set dat_music_tick_divider, image_start + DATA_BASE + D_music_tick_divider
.set dat_music_sequence_pointer, image_start + DATA_BASE + D_music_sequence_pointer
