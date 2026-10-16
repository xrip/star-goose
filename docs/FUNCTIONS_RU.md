# Каталог входов и процедур

204 восстановленных входа. Это **не утверждение о 204 независимых авторских функциях**: есть общие хвосты, wrapper-входы, обработчики прерываний и дополнительные точки внутри логической процедуры. Имена введены при анализе. Поведение описывается по инструкциям; названия визуальных объектов остаются рабочими.

Параметры `[BP+N]` относятся к установленному стековому кадру. Отсутствие `PUSH BP` особенно важно для `plot_tunnel_line_start`: он наследует BP вызывающего `draw_tunnel_line`. `RET n` снимает аргументы callee; обычный `RET` этого не делает.

## Быстрый индекс

| CS | Имя / переход к исходнику | Назначение |
|---|---|---|
| `0010` | [entry](../src/code/01_boot_and_attract.asm#L15) | Настраивает DS и восемь сегментов буферов; проверяет память, загружает ресурсы и запускает внешний цикл игры. |
| `0096` | [exit_to_dos](../src/code/01_boot_and_attract.asm#L85) | Останавливает звук, возвращает INT 1Ch/09h, включает текстовый режим и завершает процесс. |
| `00A9` | [start_game_cycle](../src/code/01_boot_and_attract.asm#L103) | Показывает заставку/таблицу рекордов, сбрасывает состояние новой игры, готовит мир и HUD. |
| `00CF` | [game_loop](../src/code/01_boot_and_attract.asm#L126) | Один игровой кадр: ввод → обновление → поверхность или туннель → сущности → HUD → показ. |
| `00FD` | [mark_attract_seen](../src/code/01_boot_and_attract.asm#L160) | Помечает, что первая игровая сессия уже закончилась. |
| `0103` | [run_attract_sequence](../src/code/01_boot_and_attract.asm#L173) | Выполняет сценарий заставки: каждые 8 итераций читает команду, создаёт надпись/сущность или запускает ввод имени. |
| `019D` | [spawn_attract_item](../src/code/01_boot_and_attract.asm#L251) | Выбирает кадр шаблона заставки, применяет его параметры и помещает копию в пул. |
| `01AD` | [wait_attract_input](../src/code/01_boot_and_attract.asm#L268) | До 8000 раз опрашивает управление; досрочно выходит по FIRE. |
| `01C0` | [poll_controls](../src/code/02_controls.asm#L15) | Собирает маску управления, обрабатывает паузу, переназначение, выход и переключение звука, затем мышь. |
| `033B` | [redefine_keys](../src/code/02_controls.asm#L190) | Запрашивает 8 различных клавиш в порядке FIRE, LEFT, RIGHT, UP, DOWN, N, M, QUIT. |
| `0442` | [keyboard_install](../src/code/03_dos_interrupts.asm#L15) | Задаёт исходные scan-code, очищает 128 состояний и устанавливает свой INT 09h. |
| `04A5` | [keyboard_restore](../src/code/03_dos_interrupts.asm#L57) | Возвращает сохранённый INT 09h через DOS. |
| `04B7` | [keyboard_irq1](../src/code/03_dos_interrupts.asm#L77) | Читает scan-code, подтверждает IRQ и записывает make/break как 1/0. |
| `04F3` | [wait_scancode](../src/code/03_dos_interrupts.asm#L124) | Ищет нажатую клавишу, кроме F1–F4 и обоих Shift; повторяет сканирование до находки. |
| `0529` | [wait_keys_released](../src/code/03_dos_interrupts.asm#L162) | Повторяет проход по 128 байтам, пока все клавиши не отпущены. |
| `0541` | [poll_key_character](../src/code/03_dos_interrupts.asm#L186) | До 20000 проходов ищет новый scan-code и переводит его через таблицу символов. |
| `0588` | [mouse_reset](../src/code/03_dos_interrupts.asm#L234) | Сбрасывает драйвер мыши INT 33h и сохраняет признак его наличия. |
| `0591` | [timer_install](../src/code/03_dos_interrupts.asm#L249) | Сохраняет и заменяет INT 1Ch; программирует PIT0 делителем 4D0Eh. |
| `05BC` | [timer_restore_vector](../src/code/03_dos_interrupts.asm#L282) | Возвращает только вектор INT 1Ch. |
| `05CE` | [timer_int1c](../src/code/03_dos_interrupts.asm#L302) | Увеличивает 8-битный счётчик тиков и обновляет звуковой автомат. |
| `05EC` | [critical_error_install](../src/code/03_dos_interrupts.asm#L337) | Устанавливает минимальный INT 24h. |
| `05FC` | [critical_error_int24](../src/code/03_dos_interrupts.asm#L357) | Возвращает AL=0 обработчику критической ошибки DOS. |
| `0600` | [load_assets](../src/code/04_resource_io.asm#L15) | Загружает BIRD1, BIRD2, BLOX, NEWBIRD, PODZ1 и INTRO в отдельные буферы. |
| `0637` | [load_32000_bytes](../src/code/04_resource_io.asm#L47) | Обёртка чтения полного ресурса в segment:0000. |
| `0644` | [load_file_to_ds](../src/code/04_resource_io.asm#L63) | Выбирает текущий DS как сегмент назначения и читает файл. |
| `064D` | [dos_open_read_close](../src/code/04_resource_io.asm#L78) | Открывает файл, читает блок и закрывает handle. |
| `0674` | [dos_create_write_close](../src/code/04_resource_io.asm#L110) | Создаёт/обнуляет файл и записывает блок. |
| `069A` | [prepare_level_maps](../src/code/05_world_map.asm#L15) | Декодирует поток тайлов 12×64 и 64 маски размещения; рисует карту в кольцевой фон A800h. |
| `07AF` | [prepare_next_map_tile](../src/code/05_world_map.asm#L156) | Берёт следующий tile, обновляет высотный профиль строки и параметры шаблона отрисовки. |
| `07FD` | [sample_terrain](../src/code/05_world_map.asm#L196) | Ищет строку/колонку тайла, читает его микропрофиль 8×8 и суммирует перепады предыдущих тайлов. |
| `08BA` | [spawn_map_row](../src/code/05_world_map.asm#L297) | Создаёт сущности для установленных битов одной строки; учитывает маски уже изменённых объектов. |
| `094A` | [place_entity_on_map](../src/code/05_world_map.asm#L380) | Вычисляет x/y/высоту шаблона по его колонке и строке в сетке. |
| `09A4` | [reset_map_object_masks](../src/code/05_world_map.asm#L434) | Заполняет FFh маски ещё доступных объектов для трёх частей уровня. |
| `09B5` | [consume_map_object](../src/code/05_world_map.asm#L453) | Меняет сущность по replacement-таблице и сбрасывает её бит в маске строки. |
| `09E9` | [test_map_object_available](../src/code/05_world_map.asm#L486) | Проверяет бит объекта в выбранной части уровня. |
| `0A1B` | [apply_entity_replacement](../src/code/05_world_map.asm#L519) | Копирует подмножество полей из шаблона, выбранного по handler_id. |
| `0A51` | [query_player_map_collision](../src/code/05_world_map.asm#L550) | Ищет препятствие текущей строки около x игрока по маскам и ширинам из таблиц. |
| `0AD5` | [query_map_collision_at](../src/code/05_world_map.asm#L617) | Проверяет препятствие строки для переданного x, с уменьшением полуширины на 14. |
| `0B58` | [allocate_entity_copy](../src/code/06_entity_pool.asm#L15) | Ищет первый слот с очищенным битом RENDER и копирует в него ровно 23 байта. |
| `0B7C` | [clear_entities](../src/code/06_entity_pool.asm#L52) | Очищает только flags каждой записи до sentinel. |
| `0B8E` | [render_entities](../src/code/06_entity_pool.asm#L75) | Семь проходов по пулу: рисует активные записи слоя 7,6,...,1. |
| `0BC5` | [render_entity_to_a800](../src/code/06_entity_pool.asm#L114) | Рисует одну запись в карту-фон A800h, а не в экранную страницу. |
| `0BDC` | [prepare_sprite_blit](../src/code/06_entity_pool.asm#L131) | Из записи вычисляет верхний левый угол, размеры, банк изображения и прозрачный цвет. |
| `0C24` | [blit_sprite](../src/code/07_sprite_blitter.asm#L15) | Отсекает и выводит packed-4bpp sprite через EGA write mode 2; сохраняет прозрачный цвет. |
| `0D98` | [reset_attract_entities](../src/code/08_game_lifecycle.asm#L15) | Сбрасывает камеру и флаг смерти, очищает пул для заставки. |
| `0DA7` | [start_level_state](../src/code/08_game_lifecycle.asm#L30) | Сбрасывает маски объектов, готовит карту и состояние игрока. |
| `0DC0` | [advance_level](../src/code/08_game_lifecycle.asm#L48) | Увеличивает номер пройденного этапа и базовую скорость, циклически переключает level_index 1..7. |
| `0E07` | [handle_player_respawn](../src/code/08_game_lifecycle.asm#L85) | Завершает игру при нуле жизней; иначе расходует жизнь и восстанавливает игрока. |
| `0E3D` | [reset_player_motion](../src/code/08_game_lifecycle.asm#L115) | Задаёт камеру из таблицы старта, скорость и исходные счётчики игрока. |
| `0E84` | [rebuild_surface_entities](../src/code/08_game_lifecycle.asm#L144) | Пересоздаёт игрока и 15 строк объектов, затем боковые устройства и специальные состояния. |
| `0F3C` | [enter_tunnel_mode](../src/code/08_game_lifecycle.asm#L228) | Инициализирует туннель, дистанцию, угловую динамику и новый пул сущностей. |
| `0F9D` | [reset_player_history](../src/code/08_game_lifecycle.asm#L269) | Заполняет четыре сохранённых позиции/кадра/высоты текущими значениями игрока. |
| `0FDA` | [update_entities](../src/code/09_entity_dispatch.asm#L15) | Обходит пул по 23 байта и вызывает handler_id-1 для записей с UPDATE; обрабатывает смерть и смену режима. |
| `10A0` | [cull_entity_before_camera](../src/code/09_entity_dispatch.asm#L136) | Удаляет сущность, если нижний край попал в невидимую половину кольца относительно камеры. |
| `10C6` | [cull_entity_after_view](../src/code/09_entity_dispatch.asm#L166) | Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024. |
| `10F3` | [update_accelerating_effect](../src/code/09_entity_dispatch.asm#L198) | Увеличивает state_16 и двигает y назад на min(counter,8), затем проверяет видимость. |
| `1108` | [update_player](../src/code/10_player_surface.asm#L15) | Обновляет скорость, камеру, горизонтальную инерцию, рельеф, столкновения и оружие игрока. |
| `12A7` | [update_player_forced_stop](../src/code/11_entity_behaviors.asm#L15) | Уменьшает скорость на 3, при отрицательном результате запускает смерть; удерживает кадр 10. |
| `12CB` | [update_player_death](../src/code/11_entity_behaviors.asm#L41) | Замедляет прокрутку, обновляет положение и по достижении 40 тиков запрашивает respawn. |
| `12EB` | [update_player_projectile](../src/code/11_entity_behaviors.asm#L66) | Двигает выстрел по y на -16, меняет высоту, проверяет рельеф и объект строки. |
| `1384` | [update_side_device_follow](../src/code/11_entity_behaviors.asm#L157) | Следует истории игрока; при состоянии кнопки 3 переключается на автономный handler 25. |
| `13A8` | [update_ground_projectile](../src/code/11_entity_behaviors.asm#L187) | Продвигает объект на -6 по y, привязывает к рельефу, проверяет препятствия и срок жизни. |
| `1429` | [update_ground_enemy_shot](../src/code/11_entity_behaviors.asm#L259) | Двигает наземный выстрел в одном из 5 направлений и проверяет близость игрока. |
| `14F0` | [update_mobile_enemy](../src/code/11_entity_behaviors.asm#L358) | Двигает сущность на +1 по y, привязывает к рельефу, проверяет игрока и его снаряды, стреляет. |
| `15FC` | [update_enemy_projectile](../src/code/11_entity_behaviors.asm#L492) | Движение в пяти направлениях, коррекция высоты, столкновение с рельефом и игроком. |
| `16D6` | [update_countdown_effect](../src/code/11_entity_behaviors.asm#L607) | Декрементирует frame/state и удаляет объект при установленном SF. |
| `16DF` | [update_four_frame_impact](../src/code/11_entity_behaviors.asm#L623) | Проигрывает четыре sprite-offset из таблицы; затем очищает flags. |
| `16FE` | [update_five_frame_explosion](../src/code/11_entity_behaviors.asm#L648) | Выбирает frame=(++age>>2)&7; кадры 0..4, затем удаление. |
| `171F` | [update_six_frame_effect](../src/code/11_entity_behaviors.asm#L674) | Выбирает frame=(++age>>2)&7; кадры 0..5, затем удаление. |
| `1740` | [update_side_impact](../src/code/11_entity_behaviors.asm#L700) | Четырёхкадровый эффект; по окончании освобождает соответствующий бит side_impact_mask. |
| `176C` | [update_looping_anim4](../src/code/11_entity_behaviors.asm#L731) | Циклическая анимация frame=(++age>>1)&3 и culling. |
| `1783` | [update_pingpong_animation](../src/code/11_entity_behaviors.asm#L751) | Анимация через шестизначную таблицу кадров; после фазы 5 сбрасывает счётчик. |
| `17AC` | [update_aimed_emitter_a](../src/code/11_entity_behaviors.asm#L779) | Выбирает направление на игрока и вызывает emitter; перезарядка через знаковый DEC с периодом сброса 8. |
| `17D6` | [update_aimed_emitter_b](../src/code/11_entity_behaviors.asm#L807) | Целится в игрока; по окончании счётчика создаёт навесной снаряд и ставит счётчик 18. |
| `17F4` | [update_arcing_projectile](../src/code/11_entity_behaviors.asm#L830) | Движение в 5 направлениях и изменение высоты по 16-байтовой таблице; затем эффект. |
| `18A9` | [update_aimed_emitter_c](../src/code/11_entity_behaviors.asm#L925) | Целится и порождает наземный снаряд при истечении счётчика, затем сброс 40. |
| `18C7` | [update_level_exit_gate](../src/code/11_entity_behaviors.asm#L948) | Открывает ворота лишь после всех 6 collectibles, подтягивает игрока к центру и запускает переход. |
| `196F` | [begin_level_exit](../src/code/11_entity_behaviors.asm#L1033) | Если frame игрока=0, задаёт переход F0h/туннель; иначе сначала переводит игрока в frame=0. |
| `1993` | [update_tunnel_entrance_a](../src/code/11_entity_behaviors.asm#L1059) | Общий вход в туннель плюс выбор destination/тип награды из таблицы A. |
| `19C2` | [update_tunnel_entrance_b](../src/code/11_entity_behaviors.asm#L1089) | Вход в туннель по таблице B; помечает перезагрузку карты и циклически меняет part 0..2. |
| `1A04` | [update_tunnel_gate_common](../src/code/11_entity_behaviors.asm#L1127) | Открывает/закрывает створку по близости, скрывает открытый кадр, центрирует игрока и ставит tunnel_mode. |
| `1A9E` | [update_refill_gate](../src/code/11_entity_behaviors.asm#L1208) | Центрирует игрока у объекта: узкая центральная зона добавляет side_charge_delta, соседняя обнуляет shield. |
| `1B09` | [update_collectible](../src/code/11_entity_behaviors.asm#L1271) | При близости игрока очищает flags, засчитывает collectible[param] и запускает звук. |
| `1B4A` | [update_drifting_effect](../src/code/11_entity_behaviors.asm#L1314) | Сдвигает y на -2 с маской 03FFh и проверяет видимость. |
| `1B57` | [sync_player_to_terrain](../src/code/12_player_world_state.asm#L15) | Держит игрока на camera_y+112, запрашивает рельеф и применяет sprite descriptor. |
| `1B80` | [update_scroll_speed](../src/code/12_player_world_state.asm#L43) | Кнопки UP/DOWN меняют скорость; при отсутствии топлива принудительно выбирается DOWN. |
| `1BB9` | [advance_surface_scroll](../src/code/12_player_world_state.asm#L83) | Обновляет счётчик расхода топлива, camera_y modulo1024 и подгружает новую строку объектов. |
| `1C0A` | [find_map_entity](../src/code/12_player_world_state.asm#L129) | Ищет запись с флагом MAP_OBJECT и совпадающими row/column. |
| `1C33` | [begin_player_death](../src/code/12_player_world_state.asm#L161) | Создаёт взрыв, переключает игрока на handler17, скрывает слой и сбрасывает death_age. |
| `1C56` | [apply_player_map_hit](../src/code/12_player_world_state.asm#L185) | Записывает отрицательный урон и преобразует найденный объект карты в разрушенное состояние. |
| `1C78` | [push_player_history](../src/code/12_player_world_state.asm#L214) | Сдвигает четыре сохранённых x/y/frame/z и вставляет текущего игрока. |
| `1CD9` | [position_side_device](../src/code/12_player_world_state.asm#L258) | Берёт самую старую из 4 позиций, смещает на ±8 по x и +6 по y/z, применяет descriptor. |
| `1D1E` | [fire_player_weapons](../src/code/13_weapons_and_frames.asm#L15) | Создаёт пару эффектов стволов и пару выстрелов; использует циклическую таблицу вариаций. |
| `1DEF` | [fire_mobile_enemy_pattern](../src/code/13_weapons_and_frames.asm#L97) | Выбирает 1,2 или 3 точки выстрела по param и frame врага. |
| `1E7D` | [spawn_enemy_shot_at_offset](../src/code/13_weapons_and_frames.asm#L160) | Создаёт эффект и, при gate, снаряд с заданным смещением от emitter. |
| `1F0B` | [spawn_aimed_shot](../src/code/13_weapons_and_frames.asm#L230) | Создаёт эффект/снаряд в выбранном направлении, беря смещения и размеры из таблиц. |
| `1FBA` | [spawn_ground_enemy_shot](../src/code/13_weapons_and_frames.asm#L312) | Порождает handler29 из шаблона DS:10E8 и таблиц смещений. |
| `2005` | [spawn_arcing_projectile](../src/code/13_weapons_and_frames.asm#L355) | Порождает handler30 из шаблона DS:1446 и таблиц смещений. |
| `2047` | [replace_with_side_impact](../src/code/13_weapons_and_frames.asm#L395) | Заменяет выстрел эффектом, если соответствующий side_impact_mask свободен; иначе удаляет. |
| `2092` | [replace_with_impact](../src/code/13_weapons_and_frames.asm#L437) | Применяет шаблон эффекта DS:1427 к текущей записи. |
| `2099` | [replace_with_arc_effect](../src/code/13_weapons_and_frames.asm#L451) | Применяет шаблон DS:12DE к текущей записи. |
| `20A0` | [apply_entity_state_template](../src/code/13_weapons_and_frames.asm#L465) | Копирует flags, layer, age, frame, размеры, якорь, sprite и handler из DI в SI. |
| `20DB` | [destroy_map_entity](../src/code/13_weapons_and_frames.asm#L497) | Начисляет 150 очков, создаёт взрыв и помечает объект карты использованным. |
| `20F0` | [spawn_explosion_for_entity](../src/code/13_weapons_and_frames.asm#L518) | Создаёт explosion из координат текущей записи, при bit10h корректирует якорь. |
| `2121` | [apply_sprite_frame](../src/code/13_weapons_and_frames.asm#L547) | Читает count и SoA-массивы width/height/anchor_x/anchor_y, затем пары bank/offset. |
| `215B` | [aim_entity_at_player](../src/code/13_weapons_and_frames.asm#L586) | Выбирает одно из 5 направлений, либо качает направление вне зоны прицеливания. |
| `21DC` | [init_hud](../src/code/14_hud.asm#L15) | Рисует общие элементы HUD, ресурсы, счёт, collectibles и жизни. |
| `2200` | [reset_hud_resources](../src/code/14_hud.asm#L44) | Восстанавливает три шкалы и 6 зарядов боковых устройств. |
| `220D` | [update_hud](../src/code/14_hud.asm#L60) | Каждый четвёртый кадр начисляет очки за этап; применяет отложенные изменения шкал и зарядов. |
| `22B1` | [init_lives](../src/code/14_hud.asm#L161) | Задаёт 3 жизни и рисует три значка. |
| `22C1` | [add_life](../src/code/14_hud.asm#L177) | Увеличивает число жизней с максимумом 3 и дорисовывает значок. |
| `22E1` | [draw_life_1](../src/code/14_hud.asm#L201) | Рисует первый значок жизни в обеих страницах. |
| `22F9` | [draw_life_2](../src/code/14_hud.asm#L224) | Рисует второй значок жизни в обеих страницах. |
| `2311` | [draw_life_3](../src/code/14_hud.asm#L247) | Рисует третий значок жизни в обеих страницах. |
| `2329` | [remove_life](../src/code/14_hud.asm#L270) | Уменьшает life_count и стирает соответствующий значок. |
| `237C` | [init_side_charges](../src/code/14_hud.asm#L323) | Устанавливает 6 зарядов и рисует все шесть индикаторов. |
| `239B` | [add_side_charge](../src/code/14_hud.asm#L343) | Добавляет заряд до 6, проигрывает звук и рисует соответствующий индикатор. |
| `23D4` | [draw_charge_1](../src/code/14_hud.asm#L378) | Рисует индикатор заряда 1 в обеих страницах. |
| `23EC` | [draw_charge_2](../src/code/14_hud.asm#L401) | Рисует индикатор заряда 2 в обеих страницах. |
| `2404` | [draw_charge_3](../src/code/14_hud.asm#L424) | Рисует индикатор заряда 3 в обеих страницах. |
| `241C` | [draw_charge_4](../src/code/14_hud.asm#L447) | Рисует индикатор заряда 4 в обеих страницах. |
| `2434` | [draw_charge_5](../src/code/14_hud.asm#L470) | Рисует индикатор заряда 5 в обеих страницах. |
| `244C` | [draw_charge_6](../src/code/14_hud.asm#L493) | Рисует индикатор заряда 6 в обеих страницах. |
| `2464` | [remove_side_charge](../src/code/14_hud.asm#L516) | Уменьшает число зарядов до минимума 0 и стирает прежний крайний индикатор. |
| `250B` | [init_fuel_gauge](../src/code/14_hud.asm#L612) | Инициализирует шкалу, расходуемую движением (вероятно топливо), значением 44. |
| `252F` | [decrease_fuel](../src/code/14_hud.asm#L637) | Уменьшает fuel_units, если результат неотрицателен. |
| `2536` | [store_fuel_gauge](../src/code/14_hud.asm#L652) | Записывает fuel_units и обновляет столбец шкалы. |
| `2542` | [increase_fuel](../src/code/14_hud.asm#L669) | При значении<44 обновляет столбец и увеличивает fuel_units. |
| `2553` | [init_ammo_gauge](../src/code/14_hud.asm#L688) | Инициализирует шкалу, расходуемую стрельбой, значением 48. |
| `2577` | [decrease_ammo](../src/code/14_hud.asm#L713) | Уменьшает ammo_units без ухода ниже нуля. |
| `257E` | [store_ammo_gauge](../src/code/14_hud.asm#L728) | Записывает ammo_units и обновляет столбец шкалы. |
| `258A` | [increase_ammo](../src/code/14_hud.asm#L745) | При значении<48 обновляет столбец и увеличивает ammo_units. |
| `259B` | [init_shield_gauge](../src/code/14_hud.asm#L764) | Инициализирует шкалу прочности значением 56. |
| `25BF` | [decrease_shield](../src/code/14_hud.asm#L789) | Уменьшает прочность без ухода ниже нуля. |
| `25C6` | [store_shield_gauge](../src/code/14_hud.asm#L804) | Записывает shield_units и обновляет столбец шкалы. |
| `25D2` | [increase_shield](../src/code/14_hud.asm#L821) | При значении<56 обновляет столбец и увеличивает прочность. |
| `25E3` | [toggle_hud_gauge_column](../src/code/14_hud.asm#L840) | Меняет выбранный бит столбца высотой 7 строк в обеих видеостраницах с графической логической операцией. |
| `264A` | [reset_score](../src/code/15_score.asm#L15) | Очищает шесть десятичных цифр текущего счёта и вызывает отрисовку. |
| `2662` | [score_add_10](../src/code/15_score.asm#L37) | Добавляет 10 через четыре десятичных разряда. |
| `2676` | [score_add_150](../src/code/15_score.asm#L58) | Добавляет 150 через четыре десятичных разряда. |
| `268A` | [score_add_2500](../src/code/15_score.asm#L79) | Добавляет 2500 через четыре десятичных разряда. |
| `269E` | [score_add_digits](../src/code/15_score.asm#L100) | Добавляет units/tens/hundreds/thousands с переносами в массив из 6 цифр. |
| `2714` | [draw_current_score](../src/code/15_score.asm#L165) | Рисует значащие цифры текущего счёта без ведущих нулей. |
| `2762` | [load_highscores](../src/code/16_highscores.asm#L15) | Готовит начальные имена/цифры/порядок и читает 140 байт HISCORES. |
| `279F` | [insert_current_highscore](../src/code/16_highscores.asm#L54) | Сортирует 7 результатов, проверяет попадание текущего в шесть лучших, заменяет последний слот и готовит ввод имени. |
| `2809` | [sort_highscore_order](../src/code/16_highscores.asm#L114) | Выбором максимума сортирует 7 индексов по результату; использует FFh как вычеркнутый элемент. |
| `2872` | [compare_highscore_candidates](../src/code/16_highscores.asm#L176) | Лексикографически сравнивает по 6 десятичных цифр два результата. |
| `28DA` | [render_highscore_table](../src/code/16_highscores.asm#L239) | Рисует имена и результаты шести победителей в packed-буфер PODZ1. |
| `2907` | [draw_highscore_digits](../src/code/16_highscores.asm#L275) | Рисует 6 цифр выбранного результата в текстовую область, заменяя ведущие нули пустым glyph. |
| `2959` | [draw_highscore_name](../src/code/16_highscores.asm#L327) | Выбирает 12-байтовое имя по индексу и проваливается в draw_name_12_chars. |
| `296F` | [draw_name_12_chars](../src/code/16_highscores.asm#L350) | Рисует 12 символов: A–Z, пробел, точка, подчёркивание по особым glyph. |
| `29AE` | [copy_text_glyph](../src/code/16_highscores.asm#L399) | Копирует glyph 12×11 пикселей из BIRD2 в PODZ1 (6 байт на строку, stride160). |
| `29DB` | [edit_highscore_name](../src/code/16_highscores.asm#L431) | Вводит до 12 символов с backspace, перерисовывает заставку и сохраняет HISCORES. |
| `2A64` | [redraw_edited_name](../src/code/16_highscores.asm#L494) | Перерисовывает только имя выбранной строки рекордов. |
| `2A7F` | [reset_collectible_hud](../src/code/16_highscores.asm#L515) | Обнуляет 6 collectible flags и рисует исходные значки. |
| `2B28` | [collect_item_and_update_hud](../src/code/16_highscores.asm#L599) | Начисляет 150 очков, выставляет collectible[AL] и меняет его значок. |
| `2C07` | [draw_redefine_dialog](../src/code/17_ui_primitives.asm#L15) | Рисует подложку и заголовок переназначения клавиш. |
| `2C45` | [draw_quit_dialog](../src/code/17_ui_primitives.asm#L56) | Рисует диалог подтверждения выхода. |
| `2C9A` | [draw_control_prompt](../src/code/17_ui_primitives.asm#L108) | Очищает строку и рисует запрос очередной клавиши; боковые действия состоят из двух частей. |
| `2CC7` | [draw_control_prompt_piece](../src/code/17_ui_primitives.asm#L144) | По индексу берёт bank/source/destination/width/height из пяти таблиц и рисует фрагмент. |
| `2CEA` | [blit_hud_sprite_both_pages](../src/code/17_ui_primitives.asm#L170) | Выводит packed-4bpp ресурс с прозрачным цветом 2 сразу в A000 и A200 страницы. |
| `2D9A` | [fill_hud_rect_both_pages](../src/code/17_ui_primitives.asm#L279) | Заливает прямоугольник через EGA set/reset одновременно в обеих страницах. |
| `2DDC` | [select_tunnel_pattern](../src/code/18_tunnel_gameplay.asm#L15) | Циклически выбирает один из четырёх потоков туннельных объектов и сбрасывает номер предыдущей секции. |
| `2DF6` | [reset_tunnel_angle_history](../src/code/18_tunnel_gameplay.asm#L36) | Обнуляет историю трёх углов для боковых устройств. |
| `2E02` | [update_tunnel_player](../src/code/18_tunnel_gameplay.asm#L52) | Обновляет скорость/дистанцию, угловую инерцию, позицию по lookup-таблицам и боковые действия. |
| `2F50` | [update_tunnel_side_device](../src/code/18_tunnel_gameplay.asm#L210) | Следует задержанному углу; при выпуске/отключении исчезает и собирает все handler15 в пуле. |
| `2FE8` | [update_tunnel_exit_marker](../src/code/18_tunnel_gameplay.asm#L292) | Выбирает размер кольца и кадр выходного маркера по оставшейся дистанции. |
| `300A` | [update_tunnel_pickup](../src/code/18_tunnel_gameplay.asm#L315) | По дистанции от появления вычисляет масштаб/позицию объекта; при совпадении сектора игрока собирает. |
| `3059` | [collect_tunnel_pickup](../src/code/18_tunnel_gameplay.asm#L362) | Удаляет объект и начисляет награду в зависимости от tunnel_reward_kind. |
| `3084` | [tunnel_angle_force](../src/code/18_tunnel_gameplay.asm#L393) | Нелинейная добавка угловой скорости: после вычитания 5 проверяет SF, ограничивает до56, делит на2, возвращает -(x+3). |
| `309D` | [advance_tunnel_distance](../src/code/18_tunnel_gameplay.asm#L421) | Продвигает дистанцию, расходует топливо, создаёт объекты по секциям и возвращает на поверхность после 896. |
| `3130` | [position_tunnel_pickup](../src/code/18_tunnel_gameplay.asm#L492) | Берёт радиус по дистанции и выбирает один из 8 лучей вокруг центра. |
| `315E` | [tunnel_set_north](../src/code/18_tunnel_gameplay.asm#L525) | y=106-radius; также содержит диагональный хвост north-east. |
| `3170` | [tunnel_set_east](../src/code/18_tunnel_gameplay.asm#L546) | x=128+radius; далее расположен хвост south-east. |
| `3181` | [tunnel_set_south](../src/code/18_tunnel_gameplay.asm#L566) | y=106+radius; далее расположен хвост south-west. |
| `3191` | [tunnel_set_west](../src/code/18_tunnel_gameplay.asm#L586) | x=128-radius; далее расположен хвост north-west. |
| `31A4` | [present_tunnel_bitmap](../src/code/19_ega_video.asm#L15) | Копирует 256×160 участок 1bpp туннеля в draw page с выбором EGA-цветовых плоскостей. |
| `31E7` | [tunnel_plane_color](../src/code/19_ega_video.asm#L59) | Выбирает маску/цвет туннеля по типу награды. |
| `3209` | [tunnel_flash_color](../src/code/19_ega_video.asm#L89) | Выдаёт цвет вспышки и переводит двухшаговое состояние к нулю. |
| `322C` | [unused_planar_blit](../src/code/19_ega_video.asm#L116) | Сохранённый, статически недостижимый blit четырёх плоскостей с шагом источника 8000. |
| `3291` | [copy_playfield_between_pages](../src/code/19_ega_video.asm#L182) | Копирует видимую область 256×160 между EGA-сегментами через latch copy. |
| `32C8` | [unused_rectangle_fill](../src/code/19_ega_video.asm#L220) | Сохранённая недостижимая заливка прямоугольника draw page. |
| `32FE` | [draw_scrolling_background](../src/code/19_ega_video.asm#L259) | Копирует 160 строк из кольцевого фона A800h, начиная с camera_y*32, на draw page. |
| `333C` | [video_init_mode0d](../src/code/19_ega_video.asm#L300) | Включает режим 0Dh, задаёт A000/A200 страницы и начальный CRTC start. |
| `3383` | [present_frame](../src/code/19_ega_video.asm#L342) | Меняет местами draw/display сегменты, пишет CRTC start high и ждёт retrace. |
| `33B3` | [clear_draw_page](../src/code/19_ega_video.asm#L377) | Заливает текущую страницу указанным EGA-цветом. |
| `33D7` | [wait_retrace_cycle](../src/code/19_ega_video.asm#L407) | Ждёт установки и затем снятия бита3 порта3DAh. |
| `33E5` | [blit_planar_fullscreen](../src/code/19_ega_video.asm#L430) | Выводит 4 плоскости по 8000 байт из последовательного буфера. |
| `3423` | [init_playfield_pages](../src/code/19_ega_video.asm#L476) | Очищает обе видеостраницы и рисует рамку. |
| `3439` | [draw_playfield_border](../src/code/19_ega_video.asm#L495) | Рисует рамку вокруг игрового поля прямыми записями в video RAM. |
| `3472` | [prepare_attract_background](../src/code/19_ega_video.asm#L530) | Выводит INTRO в обе страницы и сохраняет область игры в A400h. |
| `3498` | [restore_attract_background](../src/code/19_ega_video.asm#L555) | Восстанавливает playfield из A400h в текущую draw page. |
| `34A8` | [build_tunnel_bitmap](../src/code/20_tunnel_renderer.asm#L15) | Очищает 1bpp scratch, рисует диагонали и последовательно уменьшаемые восьмиугольники по дистанции. |
| `35BC` | [draw_nested_tunnel_rings](../src/code/20_tunnel_renderer.asm#L145) | Рисует 8 сторон кольца около центра127/128; делит радиусы пополам до порога. |
| `36D4` | [draw_tunnel_line](../src/code/20_tunnel_renderer.asm#L275) | Целочисленная линия с accumulator; масштабирует y как (10*y+5)/12 и рисует включительно оба конца. |
| `3801` | [plot_tunnel_line_start](../src/code/20_tunnel_renderer.asm#L425) | Вычисляет offset=y1*32+x1/8, mask=80h>>(x1&7), рисует первый пиксель. |
| `382A` | [sound_init](../src/code/21_audio.asm#L15) | Очищает активный/два ожидающих звука, включает звук и сбрасывает toggle latch. |
| `3840` | [sound_trigger](../src/code/21_audio.asm#L35) | Запрашивает эффект: стартует сразу либо помещает в двухступенчатую очередь; последняя ячейка хранит максимум ID. |
| `387D` | [sound_tick](../src/code/21_audio.asm#L76) | Шаг музыки/эффекта: время, очередь, изменение делителя, модуляция и запись PIT2. |
| `3914` | [start_sound_descriptor](../src/code/21_audio.asm#L152) | Загружает 6-байтовый descriptor, подставляет динамические параметры двух эффектов и включает PIT2/speaker. |
| `39A3` | [sound_stop](../src/code/21_audio.asm#L226) | Очищает очередь/время эффекта и отключает speaker gate bits0/1. |
| `39B8` | [music_start](../src/code/21_audio.asm#L246) | Запускает одну из двух последовательностей, выбирая её по pending_name_entry. |
| `39DB` | [music_tick](../src/code/21_audio.asm#L270) | Каждые 8 звуковых тиков читает команду: FF конец, FE пауза, 0 специальное заполнение, прочее транспонируется. |
| `3A3B` | [music_play_note](../src/code/21_audio.asm#L324) | Берёт PIT-делитель по note-1, сохраняет его в descriptor15 и стартует этот эффект. |

## Контракты

### 0010 — `entry`

Настраивает DS и восемь сегментов буферов; проверяет память, загружает ресурсы и запускает внешний цикл игры.

**Вход:** MZ entry, не обычный near CALL.

**Результат и оговорки:** Не возвращается; последовательность инициализации и повторного старта сохранена.

**Прямые CALL в физическом диапазоне:** `load_assets`, `video_init_mode0d`, `load_highscores`, `critical_error_install`, `keyboard_install`, `mouse_reset`, `timer_install`, `sound_init`, `start_game_cycle`, `game_loop`, `mark_attract_seen`.

**Возвраты в физическом диапазоне:** нет.

[Исходник, строка 15](../src/code/01_boot_and_attract.asm#L15)

### 0096 — `exit_to_dos`

Останавливает звук, возвращает INT 1Ch/09h, включает текстовый режим и завершает процесс.

**Вход:** Вызывается переходом, не обязательно CALL.

**Результат и оговорки:** Невозвратная процедура; PIT и INT 24h здесь явно не восстанавливаются.

**Прямые CALL в физическом диапазоне:** `sound_stop`, `timer_restore_vector`, `keyboard_restore`.

**Возвраты в физическом диапазоне:** нет.

[Исходник, строка 85](../src/code/01_boot_and_attract.asm#L85)

### 00A9 — `start_game_cycle`

Показывает заставку/таблицу рекордов, сбрасывает состояние новой игры, готовит мир и HUD.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Не обнуляет level_index; нельзя добавлять такой сброс при буквальном переносе.

**Прямые CALL в физическом диапазоне:** `run_attract_sequence`, `start_level_state`, `init_playfield_pages`, `init_hud`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 103](../src/code/01_boot_and_attract.asm#L103)

### 00CF — `game_loop`

Один игровой кадр: ввод → обновление → поверхность или туннель → сущности → HUD → показ.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Повторяет кадры до game_loop_end_flag. Порядок вызовов — часть поведения.

**Прямые CALL в физическом диапазоне:** `poll_controls`, `update_entities`, `draw_scrolling_background`, `render_entities`, `build_tunnel_bitmap`, `present_tunnel_bitmap`, `update_hud`, `present_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 126](../src/code/01_boot_and_attract.asm#L126)

### 00FD — `mark_attract_seen`

Помечает, что первая игровая сессия уже закончилась.

**Вход:** Нет аргументов.

**Результат и оговорки:** attract_seen = 1.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 160](../src/code/01_boot_and_attract.asm#L160)

### 0103 — `run_attract_sequence`

Выполняет сценарий заставки: каждые 8 итераций читает команду, создаёт надпись/сущность или запускает ввод имени.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** 0 = пауза; FF = конец; FE = ввод имени/ожидание; бит 7 = пара последовательных кадров.

**Прямые CALL в физическом диапазоне:** `insert_current_highscore`, `render_highscore_table`, `music_start`, `prepare_attract_background`, `reset_attract_entities`, `poll_controls`, `update_entities`, `restore_attract_background`, `render_entities`, `present_frame`, `edit_highscore_name`, `wait_attract_input`, `spawn_attract_item`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 173](../src/code/01_boot_and_attract.asm#L173)

### 019D — `spawn_attract_item`

Выбирает кадр шаблона заставки, применяет его параметры и помещает копию в пул.

**Вход:** AL = номер кадра; шаблон DS:0109.

**Результат и оговорки:** SI = новый слот либо 0; состояние шаблона изменяется.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `allocate_entity_copy`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 251](../src/code/01_boot_and_attract.asm#L251)

### 01AD — `wait_attract_input`

До 8000 раз опрашивает управление; досрочно выходит по FIRE.

**Вход:** Нет аргументов.

**Результат и оговорки:** Это задержка числом опросов, не таймер в миллисекундах.

**Прямые CALL в физическом диапазоне:** `poll_controls`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 268](../src/code/01_boot_and_attract.asm#L268)

### 01C0 — `poll_controls`

Собирает маску управления, обрабатывает паузу, переназначение, выход и переключение звука, затем мышь.

**Вход:** Таблица клавиш и состояние мыши.

**Результат и оговорки:** control_bits; LEFT имеет приоритет над RIGHT, UP над DOWN. Движение мыши учитывается только без клавиатурного направления.

**Прямые CALL в физическом диапазоне:** `redefine_keys`, `draw_quit_dialog`, `wait_keys_released`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/02_controls.asm#L15)

### 033B — `redefine_keys`

Запрашивает 8 различных клавиш в порядке FIRE, LEFT, RIGHT, UP, DOWN, N, M, QUIT.

**Вход:** Блокирующий интерфейс, DS = сегмент игры.

**Результат и оговорки:** Записывает scan-code в таблицу настроек; дубли отклоняются.

**Прямые CALL в физическом диапазоне:** `draw_redefine_dialog`, `draw_control_prompt`, `wait_scancode`, `wait_keys_released`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 190](../src/code/02_controls.asm#L190)

### 0442 — `keyboard_install`

Задаёт исходные scan-code, очищает 128 состояний и устанавливает свой INT 09h.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Сохраняет старый вектор; использует REP STOSW без локального CLD.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/03_dos_interrupts.asm#L15)

### 04A5 — `keyboard_restore`

Возвращает сохранённый INT 09h через DOS.

**Вход:** Сохранённые offset/segment.

**Результат и оговорки:** DS восстанавливается после временной подстановки сегмента обработчика.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 57](../src/code/03_dos_interrupts.asm#L57)

### 04B7 — `keyboard_irq1`

Читает scan-code, подтверждает IRQ и записывает make/break как 1/0.

**Вход:** Аппаратный вход INT 09h.

**Результат и оговорки:** IRET; AX/BX/DS сохранены. Расширенные префиксы не декодируются в отдельные клавиши.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `iret`.

[Исходник, строка 77](../src/code/03_dos_interrupts.asm#L77)

### 04F3 — `wait_scancode`

Ищет нажатую клавишу, кроме F1–F4 и обоих Shift; повторяет сканирование до находки.

**Вход:** Нет аргументов.

**Результат и оговорки:** AX = scan-code. Сам по себе не ждёт отпускания предыдущей клавиши.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 124](../src/code/03_dos_interrupts.asm#L124)

### 0529 — `wait_keys_released`

Повторяет проход по 128 байтам, пока все клавиши не отпущены.

**Вход:** DS = сегмент игры; ожидается DF = 0.

**Результат и оговорки:** Блокирующий busy loop.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 162](../src/code/03_dos_interrupts.asm#L162)

### 0541 — `poll_key_character`

До 20000 проходов ищет новый scan-code и переводит его через таблицу символов.

**Вход:** last_key_pointer подавляет повтор того же нажатия.

**Результат и оговорки:** AL = символ либо 0. Это ограниченное ожидание, а не мгновенный poll.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 186](../src/code/03_dos_interrupts.asm#L186)

### 0588 — `mouse_reset`

Сбрасывает драйвер мыши INT 33h и сохраняет признак его наличия.

**Вход:** Нет аргументов.

**Результат и оговорки:** mouse_present = AL результата AX=0/INT 33h.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 234](../src/code/03_dos_interrupts.asm#L234)

### 0591 — `timer_install`

Сохраняет и заменяет INT 1Ch; программирует PIT0 делителем 4D0Eh.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Звук обновляется из обработчика, а не из главного игрового цикла.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 249](../src/code/03_dos_interrupts.asm#L249)

### 05BC — `timer_restore_vector`

Возвращает только вектор INT 1Ch.

**Вход:** Сохранённые offset/segment.

**Результат и оговорки:** Делитель PIT в этой процедуре не восстанавливается.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 282](../src/code/03_dos_interrupts.asm#L282)

### 05CE — `timer_int1c`

Увеличивает 8-битный счётчик тиков и обновляет звуковой автомат.

**Вход:** Вход прерывания; свой DS устанавливается явно.

**Результат и оговорки:** IRET; tick переполняется по модулю 256. Общие регистры и DS сохранены, ES явно не сохраняется.

**Прямые CALL в физическом диапазоне:** `sound_tick`.

**Возвраты в физическом диапазоне:** `iret`.

[Исходник, строка 302](../src/code/03_dos_interrupts.asm#L302)

### 05EC — `critical_error_install`

Устанавливает минимальный INT 24h.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Старый вектор здесь не сохраняется.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 337](../src/code/03_dos_interrupts.asm#L337)

### 05FC — `critical_error_int24`

Возвращает AL=0 обработчику критической ошибки DOS.

**Вход:** Вход прерывания.

**Результат и оговорки:** IRET; не обычный RET и не логическая функция игры.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `iret`.

[Исходник, строка 357](../src/code/03_dos_interrupts.asm#L357)

### 0600 — `load_assets`

Загружает BIRD1, BIRD2, BLOX, NEWBIRD, PODZ1 и INTRO в отдельные буферы.

**Вход:** Сегменты буферов уже назначены entry.

**Результат и оговорки:** Шесть чтений по 32000 байт. Формат использования не одинаков у всех .X.

**Прямые CALL в физическом диапазоне:** `load_32000_bytes`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/04_resource_io.asm#L15)

### 0637 — `load_32000_bytes`

Обёртка чтения полного ресурса в segment:0000.

**Вход:** AX = сегмент назначения; DS:DX = имя.

**Результат и оговорки:** DI=0, CX=32000; затем файловый reader.

**Прямые CALL в физическом диапазоне:** `dos_open_read_close`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 47](../src/code/04_resource_io.asm#L47)

### 0644 — `load_file_to_ds`

Выбирает текущий DS как сегмент назначения и читает файл.

**Вход:** DS:DX = имя; DI = адрес назначения; CX = длина.

**Результат и оговорки:** Используется для HISCORES.

**Прямые CALL в физическом диапазоне:** `dos_open_read_close`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 63](../src/code/04_resource_io.asm#L63)

### 064D — `dos_open_read_close`

Открывает файл, читает блок и закрывает handle.

**Вход:** DS:DX = имя; DI = offset; CX = count; file_destination_segment = сегмент.

**Результат и оговорки:** Ошибка open возвращается; результат read не проверяется до close. Не считать AX надёжным числом прочитанных байтов.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 78](../src/code/04_resource_io.asm#L78)

### 0674 — `dos_create_write_close`

Создаёт/обнуляет файл и записывает блок.

**Вход:** DS:DX = имя; DS:DI = данные; CX = длина.

**Результат и оговорки:** При ошибке write выходит без close; сохранено, а не исправлено.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 110](../src/code/04_resource_io.asm#L110)

### 069A — `prepare_level_maps`

Декодирует поток тайлов 12×64 и 64 маски размещения; рисует карту в кольцевой фон A800h.

**Вход:** level_index, level_part; RLE-таблицы в DS.

**Результат и оговорки:** RLE: нижние 6 бит = tile, верхние 2 = длина минус 1. Ровно 768 выходных байтов.

**Прямые CALL в физическом диапазоне:** `prepare_next_map_tile`, `render_entity_to_a800`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/05_world_map.asm#L15)

### 07AF — `prepare_next_map_tile`

Берёт следующий tile, обновляет высотный профиль строки и параметры шаблона отрисовки.

**Вход:** map_tile_cursor, map_row_y, map_running_height.

**Результат и оговорки:** Изменяет шаблон DS:0274; bank=2 или 4, sprite offset берётся из таблицы.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 156](../src/code/05_world_map.asm#L156)

### 07FD — `sample_terrain`

Ищет строку/колонку тайла, читает его микропрофиль 8×8 и суммирует перепады предыдущих тайлов.

**Вход:** Стек: [BP+4]=world_y, [BP+6]=x.

**Результат и оговорки:** AX = знаковая высота; BX = variant из одной из двух таблиц; CX = world_y/16. RET 4.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 196](../src/code/05_world_map.asm#L196)

### 08BA — `spawn_map_row`

Создаёт сущности для установленных битов одной строки; учитывает маски уже изменённых объектов.

**Вход:** Стек: [BP+4]=строка 0..63.

**Результат и оговорки:** Поиск первых свободных слотов; для уже использованных объектов применяется иной шаблон состояния. RET 2.

**Прямые CALL в физическом диапазоне:** `place_entity_on_map`, `test_map_object_available`, `allocate_entity_copy`, `apply_entity_replacement`.

**Возвраты в физическом диапазоне:** `ret 0x2`.

[Исходник, строка 297](../src/code/05_world_map.asm#L297)

### 094A — `place_entity_on_map`

Вычисляет x/y/высоту шаблона по его колонке и строке в сетке.

**Вход:** DS:SI = запись сущности; state_13=колонка, state_14=строка.

**Результат и оговорки:** x=24*column-4; y=16*row; высота — сумма перепадов предшествующих тайлов.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 380](../src/code/05_world_map.asm#L380)

### 09A4 — `reset_map_object_masks`

Заполняет FFh маски ещё доступных объектов для трёх частей уровня.

**Вход:** Нет аргументов.

**Результат и оговорки:** 192 слова = 3×64 строк; ES=DS, DF=0.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 434](../src/code/05_world_map.asm#L434)

### 09B5 — `consume_map_object`

Меняет сущность по replacement-таблице и сбрасывает её бит в маске строки.

**Вход:** [BP+4]=row; [BP+6]=column; [BP+8]=entity offset.

**Результат и оговорки:** Выбирает банк масок по level_part; RET 6.

**Прямые CALL в физическом диапазоне:** `apply_entity_replacement`.

**Возвраты в физическом диапазоне:** `ret 0x6`.

[Исходник, строка 453](../src/code/05_world_map.asm#L453)

### 09E9 — `test_map_object_available`

Проверяет бит объекта в выбранной части уровня.

**Вход:** [BP+4]=row; [BP+6]=column.

**Результат и оговорки:** AX = исходный бит, не нормализованный bool; RET 4.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 486](../src/code/05_world_map.asm#L486)

### 0A1B — `apply_entity_replacement`

Копирует подмножество полей из шаблона, выбранного по handler_id.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Меняет flags, размеры/якорь, sprite offset и handler_id; координаты сохраняет.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 519](../src/code/05_world_map.asm#L519)

### 0A51 — `query_player_map_collision`

Ищет препятствие текущей строки около x игрока по маскам и ширинам из таблиц.

**Вход:** player_map_row, player_x, level_part.

**Результат и оговорки:** AX=код/величина столкновения либо 0; BX=колонка при попадании.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 550](../src/code/05_world_map.asm#L550)

### 0AD5 — `query_map_collision_at`

Проверяет препятствие строки для переданного x, с уменьшением полуширины на 14.

**Вход:** [BP+4]=row; [BP+6]=x.

**Результат и оговорки:** AX=0/1, BX=колонка при попадании; RET 4.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 617](../src/code/05_world_map.asm#L617)

### 0B58 — `allocate_entity_copy`

Ищет первый слот с очищенным битом RENDER и копирует в него ровно 23 байта.

**Вход:** DS:SI = шаблон записи.

**Результат и оговорки:** SI=адрес копии либо 0. ES=сегмент игры; DI и CX изменяются. Не проверяет flags==0.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/06_entity_pool.asm#L15)

### 0B7C — `clear_entities`

Очищает только flags каждой записи до sentinel.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Остальные 22 байта слота остаются прежними; не заменять memset всего массива без проверки.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 52](../src/code/06_entity_pool.asm#L52)

### 0B8E — `render_entities`

Семь проходов по пулу: рисует активные записи слоя 7,6,...,1.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Слой 0 не рисуется; порядок внутри слоя — порядок слотов. Stride=40, clipping включён.

**Прямые CALL в физическом диапазоне:** `prepare_sprite_blit`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 75](../src/code/06_entity_pool.asm#L75)

### 0BC5 — `render_entity_to_a800`

Рисует одну запись в карту-фон A800h, а не в экранную страницу.

**Вход:** DS:SI = сущность/шаблон.

**Результат и оговорки:** Stride=32, экранное отсечение отключено.

**Прямые CALL в физическом диапазоне:** `prepare_sprite_blit`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 114](../src/code/06_entity_pool.asm#L114)

### 0BDC — `prepare_sprite_blit`

Из записи вычисляет верхний левый угол, размеры, банк изображения и прозрачный цвет.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** x'=x-signed(anchor_x); y'=y-signed8(z+anchor_y). Передаёт четыре слова через стек.

**Прямые CALL в физическом диапазоне:** `blit_sprite`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 131](../src/code/06_entity_pool.asm#L131)

### 0C24 — `blit_sprite`

Отсекает и выводит packed-4bpp sprite через EGA write mode 2; сохраняет прозрачный цвет.

**Вход:** [BP+4]=height, +6=width, +8=y, +10=x; BX=segment; SI=source offset.

**Результат и оговорки:** RET 8; DS восстанавливается. Горизонтальная граница сдвигает x целиком, не обрезает источник; ширина обрабатывается парами пикселей.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`, `ret 0x8`.

[Исходник, строка 15](../src/code/07_sprite_blitter.asm#L15)

### 0D98 — `reset_attract_entities`

Сбрасывает камеру и флаг смерти, очищает пул для заставки.

**Вход:** Нет аргументов.

**Результат и оговорки:** camera_y=26; не загружает новый уровень.

**Прямые CALL в физическом диапазоне:** `clear_entities`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/08_game_lifecycle.asm#L15)

### 0DA7 — `start_level_state`

Сбрасывает маски объектов, готовит карту и состояние игрока.

**Вход:** level_index/part уже выбраны.

**Результат и оговорки:** Обнуляет запросы восстановления/боковых устройств.

**Прямые CALL в физическом диапазоне:** `reset_map_object_masks`, `prepare_level_maps`, `reset_player_motion`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 30](../src/code/08_game_lifecycle.asm#L30)

### 0DC0 — `advance_level`

Увеличивает номер пройденного этапа и базовую скорость, циклически переключает level_index 1..7.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Сброс масок, новая карта, игрок, шесть collectible-индикаторов.

**Прямые CALL в физическом диапазоне:** `reset_map_object_masks`, `prepare_level_maps`, `reset_player_motion`, `reset_collectible_hud`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 48](../src/code/08_game_lifecycle.asm#L48)

### 0E07 — `handle_player_respawn`

Завершает игру при нуле жизней; иначе расходует жизнь и восстанавливает игрока.

**Вход:** player_respawn_pending != 0.

**Результат и оговорки:** При возврате к part 0 заново готовит карту и HUD-ресурсы.

**Прямые CALL в физическом диапазоне:** `remove_life`, `prepare_level_maps`, `reset_hud_resources`, `reset_player_motion`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 85](../src/code/08_game_lifecycle.asm#L85)

### 0E3D — `reset_player_motion`

Задаёт камеру из таблицы старта, скорость и исходные счётчики игрока.

**Вход:** level_index, base_scroll_speed.

**Результат и оговорки:** Затем заново формирует пул видимой области.

**Прямые CALL в физическом диапазоне:** `rebuild_surface_entities`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 115](../src/code/08_game_lifecycle.asm#L115)

### 0E84 — `rebuild_surface_entities`

Пересоздаёт игрока и 15 строк объектов, затем боковые устройства и специальные состояния.

**Вход:** camera_y, respawn/side flags.

**Результат и оговорки:** Пул очищается; игрок занимает первый слот. Некоторые маршруты возвращаются раньше общей инициализации.

**Прямые CALL в физическом диапазоне:** `prepare_level_maps`, `clear_entities`, `allocate_entity_copy`, `sync_player_to_terrain`, `spawn_map_row`, `apply_sprite_frame`, `reset_player_history`, `position_side_device`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 144](../src/code/08_game_lifecycle.asm#L144)

### 0F3C — `enter_tunnel_mode`

Инициализирует туннель, дистанцию, угловую динамику и новый пул сущностей.

**Вход:** tunnel mode выбран вызывающей стороной.

**Результат и оговорки:** Использует другие шаблоны игрока и боковых устройств.

**Прямые CALL в физическом диапазоне:** `select_tunnel_pattern`, `clear_entities`, `allocate_entity_copy`, `reset_tunnel_angle_history`, `update_tunnel_side_device`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 228](../src/code/08_game_lifecycle.asm#L228)

### 0F9D — `reset_player_history`

Заполняет четыре сохранённых позиции/кадра/высоты текущими значениями игрока.

**Вход:** Глобальная запись player.

**Результат и оговорки:** История используется боковыми устройствами с задержкой.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 269](../src/code/08_game_lifecycle.asm#L269)

### 0FDA — `update_entities`

Обходит пул по 23 байта и вызывает handler_id-1 для записей с UPDATE; обрабатывает смерть и смену режима.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** SI сохраняется вокруг handler. После смены режима обход начинается заново; handler_id=0 пропускается.

**Прямые CALL в физическом диапазоне:** `handle_player_respawn`, `sound_trigger`, `enter_tunnel_mode`, `advance_level`, `rebuild_surface_entities`, `WORD PTR cs:[bx+CS_entity_handlers]`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/09_entity_dispatch.asm#L15)

### 10A0 — `cull_entity_before_camera`

Удаляет сущность, если нижний край попал в невидимую половину кольца относительно камеры.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Работает с кольцом 1024 и порогом 512, а не с простым signed y<0.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 136](../src/code/09_entity_dispatch.asm#L136)

### 10C6 — `cull_entity_after_view`

Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Общий обработчик ID 4,9,13,18; это не четыре разных алгоритма.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 166](../src/code/09_entity_dispatch.asm#L166)

### 10F3 — `update_accelerating_effect`

Увеличивает state_16 и двигает y назад на min(counter,8), затем проверяет видимость.

**Вход:** DS:SI = сущность; handler ID 27.

**Результат и оговорки:** Счётчик байтовый, проверка unsigned; его wrap не заменять бесконечным saturating счётчиком.

**Прямые CALL в физическом диапазоне:** `cull_entity_before_camera`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 198](../src/code/09_entity_dispatch.asm#L198)

### 1108 — `update_player`

Обновляет скорость, камеру, горизонтальную инерцию, рельеф, столкновения и оружие игрока.

**Вход:** Глобальная запись player; маска управления.

**Результат и оговорки:** Инерция ограничена [-24,24]; dx=(v+3) SAR 2; x ограничен [20,236]. Боковые действия — автоматы 0/1/2/3.

**Прямые CALL в физическом диапазоне:** `update_scroll_speed`, `advance_surface_scroll`, `sync_player_to_terrain`, `push_player_history`, `query_player_map_collision`, `apply_player_map_hit`, `begin_player_death`, `fire_player_weapons`, `position_side_device`, `allocate_entity_copy`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/10_player_surface.asm#L15)

### 12A7 — `update_player_forced_stop`

Уменьшает скорость на 3, при отрицательном результате запускает смерть; удерживает кадр 10.

**Вход:** Глобальная запись игрока; handler ID 35.

**Результат и оговорки:** Не обычная физика игрока; отдельное состояние после перехода.

**Прямые CALL в физическом диапазоне:** `begin_player_death`, `advance_surface_scroll`, `sync_player_to_terrain`, `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/11_entity_behaviors.asm#L15)

### 12CB — `update_player_death`

Замедляет прокрутку, обновляет положение и по достижении 40 тиков запрашивает respawn.

**Вход:** Глобальные счётчики; handler ID 17.

**Результат и оговорки:** Смерть не означает немедленное удаление первого слота.

**Прямые CALL в физическом диапазоне:** `advance_surface_scroll`, `sync_player_to_terrain`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 41](../src/code/11_entity_behaviors.asm#L41)

### 12EB — `update_player_projectile`

Двигает выстрел по y на -16, меняет высоту, проверяет рельеф и объект строки.

**Вход:** DS:SI = сущность; handler ID 19.

**Результат и оговорки:** Может изменить цель, счёт, звук и заменить выстрел эффектом.

**Прямые CALL в физическом диапазоне:** `cull_entity_before_camera`, `sample_terrain`, `replace_with_impact`, `query_map_collision_at`, `find_map_entity`, `destroy_map_entity`, `sound_trigger`, `score_add_10`, `replace_with_side_impact`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 66](../src/code/11_entity_behaviors.asm#L66)

### 1384 — `update_side_device_follow`

Следует истории игрока; при состоянии кнопки 3 переключается на автономный handler 25.

**Вход:** DS:SI = сущность; handler ID 24.

**Результат и оговорки:** Бит 20h выбирает левое/правое устройство; при смерти игрока отключается.

**Прямые CALL в физическом диапазоне:** `position_side_device`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 157](../src/code/11_entity_behaviors.asm#L157)

### 13A8 — `update_ground_projectile`

Продвигает объект на -6 по y, привязывает к рельефу, проверяет препятствия и срок жизни.

**Вход:** DS:SI = сущность; handler ID 25.

**Результат и оговорки:** В состоянии frame=0 корректирует y обратно на +4 и уменьшает state_15.

**Прямые CALL в физическом диапазоне:** `cull_entity_before_camera`, `sample_terrain`, `apply_sprite_frame`, `spawn_explosion_for_entity`, `query_map_collision_at`, `find_map_entity`, `destroy_map_entity`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 187](../src/code/11_entity_behaviors.asm#L187)

### 1429 — `update_ground_enemy_shot`

Двигает наземный выстрел в одном из 5 направлений и проверяет близость игрока.

**Вход:** DS:SI = сущность; handler ID 29.

**Результат и оговорки:** Направление в param; y modulo 1024; попадание даёт shield_delta -=100.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`, `sample_terrain`, `apply_sprite_frame`, `sound_trigger`, `spawn_explosion_for_entity`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 259](../src/code/11_entity_behaviors.asm#L259)

### 14F0 — `update_mobile_enemy`

Двигает сущность на +1 по y, привязывает к рельефу, проверяет игрока и его снаряды, стреляет.

**Вход:** DS:SI = сущность; handler ID 26.

**Результат и оговорки:** state_15 — знаковый счётчик попаданий; bit40h других сущностей определяет кандидатов столкновения.

**Прямые CALL в физическом диапазоне:** `sample_terrain`, `apply_sprite_frame`, `sound_trigger`, `spawn_explosion_for_entity`, `score_add_150`, `replace_with_impact`, `score_add_10`, `fire_mobile_enemy_pattern`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 358](../src/code/11_entity_behaviors.asm#L358)

### 15FC — `update_enemy_projectile`

Движение в пяти направлениях, коррекция высоты, столкновение с рельефом и игроком.

**Вход:** DS:SI = сущность; handler ID 20.

**Результат и оговорки:** При попадании shield_delta -=1; направления используют разные dx/dy, не тригонометрию.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`, `sample_terrain`, `replace_with_impact`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 492](../src/code/11_entity_behaviors.asm#L492)

### 16D6 — `update_countdown_effect`

Декрементирует frame/state и удаляет объект при установленном SF.

**Вход:** DS:SI = сущность; handler ID 22.

**Результат и оговорки:** Знаковый 8-битный результат после DEC; не unsigned frame<0.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 607](../src/code/11_entity_behaviors.asm#L607)

### 16DF — `update_four_frame_impact`

Проигрывает четыре sprite-offset из таблицы; затем очищает flags.

**Вход:** DS:SI = сущность; handler ID 21.

**Результат и оговорки:** Сравнивает старое frame, инкремент сделан до проверки.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 623](../src/code/11_entity_behaviors.asm#L623)

### 16FE — `update_five_frame_explosion`

Выбирает frame=(++age>>2)&7; кадры 0..4, затем удаление.

**Вход:** DS:SI = сущность; handler ID 16.

**Результат и оговорки:** Применяет descriptor DS:12B5 и culling.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 648](../src/code/11_entity_behaviors.asm#L648)

### 171F — `update_six_frame_effect`

Выбирает frame=(++age>>2)&7; кадры 0..5, затем удаление.

**Вход:** DS:SI = сущность; handler ID 31.

**Результат и оговорки:** Применяет descriptor DS:12F5 и culling.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 674](../src/code/11_entity_behaviors.asm#L674)

### 1740 — `update_side_impact`

Четырёхкадровый эффект; по окончании освобождает соответствующий бит side_impact_mask.

**Вход:** DS:SI = сущность; handler ID 23.

**Результат и оговорки:** Бит стороны 20h; mask использует 01h/80h.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 700](../src/code/11_entity_behaviors.asm#L700)

### 176C — `update_looping_anim4`

Циклическая анимация frame=(++age>>1)&3 и culling.

**Вход:** DS:SI = сущность; handler ID 2.

**Результат и оговорки:** Descriptor DS:0E2B; игровое имя объекта не установлено.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 731](../src/code/11_entity_behaviors.asm#L731)

### 1783 — `update_pingpong_animation`

Анимация через шестизначную таблицу кадров; после фазы 5 сбрасывает счётчик.

**Вход:** DS:SI = сущность; handler ID 3.

**Результат и оговорки:** Descriptor DS:0E52; визуальная роль отдельно не подтверждена.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 751](../src/code/11_entity_behaviors.asm#L751)

### 17AC — `update_aimed_emitter_a`

Выбирает направление на игрока и вызывает emitter; перезарядка через знаковый DEC с периодом сброса 8.

**Вход:** DS:SI = сущность; handler ID 5.

**Результат и оговорки:** spawn_projectile_gate включает настоящий снаряд только на выбранных кадрах; эффекты создаются чаще.

**Прямые CALL в физическом диапазоне:** `aim_entity_at_player`, `spawn_aimed_shot`, `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 779](../src/code/11_entity_behaviors.asm#L779)

### 17D6 — `update_aimed_emitter_b`

Целится в игрока; по окончании счётчика создаёт навесной снаряд и ставит счётчик 18.

**Вход:** DS:SI = сущность; handler ID 6.

**Результат и оговорки:** Descriptor DS:0E9C; общий aiming возвращает CX=0 вне зоны стрельбы.

**Прямые CALL в физическом диапазоне:** `aim_entity_at_player`, `spawn_arcing_projectile`, `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 807](../src/code/11_entity_behaviors.asm#L807)

### 17F4 — `update_arcing_projectile`

Движение в 5 направлениях и изменение высоты по 16-байтовой таблице; затем эффект.

**Вход:** DS:SI = сущность; handler ID 30.

**Результат и оговорки:** Проверяет 3D-близость к игроку; shield_delta -=10.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`, `replace_with_arc_effect`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 830](../src/code/11_entity_behaviors.asm#L830)

### 18A9 — `update_aimed_emitter_c`

Целится и порождает наземный снаряд при истечении счётчика, затем сброс 40.

**Вход:** DS:SI = сущность; handler ID 7.

**Результат и оговорки:** Descriptor DS:0EC5; нельзя объединять период с другими emitter без параметров.

**Прямые CALL в физическом диапазоне:** `aim_entity_at_player`, `spawn_ground_enemy_shot`, `apply_sprite_frame`, `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 925](../src/code/11_entity_behaviors.asm#L925)

### 18C7 — `update_level_exit_gate`

Открывает ворота лишь после всех 6 collectibles, подтягивает игрока к центру и запускает переход.

**Вход:** DS:SI = сущность; handler ID 28.

**Результат и оговорки:** Открытие по Manhattan distance, затем отдельные пороги x/y; не круговой радиус.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`, `begin_level_exit`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 948](../src/code/11_entity_behaviors.asm#L948)

### 196F — `begin_level_exit`

Если frame игрока=0, задаёт переход F0h/туннель; иначе сначала переводит игрока в frame=0.

**Вход:** DS:SI = ворота.

**Результат и оговорки:** Смена режима отсрочена до выравнивания состояния игрока.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1033](../src/code/11_entity_behaviors.asm#L1033)

### 1993 — `update_tunnel_entrance_a`

Общий вход в туннель плюс выбор destination/тип награды из таблицы A.

**Вход:** DS:SI = сущность; handler ID 10.

**Результат и оговорки:** Табличное слово: нижние 10 бит=destination_y, старшие два бита=вид награды.

**Прямые CALL в физическом диапазоне:** `update_tunnel_gate_common`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1059](../src/code/11_entity_behaviors.asm#L1059)

### 19C2 — `update_tunnel_entrance_b`

Вход в туннель по таблице B; помечает перезагрузку карты и циклически меняет part 0..2.

**Вход:** DS:SI = сущность; handler ID 11.

**Результат и оговорки:** Выбор таблицы делается до увеличения level_part.

**Прямые CALL в физическом диапазоне:** `update_tunnel_gate_common`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1089](../src/code/11_entity_behaviors.asm#L1089)

### 1A04 — `update_tunnel_gate_common`

Открывает/закрывает створку по близости, скрывает открытый кадр, центрирует игрока и ставит tunnel_mode.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Сохраняет player_x для выхода; общая процедура у двух обработчиков входа.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1127](../src/code/11_entity_behaviors.asm#L1127)

### 1A9E — `update_refill_gate`

Центрирует игрока у объекта: узкая центральная зона добавляет side_charge_delta, соседняя обнуляет shield.

**Вход:** DS:SI = сущность; handler ID 8.

**Результат и оговорки:** Предполагаемая роль — пополнение; точные проверки x/y в коде важнее названия.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1208](../src/code/11_entity_behaviors.asm#L1208)

### 1B09 — `update_collectible`

При близости игрока очищает flags, засчитывает collectible[param] и запускает звук.

**Вход:** DS:SI = сущность; handler ID 32.

**Результат и оговорки:** Игрок в состоянии смерти не собирает объект.

**Прямые CALL в физическом диапазоне:** `cull_entity_after_view`, `collect_item_and_update_hud`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1271](../src/code/11_entity_behaviors.asm#L1271)

### 1B4A — `update_drifting_effect`

Сдвигает y на -2 с маской 03FFh и проверяет видимость.

**Вход:** DS:SI = сущность; handler ID 33.

**Результат и оговорки:** Визуальное название эффекта не установлено.

**Прямые CALL в физическом диапазоне:** `cull_entity_before_camera`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 1314](../src/code/11_entity_behaviors.asm#L1314)

### 1B57 — `sync_player_to_terrain`

Держит игрока на camera_y+112, запрашивает рельеф и применяет sprite descriptor.

**Вход:** Глобальные player_x/camera_y.

**Результат и оговорки:** Записывает player_y, z, map_row, frame; SI возвращается на первую запись.

**Прямые CALL в физическом диапазоне:** `sample_terrain`, `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/12_player_world_state.asm#L15)

### 1B80 — `update_scroll_speed`

Кнопки UP/DOWN меняют скорость; при отсутствии топлива принудительно выбирается DOWN.

**Вход:** scroll_speed, base_scroll_speed, fuel_units, controls.

**Результат и оговорки:** UP: +1 до 64; DOWN: -2 не ниже base_scroll_speed.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 43](../src/code/12_player_world_state.asm#L43)

### 1BB9 — `advance_surface_scroll`

Обновляет счётчик расхода топлива, camera_y modulo1024 и подгружает новую строку объектов.

**Вход:** scroll_speed.

**Результат и оговорки:** Шаг=((speed+15)>>3)>>1; порядок округления и порог 139 сохранены.

**Прямые CALL в физическом диапазоне:** `spawn_map_row`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 83](../src/code/12_player_world_state.asm#L83)

### 1C0A — `find_map_entity`

Ищет запись с флагом MAP_OBJECT и совпадающими row/column.

**Вход:** [BP+4]=row; [BP+6]=column.

**Результат и оговорки:** SI=найденная запись или адрес sentinel, не NULL. RET 4.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 129](../src/code/12_player_world_state.asm#L129)

### 1C33 — `begin_player_death`

Создаёт взрыв, переключает игрока на handler17, скрывает слой и сбрасывает death_age.

**Вход:** Нет аргументов.

**Результат и оговорки:** Повторный вызов при handler17 ничего не делает.

**Прямые CALL в физическом диапазоне:** `sound_trigger`, `spawn_explosion_for_entity`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 161](../src/code/12_player_world_state.asm#L161)

### 1C56 — `apply_player_map_hit`

Записывает отрицательный урон и преобразует найденный объект карты в разрушенное состояние.

**Вход:** AX=damage/code; BX=column; player_map_row.

**Результат и оговорки:** shield_delta присваивается, не уменьшается на AX; вызываемые процедуры меняют SI.

**Прямые CALL в физическом диапазоне:** `sound_trigger`, `find_map_entity`, `destroy_map_entity`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 185](../src/code/12_player_world_state.asm#L185)

### 1C78 — `push_player_history`

Сдвигает четыре сохранённых x/y/frame/z и вставляет текущего игрока.

**Вход:** Глобальная запись player.

**Результат и оговорки:** Это четыре параллельных массива, не четыре последовательные Entity.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 214](../src/code/12_player_world_state.asm#L214)

### 1CD9 — `position_side_device`

Берёт самую старую из 4 позиций, смещает на ±8 по x и +6 по y/z, применяет descriptor.

**Вход:** DS:SI = боковое устройство; flags20h = сторона.

**Результат и оговорки:** Учитывает рельефный наклон по таблице.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 258](../src/code/12_player_world_state.asm#L258)

### 1D1E — `fire_player_weapons`

Создаёт пару эффектов стволов и пару выстрелов; использует циклическую таблицу вариаций.

**Вход:** Глобальная запись player.

**Результат и оговорки:** Расход ammo накапливается отдельно; сохранять наблюдаемую зависимость от BX после вызовов.

**Прямые CALL в физическом диапазоне:** `allocate_entity_copy`, `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/13_weapons_and_frames.asm#L15)

### 1DEF — `fire_mobile_enemy_pattern`

Выбирает 1,2 или 3 точки выстрела по param и frame врага.

**Вход:** DS:SI = emitter.

**Результат и оговорки:** Сам вызывает spawn_enemy_shot_at_offset с разными dx/dy/dz.

**Прямые CALL в физическом диапазоне:** `spawn_enemy_shot_at_offset`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 97](../src/code/13_weapons_and_frames.asm#L97)

### 1E7D — `spawn_enemy_shot_at_offset`

Создаёт эффект и, при gate, снаряд с заданным смещением от emitter.

**Вход:** DS:SI=emitter; AX=dx; BX=dy; CL=dz.

**Результат и оговорки:** Сохраняет SI. Если новый слот расположен раньше emitter, сразу вызывает его update.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `allocate_entity_copy`, `update_countdown_effect`, `update_enemy_projectile`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 160](../src/code/13_weapons_and_frames.asm#L160)

### 1F0B — `spawn_aimed_shot`

Создаёт эффект/снаряд в выбранном направлении, беря смещения и размеры из таблиц.

**Вход:** DS:SI = emitter; frame = направление.

**Результат и оговорки:** Тот же компенсирующий update для слота раньше текущего.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `allocate_entity_copy`, `update_countdown_effect`, `update_enemy_projectile`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 230](../src/code/13_weapons_and_frames.asm#L230)

### 1FBA — `spawn_ground_enemy_shot`

Порождает handler29 из шаблона DS:10E8 и таблиц смещений.

**Вход:** DS:SI = emitter.

**Результат и оговорки:** SI сохраняется; новый ранний слот получает один немедленный update.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`, `allocate_entity_copy`, `update_ground_enemy_shot`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 312](../src/code/13_weapons_and_frames.asm#L312)

### 2005 — `spawn_arcing_projectile`

Порождает handler30 из шаблона DS:1446 и таблиц смещений.

**Вход:** DS:SI = emitter.

**Результат и оговорки:** SI сохраняется; новый ранний слот получает один немедленный update.

**Прямые CALL в физическом диапазоне:** `allocate_entity_copy`, `update_arcing_projectile`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 355](../src/code/13_weapons_and_frames.asm#L355)

### 2047 — `replace_with_side_impact`

Заменяет выстрел эффектом, если соответствующий side_impact_mask свободен; иначе удаляет.

**Вход:** DS:SI = выстрел.

**Результат и оговорки:** Отдельные биты 01h/80h ограничивают одновременные эффекты двух сторон.

**Прямые CALL в физическом диапазоне:** `apply_entity_state_template`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 395](../src/code/13_weapons_and_frames.asm#L395)

### 2092 — `replace_with_impact`

Применяет шаблон эффекта DS:1427 к текущей записи.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Координаты не копируются; изменяются только поля replacement-процедуры.

**Прямые CALL в физическом диапазоне:** `apply_entity_state_template`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 437](../src/code/13_weapons_and_frames.asm#L437)

### 2099 — `replace_with_arc_effect`

Применяет шаблон DS:12DE к текущей записи.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Общий механизм apply_entity_state_template.

**Прямые CALL в физическом диапазоне:** `apply_entity_state_template`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 451](../src/code/13_weapons_and_frames.asm#L451)

### 20A0 — `apply_entity_state_template`

Копирует flags, layer, age, frame, размеры, якорь, sprite и handler из DI в SI.

**Вход:** DS:SI = объект; DS:DI = шаблон.

**Результат и оговорки:** НЕ копирует param, x/y/z, bank, transparent и state13..15.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 465](../src/code/13_weapons_and_frames.asm#L465)

### 20DB — `destroy_map_entity`

Начисляет 150 очков, создаёт взрыв и помечает объект карты использованным.

**Вход:** DS:SI = сущность карты.

**Результат и оговорки:** Исходная запись преобразуется, а не обязательно сразу очищается.

**Прямые CALL в физическом диапазоне:** `score_add_150`, `spawn_explosion_for_entity`, `consume_map_object`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 497](../src/code/13_weapons_and_frames.asm#L497)

### 20F0 — `spawn_explosion_for_entity`

Создаёт explosion из координат текущей записи, при bit10h корректирует якорь.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** Сохраняет SI; шаблон DS:129E изменяется перед копированием.

**Прямые CALL в физическом диапазоне:** `allocate_entity_copy`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 518](../src/code/13_weapons_and_frames.asm#L518)

### 2121 — `apply_sprite_frame`

Читает count и SoA-массивы width/height/anchor_x/anchor_y, затем пары bank/offset.

**Вход:** DS:SI = Entity; DS:BX = descriptor; Entity.frame = индекс.

**Результат и оговорки:** Обновляет поля 09..10h, не flags/координаты/transparent; границы frame здесь не проверяются.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 547](../src/code/13_weapons_and_frames.asm#L547)

### 215B — `aim_entity_at_player`

Выбирает одно из 5 направлений, либо качает направление вне зоны прицеливания.

**Вход:** DS:SI = emitter; положение player.

**Результат и оговорки:** CX=1 разрешает стрельбу, CX=0 запрещает; frame = направление 0..4.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 586](../src/code/13_weapons_and_frames.asm#L586)

### 21DC — `init_hud`

Рисует общие элементы HUD, ресурсы, счёт, collectibles и жизни.

**Вход:** DS = сегмент игры.

**Результат и оговорки:** Рисование HUD идёт сразу в обе видеостраницы.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`, `reset_hud_resources`, `reset_score`, `reset_collectible_hud`, `init_lives`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/14_hud.asm#L15)

### 2200 — `reset_hud_resources`

Восстанавливает три шкалы и 6 зарядов боковых устройств.

**Вход:** Нет аргументов.

**Результат и оговорки:** Не является полным reset всей игры.

**Прямые CALL в физическом диапазоне:** `init_fuel_gauge`, `init_ammo_gauge`, `init_shield_gauge`, `init_side_charges`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 44](../src/code/14_hud.asm#L44)

### 220D — `update_hud`

Каждый четвёртый кадр начисляет очки за этап; применяет отложенные изменения шкал и зарядов.

**Вход:** Накопленные *_delta.

**Результат и оговорки:** Fuel/ammo при отрицательной delta уменьшаются ровно на 1, shield/charge — на модуль delta.

**Прямые CALL в физическом диапазоне:** `score_add_digits`, `draw_current_score`, `decrease_fuel`, `increase_fuel`, `decrease_ammo`, `increase_ammo`, `decrease_shield`, `increase_shield`, `remove_side_charge`, `add_side_charge`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 60](../src/code/14_hud.asm#L60)

### 22B1 — `init_lives`

Задаёт 3 жизни и рисует три значка.

**Вход:** Нет аргументов.

**Результат и оговорки:** life_count=3.

**Прямые CALL в физическом диапазоне:** `draw_life_1`, `draw_life_2`, `draw_life_3`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 161](../src/code/14_hud.asm#L161)

### 22C1 — `add_life`

Увеличивает число жизней с максимумом 3 и дорисовывает значок.

**Вход:** Нет аргументов.

**Результат и оговорки:** Переходы в draw_life_* — хвостовые, без нового return address.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 177](../src/code/14_hud.asm#L177)

### 22E1 — `draw_life_1`

Рисует первый значок жизни в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Хвостовая цель add_life и отдельная CALL-процедура.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 201](../src/code/14_hud.asm#L201)

### 22F9 — `draw_life_2`

Рисует второй значок жизни в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Хвостовая цель add_life.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 224](../src/code/14_hud.asm#L224)

### 2311 — `draw_life_3`

Рисует третий значок жизни в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Хвостовая цель add_life.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 247](../src/code/14_hud.asm#L247)

### 2329 — `remove_life`

Уменьшает life_count и стирает соответствующий значок.

**Вход:** Нет аргументов.

**Результат и оговорки:** Сам не насыщает снизу; предшествующая проверка сделана в handle_player_respawn.

**Прямые CALL в физическом диапазоне:** `fill_hud_rect_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 270](../src/code/14_hud.asm#L270)

### 237C — `init_side_charges`

Устанавливает 6 зарядов и рисует все шесть индикаторов.

**Вход:** Нет аргументов.

**Результат и оговорки:** side_charge_delta=0.

**Прямые CALL в физическом диапазоне:** `draw_charge_1`, `draw_charge_2`, `draw_charge_3`, `draw_charge_4`, `draw_charge_5`, `draw_charge_6`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 323](../src/code/14_hud.asm#L323)

### 239B — `add_side_charge`

Добавляет заряд до 6, проигрывает звук и рисует соответствующий индикатор.

**Вход:** Нет аргументов.

**Результат и оговорки:** Вызовы и хвостовые переходы используют одни и те же draw_charge_*.

**Прямые CALL в физическом диапазоне:** `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 343](../src/code/14_hud.asm#L343)

### 23D4 — `draw_charge_1`

Рисует индикатор заряда 1 в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные адреса HUD — не адреса DS.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 378](../src/code/14_hud.asm#L378)

### 23EC — `draw_charge_2`

Рисует индикатор заряда 2 в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные адреса HUD — не адреса DS.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 401](../src/code/14_hud.asm#L401)

### 2404 — `draw_charge_3`

Рисует индикатор заряда 3 в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные адреса HUD — не адреса DS.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 424](../src/code/14_hud.asm#L424)

### 241C — `draw_charge_4`

Рисует индикатор заряда 4 в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные адреса HUD — не адреса DS.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 447](../src/code/14_hud.asm#L447)

### 2434 — `draw_charge_5`

Рисует индикатор заряда 5 в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные адреса HUD — не адреса DS.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 470](../src/code/14_hud.asm#L470)

### 244C — `draw_charge_6`

Рисует индикатор заряда 6 в обеих страницах.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные адреса HUD — не адреса DS.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 493](../src/code/14_hud.asm#L493)

### 2464 — `remove_side_charge`

Уменьшает число зарядов до минимума 0 и стирает прежний крайний индикатор.

**Вход:** Нет аргументов.

**Результат и оговорки:** Выбор значка сделан по значению ДО декремента.

**Прямые CALL в физическом диапазоне:** `fill_hud_rect_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 516](../src/code/14_hud.asm#L516)

### 250B — `init_fuel_gauge`

Инициализирует шкалу, расходуемую движением (вероятно топливо), значением 44.

**Вход:** Нет аргументов.

**Результат и оговорки:** fuel_delta=0; имя fuel выведено по использованию, не по авторским символам.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 612](../src/code/14_hud.asm#L612)

### 252F — `decrease_fuel`

Уменьшает fuel_units, если результат неотрицателен.

**Вход:** Нет аргументов.

**Результат и оговорки:** Хвостовой переход в store_fuel_gauge; отрицательная ветка возвращает без записи.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 637](../src/code/14_hud.asm#L637)

### 2536 — `store_fuel_gauge`

Записывает fuel_units и обновляет столбец шкалы.

**Вход:** AX = новое/текущее значение шкалы.

**Результат и оговорки:** Общий хвост decrease_fuel; add_fuel вызывает его отдельно.

**Прямые CALL в физическом диапазоне:** `toggle_hud_gauge_column`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 652](../src/code/14_hud.asm#L652)

### 2542 — `increase_fuel`

При значении<44 обновляет столбец и увеличивает fuel_units.

**Вход:** Нет аргументов.

**Результат и оговорки:** Отрисовка использует старое значение, инкремент сделан после неё.

**Прямые CALL в физическом диапазоне:** `store_fuel_gauge`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 669](../src/code/14_hud.asm#L669)

### 2553 — `init_ammo_gauge`

Инициализирует шкалу, расходуемую стрельбой, значением 48.

**Вход:** Нет аргументов.

**Результат и оговорки:** ammo_delta=0.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 688](../src/code/14_hud.asm#L688)

### 2577 — `decrease_ammo`

Уменьшает ammo_units без ухода ниже нуля.

**Вход:** Нет аргументов.

**Результат и оговорки:** Хвостовой переход в store_ammo_gauge.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 713](../src/code/14_hud.asm#L713)

### 257E — `store_ammo_gauge`

Записывает ammo_units и обновляет столбец шкалы.

**Вход:** AX = значение.

**Результат и оговорки:** Общий хвост, также самостоятельная CALL-цель.

**Прямые CALL в физическом диапазоне:** `toggle_hud_gauge_column`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 728](../src/code/14_hud.asm#L728)

### 258A — `increase_ammo`

При значении<48 обновляет столбец и увеличивает ammo_units.

**Вход:** Нет аргументов.

**Результат и оговорки:** Отрисовка до инкремента.

**Прямые CALL в физическом диапазоне:** `store_ammo_gauge`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 745](../src/code/14_hud.asm#L745)

### 259B — `init_shield_gauge`

Инициализирует шкалу прочности значением 56.

**Вход:** Нет аргументов.

**Результат и оговорки:** shield_delta=0; имя shield условное по применению урона.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 764](../src/code/14_hud.asm#L764)

### 25BF — `decrease_shield`

Уменьшает прочность без ухода ниже нуля.

**Вход:** Нет аргументов.

**Результат и оговорки:** Хвостовой переход в store_shield_gauge.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 789](../src/code/14_hud.asm#L789)

### 25C6 — `store_shield_gauge`

Записывает shield_units и обновляет столбец шкалы.

**Вход:** AX = значение.

**Результат и оговорки:** Общий хвост, также CALL-цель.

**Прямые CALL в физическом диапазоне:** `toggle_hud_gauge_column`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 804](../src/code/14_hud.asm#L804)

### 25D2 — `increase_shield`

При значении<56 обновляет столбец и увеличивает прочность.

**Вход:** Нет аргументов.

**Результат и оговорки:** Отрисовка до инкремента.

**Прямые CALL в физическом диапазоне:** `store_shield_gauge`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 821](../src/code/14_hud.asm#L821)

### 25E3 — `toggle_hud_gauge_column`

Меняет выбранный бит столбца высотой 7 строк в обеих видеостраницах с графической логической операцией.

**Вход:** [BP+4]=video offset; [BP+6]=column pixel.

**Результат и оговорки:** RET 4; это не простая запись цвета в packed-массив, действует состояние EGA.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 840](../src/code/14_hud.asm#L840)

### 264A — `reset_score`

Очищает шесть десятичных цифр текущего счёта и вызывает отрисовку.

**Вход:** Нет аргументов.

**Результат и оговорки:** Цифры хранятся отдельно, не ASCII и не packed BCD.

**Прямые CALL в физическом диапазоне:** `draw_current_score`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/15_score.asm#L15)

### 2662 — `score_add_10`

Добавляет 10 через четыре десятичных разряда.

**Вход:** Нет аргументов.

**Результат и оговорки:** Стековая обёртка score_add_digits.

**Прямые CALL в физическом диапазоне:** `score_add_digits`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 37](../src/code/15_score.asm#L37)

### 2676 — `score_add_150`

Добавляет 150 через четыре десятичных разряда.

**Вход:** Нет аргументов.

**Результат и оговорки:** Используется при уничтожении/сборе объектов.

**Прямые CALL в физическом диапазоне:** `score_add_digits`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 58](../src/code/15_score.asm#L58)

### 268A — `score_add_2500`

Добавляет 2500 через четыре десятичных разряда.

**Вход:** Нет аргументов.

**Результат и оговорки:** Используется как один из туннельных бонусов.

**Прямые CALL в физическом диапазоне:** `score_add_digits`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 79](../src/code/15_score.asm#L79)

### 269E — `score_add_digits`

Добавляет units/tens/hundreds/thousands с переносами в массив из 6 цифр.

**Вход:** [BP+4]=units, +6=tens, +8=hundreds, +10=thousands; читаются младшие байты.

**Результат и оговорки:** RET 8. Каждый разряд вычитает 10 максимум один раз; перенос 100000 даёт жизнь. Не нормализовать большие входы иначе.

**Прямые CALL в физическом диапазоне:** `add_life`.

**Возвраты в физическом диапазоне:** `ret 0x8`.

[Исходник, строка 100](../src/code/15_score.asm#L100)

### 2714 — `draw_current_score`

Рисует значащие цифры текущего счёта без ведущих нулей.

**Вход:** score_digits[6].

**Результат и оговорки:** Позиция вывода сдвигается только при реально нарисованной цифре.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 165](../src/code/15_score.asm#L165)

### 2762 — `load_highscores`

Готовит начальные имена/цифры/порядок и читает 140 байт HISCORES.

**Вход:** Нет аргументов.

**Результат и оговорки:** 7 имён×12 + 7 результатов×6 + 7 scratch + 7 order =140.

**Прямые CALL в физическом диапазоне:** `load_file_to_ds`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/16_highscores.asm#L15)

### 279F — `insert_current_highscore`

Сортирует 7 результатов, проверяет попадание текущего в шесть лучших, заменяет последний слот и готовит ввод имени.

**Вход:** score slot0 = текущий; order[7].

**Результат и оговорки:** При отсутствии рекорда очищает pending_name_entry.

**Прямые CALL в физическом диапазоне:** `sort_highscore_order`, `render_highscore_table`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 54](../src/code/16_highscores.asm#L54)

### 2809 — `sort_highscore_order`

Выбором максимума сортирует 7 индексов по результату; использует FFh как вычеркнутый элемент.

**Вход:** order[7], score_digits[7][6].

**Результат и оговорки:** Имена и результаты физически не переставляет.

**Прямые CALL в физическом диапазоне:** `compare_highscore_candidates`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 114](../src/code/16_highscores.asm#L114)

### 2872 — `compare_highscore_candidates`

Лексикографически сравнивает по 6 десятичных цифр два результата.

**Вход:** DS:SI и DS:BX указывают на байты индексов.

**Результат и оговорки:** AL=1, если кандидат BX строго больше SI; при равенстве 0. SI/DI сохранены, AX изменён.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 176](../src/code/16_highscores.asm#L176)

### 28DA — `render_highscore_table`

Рисует имена и результаты шести победителей в packed-буфер PODZ1.

**Вход:** order[0..5], таблица layout DS:01B1.

**Результат и оговорки:** Не выводит непосредственно на EGA.

**Прямые CALL в физическом диапазоне:** `draw_highscore_name`, `draw_highscore_digits`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 239](../src/code/16_highscores.asm#L239)

### 2907 — `draw_highscore_digits`

Рисует 6 цифр выбранного результата в текстовую область, заменяя ведущие нули пустым glyph.

**Вход:** DS:SI = указатель на индекс; AX=destination offset.

**Результат и оговорки:** Destination хранится в text_draw_offset; исходные glyph берутся из BIRD2.

**Прямые CALL в физическом диапазоне:** `copy_text_glyph`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 275](../src/code/16_highscores.asm#L275)

### 2959 — `draw_highscore_name`

Выбирает 12-байтовое имя по индексу и проваливается в draw_name_12_chars.

**Вход:** DS:SI = указатель на индекс; AX=destination offset.

**Результат и оговорки:** Общая функция с дополнительным входом 296F; между ними нет RET.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** нет.

[Исходник, строка 327](../src/code/16_highscores.asm#L327)

### 296F — `draw_name_12_chars`

Рисует 12 символов: A–Z, пробел, точка, подчёркивание по особым glyph.

**Вход:** DS:SI = 12 байт имени; text_draw_offset.

**Результат и оговорки:** Дополнительный вход общей процедуры; вызывается также напрямую.

**Прямые CALL в физическом диапазоне:** `copy_text_glyph`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 350](../src/code/16_highscores.asm#L350)

### 29AE — `copy_text_glyph`

Копирует glyph 12×11 пикселей из BIRD2 в PODZ1 (6 байт на строку, stride160).

**Вход:** [BP+4]=x offset bytes; [BP+6]=glyph source offset.

**Результат и оговорки:** RET 4; DS сохраняется, ES становится сегментом PODZ1; ожидается DF=0.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 399](../src/code/16_highscores.asm#L399)

### 29DB — `edit_highscore_name`

Вводит до 12 символов с backspace, перерисовывает заставку и сохраняет HISCORES.

**Вход:** pending name pointer/rank.

**Результат и оговорки:** Нулевой результат poll_key_character завершает ввод; нет отдельного бесконечного ожидания Enter.

**Прямые CALL в физическом диапазоне:** `poll_key_character`, `redraw_edited_name`, `restore_attract_background`, `render_entities`, `present_frame`, `dos_create_write_close`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 431](../src/code/16_highscores.asm#L431)

### 2A64 — `redraw_edited_name`

Перерисовывает только имя выбранной строки рекордов.

**Вход:** name pointer, pending rank.

**Результат и оговорки:** Вызывает дополнительный вход draw_name_12_chars.

**Прямые CALL в физическом диапазоне:** `draw_name_12_chars`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 494](../src/code/16_highscores.asm#L494)

### 2A7F — `reset_collectible_hud`

Обнуляет 6 collectible flags и рисует исходные значки.

**Вход:** Нет аргументов.

**Результат и оговорки:** Массив flags одновременно проверяется выходными воротами.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 515](../src/code/16_highscores.asm#L515)

### 2B28 — `collect_item_and_update_hud`

Начисляет 150 очков, выставляет collectible[AL] и меняет его значок.

**Вход:** AL=0..5; значения вне диапазона всё равно сначала начисляют очки.

**Результат и оговорки:** Не добавлять раннюю проверку индекса перед начислением при буквальном переносе.

**Прямые CALL в физическом диапазоне:** `score_add_digits`, `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 599](../src/code/16_highscores.asm#L599)

### 2C07 — `draw_redefine_dialog`

Рисует подложку и заголовок переназначения клавиш.

**Вход:** Нет аргументов.

**Результат и оговорки:** Фиксированные прямоугольники/ресурсы в обеих страницах.

**Прямые CALL в физическом диапазоне:** `fill_hud_rect_both_pages`, `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/17_ui_primitives.asm#L15)

### 2C45 — `draw_quit_dialog`

Рисует диалог подтверждения выхода.

**Вход:** Нет аргументов.

**Результат и оговорки:** Опрос Y/N находится в poll_controls, не здесь.

**Прямые CALL в физическом диапазоне:** `fill_hud_rect_both_pages`, `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 56](../src/code/17_ui_primitives.asm#L56)

### 2C9A — `draw_control_prompt`

Очищает строку и рисует запрос очередной клавиши; боковые действия состоят из двух частей.

**Вход:** AL = control index 0..7.

**Результат и оговорки:** Может проваливаться в draw_control_prompt_piece; общий хвост.

**Прямые CALL в физическом диапазоне:** `fill_hud_rect_both_pages`, `draw_control_prompt_piece`.

**Возвраты в физическом диапазоне:** нет.

[Исходник, строка 108](../src/code/17_ui_primitives.asm#L108)

### 2CC7 — `draw_control_prompt_piece`

По индексу берёт bank/source/destination/width/height из пяти таблиц и рисует фрагмент.

**Вход:** AL = индекс.

**Результат и оговорки:** Также самостоятельная CALL-цель и хвост предыдущей процедуры.

**Прямые CALL в физическом диапазоне:** `blit_hud_sprite_both_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 144](../src/code/17_ui_primitives.asm#L144)

### 2CEA — `blit_hud_sprite_both_pages`

Выводит packed-4bpp ресурс с прозрачным цветом 2 сразу в A000 и A200 страницы.

**Вход:** [BP+4]=bank offset, +6=height, +8=width, +10=dest byte offset, +12=source offset.

**Результат и оговорки:** RET 10; width обрабатывается парами. Source stride160, destination stride40.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0xa`.

[Исходник, строка 170](../src/code/17_ui_primitives.asm#L170)

### 2D9A — `fill_hud_rect_both_pages`

Заливает прямоугольник через EGA set/reset одновременно в обеих страницах.

**Вход:** [BP+4]=color, +6=height, +8=width BYTES, +10=destination byte offset.

**Результат и оговорки:** RET 8; ширина в байтах плоскости, не в пикселях.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x8`.

[Исходник, строка 279](../src/code/17_ui_primitives.asm#L279)

### 2DDC — `select_tunnel_pattern`

Циклически выбирает один из четырёх потоков туннельных объектов и сбрасывает номер предыдущей секции.

**Вход:** tunnel_pattern_index.

**Результат и оговорки:** Индекс растёт байтово, таблица индексируется по младшим двум битам.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/18_tunnel_gameplay.asm#L15)

### 2DF6 — `reset_tunnel_angle_history`

Обнуляет историю трёх углов для боковых устройств.

**Вход:** Нет аргументов.

**Результат и оговорки:** Не обнуляет всю физику туннеля.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 36](../src/code/18_tunnel_gameplay.asm#L36)

### 2E02 — `update_tunnel_player`

Обновляет скорость/дистанцию, угловую инерцию, позицию по lookup-таблицам и боковые действия.

**Вход:** DS:SI = игрок туннеля; handler ID12.

**Результат и оговорки:** Инерция ограничена ±64, угол modulo128, координаты рассчитаны целочисленно.

**Прямые CALL в физическом диапазоне:** `update_scroll_speed`, `advance_tunnel_distance`, `tunnel_angle_force`, `apply_sprite_frame`, `sound_trigger`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 52](../src/code/18_tunnel_gameplay.asm#L52)

### 2F50 — `update_tunnel_side_device`

Следует задержанному углу; при выпуске/отключении исчезает и собирает все handler15 в пуле.

**Вход:** DS:SI = устройство; handler ID34.

**Результат и оговорки:** SI на пути сбора перебирает пул; dispatcher обязан сохранять свой SI.

**Прямые CALL в физическом диапазоне:** `collect_tunnel_pickup`, `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 210](../src/code/18_tunnel_gameplay.asm#L210)

### 2FE8 — `update_tunnel_exit_marker`

Выбирает размер кольца и кадр выходного маркера по оставшейся дистанции.

**Вход:** DS:SI = сущность; handler ID14.

**Результат и оговорки:** Две таблицы по 32 элемента, индекс=(remaining>>3)&31.

**Прямые CALL в физическом диапазоне:** `apply_sprite_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 292](../src/code/18_tunnel_gameplay.asm#L292)

### 300A — `update_tunnel_pickup`

По дистанции от появления вычисляет масштаб/позицию объекта; при совпадении сектора игрока собирает.

**Вход:** DS:SI = сущность; handler ID15.

**Результат и оговорки:** state13..14 используются как WORD spawn_distance, не независимые row/column.

**Прямые CALL в физическом диапазоне:** `position_tunnel_pickup`, `apply_sprite_frame`, `sound_trigger`, `collect_tunnel_pickup`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 315](../src/code/18_tunnel_gameplay.asm#L315)

### 3059 — `collect_tunnel_pickup`

Удаляет объект и начисляет награду в зависимости от tunnel_reward_kind.

**Вход:** DS:SI = сущность.

**Результат и оговорки:** 40h:+6 ammo, 80h:+4 fuel, C0h:+8 shield, иначе +2500 очков.

**Прямые CALL в физическом диапазоне:** `score_add_2500`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 362](../src/code/18_tunnel_gameplay.asm#L362)

### 3084 — `tunnel_angle_force`

Нелинейная добавка угловой скорости: после вычитания 5 проверяет SF, ограничивает до56, делит на2, возвращает -(x+3).

**Вход:** AL = нормализованный угол 0..63 у текущих caller.

**Результат и оговорки:** AX = signed сила. Входы за пределами фактического домена требуют сохранения 8-битного wrap.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 393](../src/code/18_tunnel_gameplay.asm#L393)

### 309D — `advance_tunnel_distance`

Продвигает дистанцию, расходует топливо, создаёт объекты по секциям и возвращает на поверхность после 896.

**Вход:** scroll_speed; pattern stream.

**Результат и оговорки:** Каждые 64 единицы новая секция; выходной маркер появляется в последних 256 единицах.

**Прямые CALL в физическом диапазоне:** `allocate_entity_copy`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 421](../src/code/18_tunnel_gameplay.asm#L421)

### 3130 — `position_tunnel_pickup`

Берёт радиус по дистанции и выбирает один из 8 лучей вокруг центра.

**Вход:** AX = индекс масштаба 0..45; DS:SI = pickup, param=1..8.

**Результат и оговорки:** Оси: центр x=128, y=106; диагонали не нормализованы через sqrt(2).

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 492](../src/code/18_tunnel_gameplay.asm#L492)

### 315E — `tunnel_set_north`

y=106-radius; также содержит диагональный хвост north-east.

**Вход:** DS:SI = pickup; CX = radius.

**Результат и оговорки:** Дополнительные входы внутри позиционирования, не изолировать хвосты как независимый C-код.

**Прямые CALL в физическом диапазоне:** `tunnel_set_north`, `tunnel_set_east`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 525](../src/code/18_tunnel_gameplay.asm#L525)

### 3170 — `tunnel_set_east`

x=128+radius; далее расположен хвост south-east.

**Вход:** DS:SI = pickup; CX = radius.

**Результат и оговорки:** CX изменяется при записи; диагональные caller сохраняют исходный CX.

**Прямые CALL в физическом диапазоне:** `tunnel_set_east`, `tunnel_set_south`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 546](../src/code/18_tunnel_gameplay.asm#L546)

### 3181 — `tunnel_set_south`

y=106+radius; далее расположен хвост south-west.

**Вход:** DS:SI = pickup; CX = radius.

**Результат и оговорки:** Общий набор позиционных помощников.

**Прямые CALL в физическом диапазоне:** `tunnel_set_south`, `tunnel_set_west`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 566](../src/code/18_tunnel_gameplay.asm#L566)

### 3191 — `tunnel_set_west`

x=128-radius; далее расположен хвост north-west.

**Вход:** DS:SI = pickup; CX = radius.

**Результат и оговорки:** Общий набор позиционных помощников.

**Прямые CALL в физическом диапазоне:** `tunnel_set_west`, `tunnel_set_north`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 586](../src/code/18_tunnel_gameplay.asm#L586)

### 31A4 — `present_tunnel_bitmap`

Копирует 256×160 участок 1bpp туннеля в draw page с выбором EGA-цветовых плоскостей.

**Вход:** scratch0 с bitmap; tunnel_reward_kind и flash state.

**Результат и оговорки:** Источник stride32, назначение40; это не packed4bpp sprite.

**Прямые CALL в физическом диапазоне:** `tunnel_plane_color`, `tunnel_flash_color`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/19_ega_video.asm#L15)

### 31E7 — `tunnel_plane_color`

Выбирает маску/цвет туннеля по типу награды.

**Вход:** tunnel_reward_kind.

**Результат и оговорки:** AX: 40h→2, 80h→4, C0h→1, иначе3.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 59](../src/code/19_ega_video.asm#L59)

### 3209 — `tunnel_flash_color`

Выдаёт цвет вспышки и переводит двухшаговое состояние к нулю.

**Вход:** tunnel_flash_state.

**Результат и оговорки:** 0→color0; 1→color7/state0; другое→color15/state1.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 89](../src/code/19_ega_video.asm#L89)

### 322C — `unused_planar_blit`

Сохранённый, статически недостижимый blit четырёх плоскостей с шагом источника 8000.

**Вход:** [BP+4]=bank,+6=height,+8=width bytes,+10=dest,+12=source.

**Результат и оговорки:** RET10; не удалять из byte-exact сборки. Не доказано, что никогда не выполняется.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0xa`.

[Исходник, строка 116](../src/code/19_ega_video.asm#L116)

### 3291 — `copy_playfield_between_pages`

Копирует видимую область 256×160 между EGA-сегментами через latch copy.

**Вход:** [BP+4]=destination segment; [BP+6]=source segment.

**Результат и оговорки:** RET4; начальный offset=0144h, stride40, 32 байта на строку.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 182](../src/code/19_ega_video.asm#L182)

### 32C8 — `unused_rectangle_fill`

Сохранённая недостижимая заливка прямоугольника draw page.

**Вход:** [BP+4]=color,+6=height,+8=width bytes,+10=dest.

**Результат и оговорки:** RET8; отсутствие статического пути не доказательство неиспользования при любых условиях.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret 0x8`.

[Исходник, строка 220](../src/code/19_ega_video.asm#L220)

### 32FE — `draw_scrolling_background`

Копирует 160 строк из кольцевого фона A800h, начиная с camera_y*32, на draw page.

**Вход:** camera_y.

**Результат и оговорки:** Source address маскируется 7FFFh после каждой строки; viewport offset0144h.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 259](../src/code/19_ega_video.asm#L259)

### 333C — `video_init_mode0d`

Включает режим 0Dh, задаёт A000/A200 страницы и начальный CRTC start.

**Вход:** Нет аргументов.

**Результат и оговорки:** Обращается к BIOS и EGA, в C-порте заменяется backend, а не DOS-эмуляцией в игровой логике.

**Прямые CALL в физическом диапазоне:** `clear_draw_page`, `wait_retrace_cycle`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 300](../src/code/19_ega_video.asm#L300)

### 3383 — `present_frame`

Меняет местами draw/display сегменты, пишет CRTC start high и ждёт retrace.

**Вход:** Две страницы.

**Результат и оговорки:** Не использует timer_ticks для шага игровой логики; frame pacing привязан к retrace.

**Прямые CALL в физическом диапазоне:** `wait_retrace_cycle`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 342](../src/code/19_ega_video.asm#L342)

### 33B3 — `clear_draw_page`

Заливает текущую страницу указанным EGA-цветом.

**Вход:** AL = цвет.

**Результат и оговорки:** 8000 байт адресов плоскости, ES=draw segment, DF=0.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 377](../src/code/19_ega_video.asm#L377)

### 33D7 — `wait_retrace_cycle`

Ждёт установки и затем снятия бита3 порта3DAh.

**Вход:** Нет аргументов.

**Результат и оговорки:** Два busy loop; нельзя заменять частоту звукового PIT частотой кадров без измерения.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 407](../src/code/19_ega_video.asm#L407)

### 33E5 — `blit_planar_fullscreen`

Выводит 4 плоскости по 8000 байт из последовательного буфера.

**Вход:** BX=source segment, SI=source offset.

**Результат и оговорки:** Цикл масок sequencer 8,4,2,1; используется для INTRO.X, не для packed sprites.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 430](../src/code/19_ega_video.asm#L430)

### 3423 — `init_playfield_pages`

Очищает обе видеостраницы и рисует рамку.

**Вход:** Нет аргументов.

**Результат и оговорки:** Между страницами выполняется present_frame.

**Прямые CALL в физическом диапазоне:** `clear_draw_page`, `draw_playfield_border`, `present_frame`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 476](../src/code/19_ega_video.asm#L476)

### 3439 — `draw_playfield_border`

Рисует рамку вокруг игрового поля прямыми записями в video RAM.

**Вход:** draw segment, ожидаемое состояние EGA и DF.

**Результат и оговорки:** Ширина поля 256; аппаратное состояние важно, не обычный массив RGB.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 495](../src/code/19_ega_video.asm#L495)

### 3472 — `prepare_attract_background`

Выводит INTRO в обе страницы и сохраняет область игры в A400h.

**Вход:** segment_intro.

**Результат и оговорки:** A400 — фоновый backup для заставки, не третий игровой world buffer.

**Прямые CALL в физическом диапазоне:** `blit_planar_fullscreen`, `present_frame`, `copy_playfield_between_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 530](../src/code/19_ega_video.asm#L530)

### 3498 — `restore_attract_background`

Восстанавливает playfield из A400h в текущую draw page.

**Вход:** Backup уже создан.

**Результат и оговорки:** Сущности рисуются после восстановления фона.

**Прямые CALL в физическом диапазоне:** `copy_playfield_between_pages`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 555](../src/code/19_ega_video.asm#L555)

### 34A8 — `build_tunnel_bitmap`

Очищает 1bpp scratch, рисует диагонали и последовательно уменьшаемые восьмиугольники по дистанции.

**Вход:** tunnel_distance, radius tables.

**Результат и оговорки:** Bitmap 256×256 с stride32; на экран берётся его центральная часть.

**Прямые CALL в физическом диапазоне:** `draw_tunnel_line`, `draw_nested_tunnel_rings`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/20_tunnel_renderer.asm#L15)

### 35BC — `draw_nested_tunnel_rings`

Рисует 8 сторон кольца около центра127/128; делит радиусы пополам до порога.

**Вход:** [BP+4]=radiusA, [BP+6]=radiusB.

**Результат и оговорки:** RET4; цикл, а не рекурсивные CALL; после уменьшения сравнивает AL с минимальным радиусом.

**Прямые CALL в физическом диапазоне:** `draw_tunnel_line`.

**Возвраты в физическом диапазоне:** `ret 0x4`.

[Исходник, строка 145](../src/code/20_tunnel_renderer.asm#L145)

### 36D4 — `draw_tunnel_line`

Целочисленная линия с accumulator; масштабирует y как (10*y+5)/12 и рисует включительно оба конца.

**Вход:** [BP+4]=y2,+6=x2,+8=y1,+10=x1; исходный домен 0..255.

**Результат и оговорки:** RET8; порог накопителя строго >, не >=. Это влияет на пиксели и должно быть проверено при переносе.

**Прямые CALL в физическом диапазоне:** `plot_tunnel_line_start`.

**Возвраты в физическом диапазоне:** `ret 0x8`.

[Исходник, строка 275](../src/code/20_tunnel_renderer.asm#L275)

### 3801 — `plot_tunnel_line_start`

Вычисляет offset=y1*32+x1/8, mask=80h>>(x1&7), рисует первый пиксель.

**Вход:** НАСЛЕДУЕТ BP draw_tunnel_line; ES=bitmap. Собственного пролога нет.

**Результат и оговорки:** DI=offset, AL=mask; не трактовать [BP+8/+10] как аргументы собственного CALL.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 425](../src/code/20_tunnel_renderer.asm#L425)

### 382A — `sound_init`

Очищает активный/два ожидающих звука, включает звук и сбрасывает toggle latch.

**Вход:** Нет аргументов.

**Результат и оговорки:** Не инициализирует всю память audio, а только выбранные поля.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 15](../src/code/21_audio.asm#L15)

### 3840 — `sound_trigger`

Запрашивает эффект: стартует сразу либо помещает в двухступенчатую очередь; последняя ячейка хранит максимум ID.

**Вход:** AL = sound ID.

**Результат и оговорки:** Во время music_active запрос игнорируется. Это не FIFO произвольной длины.

**Прямые CALL в физическом диапазоне:** `start_sound_descriptor`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 35](../src/code/21_audio.asm#L35)

### 387D — `sound_tick`

Шаг музыки/эффекта: время, очередь, изменение делителя, модуляция и запись PIT2.

**Вход:** Состояние audio; вызывается из INT1Ch.

**Результат и оговорки:** Счётчики 8-битные, делители16-битные; IRQ-контекст отделить от simulation.

**Прямые CALL в физическом диапазоне:** `music_tick`, `sound_stop`, `start_sound_descriptor`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 76](../src/code/21_audio.asm#L76)

### 3914 — `start_sound_descriptor`

Загружает 6-байтовый descriptor, подставляет динамические параметры двух эффектов и включает PIT2/speaker.

**Вход:** AL = sound ID1..15.

**Результат и оговорки:** Один вариант pitch зависит от зарядов, другой от угла игрока; таблицы могут изменяться.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 152](../src/code/21_audio.asm#L152)

### 39A3 — `sound_stop`

Очищает очередь/время эффекта и отключает speaker gate bits0/1.

**Вход:** Нет аргументов.

**Результат и оговорки:** Не очищает все музыкальные указатели.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 226](../src/code/21_audio.asm#L226)

### 39B8 — `music_start`

Запускает одну из двух последовательностей, выбирая её по pending_name_entry.

**Вход:** Нет аргументов.

**Результат и оговорки:** sequence cursor=0, music divider=0, active=1.

**Прямые CALL в физическом диапазоне:** нет.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 246](../src/code/21_audio.asm#L246)

### 39DB — `music_tick`

Каждые 8 звуковых тиков читает команду: FF конец, FE пауза, 0 специальное заполнение, прочее транспонируется.

**Вход:** music sequence/cursor, sound state.

**Результат и оговорки:** Пустая команда может взять note из отдельной таблицы; не простая нулевая пауза.

**Прямые CALL в физическом диапазоне:** `music_play_note`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 270](../src/code/21_audio.asm#L270)

### 3A3B — `music_play_note`

Берёт PIT-делитель по note-1, сохраняет его в descriptor15 и стартует этот эффект.

**Вход:** AL=номер ноты; ноль возвращает без старта.

**Результат и оговорки:** Именно конец этой процедуры физически перекрывает DS:0000..0005.

**Прямые CALL в физическом диапазоне:** `start_sound_descriptor`.

**Возвраты в физическом диапазоне:** `ret`.

[Исходник, строка 324](../src/code/21_audio.asm#L324)

