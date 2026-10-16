/* DS:0006 — данные; не исполняемый код. */

/* DS:0006..0006: D_attract_seen — Первый attract уже завершён. */
    .byte 0x00    # DS:0006

/* DS:0007..0029: D_msg_insufficient_ram — Сообщение о недостатке памяти, завершается долларом. */
    .byte 0x49, 0x6e, 0x73, 0x75, 0x66, 0x66, 0x69, 0x63, 0x69, 0x65, 0x6e, 0x74, 0x20, 0x52, 0x41, 0x4d    # DS:0007
    .byte 0x20, 0x74, 0x6f, 0x20, 0x72, 0x75, 0x6e, 0x20, 0x53, 0x74, 0x61, 0x72, 0x20, 0x47, 0x6f, 0x6f    # DS:0017
    .byte 0x73, 0x65, 0x24    # DS:0027

/* DS:002A..0059: D_msg_loading — Сообщение о загрузке, завершается долларом. */
    .byte 0x53, 0x74, 0x61, 0x72, 0x20, 0x47, 0x6f, 0x6f, 0x73, 0x65, 0x20, 0x28, 0x43, 0x29, 0x20, 0x31    # DS:002A
    .byte 0x39, 0x38, 0x38, 0x20, 0x4c, 0x6f, 0x67, 0x6f, 0x74, 0x72, 0x6f, 0x6e, 0x20, 0x4c, 0x74, 0x64    # DS:003A
    .byte 0x20, 0x2e, 0x2e, 0x2e, 0x4c, 0x6f, 0x61, 0x64, 0x69, 0x6e, 0x67, 0x2e, 0x2e, 0x2e, 0x20, 0x24    # DS:004A

/* DS:005A..00D9: D_key_to_character — Таблица scan-code → символ для ввода имени. */
    .byte 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00    # DS:005A
    .byte 0x51, 0x57, 0x45, 0x52, 0x54, 0x59, 0x55, 0x49, 0x4f, 0x50, 0x00, 0x00, 0xff, 0x00, 0x41, 0x53    # DS:006A
    .byte 0x44, 0x46, 0x47, 0x48, 0x4a, 0x4b, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0x58, 0x43, 0x56    # DS:007A
    .byte 0x42, 0x4e, 0x4d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:008A
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:009A
    .byte 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:00AA
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:00BA
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:00CA

/* DS:00DA..00E8: D_attract_script_scores — Сценарий после игровой сессии; первый читаемый элемент имеет индекс1. */
    .byte 0x00, 0x01, 0x0b, 0x00, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x00, 0xfe, 0x00, 0x00, 0x00    # DS:00DA

/* DS:00E9..0108: D_attract_script_intro — Сценарий первого показа; первый читаемый элемент имеет индекс1. */
    .byte 0x00, 0x01, 0x02, 0x00, 0x09, 0x00, 0x0a, 0x00, 0x00, 0x06, 0x00, 0x00, 0x07, 0x00, 0x00, 0x03    # DS:00E9
    .byte 0x04, 0x05, 0x00, 0x08, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff    # DS:00F9

/* DS:0109..011F: Entity template: attract */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 4, 0, 0    # param, layer, signed z, frame
    .word 128, 200    # x, world_y
    .byte 32, 30, 0, 0    # width, height, signed anchor_x/y
    .word 2, 0x4bb8    # bank byte-offset, sprite offset
    .byte 2, 33    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0120..01E0: FrameSet 0120: 24 frames, SoA layout */
    .byte 24    # frame_count
    .byte 32, 32, 140, 104, 36, 48, 228, 156, 240, 100, 184, 148, 144, 72, 144, 72, 144, 72, 144, 72, 144, 72, 144, 72    # width[frame]
    .byte 30, 30, 23, 15, 19, 14, 15, 15, 12, 15, 12, 23, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11    # height[frame]
    .byte 0, 90, 40, 90, -18, -54, 114, 78, 120, 50, 92, 40, 120, -40, 120, -40, 120, -40, 120, -40, 120, -40, 120, -40    # anchor_x (signed)[frame]
    .byte 0, 0, 12, -13, 7, 21, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    # anchor_y (signed)[frame]
    .word 2, 0x4bb8    # frame  0: bank byte-offset, source offset
    .word 2, 0x4bb8    # frame  1: bank byte-offset, source offset
    .word 2, 0x3660    # frame  2: bank byte-offset, source offset
    .word 2, 0x23a0    # frame  3: bank byte-offset, source offset
    .word 2, 0x2156    # frame  4: bank byte-offset, source offset
    .word 2, 0x22a8    # frame  5: bank byte-offset, source offset
    .word 2, 0x17c0    # frame  6: bank byte-offset, source offset
    .word 2, 0x2d00    # frame  7: bank byte-offset, source offset
    .word 2, 0x1040    # frame  8: bank byte-offset, source offset
    .word 2, 0x06e0    # frame  9: bank byte-offset, source offset
    .word 2, 0x0712    # frame 10: bank byte-offset, source offset
    .word 8, 0x0052    # frame 11: bank byte-offset, source offset
    .word 8, 0x0eb6    # frame 12: bank byte-offset, source offset
    .word 8, 0x1596    # frame 13: bank byte-offset, source offset
    .word 8, 0x1c76    # frame 14: bank byte-offset, source offset
    .word 8, 0x2356    # frame 15: bank byte-offset, source offset
    .word 8, 0x2a36    # frame 16: bank byte-offset, source offset
    .word 8, 0x3116    # frame 17: bank byte-offset, source offset
    .word 8, 0x37f6    # frame 18: bank byte-offset, source offset
    .word 8, 0x3ed6    # frame 19: bank byte-offset, source offset
    .word 8, 0x45b6    # frame 20: bank byte-offset, source offset
    .word 8, 0x4c96    # frame 21: bank byte-offset, source offset
    .word 8, 0x5376    # frame 22: bank byte-offset, source offset
    .word 8, 0x5a56    # frame 23: bank byte-offset, source offset

/* DS:01E1..01E1: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00    # DS:01E1

/* DS:01E2..01E9: D_filename_bird1 — Имя внешнего файла, нулевой терминатор. */
    .byte 0x62, 0x69, 0x72, 0x64, 0x31, 0x2e, 0x78, 0x00    # DS:01E2

/* DS:01EA..01F1: D_filename_bird2 — Имя внешнего файла, нулевой терминатор. */
    .byte 0x62, 0x69, 0x72, 0x64, 0x32, 0x2e, 0x78, 0x00    # DS:01EA

/* DS:01F2..01F8: D_filename_blox — Имя внешнего файла, нулевой терминатор. */
    .byte 0x62, 0x6c, 0x6f, 0x78, 0x2e, 0x78, 0x00    # DS:01F2

/* DS:01F9..0202: D_filename_newbird — Имя внешнего файла, нулевой терминатор. */
    .byte 0x6e, 0x65, 0x77, 0x62, 0x69, 0x72, 0x64, 0x2e, 0x78, 0x00    # DS:01F9

/* DS:0203..020A: D_filename_podz1 — Имя внешнего файла, нулевой терминатор. */
    .byte 0x70, 0x6f, 0x64, 0x7a, 0x31, 0x2e, 0x78, 0x00    # DS:0203

/* DS:020B..0213: D_filename_intro — Имя внешнего файла, нулевой терминатор. */
    .byte 0x69, 0x6e, 0x74, 0x72, 0x6f, 0x2e, 0x78, 0x00, 0x00    # DS:020B

/* DS:0214..0243: Таблица D_level_tile_streams */
    .word D_level_01_part_1_tiles, D_level_01_part_2_tiles, D_level_01_part_3_tiles, D_level_02_part_1_tiles, D_level_02_part_2_tiles, D_level_02_part_3_tiles, D_level_03_part_1_tiles, D_level_03_part_2_tiles
    .word D_level_03_part_3_tiles, D_level_04_part_1_tiles, D_level_04_part_2_tiles, D_level_04_part_3_tiles, D_level_05_part_1_tiles, D_level_05_part_2_tiles, D_level_05_part_3_tiles, D_level_06_part_1_tiles
    .word D_level_06_part_2_tiles, D_level_06_part_3_tiles, D_level_07_part_1_tiles, D_level_07_part_2_tiles, D_level_07_part_3_tiles, D_level_08_part_1_tiles, D_level_08_part_2_tiles, D_level_08_part_3_tiles

/* DS:0244..0273: Таблица D_level_object_streams */
    .word D_level_01_part_1_objects, D_level_01_part_2_objects, D_level_01_part_3_objects, D_level_02_part_1_objects, D_level_02_part_2_objects, D_level_02_part_3_objects, D_level_03_part_1_objects, D_level_03_part_2_objects
    .word D_level_03_part_3_objects, D_level_04_part_1_objects, D_level_04_part_2_objects, D_level_04_part_3_objects, D_level_05_part_1_objects, D_level_05_part_2_objects, D_level_05_part_3_objects, D_level_06_part_1_objects
    .word D_level_06_part_2_objects, D_level_06_part_3_objects, D_level_07_part_1_objects, D_level_07_part_2_objects, D_level_07_part_3_objects, D_level_08_part_1_objects, D_level_08_part_2_objects, D_level_08_part_3_objects

/* DS:0274..028A: Entity template: map_tile */
    .byte EF_RENDER    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 24, 0, 0, 0    # width, height, signed anchor_x/y
    .word 0, 0x0000    # bank byte-offset, sprite offset
    .byte 0, 0    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:028B..02B4: Таблица D_map_entity_templates */
    .word D_template_map_kind_00, D_template_map_kind_01, D_template_map_kind_02, D_template_map_kind_03, D_template_map_kind_04, D_template_map_kind_05, D_template_map_kind_06, D_template_map_kind_07
    .word D_template_map_kind_08, D_template_map_kind_09, D_template_map_kind_10, D_template_map_kind_11, D_template_map_kind_12, D_template_map_kind_13, D_template_map_kind_14, D_template_map_kind_15
    .word D_template_map_kind_15, D_template_map_kind_15, D_template_map_kind_15, D_template_map_kind_15, D_template_map_kind_15

/* DS:02B5..02C9: Код/урон препятствия; 0 означает отсутствие столкновения. */
    .byte 0x0a, 0x0a, 0x0a, 0x0f, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:02B5
    .byte 0x00, 0x00, 0x00, 0x00, 0x00    # DS:02C5

/* DS:02CA..02F3: Таблица D_map_collision_halfwidth */
    .word 0x001c, 0x0016, 0x0016, 0x001c, 0x0018, 0x001c, 0x0000, 0x0000
    .word 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    .word 0x0000, 0x0000, 0x0000, 0x0000, 0x0000

/* DS:02F4..031D: Таблица D_map_collision_rightmost_x */
    .word 0x00ec, 0x00ec, 0x00e0, 0x00ec, 0x00ec, 0x00ec, 0x0000, 0x0000
    .word 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
    .word 0x0000, 0x0000, 0x0000, 0x0000, 0x0000

/* DS:031E..032D: Таблица D_entity_replacement_templates */
    .word D_template_collectible_00, D_template_collectible_00, D_template_collectible_00, D_template_collectible_03, D_template_collectible_04, D_template_collectible_05, D_template_collectible_06, D_template_collectible_05

/* DS:032E..0339: Таблица D_collectible_sprite_offsets */
    .word 0x43ae, 0x161a, 0x1d9a, 0x251a, 0x2c9a, 0x0cc4

/* DS:033A..0350: Entity template: map_kind_00 */
    .byte EF_RENDER | EF_UPDATE | EF_MAP_OBJECT    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 28, 24, 15, 18    # width, height, signed anchor_x/y
    .word 0, 0x0030    # bank byte-offset, sprite offset
    .byte 2, 2    # transparent, handler ID
    .byte 0x00, 0x00, 0x28, 0x00    # state[4], interpretation is handler-specific

/* DS:0351..0367: Entity template: map_kind_01 */
    .byte EF_RENDER | EF_UPDATE | EF_MAP_OBJECT    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 20, 25, 10, 11    # width, height, signed anchor_x/y
    .word 0, 0x4d10    # bank byte-offset, sprite offset
    .byte 2, 3    # transparent, handler ID
    .byte 0x00, 0x00, 0x28, 0x00    # state[4], interpretation is handler-specific

/* DS:0368..037E: Entity template: map_kind_02 */
    .byte EF_RENDER | EF_UPDATE | EF_MAP_OBJECT | EF_LARGE_ANCHOR    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 16, 19, 19, 13    # width, height, signed anchor_x/y
    .word 0, 0x5b7a    # bank byte-offset, sprite offset
    .byte 2, 4    # transparent, handler ID
    .byte 0x00, 0x00, 0x28, 0x00    # state[4], interpretation is handler-specific

/* DS:037F..0395: Entity template: map_kind_03 */
    .byte EF_RENDER | EF_UPDATE | EF_MAP_OBJECT    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 28, 30, 16, 15    # width, height, signed anchor_x/y
    .word 0, 0x0064    # bank byte-offset, sprite offset
    .byte 2, 5    # transparent, handler ID
    .byte 0x00, 0x00, 0x3c, 0x00    # state[4], interpretation is handler-specific

/* DS:0396..03AC: Entity template: map_kind_04 */
    .byte EF_RENDER | EF_UPDATE | EF_MAP_OBJECT    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 24, 21, 13, 19    # width, height, signed anchor_x/y
    .word 0, 0x0080    # bank byte-offset, sprite offset
    .byte 2, 6    # transparent, handler ID
    .byte 0x00, 0x00, 0x46, 0x00    # state[4], interpretation is handler-specific

/* DS:03AD..03C3: Entity template: map_kind_05 */
    .byte EF_RENDER | EF_UPDATE | EF_MAP_OBJECT    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 32, 27, 16, 12    # width, height, signed anchor_x/y
    .word 0, 0x1b54    # bank byte-offset, sprite offset
    .byte 2, 7    # transparent, handler ID
    .byte 0x00, 0x00, 0x64, 0x00    # state[4], interpretation is handler-specific

/* DS:03C4..03DA: Entity template: map_kind_06 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 44, 23, 10, 19    # width, height, signed anchor_x/y
    .word 2, 0x6540    # bank byte-offset, sprite offset
    .byte 2, 8    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:03DB..03F1: Entity template: map_kind_10 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 1, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 44, 9, 10, 12    # width, height, signed anchor_x/y
    .word 2, 0x5fa0    # bank byte-offset, sprite offset
    .byte 2, 13    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:03F2..0408: Entity template: map_kind_08 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 32, 30, 4, 8    # width, height, signed anchor_x/y
    .word 4, 0x2348    # bank byte-offset, sprite offset
    .byte 2, 10    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0409..041F: Entity template: map_kind_09 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 32, 30, 4, 8    # width, height, signed anchor_x/y
    .word 4, 0x2348    # bank byte-offset, sprite offset
    .byte 2, 11    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0420..0436: Entity template: map_kind_07 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 44, 6, 8, -3    # width, height, signed anchor_x/y
    .word 4, 0x46a0    # bank byte-offset, sprite offset
    .byte 2, 9    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0437..044D: Entity template: map_kind_11 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 6, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 32, 25, 15, 13    # width, height, signed anchor_x/y
    .word 0, 0x2502    # bank byte-offset, sprite offset
    .byte 2, 26    # transparent, handler ID
    .byte 0x00, 0x00, 0x32, 0x00    # state[4], interpretation is handler-specific

/* DS:044E..0464: Entity template: map_kind_12 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 1, 6, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 32, 25, 16, 15    # width, height, signed anchor_x/y
    .word 2, 0x4286    # bank byte-offset, sprite offset
    .byte 13, 26    # transparent, handler ID
    .byte 0x00, 0x00, 0x28, 0x00    # state[4], interpretation is handler-specific

/* DS:0465..047B: Entity template: map_kind_14 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 2, 6, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 28, 25, 15, 15    # width, height, signed anchor_x/y
    .word 4, 0x2068    # bank byte-offset, sprite offset
    .byte 2, 26    # transparent, handler ID
    .byte 0x00, 0x00, 0x3c, 0x00    # state[4], interpretation is handler-specific

/* DS:047C..0492: Entity template: map_kind_13 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 32, 24, 4, -3    # width, height, signed anchor_x/y
    .word 2, 0x6e32    # bank byte-offset, sprite offset
    .byte 2, 28    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0493..04A9: Entity template: map_kind_15 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 16, 12, 7, -2    # width, height, signed anchor_x/y
    .word 0, 0x43ae    # bank byte-offset, sprite offset
    .byte 2, 32    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:04AA..04C0: Entity template: collectible_00 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 20, 11, 10, -2    # width, height, signed anchor_x/y
    .word 0, 0x0f3a    # bank byte-offset, sprite offset
    .byte 2, 18    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:04C1..04D7: Entity template: collectible_03 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 8, 5, 4, -4    # width, height, signed anchor_x/y
    .word 0, 0x6890    # bank byte-offset, sprite offset
    .byte 2, 18    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:04D8..04EE: Entity template: collectible_04 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 8, 5, 16, 4    # width, height, signed anchor_x/y
    .word 0, 0x6890    # bank byte-offset, sprite offset
    .byte 2, 18    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:04EF..0505: Entity template: collectible_05 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 24, 13, 10, 0    # width, height, signed anchor_x/y
    .word 0, 0x5b9e    # bank byte-offset, sprite offset
    .byte 2, 18    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0506..051C: Entity template: collectible_06 */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 24, 13, 11, 1    # width, height, signed anchor_x/y
    .word 0, 0x7222    # bank byte-offset, sprite offset
    .byte 2, 18    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:051D..0546: Высота изображения тайла в пикселях. */
    .byte 0x10, 0x10, 0x08, 0x18, 0x18, 0x18, 0x10, 0x10, 0x20, 0x20, 0x10, 0x10, 0x18, 0x18, 0x18, 0x18    # DS:051D
    .byte 0x10, 0x10, 0x10, 0x10, 0x18, 0x18, 0x18, 0x18, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x20, 0x20    # DS:052D
    .byte 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10    # DS:053D

/* DS:0547..0570: Поправка y изображения тайла. */
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0xf8, 0x00, 0x00, 0xf8, 0x00    # DS:0547
    .byte 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xf8    # DS:0557
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:0567

/* DS:0571..059A: Перепад высоты от левого к правому краю тайла. */
    .byte 0x00, 0x00, 0x00, 0x00, 0xf8, 0x08, 0xf8, 0x08, 0xf8, 0x08, 0xf8, 0x08, 0x00, 0x00, 0x08, 0xf8    # DS:0571
    .byte 0x00, 0x00, 0xf8, 0x08, 0x00, 0x00, 0x08, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:0581
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:0591

/* DS:059B..05C4: Вариант frame/surface при очищенном бите80h sample. */
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01    # DS:059B
    .byte 0x01, 0x01, 0x02, 0x02, 0x04, 0x05, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03    # DS:05AB
    .byte 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01    # DS:05BB

/* DS:05C5..05EE: Вариант при установленном бите80h sample. */
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x06, 0x07, 0x08, 0x09, 0x09, 0x08    # DS:05C5
    .byte 0x07, 0x06, 0x04, 0x05, 0x03, 0x03, 0x05, 0x04, 0x05, 0x04, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03    # DS:05D5
    .byte 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01    # DS:05E5

/* DS:05EF..0642: Таблица D_tile_sprite_offsets */
    .word 0x1466, 0x0000, 0x0a00, 0x000c, 0x0018, 0x0024, 0x2800, 0x280c
    .word 0x0f48, 0x0f54, 0x2818, 0x2824, 0x0030, 0x003c, 0x0048, 0x0054
    .word 0x2830, 0x283c, 0x1e00, 0x1e0c, 0x0060, 0x006c, 0x0f00, 0x0f0c
    .word 0x1e18, 0x1e24, 0x0f30, 0x0f3c, 0x0f18, 0x0f24, 0x0f60, 0x0f6c
    .word 0x1418, 0x1424, 0x1430, 0x143c, 0x1e30, 0x1e3c, 0x55a0, 0x55ac
    .word 0x4ba0, 0x4bac

/* DS:0643..0696: Таблица D_tile_profile_offsets */
    .word 0x0697, 0x0697, 0x06d7, 0x0717, 0x0757, 0x0797, 0x07d7, 0x0817
    .word 0x0857, 0x0897, 0x08d7, 0x0917, 0x0957, 0x0997, 0x09d7, 0x0a17
    .word 0x0a57, 0x0a97, 0x0ad7, 0x0b17, 0x0b57, 0x0b97, 0x0bd7, 0x0c17
    .word 0x0c57, 0x0c97, 0x06d7, 0x06d7, 0x06d7, 0x06d7, 0x0717, 0x0717
    .word 0x0717, 0x0717, 0x0717, 0x0717, 0x0717, 0x0717, 0x0697, 0x0697
    .word 0x0697, 0x0697

/* DS:0697..0CD6: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:0697
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:06A7
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:06B7
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:06C7
    .byte 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02    # DS:06D7
    .byte 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04    # DS:06E7
    .byte 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06    # DS:06F7
    .byte 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08    # DS:0707
    .byte 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e    # DS:0717
    .byte 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c    # DS:0727
    .byte 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1b, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a    # DS:0737
    .byte 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18    # DS:0747
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18    # DS:0757
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18    # DS:0767
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18    # DS:0777
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18    # DS:0787
    .byte 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08    # DS:0797
    .byte 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08    # DS:07A7
    .byte 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08    # DS:07B7
    .byte 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08    # DS:07C7
    .byte 0x00, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x01, 0x00, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a    # DS:07D7
    .byte 0x02, 0x01, 0x00, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x03, 0x02, 0x01, 0x00, 0x1f, 0x1e, 0x1d, 0x1c    # DS:07E7
    .byte 0x04, 0x03, 0x02, 0x01, 0x00, 0x1f, 0x1e, 0x1d, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x1f, 0x1e    # DS:07F7
    .byte 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x1f, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00    # DS:0807
    .byte 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09    # DS:0817
    .byte 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b    # DS:0827
    .byte 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d    # DS:0837
    .byte 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f    # DS:0847
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17    # DS:0857
    .byte 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15    # DS:0867
    .byte 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13    # DS:0877
    .byte 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11    # DS:0887
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x1f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06    # DS:0897
    .byte 0x1e, 0x1f, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x1d, 0x1e, 0x1f, 0x00, 0x01, 0x02, 0x03, 0x04    # DS:08A7
    .byte 0x1c, 0x1d, 0x1e, 0x1f, 0x00, 0x01, 0x02, 0x03, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x00, 0x01, 0x02    # DS:08B7
    .byte 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x00, 0x01, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x00    # DS:08C7
    .byte 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99    # DS:08D7
    .byte 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b    # DS:08E7
    .byte 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x9d    # DS:08F7
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f    # DS:0907
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x08    # DS:0917
    .byte 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x08, 0x08, 0x84, 0x85, 0x86, 0x87, 0x88, 0x08, 0x08, 0x08    # DS:0927
    .byte 0x85, 0x86, 0x87, 0x88, 0x08, 0x08, 0x08, 0x08, 0x86, 0x87, 0x88, 0x08, 0x08, 0x08, 0x08, 0x08    # DS:0937
    .byte 0x87, 0x88, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x88, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08    # DS:0947
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9e    # DS:0957
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x9c    # DS:0967
    .byte 0x00, 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x00, 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a    # DS:0977
    .byte 0x00, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98    # DS:0987
    .byte 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:0997
    .byte 0x9d, 0x9e, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9c, 0x9d, 0x9e, 0x9f, 0x00, 0x00, 0x00, 0x00    # DS:09A7
    .byte 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x00, 0x00, 0x00, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x00, 0x00    # DS:09B7
    .byte 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x00, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f    # DS:09C7
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87    # DS:09D7
    .byte 0x00, 0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x00, 0x00, 0x00, 0x81, 0x82, 0x83, 0x84, 0x85    # DS:09E7
    .byte 0x00, 0x00, 0x00, 0x00, 0x81, 0x82, 0x83, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x82, 0x83    # DS:09F7
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81    # DS:0A07
    .byte 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x18    # DS:0A17
    .byte 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x18, 0x18, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x18, 0x18, 0x18    # DS:0A27
    .byte 0x9b, 0x9a, 0x99, 0x98, 0x18, 0x18, 0x18, 0x18, 0x9a, 0x99, 0x98, 0x18, 0x18, 0x18, 0x18, 0x18    # DS:0A37
    .byte 0x99, 0x98, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x98, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18    # DS:0A47
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x82    # DS:0A57
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x82, 0x83, 0x00, 0x00, 0x00, 0x00, 0x81, 0x82, 0x83, 0x84    # DS:0A67
    .byte 0x00, 0x00, 0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x00, 0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86    # DS:0A77
    .byte 0x00, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88    # DS:0A87
    .byte 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:0A97
    .byte 0x83, 0x82, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0x83, 0x82, 0x81, 0x00, 0x00, 0x00, 0x00    # DS:0AA7
    .byte 0x85, 0x84, 0x83, 0x82, 0x81, 0x00, 0x00, 0x00, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x00, 0x00    # DS:0AB7
    .byte 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x00, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81    # DS:0AC7
    .byte 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x19    # DS:0AD7
    .byte 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x1a, 0x1a, 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x1b, 0x1b, 0x1b    # DS:0AE7
    .byte 0x9f, 0x9e, 0x9d, 0x9c, 0x1c, 0x1c, 0x1c, 0x1c, 0x9f, 0x9e, 0x9d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d    # DS:0AF7
    .byte 0x9f, 0x9e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x9f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f    # DS:0B07
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x02, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88    # DS:0B17
    .byte 0x03, 0x03, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x04, 0x04, 0x04, 0x84, 0x85, 0x86, 0x87, 0x88    # DS:0B27
    .byte 0x05, 0x05, 0x05, 0x05, 0x85, 0x86, 0x87, 0x88, 0x06, 0x06, 0x06, 0x06, 0x06, 0x86, 0x87, 0x88    # DS:0B37
    .byte 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x87, 0x88, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x88    # DS:0B47
    .byte 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x1f, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e    # DS:0B57
    .byte 0x1f, 0x1e, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x1f, 0x1e, 0x1d, 0x9c, 0x9c, 0x9c, 0x9c, 0x9c    # DS:0B67
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x9b, 0x9b, 0x9b, 0x9b, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x9a, 0x9a, 0x9a    # DS:0B77
    .byte 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x99, 0x99, 0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x98    # DS:0B87
    .byte 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x9e, 0x1f    # DS:0B97
    .byte 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x1e, 0x1f, 0x9c, 0x9c, 0x9c, 0x9c, 0x9c, 0x1d, 0x1e, 0x1f    # DS:0BA7
    .byte 0x9b, 0x9b, 0x9b, 0x9b, 0x1c, 0x1d, 0x1e, 0x1f, 0x9a, 0x9a, 0x9a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f    # DS:0BB7
    .byte 0x99, 0x99, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x98, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f    # DS:0BC7
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x07    # DS:0BD7
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x06, 0x06, 0x81, 0x82, 0x83, 0x84, 0x85, 0x05, 0x05, 0x05    # DS:0BE7
    .byte 0x81, 0x82, 0x83, 0x84, 0x04, 0x04, 0x04, 0x04, 0x81, 0x82, 0x83, 0x03, 0x03, 0x03, 0x03, 0x03    # DS:0BF7
    .byte 0x81, 0x82, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01    # DS:0C07
    .byte 0x9f, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x1e, 0x9e, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98    # DS:0C17
    .byte 0x1d, 0x1d, 0x9d, 0x9c, 0x9b, 0x9a, 0x99, 0x98, 0x1c, 0x1c, 0x1c, 0x9c, 0x9b, 0x9a, 0x99, 0x98    # DS:0C27
    .byte 0x1b, 0x1b, 0x1b, 0x1b, 0x9b, 0x9a, 0x99, 0x98, 0x1a, 0x1a, 0x1a, 0x1a, 0x1a, 0x9a, 0x99, 0x98    # DS:0C37
    .byte 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x99, 0x98, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x98    # DS:0C47
    .byte 0x81, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x81, 0x82, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02    # DS:0C57
    .byte 0x81, 0x82, 0x83, 0x03, 0x03, 0x03, 0x03, 0x03, 0x81, 0x82, 0x83, 0x84, 0x04, 0x04, 0x04, 0x04    # DS:0C67
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x05, 0x05, 0x05, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x06, 0x06    # DS:0C77
    .byte 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x07, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88    # DS:0C87
    .byte 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x81, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x82, 0x81    # DS:0C97
    .byte 0x03, 0x03, 0x03, 0x03, 0x03, 0x83, 0x82, 0x81, 0x04, 0x04, 0x04, 0x04, 0x84, 0x83, 0x82, 0x81    # DS:0CA7
    .byte 0x05, 0x05, 0x05, 0x85, 0x84, 0x83, 0x82, 0x81, 0x06, 0x06, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81    # DS:0CB7
    .byte 0x07, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x88, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81    # DS:0CC7

/* DS:0CD7..0CEA: Таблица D_map_column_bits */
    .word 0x0200, 0x0100, 0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004
    .word 0x0002, 0x0001

/* DS:0CEB..0D03: D_map_column_clear_masks — Инверсные маски для пометки использованного объекта; значения сохранить точно. */
    .byte 0xff, 0xff, 0xff, 0xfd, 0xff, 0xfe, 0x7f, 0xff, 0xbf, 0xff, 0xdf, 0xff, 0xef, 0xff, 0xf7, 0xff    # DS:0CEB
    .byte 0xfb, 0xff, 0xfd, 0xff, 0xfe, 0xff, 0xff, 0xff, 0x00    # DS:0CFB

/* DS:0D04..0D1A: Entity template: tunnel_player */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 2, 0, 0    # param, layer, signed z, frame
    .word 128, 154    # x, world_y
    .byte 64, 39, 32, 19    # width, height, signed anchor_x/y
    .word 6, 0x0000    # bank byte-offset, sprite offset
    .byte 2, 12    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0D1B..0D31: Entity template: tunnel_side */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 2, 0, 0    # param, layer, signed z, frame
    .word 128, 154    # x, world_y
    .byte 24, 19, 11, 10    # width, height, signed anchor_x/y
    .word 2, 0x5238    # bank byte-offset, sprite offset
    .byte 2, 34    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0D32..0D48: Entity template: surface_player */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 4, 0, 0    # param, layer, signed z, frame
    .word 128, 0    # x, world_y
    .byte 0, 0, 0, 0    # width, height, signed anchor_x/y
    .word 0, 0x1e10    # bank byte-offset, sprite offset
    .byte 2, 1    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0D49..0DA1: FrameSet 0D49: 11 frames, SoA layout */
    .byte 11    # frame_count
    .byte 32, 32, 32, 32, 28, 28, 32, 32, 28, 28, 32    # width[frame]
    .byte 19, 24, 23, 27, 25, 27, 24, 25, 29, 30, 23    # height[frame]
    .byte 14, 14, 14, 14, 15, 12, 14, 14, 13, 12, 14    # anchor_x (signed)[frame]
    .byte 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12    # anchor_y (signed)[frame]
    .word 0, 0x1e10    # frame  0: bank byte-offset, source offset
    .word 0, 0x5640    # frame  1: bank byte-offset, source offset
    .word 0, 0x0fae    # frame  2: bank byte-offset, source offset
    .word 0, 0x24e0    # frame  3: bank byte-offset, source offset
    .word 0, 0x35c0    # frame  4: bank byte-offset, source offset
    .word 0, 0x4560    # frame  5: bank byte-offset, source offset
    .word 0, 0x6540    # frame  6: bank byte-offset, source offset
    .word 0, 0x000e    # frame  7: bank byte-offset, source offset
    .word 0, 0x0000    # frame  8: bank byte-offset, source offset
    .word 0, 0x1220    # frame  9: bank byte-offset, source offset
    .word 2, 0x6f02    # frame 10: bank byte-offset, source offset

/* DS:0DA2..0DB8: Entity template: respawn_effect */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 1, 0, 0    # param, layer, signed z, frame
    .word 128, 80    # x, world_y
    .byte 64, 42, 32, 21    # width, height, signed anchor_x/y
    .word 0, 0x632e    # bank byte-offset, sprite offset
    .byte 2, 27    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:0DB9..0DC2: D_side_device_z_offsets — Поправка высоты бокового устройства по frame игрока. */
    .byte 0x00, 0x00, 0x00, 0x00, 0x04, 0xfc, 0x04, 0xfc, 0x04, 0xfc    # DS:0DB9

/* DS:0DC3..0DD9: Entity template: side_device */
    .byte EF_RENDER | EF_UPDATE | EF_PROJECTILE    # E_FLAGS
    .byte 0, 2, 0, 0    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 0, 0, 0, 0    # width, height, signed anchor_x/y
    .word 0, 0x0000    # bank byte-offset, sprite offset
    .byte 2, 24    # transparent, handler ID
    .byte 0x00, 0x00, 0x05, 0x00    # state[4], interpretation is handler-specific

/* DS:0DDA..0E2A: FrameSet 0DDA: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 12, 16, 16, 16, 12, 12, 12, 12, 12, 12    # width[frame]
    .byte 5, 12, 9, 15, 12, 12, 9, 9, 15, 15    # height[frame]
    .byte 5, 6, 6, 6, 5, 5, 5, 5, 5, 5    # anchor_x (signed)[frame]
    .byte 2, 5, 4, 7, 5, 5, 4, 4, 7, 7    # anchor_y (signed)[frame]
    .word 0, 0x311c    # frame  0: bank byte-offset, source offset
    .word 0, 0x5eb6    # frame  1: bank byte-offset, source offset
    .word 0, 0x717a    # frame  2: bank byte-offset, source offset
    .word 0, 0x4652    # frame  3: bank byte-offset, source offset
    .word 0, 0x4fb4    # frame  4: bank byte-offset, source offset
    .word 0, 0x5736    # frame  5: bank byte-offset, source offset
    .word 0, 0x663a    # frame  6: bank byte-offset, source offset
    .word 0, 0x6bda    # frame  7: bank byte-offset, source offset
    .word 0, 0x3116    # frame  8: bank byte-offset, source offset
    .word 0, 0x3a76    # frame  9: bank byte-offset, source offset

/* DS:0E2B..0E4B: FrameSet 0E2B: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 28, 32, 28, 16    # width[frame]
    .byte 24, 20, 23, 28    # height[frame]
    .byte 15, 17, 14, 11    # anchor_x (signed)[frame]
    .byte 10, 8, 10, 13    # anchor_y (signed)[frame]
    .word 0, 0x0030    # frame  0: bank byte-offset, source offset
    .word 0, 0x003e    # frame  1: bank byte-offset, source offset
    .word 0, 0x004e    # frame  2: bank byte-offset, source offset
    .word 0, 0x0f32    # frame  3: bank byte-offset, source offset

/* DS:0E4C..0E51: D_pingpong_frame_sequence — Последовательность циклической анимации handler3. */
    .byte 0x00, 0x01, 0x02, 0x03, 0x02, 0x01    # DS:0E4C

/* DS:0E52..0E72: FrameSet 0E52: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 20, 20, 24, 24    # width[frame]
    .byte 25, 24, 23, 22    # height[frame]
    .byte 10, 11, 12, 13    # anchor_x (signed)[frame]
    .byte 11, 10, 9, 8    # anchor_y (signed)[frame]
    .word 0, 0x4d10    # frame  0: bank byte-offset, source offset
    .word 0, 0x5cb0    # frame  1: bank byte-offset, source offset
    .word 0, 0x6bb0    # frame  2: bank byte-offset, source offset
    .word 0, 0x4dba    # frame  3: bank byte-offset, source offset

/* DS:0E73..0E9B: FrameSet 0E73: 5 frames, SoA layout */
    .byte 5    # frame_count
    .byte 28, 28, 28, 28, 32    # width[frame]
    .byte 27, 29, 30, 31, 29    # height[frame]
    .byte 16, 15, 16, 13, 15    # anchor_x (signed)[frame]
    .byte 14, 15, 15, 16, 14    # anchor_y (signed)[frame]
    .word 0, 0x2542    # frame  0: bank byte-offset, source offset
    .word 0, 0x497c    # frame  1: bank byte-offset, source offset
    .word 0, 0x0064    # frame  2: bank byte-offset, source offset
    .word 0, 0x361c    # frame  3: bank byte-offset, source offset
    .word 0, 0x1324    # frame  4: bank byte-offset, source offset

/* DS:0E9C..0EC4: FrameSet 0E9C: 5 frames, SoA layout */
    .byte 5    # frame_count
    .byte 24, 24, 24, 24, 28    # width[frame]
    .byte 21, 23, 23, 22, 21    # height[frame]
    .byte 14, 13, 11, 11, 11    # anchor_x (signed)[frame]
    .byte 9, 10, 10, 10, 9    # anchor_y (signed)[frame]
    .word 0, 0x0080    # frame  0: bank byte-offset, source offset
    .word 0, 0x008c    # frame  1: bank byte-offset, source offset
    .word 0, 0x63c2    # frame  2: bank byte-offset, source offset
    .word 0, 0x0d94    # frame  3: bank byte-offset, source offset
    .word 0, 0x0072    # frame  4: bank byte-offset, source offset

/* DS:0EC5..0EED: FrameSet 0EC5: 5 frames, SoA layout */
    .byte 5    # frame_count
    .byte 32, 32, 24, 32, 32    # width[frame]
    .byte 27, 28, 27, 29, 26    # height[frame]
    .byte 16, 16, 12, 15, 15    # anchor_x (signed)[frame]
    .byte 14, 14, 14, 14, 14    # anchor_y (signed)[frame]
    .word 0, 0x1b54    # frame  0: bank byte-offset, source offset
    .word 0, 0x2c30    # frame  1: bank byte-offset, source offset
    .word 0, 0x3daa    # frame  2: bank byte-offset, source offset
    .word 0, 0x4e8a    # frame  3: bank byte-offset, source offset
    .word 0, 0x52fa    # frame  4: bank byte-offset, source offset

/* DS:0EEE..0F3E: FrameSet 0EEE: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 32, 32, 32, 32, 32, 28, 32, 28, 32, 28    # width[frame]
    .byte 25, 25, 24, 30, 28, 31, 28, 33, 32, 32    # height[frame]
    .byte 15, 15, 15, 16, 15, 16, 17, 14, 16, 15    # anchor_x (signed)[frame]
    .byte 15, 15, 14, 15, 19, 19, 18, 18, 19, 19    # anchor_y (signed)[frame]
    .word 0, 0x2502    # frame  0: bank byte-offset, source offset
    .word 0, 0x2502    # frame  1: bank byte-offset, source offset
    .word 0, 0x5d40    # frame  2: bank byte-offset, source offset
    .word 0, 0x69b0    # frame  3: bank byte-offset, source offset
    .word 0, 0x1382    # frame  4: bank byte-offset, source offset
    .word 0, 0x001e    # frame  5: bank byte-offset, source offset
    .word 0, 0x4bc0    # frame  6: bank byte-offset, source offset
    .word 0, 0x371e    # frame  7: bank byte-offset, source offset
    .word 0, 0x55b0    # frame  8: bank byte-offset, source offset
    .word 0, 0x41ae    # frame  9: bank byte-offset, source offset

/* DS:0F3F..0F8F: FrameSet 0F3F: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 32, 32, 32, 32, 32, 28, 28, 28, 20, 28    # width[frame]
    .byte 25, 25, 27, 27, 25, 29, 24, 28, 29, 23    # height[frame]
    .byte 16, 16, 17, 17, 18, 13, 15, 12, 10, 17    # anchor_x (signed)[frame]
    .byte 14, 14, 17, 15, 16, 16, 16, 16, 13, 13    # anchor_y (signed)[frame]
    .word 2, 0x4286    # frame  0: bank byte-offset, source offset
    .word 2, 0x4286    # frame  1: bank byte-offset, source offset
    .word 2, 0x4d50    # frame  2: bank byte-offset, source offset
    .word 2, 0x308c    # frame  3: bank byte-offset, source offset
    .word 2, 0x4296    # frame  4: bank byte-offset, source offset
    .word 2, 0x4cce    # frame  5: bank byte-offset, source offset
    .word 2, 0x51c0    # frame  6: bank byte-offset, source offset
    .word 2, 0x5ef2    # frame  7: bank byte-offset, source offset
    .word 2, 0x2d62    # frame  8: bank byte-offset, source offset
    .word 2, 0x31dc    # frame  9: bank byte-offset, source offset

/* DS:0F90..0FE0: FrameSet 0F90: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 28, 28, 28, 28, 32, 32, 32, 32, 28, 24    # width[frame]
    .byte 25, 25, 22, 32, 27, 27, 20, 22, 32, 32    # height[frame]
    .byte 15, 15, 15, 15, 18, 14, 17, 14, 16, 9    # anchor_x (signed)[frame]
    .byte 15, 15, 11, 20, 16, 16, 9, 9, 18, 18    # anchor_y (signed)[frame]
    .word 4, 0x2068    # frame  0: bank byte-offset, source offset
    .word 4, 0x2068    # frame  1: bank byte-offset, source offset
    .word 4, 0x3008    # frame  2: bank byte-offset, source offset
    .word 4, 0x322e    # frame  3: bank byte-offset, source offset
    .word 4, 0x0090    # frame  4: bank byte-offset, source offset
    .word 4, 0x3628    # frame  5: bank byte-offset, source offset
    .word 4, 0x3638    # frame  6: bank byte-offset, source offset
    .word 4, 0x4708    # frame  7: bank byte-offset, source offset
    .word 4, 0x3220    # frame  8: bank byte-offset, source offset
    .word 4, 0x361c    # frame  9: bank byte-offset, source offset

/* DS:0FE1..0FE6: Таблица D_mobile_enemy_descriptors */
    .word D_sprite_frames_0eee, D_sprite_frames_0f3f, D_sprite_frames_0f90

/* DS:0FE7..0FF6: D_shot_variation_sequence — Детерминированная циклическая вариация; не PRNG. */
    .byte 0x00, 0x01, 0x02, 0x03, 0x01, 0x00, 0x03, 0x01, 0x00, 0x02, 0x03, 0x02, 0x00, 0x01, 0x03, 0x02    # DS:0FE7

/* DS:0FF7..100D: Entity template: muzzle_effect */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 5, 0, 1    # param, layer, signed z, frame
    .word 0, 0    # x, world_y
    .byte 0, 0, 0, 0    # width, height, signed anchor_x/y
    .word 0, 0x0000    # bank byte-offset, sprite offset
    .byte 2, 22    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:100E..102E: FrameSet 100E: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 4, 4, 4, 4    # width[frame]
    .byte 7, 7, 7, 7    # height[frame]
    .byte 1, 1, 2, 1    # anchor_x (signed)[frame]
    .byte 6, 6, 6, 6    # anchor_y (signed)[frame]
    .word 0, 0x78a2    # frame  0: bank byte-offset, source offset
    .word 0, 0x78a4    # frame  1: bank byte-offset, source offset
    .word 0, 0x78a6    # frame  2: bank byte-offset, source offset
    .word 0, 0x78a8    # frame  3: bank byte-offset, source offset

/* DS:102F..1038: Таблица D_aimed_effect_descriptors */
    .word D_sprite_frames_105a, D_sprite_frames_107b, D_sprite_frames_1039, D_sprite_frames_109c, D_sprite_frames_10bd

/* DS:1039..1059: FrameSet 1039: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 4, 4, 4, 4    # width[frame]
    .byte 7, 7, 7, 7    # height[frame]
    .byte 1, 1, 2, 1    # anchor_x (signed)[frame]
    .byte 0, 0, 0, 0    # anchor_y (signed)[frame]
    .word 0, 0x34ac    # frame  0: bank byte-offset, source offset
    .word 0, 0x390c    # frame  1: bank byte-offset, source offset
    .word 0, 0x3d6c    # frame  2: bank byte-offset, source offset
    .word 0, 0x41cc    # frame  3: bank byte-offset, source offset

/* DS:105A..107A: FrameSet 105A: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 8, 8, 8, 12    # width[frame]
    .byte 5, 5, 5, 5    # height[frame]
    .byte 8, 8, 8, 9    # anchor_x (signed)[frame]
    .byte 3, 3, 3, 3    # anchor_y (signed)[frame]
    .word 0, 0x7482    # frame  0: bank byte-offset, source offset
    .word 0, 0x729c    # frame  1: bank byte-offset, source offset
    .word 0, 0x75bc    # frame  2: bank byte-offset, source offset
    .word 0, 0x78dc    # frame  3: bank byte-offset, source offset

/* DS:107B..109B: FrameSet 107B: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 8, 8, 8, 8    # width[frame]
    .byte 4, 4, 4, 4    # height[frame]
    .byte 6, 6, 6, 6    # anchor_x (signed)[frame]
    .byte 1, 1, 1, 1    # anchor_y (signed)[frame]
    .word 0, 0x7780    # frame  0: bank byte-offset, source offset
    .word 0, 0x7784    # frame  1: bank byte-offset, source offset
    .word 0, 0x7788    # frame  2: bank byte-offset, source offset
    .word 0, 0x778c    # frame  3: bank byte-offset, source offset

/* DS:109C..10BC: FrameSet 109C: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 8, 8, 8, 8    # width[frame]
    .byte 5, 5, 5, 5    # height[frame]
    .byte 0, 0, 0, 0    # anchor_x (signed)[frame]
    .byte 1, 1, 1, 1    # anchor_y (signed)[frame]
    .word 0, 0x002c    # frame  0: bank byte-offset, source offset
    .word 0, 0x034c    # frame  1: bank byte-offset, source offset
    .word 0, 0x066c    # frame  2: bank byte-offset, source offset
    .word 0, 0x098c    # frame  3: bank byte-offset, source offset

/* DS:10BD..10DD: FrameSet 10BD: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 12, 12, 12, 12    # width[frame]
    .byte 5, 5, 5, 5    # height[frame]
    .byte 0, 0, 0, 0    # anchor_x (signed)[frame]
    .byte 3, 3, 3, 3    # anchor_y (signed)[frame]
    .word 0, 0x77a2    # frame  0: bank byte-offset, source offset
    .word 0, 0x7488    # frame  1: bank byte-offset, source offset
    .word 0, 0x77a8    # frame  2: bank byte-offset, source offset
    .word 0, 0x752e    # frame  3: bank byte-offset, source offset

/* DS:10DE..10E2: D_ground_shot_dx — Поправки spawn x по направлению. */
    .byte 0xf4, 0xfc, 0x00, 0x04, 0x0c    # DS:10DE

/* DS:10E3..10E7: D_ground_shot_dy — Поправки spawn y по направлению. */
    .byte 0x06, 0x07, 0x07, 0x07, 0x06    # DS:10E3

/* DS:10E8..10FE: Entity template: ground_shot */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 3, 0, 0    # param, layer, signed z, frame
    .word 128, 96    # x, world_y
    .byte 16, 12, 8, 6    # width, height, signed anchor_x/y
    .word 0, 0x3794    # bank byte-offset, sprite offset
    .byte 13, 29    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:10FF..1108: Таблица D_ground_shot_descriptors */
    .word D_sprite_frames_1109, D_sprite_frames_115a, D_sprite_frames_11ab, D_sprite_frames_11fc, D_sprite_frames_124d

/* DS:1109..1159: FrameSet 1109: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 16, 16, 16, 16, 16, 16, 16, 16, 16, 16    # width[frame]
    .byte 11, 11, 8, 13, 11, 13, 10, 10, 13, 11    # height[frame]
    .byte 8, 8, 8, 8, 8, 8, 8, 8, 8, 8    # anchor_x (signed)[frame]
    .byte 6, 6, 4, 6, 6, 6, 6, 6, 6, 6    # anchor_y (signed)[frame]
    .word 0, 0x10d0    # frame  0: bank byte-offset, source offset
    .word 0, 0x10d0    # frame  1: bank byte-offset, source offset
    .word 0, 0x15c0    # frame  2: bank byte-offset, source offset
    .word 0, 0x08b8    # frame  3: bank byte-offset, source offset
    .word 0, 0x0ee8    # frame  4: bank byte-offset, source offset
    .word 0, 0x0da0    # frame  5: bank byte-offset, source offset
    .word 0, 0x1848    # frame  6: bank byte-offset, source offset
    .word 0, 0x17b0    # frame  7: bank byte-offset, source offset
    .word 0, 0x0098    # frame  8: bank byte-offset, source offset
    .word 0, 0x10d8    # frame  9: bank byte-offset, source offset

/* DS:115A..11AA: FrameSet 115A: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 16, 16, 12, 16, 16, 16, 12, 16, 20, 16    # width[frame]
    .byte 12, 12, 8, 15, 11, 13, 8, 9, 15, 16    # height[frame]
    .byte 8, 8, 7, 8, 8, 8, 7, 8, 9, 8    # anchor_x (signed)[frame]
    .byte 6, 6, 5, 6, 6, 6, 5, 6, 7, 6    # anchor_y (signed)[frame]
    .word 0, 0x27ec    # frame  0: bank byte-offset, source offset
    .word 0, 0x27ec    # frame  1: bank byte-offset, source offset
    .word 0, 0x2f66    # frame  2: bank byte-offset, source offset
    .word 0, 0x1e8e    # frame  3: bank byte-offset, source offset
    .word 0, 0x27e4    # frame  4: bank byte-offset, source offset
    .word 0, 0x27f4    # frame  5: bank byte-offset, source offset
    .word 0, 0x2ec0    # frame  6: bank byte-offset, source offset
    .word 0, 0x300e    # frame  7: bank byte-offset, source offset
    .word 0, 0x1e84    # frame  8: bank byte-offset, source offset
    .word 0, 0x1df6    # frame  9: bank byte-offset, source offset

/* DS:11AB..11FB: FrameSet 11AB: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 16, 16, 16, 16, 12, 12, 12, 12, 12, 12    # width[frame]
    .byte 12, 12, 9, 15, 11, 11, 10, 9, 15, 15    # height[frame]
    .byte 6, 6, 6, 6, 5, 6, 5, 5, 5, 5    # anchor_x (signed)[frame]
    .byte 6, 6, 4, 7, 6, 6, 5, 4, 7, 7    # anchor_y (signed)[frame]
    .word 0, 0x3794    # frame  0: bank byte-offset, source offset
    .word 0, 0x3794    # frame  1: bank byte-offset, source offset
    .word 0, 0x3f02    # frame  2: bank byte-offset, source offset
    .word 0, 0x35a6    # frame  3: bank byte-offset, source offset
    .word 0, 0x3016    # frame  4: bank byte-offset, source offset
    .word 0, 0x3db6    # frame  5: bank byte-offset, source offset
    .word 0, 0x3dbc    # frame  6: bank byte-offset, source offset
    .word 0, 0x3f0a    # frame  7: bank byte-offset, source offset
    .word 0, 0x3460    # frame  8: bank byte-offset, source offset
    .word 0, 0x35ae    # frame  9: bank byte-offset, source offset

/* DS:11FC..124C: FrameSet 11FC: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 16, 16, 16, 16, 16, 16, 16, 12, 16, 20    # width[frame]
    .byte 12, 12, 8, 15, 13, 11, 9, 8, 16, 16    # height[frame]
    .byte 6, 6, 5, 6, 6, 6, 6, 6, 7, 7    # anchor_x (signed)[frame]
    .byte 6, 6, 5, 7, 6, 6, 5, 4, 7, 7    # anchor_y (signed)[frame]
    .word 0, 0x4910    # frame  0: bank byte-offset, source offset
    .word 0, 0x4910    # frame  1: bank byte-offset, source offset
    .word 0, 0x4b98    # frame  2: bank byte-offset, source offset
    .word 0, 0x4536    # frame  3: bank byte-offset, source offset
    .word 0, 0x4548    # frame  4: bank byte-offset, source offset
    .word 0, 0x3f18    # frame  5: bank byte-offset, source offset
    .word 0, 0x45f8    # frame  6: bank byte-offset, source offset
    .word 0, 0x4d68    # frame  7: bank byte-offset, source offset
    .word 0, 0x3f10    # frame  8: bank byte-offset, source offset
    .word 0, 0x449e    # frame  9: bank byte-offset, source offset

/* DS:124D..129D: FrameSet 124D: 10 frames, SoA layout */
    .byte 10    # frame_count
    .byte 16, 16, 16, 16, 16, 16, 16, 16, 16, 16    # width[frame]
    .byte 11, 11, 8, 13, 11, 11, 10, 10, 13, 13    # height[frame]
    .byte 6, 6, 6, 6, 6, 6, 6, 6, 6, 6    # anchor_x (signed)[frame]
    .byte 5, 5, 4, 6, 5, 5, 5, 5, 6, 6    # anchor_y (signed)[frame]
    .word 0, 0x62ae    # frame  0: bank byte-offset, source offset
    .word 0, 0x62ae    # frame  1: bank byte-offset, source offset
    .word 0, 0x6996    # frame  2: bank byte-offset, source offset
    .word 0, 0x5272    # frame  3: bank byte-offset, source offset
    .word 0, 0x5a92    # frame  4: bank byte-offset, source offset
    .word 0, 0x62b6    # frame  5: bank byte-offset, source offset
    .word 0, 0x6dee    # frame  6: bank byte-offset, source offset
    .word 0, 0x742e    # frame  7: bank byte-offset, source offset
    .word 0, 0x526a    # frame  8: bank byte-offset, source offset
    .word 0, 0x5a8a    # frame  9: bank byte-offset, source offset

/* DS:129E..12B4: Entity template: explosion */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 5, 0, 0    # param, layer, signed z, frame
    .word 128, 80    # x, world_y
    .byte 32, 29, 16, 14    # width, height, signed anchor_x/y
    .word 2, 0x4bc8    # bank byte-offset, sprite offset
    .byte 2, 16    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:12B5..12DD: FrameSet 12B5: 5 frames, SoA layout */
    .byte 5    # frame_count
    .byte 32, 32, 32, 28, 20    # width[frame]
    .byte 29, 27, 24, 20, 15    # height[frame]
    .byte 16, 16, 16, 16, 16    # anchor_x (signed)[frame]
    .byte 14, 14, 14, 14, 14    # anchor_y (signed)[frame]
    .word 2, 0x4bc8    # frame  0: bank byte-offset, source offset
    .word 2, 0x5e82    # frame  1: bank byte-offset, source offset
    .word 2, 0x2b72    # frame  2: bank byte-offset, source offset
    .word 2, 0x0772    # frame  3: bank byte-offset, source offset
    .word 2, 0x0edc    # frame  4: bank byte-offset, source offset

/* DS:12DE..12F4: Entity template: arc_effect */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 2, 0, 0    # param, layer, signed z, frame
    .word 128, 80    # x, world_y
    .byte 16, 15, 8, 7    # width, height, signed anchor_x/y
    .word 0, 0x005c    # bank byte-offset, sprite offset
    .byte 2, 31    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:12F5..1325: FrameSet 12F5: 6 frames, SoA layout */
    .byte 6    # frame_count
    .byte 16, 16, 16, 16, 12, 12    # width[frame]
    .byte 15, 15, 16, 13, 11, 8    # height[frame]
    .byte 8, 8, 8, 8, 6, 4    # anchor_x (signed)[frame]
    .byte 7, 7, 8, 7, 6, 4    # anchor_y (signed)[frame]
    .word 0, 0x005c    # frame  0: bank byte-offset, source offset
    .word 0, 0x09bc    # frame  1: bank byte-offset, source offset
    .word 0, 0x131c    # frame  2: bank byte-offset, source offset
    .word 0, 0x1d1c    # frame  3: bank byte-offset, source offset
    .word 0, 0x253c    # frame  4: bank byte-offset, source offset
    .word 0, 0x2c1a    # frame  5: bank byte-offset, source offset

/* DS:1326..133C: Entity template: side_impact */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 5, 0, 0    # param, layer, signed z, frame
    .word 128, 80    # x, world_y
    .byte 20, 15, 10, 7    # width, height, signed anchor_x/y
    .word 0, 0x445e    # bank byte-offset, sprite offset
    .byte 2, 23    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:133D..135D: FrameSet 133D: 4 frames, SoA layout */
    .byte 4    # frame_count
    .byte 20, 20, 16, 12    # width[frame]
    .byte 15, 15, 13, 8    # height[frame]
    .byte 10, 10, 8, 6    # anchor_x (signed)[frame]
    .byte 7, 7, 6, 4    # anchor_y (signed)[frame]
    .word 0, 0x445e    # frame  0: bank byte-offset, source offset
    .word 0, 0x4648    # frame  1: bank byte-offset, source offset
    .word 0, 0x3e2c    # frame  2: bank byte-offset, source offset
    .word 0, 0x3e26    # frame  3: bank byte-offset, source offset

/* DS:135E..1374: Entity template: player_shot */
    .byte EF_RENDER | EF_UPDATE | EF_PROJECTILE    # E_FLAGS
    .byte 0, 3, 0, 0    # param, layer, signed z, frame
    .word 128, 96    # x, world_y
    .byte 4, 4, 1, 2    # width, height, signed anchor_x/y
    .word 0, 0x7442    # bank byte-offset, sprite offset
    .byte 2, 19    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:1375..137E: D_player_left_muzzle_dx — Смещения левого ствола по frame. */
    .byte 0xfc, 0xfc, 0xfc, 0xfc, 0xfe, 0xfb, 0xfd, 0xfb, 0xfe, 0xfc    # DS:1375

/* DS:137F..1388: D_player_right_muzzle_dx — Смещения правого ствола по frame. */
    .byte 0x04, 0x04, 0x04, 0x04, 0x05, 0x02, 0x05, 0x03, 0x05, 0x03    # DS:137F

/* DS:1389..1392: D_player_left_muzzle_dz — Поправка высоты левого ствола. */
    .byte 0x00, 0x04, 0x05, 0x03, 0x05, 0x02, 0x06, 0x04, 0x05, 0x02    # DS:1389

/* DS:1393..139C: D_player_right_muzzle_dz — Поправка высоты правого ствола. */
    .byte 0x00, 0x04, 0x05, 0x03, 0x03, 0x05, 0x04, 0x06, 0x02, 0x04    # DS:1393

/* DS:139D..13A6: D_player_shot_frame_map — Выбор варианта выстрела по frame игрока. */
    .byte 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02    # DS:139D

/* DS:13A7..13A9: D_shot_variant_heights — Высоты трёх вариантов выстрела. */
    .byte 0x04, 0x03, 0x05    # DS:13A7

/* DS:13AA..13AF: D_player_shot_sprite_offsets — Sprite offsets выстрела игрока. */
    .byte 0x42, 0x74, 0x44, 0x74, 0x40, 0x74    # DS:13AA

/* DS:13B0..13C6: Entity template: enemy_shot */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 3, 0, 0    # param, layer, signed z, frame
    .word 128, 96    # x, world_y
    .byte 4, 4, 1, 2    # width, height, signed anchor_x/y
    .word 0, 0x7446    # bank byte-offset, sprite offset
    .byte 2, 20    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:13C7..13CC: D_enemy_shot_sprite_offsets — Sprite offsets выстрела противника. */
    .byte 0x46, 0x74, 0x44, 0x74, 0x48, 0x74    # DS:13C7

/* DS:13CD..13D6: D_enemy_muzzle_a_left_dz — Таблица высот pattern0, левая точка. */
    .byte 0x00, 0x04, 0x04, 0x01, 0x05, 0x04, 0x06, 0x05, 0x02, 0x01    # DS:13CD

/* DS:13D7..13E0: D_enemy_muzzle_a_right_dz — Таблица высот pattern0, правая точка. */
    .byte 0x00, 0x04, 0x04, 0x01, 0x04, 0x05, 0x05, 0x06, 0x01, 0x02    # DS:13D7

/* DS:13E1..13EA: D_enemy_muzzle_b_dz — Таблица высот pattern1. */
    .byte 0x00, 0x04, 0x08, 0x01, 0x06, 0x06, 0x08, 0x08, 0x01, 0x01    # DS:13E1

/* DS:13EB..13F4: D_enemy_muzzle_c_left_dz — Таблица высот pattern2, левая точка. */
    .byte 0x00, 0x04, 0x04, 0x08, 0x07, 0x04, 0x04, 0x01, 0x08, 0x08    # DS:13EB

/* DS:13F5..13FE: D_enemy_muzzle_c_center_dz — Таблица высот pattern2, центральная точка. */
    .byte 0x00, 0x04, 0x04, 0x08, 0x03, 0x04, 0x04, 0x04, 0x08, 0x08    # DS:13F5

/* DS:13FF..1408: D_enemy_muzzle_c_right_dz — Таблица высот pattern2, правая точка. */
    .byte 0x00, 0x04, 0x05, 0x01, 0x04, 0x06, 0x01, 0x04, 0x01, 0x01    # DS:13FF

/* DS:1409..140D: D_aimed_muzzle_dx — X offset по направлению. */
    .byte 0xf0, 0xf3, 0x00, 0x0d, 0x11    # DS:1409

/* DS:140E..1412: D_aimed_muzzle_dy — Y offset по направлению. */
    .byte 0x03, 0x09, 0x0c, 0x09, 0x03    # DS:140E

/* DS:1413..1417: D_enemy_shot_widths — Ширина выстрела по направлению. */
    .byte 0x08, 0x04, 0x04, 0x04, 0x08    # DS:1413

/* DS:1418..141C: D_enemy_shot_heights — Высота выстрела по направлению. */
    .byte 0x03, 0x04, 0x04, 0x04, 0x03    # DS:1418

/* DS:141D..1426: D_enemy_aimed_shot_sprites — Sprite offsets направленных выстрелов. */
    .byte 0xca, 0x76, 0x4c, 0x74, 0x46, 0x74, 0x4a, 0x74, 0xc4, 0x76    # DS:141D

/* DS:1427..143D: Entity template: impact */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 3, 0, 0    # param, layer, signed z, frame
    .word 128, 96    # x, world_y
    .byte 8, 5, 2, 2    # width, height, signed anchor_x/y
    .word 0, 0x3b06    # bank byte-offset, sprite offset
    .byte 2, 21    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:143E..1445: D_impact_frame_sprites — Четыре последовательных кадра попадания. */
    .byte 0x06, 0x3b, 0x0a, 0x3b, 0x0e, 0x3b, 0x12, 0x3b    # DS:143E

/* DS:1446..145C: Entity template: arc_shot */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 2, 0, 0    # param, layer, signed z, frame
    .word 128, 96    # x, world_y
    .byte 8, 7, 3, 3    # width, height, signed anchor_x/y
    .word 0, 0x78aa    # bank byte-offset, sprite offset
    .byte 2, 30    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:145D..1461: D_arc_spawn_dx — X offset навесного снаряда. */
    .byte 0xf7, 0xfb, 0x00, 0x06, 0x0a    # DS:145D

/* DS:1462..1466: D_arc_spawn_dy — Y offset навесного снаряда. */
    .byte 0x04, 0x04, 0x01, 0x04, 0x04    # DS:1462

/* DS:1467..1476: D_arc_height_deltas — Вертикальные приращения по возрасту снаряда. */
    .byte 0x00, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xf9, 0xf7    # DS:1467

/* DS:1477..1482: D_tunnel_gate_frame_sprites — Кадры створок входа; fully-open скрывает layer. */
    .byte 0x48, 0x23, 0x58, 0x23, 0x68, 0x23, 0x78, 0x23, 0x00, 0x32, 0x10, 0x32    # DS:1477

/* DS:1483..1494: D_exit_gate_frame_sprites — Кадры ворот выхода после6 collectibles. */
    .byte 0x32, 0x6e, 0x42, 0x6e, 0x52, 0x6e, 0x72, 0x6e, 0x82, 0x6e, 0x92, 0x5e, 0xa2, 0x5e, 0xb2, 0x5e    # DS:1483
    .byte 0xc2, 0x5e    # DS:1493

/* DS:1495..14C4: D_tunnel_destinations_a — Destination_y в10 битах и reward kind в старших2. */
    .byte 0x80, 0x03, 0xe0, 0xc0, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0xc1, 0xd0, 0x80, 0x80, 0x03    # DS:1495
    .byte 0x60, 0xc0, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03    # DS:14A5
    .byte 0x80, 0x03, 0x80, 0x03, 0x50, 0xc0, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03    # DS:14B5

/* DS:14C5..14F4: D_tunnel_destinations_b — Вторая таблица входов/выходов туннеля. */
    .byte 0x10, 0x43, 0xf0, 0x82, 0xc0, 0xc3, 0x40, 0x43, 0x30, 0x83, 0xa0, 0xc3, 0xc0, 0x43, 0x70, 0xc3    # DS:14C5
    .byte 0xa0, 0x83, 0x20, 0x83, 0x00, 0xc3, 0xa0, 0x43, 0xa0, 0x43, 0xc0, 0x83, 0xa0, 0xc3, 0x50, 0x83    # DS:14D5
    .byte 0x30, 0x43, 0xa0, 0xc3, 0xf0, 0x42, 0x30, 0x83, 0xa0, 0xc3, 0xa0, 0xc3, 0x30, 0x43, 0x90, 0x83    # DS:14E5

/* DS:14F5..1505: D_level_start_y — Исходная позиция камеры по level_index. */
    .byte 0x50, 0x03, 0x50, 0x03, 0x50, 0x03, 0x50, 0x03, 0x50, 0x03, 0x50, 0x03, 0x50, 0x03, 0x50, 0x03    # DS:14F5
    .byte 0x00    # DS:1505

/* DS:1506..150E: D_filename_hiscores — Имя внешнего файла, нулевой терминатор. */
    .byte 0x68, 0x69, 0x73, 0x63, 0x6f, 0x72, 0x65, 0x73, 0x00    # DS:1506

/* DS:150F..151E: D_control_prompt_banks — Bank offsets фрагментов запросов управления. */
    .byte 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x02, 0x00, 0x02, 0x00, 0x04, 0x00, 0x04, 0x00    # DS:150F

/* DS:151F..152E: D_control_prompt_sources — Sprite offsets запросов управления. */
    .byte 0x38, 0x13, 0xc8, 0x3d, 0xfc, 0x35, 0x18, 0x47, 0xa0, 0x73, 0x92, 0x75, 0x98, 0x4e, 0x98, 0x4e    # DS:151F

/* DS:152F..153E: D_control_prompt_destinations — Адреса назначения в video RAM, не DS-указатели. */
    .byte 0xcd, 0x0d, 0xcd, 0x0d, 0xcd, 0x0d, 0xcd, 0x0d, 0xcd, 0x0d, 0xcd, 0x0d, 0xd3, 0x0d, 0xd4, 0x0d    # DS:152F

/* DS:153F..154E: D_control_prompt_widths — Ширины фрагментов в пикселях. */
    .byte 0x28, 0x00, 0x30, 0x00, 0x3c, 0x00, 0x40, 0x00, 0x24, 0x00, 0x28, 0x00, 0x3c, 0x00, 0x3c, 0x00    # DS:153F

/* DS:154F..155E: D_control_prompt_heights — Высоты фрагментов. */
    .byte 0x0c, 0x00, 0x0f, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0c, 0x00    # DS:154F

/* DS:155F..1572: D_hud_digit_sprites — Sprite offsets цифр HUD. */
    .byte 0xfc, 0x44, 0x9c, 0x4a, 0x70, 0x3a, 0x78, 0x3a, 0x4a, 0x31, 0x52, 0x31, 0xea, 0x36, 0xf2, 0x36    # DS:155F
    .byte 0xb2, 0x44, 0x98, 0x3c    # DS:156F

/* DS:1573..157E: D_hud_digit_destinations — Адреса цифр в video RAM. */
    .byte 0x19, 0x1e, 0x1b, 0x1e, 0x1d, 0x1e, 0x1f, 0x1e, 0x21, 0x1e, 0x23, 0x1e    # DS:1573

/* DS:157F..1593: D_highscore_digit_sprites — Sprite offsets цифр таблицы рекордов. */
    .byte 0xc0, 0x44, 0xc6, 0x44, 0xcc, 0x44, 0xd2, 0x44, 0xd8, 0x44, 0xde, 0x44, 0xe4, 0x44, 0xea, 0x44    # DS:157F
    .byte 0xf0, 0x44, 0xf6, 0x44, 0x00    # DS:158F

/* DS:1594..1674: FrameSet 1594: 28 frames, SoA layout */
    .byte 28    # frame_count
    .byte 64, 64, 64, 64, 64, 64, 64, 64, 56, 44, 40, 44, 56, 64, 64, 64, 56, 44, 40, 44, 56, 64, 64, 64, 64, 64, 64, 64    # width[frame]
    .byte 39, 39, 38, 38, 38, 41, 42, 43, 53, 61, 63, 61, 53, 43, 39, 43, 53, 61, 63, 61, 53, 43, 42, 41, 38, 38, 38, 39    # height[frame]
    .byte 31, 30, 31, 31, 30, 30, 30, 28, 22, 18, 18, 18, 22, 28, 31, 32, 30, 24, 20, 24, 30, 32, 32, 33, 33, 31, 31, 33    # anchor_x (signed)[frame]
    .byte 18, 19, 18, 18, 18, 19, 20, 18, 22, 28, 31, 32, 30, 24, 20, 24, 30, 32, 31, 28, 22, 18, 20, 19, 18, 18, 18, 19    # anchor_y (signed)[frame]
    .word 6, 0x0000    # frame  0: bank byte-offset, source offset
    .word 6, 0x1860    # frame  1: bank byte-offset, source offset
    .word 6, 0x30c0    # frame  2: bank byte-offset, source offset
    .word 6, 0x4880    # frame  3: bank byte-offset, source offset
    .word 6, 0x6040    # frame  4: bank byte-offset, source offset
    .word 6, 0x0020    # frame  5: bank byte-offset, source offset
    .word 6, 0x19c0    # frame  6: bank byte-offset, source offset
    .word 6, 0x3400    # frame  7: bank byte-offset, source offset
    .word 6, 0x4ee0    # frame  8: bank byte-offset, source offset
    .word 6, 0x0040    # frame  9: bank byte-offset, source offset
    .word 6, 0x2660    # frame 10: bank byte-offset, source offset
    .word 6, 0x4efc    # frame 11: bank byte-offset, source offset
    .word 6, 0x0056    # frame 12: bank byte-offset, source offset
    .word 6, 0x2174    # frame 13: bank byte-offset, source offset
    .word 6, 0x3c54    # frame 14: bank byte-offset, source offset
    .word 6, 0x54b2    # frame 15: bank byte-offset, source offset
    .word 6, 0x0072    # frame 16: bank byte-offset, source offset
    .word 6, 0x2194    # frame 17: bank byte-offset, source offset
    .word 6, 0x47b4    # frame 18: bank byte-offset, source offset
    .word 6, 0x21aa    # frame 19: bank byte-offset, source offset
    .word 4, 0x4460    # frame 20: bank byte-offset, source offset
    .word 4, 0x4a80    # frame 21: bank byte-offset, source offset
    .word 4, 0x4a60    # frame 22: bank byte-offset, source offset
    .word 4, 0x63e0    # frame 23: bank byte-offset, source offset
    .word 4, 0x65a0    # frame 24: bank byte-offset, source offset
    .word 4, 0x6580    # frame 25: bank byte-offset, source offset
    .word 4, 0x6560    # frame 26: bank byte-offset, source offset
    .word 4, 0x64a0    # frame 27: bank byte-offset, source offset

/* DS:1675..16F4: D_tunnel_circle_x — Готовые x координаты окружности; сохранить lookup, не заменять sin/cos. */
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f    # DS:1675
    .byte 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18    # DS:1685
    .byte 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10    # DS:1695
    .byte 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00    # DS:16A5
    .byte 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0    # DS:16B5
    .byte 0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7    # DS:16C5
    .byte 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef    # DS:16D5
    .byte 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff    # DS:16E5

/* DS:16F5..1774: D_tunnel_circle_y — Готовые y координаты окружности. */
    .byte 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11    # DS:16F5
    .byte 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01    # DS:1705
    .byte 0x00, 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1    # DS:1715
    .byte 0xf0, 0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xe9, 0xe8, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7    # DS:1725
    .byte 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef    # DS:1735
    .byte 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff    # DS:1745
    .byte 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f    # DS:1755
    .byte 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18    # DS:1765

/* DS:1775..17F4: D_tunnel_player_frames — Frame корабля по углу. */
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x08, 0x08, 0x08    # DS:1775
    .byte 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0a, 0x0a, 0x0a, 0x0a    # DS:1785
    .byte 0x0a, 0x0a, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c    # DS:1795
    .byte 0x0c, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0e, 0x0e    # DS:17A5
    .byte 0x0e, 0x0e, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x10, 0x10, 0x10, 0x10    # DS:17B5
    .byte 0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12    # DS:17C5
    .byte 0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x14    # DS:17D5
    .byte 0x14, 0x14, 0x14, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:17E5

/* DS:17F5..1810: D_tunnel_frame_to_sector — Сектор сбора предмета по frame игрока; фактический домен см. таблицу. */
    .byte 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01    # DS:17F5
    .byte 0x08, 0x08, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05    # DS:1805

/* DS:1811..1891: FrameSet 1811: 16 frames, SoA layout */
    .byte 16    # frame_count
    .byte 24, 24, 20, 20, 20, 20, 20, 24, 24, 24, 20, 20, 20, 20, 20, 24    # width[frame]
    .byte 19, 17, 19, 21, 23, 21, 19, 18, 19, 18, 19, 21, 23, 21, 19, 18    # height[frame]
    .byte 11, 11, 10, 10, 10, 10, 10, 11, 11, 11, 10, 10, 10, 10, 10, 11    # anchor_x (signed)[frame]
    .byte 10, 10, 10, 10, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10    # anchor_y (signed)[frame]
    .word 2, 0x5238    # frame  0: bank byte-offset, source offset
    .word 2, 0x52e4    # frame  1: bank byte-offset, source offset
    .word 2, 0x61f2    # frame  2: bank byte-offset, source offset
    .word 2, 0x60bc    # frame  3: bank byte-offset, source offset
    .word 2, 0x1c06    # frame  4: bank byte-offset, source offset
    .word 2, 0x1fd0    # frame  5: bank byte-offset, source offset
    .word 2, 0x6206    # frame  6: bank byte-offset, source offset
    .word 2, 0x1836    # frame  7: bank byte-offset, source offset
    .word 2, 0x13f2    # frame  8: bank byte-offset, source offset
    .word 2, 0x10c6    # frame  9: bank byte-offset, source offset
    .word 2, 0x2188    # frame 10: bank byte-offset, source offset
    .word 2, 0x2372    # frame 11: bank byte-offset, source offset
    .word 2, 0x237c    # frame 12: bank byte-offset, source offset
    .word 2, 0x50d8    # frame 13: bank byte-offset, source offset
    .word 2, 0x5222    # frame 14: bank byte-offset, source offset
    .word 2, 0x52cc    # frame 15: bank byte-offset, source offset

/* DS:1892..1911: D_tunnel_side_frames — Frame бокового устройства по углу игрока. */
    .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02    # DS:1892
    .byte 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04    # DS:18A2
    .byte 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06    # DS:18B2
    .byte 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x08, 0x08, 0x08, 0x08    # DS:18C2
    .byte 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x0a, 0x0a, 0x0a, 0x0a    # DS:18D2
    .byte 0x0a, 0x0a, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x0c    # DS:18E2
    .byte 0x0c, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0e, 0x0e    # DS:18F2
    .byte 0x0e, 0x0e, 0x0e, 0x0e, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:1902

/* DS:1912..1928: Entity template: tunnel_exit */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 7, 0, 0    # param, layer, signed z, frame
    .word 128, 106    # x, world_y
    .byte 4, 2, 0, 1    # width, height, signed anchor_x/y
    .word 8, 0x7bee    # bank byte-offset, sprite offset
    .byte 2, 14    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:1929..19E9: FrameSet 1929: 24 frames, SoA layout */
    .byte 24    # frame_count
    .byte 4, 4, 4, 4, 8, 8, 8, 8, 8, 12, 12, 12, 16, 16, 20, 24, 28, 32, 36, 40, 44, 48, 56, 64    # width[frame]
    .byte 2, 2, 3, 4, 5, 6, 7, 7, 8, 9, 9, 11, 13, 15, 17, 19, 22, 25, 29, 33, 38, 43, 49, 57    # height[frame]
    .byte 0, 1, 1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 7, 8, 9, 11, 12, 14, 16, 19, 21, 23, 27, 31    # anchor_x (signed)[frame]
    .byte 0, 0, 1, 2, 2, 3, 3, 3, 4, 4, 4, 5, 6, 7, 8, 9, 11, 12, 14, 16, 19, 21, 24, 28    # anchor_y (signed)[frame]
    .word 8, 0x7bee    # frame  0: bank byte-offset, source offset
    .word 8, 0x7bec    # frame  1: bank byte-offset, source offset
    .word 8, 0x7b4a    # frame  2: bank byte-offset, source offset
    .word 8, 0x7aa8    # frame  3: bank byte-offset, source offset
    .word 8, 0x7518    # frame  4: bank byte-offset, source offset
    .word 8, 0x7514    # frame  5: bank byte-offset, source offset
    .word 8, 0x7510    # frame  6: bank byte-offset, source offset
    .word 8, 0x750c    # frame  7: bank byte-offset, source offset
    .word 8, 0x7508    # frame  8: bank byte-offset, source offset
    .word 8, 0x23e6    # frame  9: bank byte-offset, source offset
    .word 8, 0x04ac    # frame 10: bank byte-offset, source offset
    .word 8, 0x0a4c    # frame 11: bank byte-offset, source offset
    .word 8, 0x112c    # frame 12: bank byte-offset, source offset
    .word 8, 0x194c    # frame 13: bank byte-offset, source offset
    .word 8, 0x22ac    # frame 14: bank byte-offset, source offset
    .word 8, 0x2d46    # frame 15: bank byte-offset, source offset
    .word 8, 0x3926    # frame 16: bank byte-offset, source offset
    .word 8, 0x46e0    # frame 17: bank byte-offset, source offset
    .word 8, 0x5680    # frame 18: bank byte-offset, source offset
    .word 8, 0x689e    # frame 19: bank byte-offset, source offset
    .word 8, 0x5d48    # frame 20: bank byte-offset, source offset
    .word 8, 0x4268    # frame 21: bank byte-offset, source offset
    .word 8, 0x23ca    # frame 22: bank byte-offset, source offset
    .word 8, 0x002c    # frame 23: bank byte-offset, source offset

/* DS:19EA..1A09: D_tunnel_exit_frames — Анимация выходного маркера по remaining distance. */
    .byte 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08    # DS:19EA
    .byte 0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00    # DS:19FA

/* DS:1A0A..1A29: D_tunnel_min_ring_radii — Минимальный радиус колец по дистанции. */
    .byte 0x2e, 0x2a, 0x26, 0x22, 0x1d, 0x1a, 0x17, 0x14, 0x12, 0x10, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09    # DS:1A0A
    .byte 0x08, 0x08, 0x07, 0x07, 0x06, 0x06, 0x05, 0x05, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03    # DS:1A1A

/* DS:1A2A..1A69: D_tunnel_pattern_pointers — Четыре потока появления туннельных предметов. */
    .byte 0x32, 0x1a, 0x40, 0x1a, 0x4e, 0x1a, 0x5c, 0x1a, 0x05, 0x06, 0x07, 0x08, 0x01, 0x02, 0x03, 0x04    # DS:1A2A
    .byte 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x04, 0x03, 0x02, 0x01, 0x08, 0x07, 0x06, 0x05, 0x00    # DS:1A3A
    .byte 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x01, 0x00, 0x00, 0x00    # DS:1A4A
    .byte 0x00, 0x00, 0x01, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00    # DS:1A5A

/* DS:1A6A..1A99: D_tunnel_pickup_radii — Радиус изображения по дистанции от spawn. */
    .byte 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06    # DS:1A6A
    .byte 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x12, 0x13, 0x14, 0x16, 0x18, 0x1a    # DS:1A7A
    .byte 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2c, 0x2e, 0x31, 0x34, 0x38, 0x3c, 0x40, 0x45, 0x4b    # DS:1A8A

/* DS:1A9A..1AB1: D_tunnel_pickup_layers — Layer по кадру приближения предмета. */
    .byte 0x07, 0x07, 0x07, 0x07, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04    # DS:1A9A
    .byte 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01    # DS:1AAA

/* DS:1AB2..1AC8: Entity template: tunnel_pickup */
    .byte EF_RENDER | EF_UPDATE    # E_FLAGS
    .byte 0, 6, 0, 0    # param, layer, signed z, frame
    .word 128, 106    # x, world_y
    .byte 4, 2, 0, 1    # width, height, signed anchor_x/y
    .word 8, 0x20a8    # bank byte-offset, sprite offset
    .byte 2, 15    # transparent, handler ID
    .byte 0x00, 0x00, 0x00, 0x00    # state[4], interpretation is handler-specific

/* DS:1AC9..1B89: FrameSet 1AC9: 24 frames, SoA layout */
    .byte 24    # frame_count
    .byte 4, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 16, 16, 16, 20, 20, 24, 28, 32, 36, 40, 48, 56, 60    # width[frame]
    .byte 2, 2, 2, 3, 4, 4, 5, 6, 7, 8, 10, 12, 14, 15, 17, 20, 23, 27, 30, 35, 40, 45, 52, 60    # height[frame]
    .byte 0, 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 8, 10, 11, 13, 15, 17, 20, 22, 26, 30    # anchor_x (signed)[frame]
    .byte 0, 0, 0, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 8, 8, 10, 11, 13, 15, 17, 20, 22, 26, 30    # anchor_y (signed)[frame]
    .word 8, 0x20a8    # frame  0: bank byte-offset, source offset
    .word 8, 0x1f68    # frame  1: bank byte-offset, source offset
    .word 8, 0x1e28    # frame  2: bank byte-offset, source offset
    .word 8, 0x1c48    # frame  3: bank byte-offset, source offset
    .word 8, 0x19c8    # frame  4: bank byte-offset, source offset
    .word 8, 0x1748    # frame  5: bank byte-offset, source offset
    .word 8, 0x1428    # frame  6: bank byte-offset, source offset
    .word 8, 0x1066    # frame  7: bank byte-offset, source offset
    .word 8, 0x0c06    # frame  8: bank byte-offset, source offset
    .word 8, 0x0706    # frame  9: bank byte-offset, source offset
    .word 8, 0x0026    # frame 10: bank byte-offset, source offset
    .word 8, 0x047e    # frame 11: bank byte-offset, source offset
    .word 8, 0x0c9e    # frame 12: bank byte-offset, source offset
    .word 8, 0x14be    # frame 13: bank byte-offset, source offset
    .word 8, 0x1e1e    # frame 14: bank byte-offset, source offset
    .word 8, 0x28bc    # frame 15: bank byte-offset, source offset
    .word 8, 0x353c    # frame 16: bank byte-offset, source offset
    .word 8, 0x439a    # frame 17: bank byte-offset, source offset
    .word 8, 0x5478    # frame 18: bank byte-offset, source offset
    .word 8, 0x6736    # frame 19: bank byte-offset, source offset
    .word 8, 0x6220    # frame 20: bank byte-offset, source offset
    .word 8, 0x4600    # frame 21: bank byte-offset, source offset
    .word 8, 0x2580    # frame 22: bank byte-offset, source offset
    .word 8, 0x0000    # frame 23: bank byte-offset, source offset

/* DS:1B8A..1FD9: Entity pool: 48 records; original initializes every byte to zero. */
    .fill ENTITY_CAPACITY * ENTITY_SIZE, 1, 0

/* DS:1FDA..1FDB: Pool sentinel, not a 49th Entity */
    .byte EF_SENTINEL, 0

/* DS:1FDC..201B: Радиус по младшим6 битам дистанции. */
    .byte 0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1a, 0x1b, 0x1b, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1e, 0x1e    # DS:1FDC
    .byte 0x1f, 0x1f, 0x1f, 0x20, 0x20, 0x21, 0x21, 0x21, 0x22, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x24    # DS:1FEC
    .byte 0x25, 0x25, 0x26, 0x26, 0x26, 0x27, 0x27, 0x27, 0x28, 0x28, 0x29, 0x29, 0x29, 0x2a, 0x2a, 0x2a    # DS:1FFC
    .byte 0x2b, 0x2b, 0x2c, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2e, 0x2f, 0x2f, 0x2f, 0x30, 0x30, 0x31    # DS:200C

/* DS:201C..2028: Дополнительные линии при phase<=12. */
    .byte 0x31, 0x36, 0x39, 0x3e, 0x41, 0x44, 0x48, 0x4b, 0x50, 0x53, 0x56, 0x58, 0x5b    # DS:201C

/* DS:2029..2035: Вторая таблица дополнительных линий. */
    .byte 0x31, 0x36, 0x39, 0x3e, 0x41, 0x43, 0x49, 0x4b, 0x50, 0x53, 0x56, 0x5a, 0x5e    # DS:2029
