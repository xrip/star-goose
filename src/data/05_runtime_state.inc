/* DS:8E7E — данные; не исполняемый код. */

/* DS:8E7E..8E7F: D_attract_script_pointer — DS offset сценария заставки. */
    .word 0

/* DS:8E80..8E81: D_attract_step_counter — Счётчик кадров сценария; команда каждую8-ю итерацию. */
    .word 0

/* DS:8E82..8E83: D_old_keyboard_segment — Сегмент старого INT09h. */
    .word 0

/* DS:8E84..8E85: D_old_keyboard_offset — Offset старого INT09h. */
    .word 0

/* DS:8E86..8E87: D_old_timer_segment — Сегмент старого INT1Ch. */
    .word 0

/* DS:8E88..8E89: D_old_timer_offset — Offset старого INT1Ch. */
    .word 0

/* DS:8E8A..8F09: D_keyboard_state — 0 = отпущена, 1 = нажата; индекс — scan-code. */
    .fill 128, 1, 0x00    # DS:8E8A..8F09; сохранённые данные

/* DS:8F0A..8F0A: D_mouse_present — Признак установленного драйвера мыши. */
    .byte 0

/* DS:8F0B..8F0C: D_last_key_pointer — DS offset последней клавиши при вводе имени. */
    .word 0

/* DS:8F0D..8F0E: D_key_left — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F0F..8F10: D_key_right — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F11..8F12: D_key_up — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F13..8F14: D_key_down — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F15..8F16: D_key_fire — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F17..8F18: D_key_action_n — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F19..8F1A: D_key_action_m — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F1B..8F1C: D_key_quit — Настроенный scan-code; фактически используется младший байт. */
    .word 0

/* DS:8F1D..8F1D: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00    # DS:8F1D

/* DS:8F1E..8F1F: D_file_handle — DOS file handle. */
    .word 0

/* DS:8F20..8F21: D_file_destination_segment — Временный сегмент назначения DOS read. */
    .word 0

/* DS:8F22..8F22: D_map_running_height — Накопленная высота вдоль текущей строки тайлов. */
    .byte 0

/* DS:8F23..8F24: D_map_row_y — Текущая y-строка карты при prerender. */
    .word 0

/* DS:8F25..8F26: D_map_tile_cursor — DS offset следующего декодированного тайла. */
    .word 0

/* DS:8F27..8F28: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00, 0x00    # DS:8F27

/* DS:8F29..9228: D_decoded_map_tiles — 12 столбцов ×64 строки; хранение обратное world_y. */
    .fill 768, 1, 0x00    # DS:8F29..9228; сохранённые данные

/* DS:9229..92A8: D_map_spawn_rows — 10 бит колонок +6 бит вида объекта строки. */
    .fill 128, 1, 0x00    # DS:9229..92A8; сохранённые данные

/* DS:92A9..9428: D_map_available_masks — Сброшенный бит означает использованный объект этой части уровня. */
    .fill 384, 1, 0x00    # DS:92A9..9428; сохранённые данные

/* DS:9429..9429: D_collision_damage_tmp — Временный результат query_player_map_collision. */
    .byte 0

/* DS:942A..942B: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00, 0x00    # DS:942A

/* DS:942C..942D: D_collision_column_tmp — Текущий столбец поиска препятствия. */
    .word 0

/* DS:942E..942F: D_sprite_dest_segment — EGA segment для следующего blit. */
    .word 0

/* DS:9430..9431: D_sprite_clip_enabled — 1 для игрового экрана, 0 для prerender карты. */
    .word 0

/* DS:9432..9432: D_player_forced_stop — Запрос отдельного состояния игрока после выхода из туннеля. */
    .byte 0

/* DS:9433..9434: D_player_history_x0 — Новейшая из4 сохранённых координат x. */
    .word 0

/* DS:9435..9436: D_player_history_x1 — История x, задержка1. */
    .word 0

/* DS:9437..9438: D_player_history_x2 — История x, задержка2. */
    .word 0

/* DS:9439..943A: D_player_history_x3 — История x, задержка3. */
    .word 0

/* DS:943B..943C: D_player_history_y0 — Новейшая y. */
    .word 0

/* DS:943D..943E: D_player_history_y1 — История y1. */
    .word 0

/* DS:943F..9440: D_player_history_y2 — История y2. */
    .word 0

/* DS:9441..9442: D_player_history_y3 — История y3. */
    .word 0

/* DS:9443..9443: D_player_history_frame0 — Новейший frame. */
    .byte 0

/* DS:9444..9444: D_player_history_frame1 — История frame1. */
    .byte 0

/* DS:9445..9445: D_player_history_frame2 — История frame2. */
    .byte 0

/* DS:9446..9446: D_player_history_frame3 — История frame3. */
    .byte 0

/* DS:9447..9447: D_player_history_z0 — Новейшая высота. */
    .byte 0

/* DS:9448..9448: D_player_history_z1 — История z1. */
    .byte 0

/* DS:9449..9449: D_player_history_z2 — История z2. */
    .byte 0

/* DS:944A..944A: D_player_history_z3 — История z3. */
    .byte 0

/* DS:944B..944B: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00    # DS:944B

/* DS:944C..944C: D_hud_frame_counter — Очки за этап начисляются при (counter&3)==0. */
    .byte 0

/* DS:944D..944D: D_score_leading_zero — Флаг подавления ведущих нулей. */
    .byte 0

/* DS:944E..944F: D_text_draw_offset — Offset в PODZ1 для текста таблицы рекордов. */
    .word 0

/* DS:9450..9450: D_pending_highscore_rank — Строка редактируемого имени. */
    .byte 0

/* DS:9451..9452: D_pending_name_pointer — DS offset 12-байтового имени. */
    .word 0

/* DS:9453..9454: D_name_cursor — Позиция0..12 при редактировании имени. */
    .word 0

/* DS:9455..94A8: D_highscore_data — HISCORES: 84 байта имён, 42 цифр, 7 scratch, 7 индексов. */
    .fill 84, 1, 0x00    # DS:9455..94A8; сохранённые данные

/* DS:94A9..94A9: D_score_digit_100000 — Первая из6 цифр текущего счёта; начало7 результатов. */
    .byte 0

/* DS:94AA..94AA: D_score_digit_10000 — Десятичная цифра10000. */
    .byte 0

/* DS:94AB..94AB: D_score_digit_1000 — Десятичная цифра1000. */
    .byte 0

/* DS:94AC..94AC: D_score_digit_100 — Десятичная цифра100. */
    .byte 0

/* DS:94AD..94AD: D_score_digit_10 — Десятичная цифра10. */
    .byte 0

/* DS:94AE..94AE: D_score_digit_1 — Десятичная цифра1. */
    .byte 0

/* DS:94AF..94D2: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .fill 36, 1, 0x00    # DS:94AF..94D2; сохранённые данные

/* DS:94D3..94D3: D_highscore_sort_scratch0 — Первый из7 scratch индексов сортировки. */
    .byte 0

/* DS:94D4..94D4: D_highscore_sort_scratch1 — Scratch1. */
    .byte 0

/* DS:94D5..94D5: D_highscore_sort_scratch2 — Scratch2. */
    .byte 0

/* DS:94D6..94D6: D_highscore_sort_scratch3 — Scratch3. */
    .byte 0

/* DS:94D7..94D7: D_highscore_sort_scratch4 — Scratch4. */
    .byte 0

/* DS:94D8..94D8: D_highscore_sort_scratch5 — Scratch5. */
    .byte 0

/* DS:94D9..94D9: D_highscore_sort_scratch6 — Scratch6. */
    .byte 0

/* DS:94DA..94DA: D_highscore_order0 — Первый из7 отсортированных индексов. */
    .byte 0

/* DS:94DB..94DB: D_highscore_order1 — Order1. */
    .byte 0

/* DS:94DC..94DC: D_highscore_order2 — Order2. */
    .byte 0

/* DS:94DD..94DD: D_highscore_order3 — Order3. */
    .byte 0

/* DS:94DE..94DE: D_highscore_order4 — Order4. */
    .byte 0

/* DS:94DF..94DF: D_highscore_order5 — Order5. */
    .byte 0

/* DS:94E0..94E0: D_highscore_order6 — Последний индекс; 0 означает, что текущий результат не вошёл в6 лучших. */
    .byte 0

/* DS:94E1..94E1: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00    # DS:94E1

/* DS:94E2..94E3: D_tunnel_pattern_pointer — DS offset потока объектов туннеля. */
    .word 0

/* DS:94E4..94E4: D_tunnel_pattern_index — Циклический выбор паттерна по &3. */
    .byte 0

/* DS:94E5..94E5: D_tunnel_angle_history0 — История угла0. */
    .byte 0

/* DS:94E6..94E6: D_tunnel_angle_history1 — История угла1. */
    .byte 0

/* DS:94E7..94E7: D_tunnel_angle_history2 — История угла2. */
    .byte 0

/* DS:94E8..94E9: D_segment_bird1 — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94EA..94EB: D_segment_bird2 — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94EC..94ED: D_segment_blox — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94EE..94EF: D_segment_newbird — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94F0..94F1: D_segment_podz1 — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94F2..94F3: D_segment_intro — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94F4..94F5: D_segment_scratch0 — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94F6..94F7: D_segment_scratch1 — Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. */
    .word 0

/* DS:94F8..94F9: D_video_draw_segment — Сегмент текущей экранной страницы для рисования. */
    .word 0

/* DS:94FA..94FB: D_video_display_segment — Сегмент второй видеостраницы. */
    .word 0

/* DS:94FC..94FD: D_base_scroll_speed — Базовая скорость; start_game_cycle пишет только младший байт. */
    .word 0

/* DS:94FE..94FE: D_stage_counter — Число этапов, влияет на базовую скорость и очки. */
    .byte 0

/* DS:94FF..94FF: D_tunnel_mode — 0=поверхность,1=туннель. */
    .byte 0

/* DS:9500..9500: D_game_loop_end_flag — Запрос завершить game_loop. */
    .byte 0

/* DS:9501..9501: D_level_index — Индекс уровня 0..7. */
    .byte 0

/* DS:9502..9502: D_level_part — Часть уровня 0..2. */
    .byte 0

/* DS:9503..9503: D_attract_active — Маркер заставки; не подменять им состояние главного цикла. */
    .byte 0

/* DS:9504..9504: D_timer_ticks — Счётчик INT1Ch; переполнение modulo256. */
    .byte 0

/* DS:9505..9505: D_sound_toggle_state — Автомат кнопки звука0/1/2/3. */
    .byte 0

/* DS:9506..9506: D_sound_enabled — Разрешение звуков/музыки. */
    .byte 0

/* DS:9507..9508: D_last_spawn_row — Последняя обработанная строка перед камерой. */
    .word 0

/* DS:9509..9509: D_dispatch_mode_snapshot — Режим на входе в проход update_entities. */
    .byte 0

/* DS:950A..950B: D_surface_resume_x — X для создания игрока после смены режима. */
    .word 0

/* DS:950C..950D: D_surface_resume_y — Y камеры при выходе из туннеля. */
    .word 0

/* DS:950E..950E: D_map_reload_pending — Запрос rebuild после перехода между частями. */
    .byte 0

/* DS:950F..9510: D_fuel_distance_accumulator — Разные пороги расхода в surface/tunnel режимах. */
    .word 0

/* DS:9511..9512: D_ammo_use_accumulator — Отложенный расход шкалы стрельбы. */
    .word 0

/* DS:9513..9513: D_player_respawn_pending — Запрос смерти/respawn dispatcher. */
    .byte 0

/* DS:9514..9514: D_player_death_age — Таймер handler17. */
    .byte 0

/* DS:9515..9515: D_player_zero_frame_age — Срок пребывания в frame0 до смерти. */
    .byte 0

/* DS:9516..9516: D_shot_variation_index — Циклическая последовательность16 значений, не RNG. */
    .byte 0

/* DS:9517..9517: D_side_impact_mask — Занятые эффекты левого/правого ствола01h/80h. */
    .byte 0

/* DS:9518..9518: D_spawn_projectile_gate — Разрешает реальный снаряд вместе с эффектом. */
    .byte 0

/* DS:9519..9519: D_respawn_effect_pending — Создать эффект восстановления игрока. */
    .byte 0

/* DS:951A..951A: D_left_device_state — Автомат левой кнопки/устройства0..3. */
    .byte 0

/* DS:951B..951B: D_right_device_state — Автомат правой кнопки/устройства0..3. */
    .byte 0

/* DS:951C..951C: D_tunnel_flash_state — Вспышка:2→1→0. */
    .byte 0

/* DS:951D..951D: D_tunnel_reward_kind — 40 ammo,80 fuel,C0 shield,F0 переход уровня,прочее score. */
    .byte 0

/* DS:951E..951F: D_tunnel_angular_velocity — Инерция угла, clamp[-64,64]. */
    .word 0

/* DS:9520..9521: D_tunnel_passive_force — Пассивная добавка угловой скорости. */
    .word 0

/* DS:9522..9523: D_tunnel_input_force — Добавка от LEFT/RIGHT. */
    .word 0

/* DS:9524..9524: D_tunnel_exit_spawned — Выходной marker уже создан. */
    .byte 0

/* DS:9525..9526: D_tunnel_distance_remaining — Дистанция до конца для маркера. */
    .word 0

/* DS:9527..9527: D_tunnel_last_section — Последняя секция потока по distance>>6. */
    .byte 0

/* DS:9528..9529: D_camera_y — Кольцевая координата поверхности modulo1024. */
    .word 0

/* DS:952A..952B: D_scroll_speed — Скорость прокрутки/движения. */
    .word 0

/* DS:952C..952D: D_player_x_velocity — Горизонтальная инерция [-24,24] на поверхности. */
    .word 0

/* DS:952E..952E: D_collectible_0 — Collected flag0; подряд6 байтов. */
    .byte 0

/* DS:952F..952F: D_collectible_1 — Collected flag1. */
    .byte 0

/* DS:9530..9530: D_collectible_2 — Collected flag2. */
    .byte 0

/* DS:9531..9531: D_collectible_3 — Collected flag3. */
    .byte 0

/* DS:9532..9532: D_collectible_4 — Collected flag4. */
    .byte 0

/* DS:9533..9533: D_collectible_5 — Collected flag5. */
    .byte 0

/* DS:9534..9535: D_life_count — Жизни0..3 на штатных путях. */
    .word 0

/* DS:9536..9537: D_side_charge_units — Заряды боковых устройств0..6. */
    .word 0

/* DS:9538..9539: D_side_charge_delta — Накопленное изменение зарядов; применяется HUD. */
    .word 0

/* DS:953A..953B: D_fuel_units — Шкала движения0..44; имя fuel — вывод по использованию. */
    .word 0

/* DS:953C..953D: D_fuel_delta — Отложенное изменение топлива; отрицательное значит один decrement. */
    .word 0

/* DS:953E..953F: D_ammo_units — Шкала стрельбы0..48. */
    .word 0

/* DS:9540..9541: D_ammo_delta — Отложенное изменение ammo; отрицательное значит один decrement. */
    .word 0

/* DS:9542..9543: D_shield_units — Шкала урона0..56; имя shield условное. */
    .word 0

/* DS:9544..9545: D_shield_delta — Накопленное изменение прочности. */
    .word 0

/* DS:9546..9546: D_pending_name_entry — Нужно ввести имя для рекорда; выбирает музыку заставки. */
    .byte 0

/* DS:9547..9548: D_tunnel_distance — Пройденная дистанция0..896 на штатном пути. */
    .word 0

/* DS:9549..9549: D_tunnel_min_radius — Порог уменьшения колец. */
    .byte 0

/* DS:954A..954B: Сохранённые байты; формат/границы этого фрагмента ещё не установлены. */
    .byte 0x00, 0x00    # DS:954A

/* DS:954C..954D: D_control_bits — Маска управления; смысл битов см. constants.inc. */
    .word 0

/* DS:954E..954F: D_tunnel_ring_phase — distance&63 для построения bitmap. */
    .word 0

/* DS:9550..9550: D_sound_active_id — Активный sound ID;0=нет. */
    .byte 0

/* DS:9551..9551: D_sound_pending_id — Первый ожидающий эффект. */
    .byte 0

/* DS:9552..9552: D_sound_priority_id — Второй ожидающий; хранит максимальный ID из запросов. */
    .byte 0

/* DS:9553..9553: D_sound_modulation_id — 0=без модуляции; иначе индекс таблицы+1. */
    .byte 0

/* DS:9554..9554: D_sound_modulation_phase — Индекс последовательности по &15. */
    .byte 0

/* DS:9555..9556: D_sound_modulation_pointer — DS offset16 слов модуляции. */
    .word 0

/* DS:9557..9557: D_sound_ticks_left — Тики до завершения эффекта. */
    .byte 0

/* DS:9558..9559: D_sound_divisor — Основной делитель PIT2. */
    .word 0

/* DS:955A..955B: D_sound_divisor_step — Приращение делителя на тик (16-битный wrap). */
    .word 0

/* DS:955C..955D: D_sound_output_divisor — Сумма основного делителя и модуляции. */
    .word 0

/* DS:955E..955E: D_music_active — Запрещает обычные sound_trigger пока активен. */
    .byte 0

/* DS:955F..9560: D_music_cursor — Позиция в потоке команд музыки. */
    .word 0

/* DS:9561..9561: D_music_tick_divider — Шаг последовательности раз в8 IRQ-тиков. */
    .byte 0

/* DS:9562..9563: D_music_sequence_pointer — DS offset выбранной последовательности. */
    .word 0
