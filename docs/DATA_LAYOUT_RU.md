# Форматы и карта данных

Типы и имена ниже получены из обращений реального бинарника. Формат на диске little-endian; размеры host C-структур не используются для сериализации. `D_*` — абсолютное DS-смещение, `E_*` — относительное поле Entity. `dat_*` — физическая метка в EXE, в том числе aliases полей.

## Entity: 23 байта

| Offset | Тип | Поле | Значение |
|---|---|---|---|
| `00` | u8 | `flags` | RENDER1, UPDATE2; прочие биты контекстны, sentinel80 вне обычных записей. |
| `01` | u8 | `param` | Индекс/параметр обработчика, не универсальное health. |
| `02` | u8 | `layer` | Слои7..1; 0 не рисуется. |
| `03` | i8 | `z` | Проекционное смещение. |
| `04` | u8 | `frame` | Индекс FrameSet или вариант рельефа. |
| `05` | u16 LE | `x` | Положение X. |
| `07` | u16 LE | `y` | Y/координата мира. На поверхности modulo1024. |
| `09` | u8 | `width` | Ширина спрайта. |
| `0A` | u8 | `height` | Высота спрайта. |
| `0B` | i8 | `anchor_x` | Горизонтальный anchor. |
| `0C` | i8 | `anchor_y` | Вертикальный anchor. |
| `0D` | u16 LE | `bank_offset` | БАЙТОВОЕ смещение в таблице сегментов; 0,2,4,... |
| `0F` | u16 LE | `sprite_offset` | Смещение пикселей внутри банка. |
| `11` | u8 | `transparent` | Индекс прозрачного цвета. |
| `12` | u8 | `handler_id` | 0 без вызова;1..35 table[id-1]. |
| `13..16` | 4 bytes | `state` | Могут пересекаться с word; отдельный контракт на каждый handler. |

**Пул:** DS:1B8A..1FD9 =48×23 байта. Sentinel DS:1FDA..1FDB. Первая запись — игрок; глобальные `D_player_x` и т.п. являются aliases этой записи, не второй копией состояния. 39 шаблонов находятся в `src/data/01_strings_templates_tables.inc`; их поля размечены отдельными .byte/.word.

Примеры перекрывающихся state: у tunnel pickup +13..14 — WORD spawn_distance; у surface player +14 используется как строка карты; +16 у игрока в туннеле — угол, у многих эффектов — счётчик возраста. Без контекста handler эти байты не следует называть общим vx/vy или lifetime.

## Таблица поведения

| ID | CS | Рабочее имя | Основное действие |
|---|---|---|---|
| 1 | `1108` | `update_player` | Обновляет скорость, камеру, горизонтальную инерцию, рельеф, столкновения и оружие игрока. |
| 2 | `176C` | `update_looping_anim4` | Циклическая анимация frame=(++age>>1)&3 и culling. |
| 3 | `1783` | `update_pingpong_animation` | Анимация через шестизначную таблицу кадров; после фазы 5 сбрасывает счётчик. |
| 4 | `10C6` | `cull_entity_after_view` | Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024. |
| 5 | `17AC` | `update_aimed_emitter_a` | Выбирает направление на игрока и вызывает emitter; перезарядка через знаковый DEC с периодом сброса 8. |
| 6 | `17D6` | `update_aimed_emitter_b` | Целится в игрока; по окончании счётчика создаёт навесной снаряд и ставит счётчик 18. |
| 7 | `18A9` | `update_aimed_emitter_c` | Целится и порождает наземный снаряд при истечении счётчика, затем сброс 40. |
| 8 | `1A9E` | `update_refill_gate` | Центрирует игрока у объекта: узкая центральная зона добавляет side_charge_delta, соседняя обнуляет shield. |
| 9 | `10C6` | `cull_entity_after_view` | Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024. |
| 10 | `1993` | `update_tunnel_entrance_a` | Общий вход в туннель плюс выбор destination/тип награды из таблицы A. |
| 11 | `19C2` | `update_tunnel_entrance_b` | Вход в туннель по таблице B; помечает перезагрузку карты и циклически меняет part 0..2. |
| 12 | `2E02` | `update_tunnel_player` | Обновляет скорость/дистанцию, угловую инерцию, позицию по lookup-таблицам и боковые действия. |
| 13 | `10C6` | `cull_entity_after_view` | Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024. |
| 14 | `2FE8` | `update_tunnel_exit_marker` | Выбирает размер кольца и кадр выходного маркера по оставшейся дистанции. |
| 15 | `300A` | `update_tunnel_pickup` | По дистанции от появления вычисляет масштаб/позицию объекта; при совпадении сектора игрока собирает. |
| 16 | `16FE` | `update_five_frame_explosion` | Выбирает frame=(++age>>2)&7; кадры 0..4, затем удаление. |
| 17 | `12CB` | `update_player_death` | Замедляет прокрутку, обновляет положение и по достижении 40 тиков запрашивает respawn. |
| 18 | `10C6` | `cull_entity_after_view` | Удаляет сущность по верхнему краю относительно camera_y+160 в кольце 1024. |
| 19 | `12EB` | `update_player_projectile` | Двигает выстрел по y на -16, меняет высоту, проверяет рельеф и объект строки. |
| 20 | `15FC` | `update_enemy_projectile` | Движение в пяти направлениях, коррекция высоты, столкновение с рельефом и игроком. |
| 21 | `16DF` | `update_four_frame_impact` | Проигрывает четыре sprite-offset из таблицы; затем очищает flags. |
| 22 | `16D6` | `update_countdown_effect` | Декрементирует frame/state и удаляет объект при установленном SF. |
| 23 | `1740` | `update_side_impact` | Четырёхкадровый эффект; по окончании освобождает соответствующий бит side_impact_mask. |
| 24 | `1384` | `update_side_device_follow` | Следует истории игрока; при состоянии кнопки 3 переключается на автономный handler 25. |
| 25 | `13A8` | `update_ground_projectile` | Продвигает объект на -6 по y, привязывает к рельефу, проверяет препятствия и срок жизни. |
| 26 | `14F0` | `update_mobile_enemy` | Двигает сущность на +1 по y, привязывает к рельефу, проверяет игрока и его снаряды, стреляет. |
| 27 | `10F3` | `update_accelerating_effect` | Увеличивает state_16 и двигает y назад на min(counter,8), затем проверяет видимость. |
| 28 | `18C7` | `update_level_exit_gate` | Открывает ворота лишь после всех 6 collectibles, подтягивает игрока к центру и запускает переход. |
| 29 | `1429` | `update_ground_enemy_shot` | Двигает наземный выстрел в одном из 5 направлений и проверяет близость игрока. |
| 30 | `17F4` | `update_arcing_projectile` | Движение в 5 направлениях и изменение высоты по 16-байтовой таблице; затем эффект. |
| 31 | `171F` | `update_six_frame_effect` | Выбирает frame=(++age>>2)&7; кадры 0..5, затем удаление. |
| 32 | `1B09` | `update_collectible` | При близости игрока очищает flags, засчитывает collectible[param] и запускает звук. |
| 33 | `1B4A` | `update_drifting_effect` | Сдвигает y на -2 с маской 03FFh и проверяет видимость. |
| 34 | `2F50` | `update_tunnel_side_device` | Следует задержанному углу; при выпуске/отключении исчезает и собирает все handler15 в пуле. |
| 35 | `12A7` | `update_player_forced_stop` | Уменьшает скорость на 3, при отрицательном результате запускает смерть; удерживает кадр 10. |

35ID отображаются в меньшее число уникальных процедур. ID4/9/13/18, например, используют общий culling. Нельзя присвоить каждому адресу единственное визуальное имя типа объекта.

## FrameSet: structure of arrays

Первый байт N. Затем N widths, N heights, N signed anchors X и N signed anchors Y; затем N пар `{u16 bank_offset, u16 sprite_offset}`. Общий размер `1+8*N`. Для frame i адрес пары = `base+1+4*N+4*i`. `apply_sprite_frame` копирует выбранные поля в Entity, не создавая отдельный Sprite-объект. Найдены и размечены29 наборов с279 кадрами в сумме.

## Карты и размещение

Таблица DS:0214 содержит24 указателя RLE тайлов (8 уровней ×3 части), DS:0244 —24 указателя потоков размещения. Каждая карта декодируется ровно в768 байт =64 строки по12 колонок. В `src/data/03_levels.inc` токены представлены макросами `MAP_RUN tile, count`, сохраняющими точный байт оригинала; исходное разделение runs не объединялось.

Поток размещения: `kind, column_index, source_row`, затем FF как отдельный терминатор. `stored_row=63-source_row`. Таблица DS:0CD7 задаёт бит колонки; запись строки объединяет бит колонки с kind в старших битах. JSON `analysis/levels.json` содержит исходные токены, матрицы12×64 и тройки всех24 частей; это удобный исходный материал для инструментов порта.

## Рельеф

Тайлы24×16 имеют профиль8×8: колонка профиля = remainder((x+16)/24)/3, строка профиля = (y&15)/2. Профиль выбирается таблицей word offsets DS:0643. Младшие5 бит его байта задают signed height; старший бит выбирает одну из таблиц вариантов DS:059B/05C5. Накопленные height deltas предыдущих тайлов строки (DS:0571) прибавляются в8-битной арифметике, затем результат расширяется со знаком.

## Счёт и HISCORES

DS:9455..94A8: семь имён по12 байт. DS:94A9..94D2: семь результатов по6 десятичных цифр, старшая первой. DS:94D3..94D9: семь scratch-индексов; DS:94DA..94E0: семь индексов порядка. Суммарно140 байт. Текущий счёт занимает первый шестибайтовый результат. Хранить число как uint32_t в интерфейсе можно только с точным преобразованием и сохранением правил carry/extra life.

## Звуковые таблицы

DS:8BE4 —15 указателей на6-байтовые descriptor: `u8 modulation`, `u8 duration`, `u16 divisor`, `i16 divisor_step`. DS:8C5C —четыре указателя на16-словные циклы модуляции. Два descriptor меняют pitch во время игры; descriptor15 используется для нот. Поэтому эти данные нельзя объявить безусловным immutable ROM без отделения изменяемых полей.

## Неустановленные данные

Отдельный модуль DS:2036..62B5 сохраняется как явно нерасшифрованный. Его наличие не означает, что из ASM исключён код: все6022 выделенные инструкции остаются инструкциями. Остальные gaps и `D_unknown_*` требуют последующего сопоставления с использованием. Ресурсные .X сохранены; полное визуальное декодирование каждого банка не выполнялось.

## Полный индекс DS offsets

418 символов включают aliases частей массивов и записей; это не число независимых global variables. Текущие обращения к каждому символу есть в `analysis/global_xrefs.json`.

| DS | Символ | Тип/форма | Значение |
|---|---|---|---|
| `0006` | `D_attract_seen` | u8 | Первый attract уже завершён. |
| `0007` | `D_msg_insufficient_ram` | DOS string | Сообщение о недостатке памяти, завершается долларом. |
| `002A` | `D_msg_loading` | DOS string | Сообщение о загрузке, завершается долларом. |
| `005A` | `D_key_to_character` | u8[128] | Таблица scan-code → символ для ввода имени. |
| `00DA` | `D_attract_script_scores` | byte stream | Сценарий после игровой сессии; первый читаемый элемент имеет индекс1. |
| `00E9` | `D_attract_script_intro` | byte stream | Сценарий первого показа; первый читаемый элемент имеет индекс1. |
| `0109` | `D_template_attract` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0120` | `D_sprite_frames_0120` | FrameSet[24] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `01B1` | `D_highscore_layout` | records | Координаты/смещения для имён и результатов шести строк. |
| `01B3` | `D_highscore_name_destinations` | strided u16 | Первое поле адреса имени; следующий элемент через8 байт. |
| `01E2` | `D_filename_bird1` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `01EA` | `D_filename_bird2` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `01F2` | `D_filename_blox` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `01F9` | `D_filename_newbird` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `0203` | `D_filename_podz1` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `020B` | `D_filename_intro` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `0214` | `D_level_tile_streams` | u16[24] | 8 уровней ×3 части: DS offsets RLE-потоков 12×64. |
| `0244` | `D_level_object_streams` | u16[24] | Потоки троек размещения объектов, терминатор FFh. |
| `0274` | `D_template_map_tile` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0277` | `D_map_tile_template_z` | i8 | Высотное поле шаблона DS:0274. |
| `0279` | `D_map_tile_template_x` | u16 | Поле x шаблона DS:0274. |
| `027B` | `D_map_tile_template_y` | u16 | Поле world_y шаблона DS:0274. |
| `027D` | `D_map_tile_template_width` | u8 | Поле width шаблона DS:0274. |
| `027E` | `D_map_tile_template_height` | u8 | Поле height шаблона DS:0274. |
| `0281` | `D_map_tile_template_bank` | u16 | Смещение в segment table, не номер файла. |
| `0283` | `D_map_tile_template_sprite` | u16 | Offset packed4bpp-изображения тайла. |
| `028B` | `D_map_entity_templates` | u16[21] | Шаблон Entity по kind, извлечённому из маски строки. |
| `02B5` | `D_map_collision_damage` | u8[21] | Код/урон препятствия; 0 означает отсутствие столкновения. |
| `02CA` | `D_map_collision_halfwidth` | u16[21] | Полуширина области x для препятствия. |
| `02F4` | `D_map_collision_rightmost_x` | u16[21] | Базовая координата первого проверяемого из10 столбцов. |
| `031E` | `D_entity_replacement_templates` | u16[8] | Шаблон по прежнему handler_id; только подмножество ID используется. |
| `032E` | `D_collectible_sprite_offsets` | u16[6] | Шесть изображений коллекционных объектов. |
| `033A` | `D_template_map_kind_00` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0351` | `D_template_map_kind_01` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0368` | `D_template_map_kind_02` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `037F` | `D_template_map_kind_03` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0396` | `D_template_map_kind_04` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `03AD` | `D_template_map_kind_05` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `03C4` | `D_template_map_kind_06` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `03DB` | `D_template_map_kind_10` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `03F2` | `D_template_map_kind_08` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0409` | `D_template_map_kind_09` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0420` | `D_template_map_kind_07` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0437` | `D_template_map_kind_11` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `044E` | `D_template_map_kind_12` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0465` | `D_template_map_kind_14` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `047C` | `D_template_map_kind_13` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0493` | `D_template_map_kind_15` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `04AA` | `D_template_collectible_00` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `04C1` | `D_template_collectible_03` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `04D8` | `D_template_collectible_04` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `04EF` | `D_template_collectible_05` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0506` | `D_template_collectible_06` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `051D` | `D_tile_heights` | u8[42] | Высота изображения тайла в пикселях. |
| `0547` | `D_tile_y_adjust` | i8[42] | Поправка y изображения тайла. |
| `0571` | `D_tile_height_delta` | i8[42] | Перепад высоты от левого к правому краю тайла. |
| `059B` | `D_tile_surface_variant_a` | u8[42] | Вариант frame/surface при очищенном бите80h sample. |
| `05C5` | `D_tile_surface_variant_b` | u8[42] | Вариант при установленном бите80h sample. |
| `05EF` | `D_tile_sprite_offsets` | u16[42] | Смещения packed-изображений тайлов в выбранном bank. |
| `0643` | `D_tile_profile_offsets` | u16[42] | Указатели на 64-байтовые микропрофили 8×8. |
| `0CD7` | `D_map_column_bits` | u16[10] | Биты столбцов: 0200h..0001h. |
| `0CEB` | `D_map_column_clear_masks` | u16[12] | Инверсные маски для пометки использованного объекта; значения сохранить точно. |
| `0D04` | `D_template_tunnel_player` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0D1B` | `D_template_tunnel_side` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0D32` | `D_template_surface_player` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0D49` | `D_sprite_frames_0d49` | FrameSet[11] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0DA2` | `D_template_respawn_effect` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0DB9` | `D_side_device_z_offsets` | i8 table | Поправка высоты бокового устройства по frame игрока. |
| `0DC3` | `D_template_side_device` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0DDA` | `D_sprite_frames_0dda` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0E2B` | `D_sprite_frames_0e2b` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0E4C` | `D_pingpong_frame_sequence` | u8[6] | Последовательность циклической анимации handler3. |
| `0E52` | `D_sprite_frames_0e52` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0E73` | `D_sprite_frames_0e73` | FrameSet[5] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0E9C` | `D_sprite_frames_0e9c` | FrameSet[5] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0EC5` | `D_sprite_frames_0ec5` | FrameSet[5] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0EEE` | `D_sprite_frames_0eee` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0F3F` | `D_sprite_frames_0f3f` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0F90` | `D_sprite_frames_0f90` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `0FE1` | `D_mobile_enemy_descriptors` | u16[3] | Таблица descriptor по param мобильного врага. |
| `0FE7` | `D_shot_variation_sequence` | u8[16] | Детерминированная циклическая вариация; не PRNG. |
| `0FF7` | `D_template_muzzle_effect` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `0FFA` | `D_shot_effect_template_z` | i8 | Высота шаблона DS:0FF7. |
| `0FFB` | `D_shot_effect_template_frame` | u8 | Frame/state шаблона эффекта. |
| `0FFC` | `D_shot_effect_template_x` | u16 | Координата x шаблона эффекта. |
| `0FFE` | `D_shot_effect_template_y` | u16 | Координата y шаблона эффекта. |
| `100E` | `D_sprite_frames_100e` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `102F` | `D_aimed_effect_descriptors` | u16[5] | Sprite descriptors по направлению выстрела. |
| `1039` | `D_sprite_frames_1039` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `105A` | `D_sprite_frames_105a` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `107B` | `D_sprite_frames_107b` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `109C` | `D_sprite_frames_109c` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `10BD` | `D_sprite_frames_10bd` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `10DE` | `D_ground_shot_dx` | i8[5] | Поправки spawn x по направлению. |
| `10E3` | `D_ground_shot_dy` | i8[5] | Поправки spawn y по направлению. |
| `10E8` | `D_template_ground_shot` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `10FF` | `D_ground_shot_descriptors` | u16[5] | Sprite descriptors наземного снаряда. |
| `1109` | `D_sprite_frames_1109` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `115A` | `D_sprite_frames_115a` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `11AB` | `D_sprite_frames_11ab` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `11FC` | `D_sprite_frames_11fc` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `124D` | `D_sprite_frames_124d` | FrameSet[10] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `129E` | `D_template_explosion` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `12A0` | `D_explosion_template_layer` | u8 | Layer шаблона взрыва DS:129E. |
| `12A1` | `D_explosion_template_z` | i8 | Z шаблона взрыва. |
| `12A3` | `D_explosion_template_x` | u16 | X шаблона взрыва. |
| `12A5` | `D_explosion_template_y` | u16 | Y шаблона взрыва. |
| `12B5` | `D_sprite_frames_12b5` | FrameSet[5] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `12DE` | `D_template_arc_effect` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `12F5` | `D_sprite_frames_12f5` | FrameSet[6] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `1326` | `D_side_impact_template` | Entity | Шаблон эффектов двух сторон; flags20h меняется перед копированием. |
| `133D` | `D_sprite_frames_133d` | FrameSet[4] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `135E` | `D_template_player_shot` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `1361` | `D_player_shot_template_z` | i8 | Поле шаблона player shot DS:135E. |
| `1363` | `D_player_shot_template_x` | u16 | Поле шаблона player shot. |
| `1365` | `D_player_shot_template_y` | u16 | Поле шаблона player shot. |
| `1375` | `D_player_left_muzzle_dx` | i8[10] | Смещения левого ствола по frame. |
| `137F` | `D_player_right_muzzle_dx` | i8[10] | Смещения правого ствола по frame. |
| `1389` | `D_player_left_muzzle_dz` | i8[10] | Поправка высоты левого ствола. |
| `1393` | `D_player_right_muzzle_dz` | i8[10] | Поправка высоты правого ствола. |
| `139D` | `D_player_shot_frame_map` | u8[10] | Выбор варианта выстрела по frame игрока. |
| `13A7` | `D_shot_variant_heights` | u8[3] | Высоты трёх вариантов выстрела. |
| `13AA` | `D_player_shot_sprite_offsets` | u16[3] | Sprite offsets выстрела игрока. |
| `13B0` | `D_template_enemy_shot` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `13C7` | `D_enemy_shot_sprite_offsets` | u16[3] | Sprite offsets выстрела противника. |
| `13CD` | `D_enemy_muzzle_a_left_dz` | i8[10] | Таблица высот pattern0, левая точка. |
| `13D7` | `D_enemy_muzzle_a_right_dz` | i8[10] | Таблица высот pattern0, правая точка. |
| `13E1` | `D_enemy_muzzle_b_dz` | i8[10] | Таблица высот pattern1. |
| `13EB` | `D_enemy_muzzle_c_left_dz` | i8[10] | Таблица высот pattern2, левая точка. |
| `13F5` | `D_enemy_muzzle_c_center_dz` | i8[10] | Таблица высот pattern2, центральная точка. |
| `13FF` | `D_enemy_muzzle_c_right_dz` | i8[10] | Таблица высот pattern2, правая точка. |
| `1409` | `D_aimed_muzzle_dx` | i8[5] | X offset по направлению. |
| `140E` | `D_aimed_muzzle_dy` | i8[5] | Y offset по направлению. |
| `1413` | `D_enemy_shot_widths` | u8[5] | Ширина выстрела по направлению. |
| `1418` | `D_enemy_shot_heights` | u8[5] | Высота выстрела по направлению. |
| `141D` | `D_enemy_aimed_shot_sprites` | u16[5] | Sprite offsets направленных выстрелов. |
| `1427` | `D_template_impact` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `143E` | `D_impact_frame_sprites` | u16[4] | Четыре последовательных кадра попадания. |
| `1446` | `D_template_arc_shot` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `145D` | `D_arc_spawn_dx` | i8[5] | X offset навесного снаряда. |
| `1462` | `D_arc_spawn_dy` | i8[5] | Y offset навесного снаряда. |
| `1467` | `D_arc_height_deltas` | i8[16] | Вертикальные приращения по возрасту снаряда. |
| `1477` | `D_tunnel_gate_frame_sprites` | u16[6] | Кадры створок входа; fully-open скрывает layer. |
| `1483` | `D_exit_gate_frame_sprites` | u16[9] | Кадры ворот выхода после6 collectibles. |
| `1495` | `D_tunnel_destinations_a` | u16[24] | Destination_y в10 битах и reward kind в старших2. |
| `14C5` | `D_tunnel_destinations_b` | u16[24] | Вторая таблица входов/выходов туннеля. |
| `14F5` | `D_level_start_y` | u16[8] | Исходная позиция камеры по level_index. |
| `1506` | `D_filename_hiscores` | ASCIIZ | Имя внешнего файла, нулевой терминатор. |
| `150F` | `D_control_prompt_banks` | u16[8] | Bank offsets фрагментов запросов управления. |
| `151F` | `D_control_prompt_sources` | u16[8] | Sprite offsets запросов управления. |
| `152F` | `D_control_prompt_destinations` | u16[8] | Адреса назначения в video RAM, не DS-указатели. |
| `153F` | `D_control_prompt_widths` | u16[8] | Ширины фрагментов в пикселях. |
| `154F` | `D_control_prompt_heights` | u16[8] | Высоты фрагментов. |
| `155F` | `D_hud_digit_sprites` | u16[10] | Sprite offsets цифр HUD. |
| `1573` | `D_hud_digit_destinations` | u16[6] | Адреса цифр в video RAM. |
| `157F` | `D_highscore_digit_sprites` | u16[10] | Sprite offsets цифр таблицы рекордов. |
| `1594` | `D_sprite_frames_1594` | FrameSet[28] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `1675` | `D_tunnel_circle_x` | i8[128] | Готовые x координаты окружности; сохранить lookup, не заменять sin/cos. |
| `16F5` | `D_tunnel_circle_y` | i8[128] | Готовые y координаты окружности. |
| `1775` | `D_tunnel_player_frames` | u8[128] | Frame корабля по углу. |
| `17F5` | `D_tunnel_frame_to_sector` | u8[28] | Сектор сбора предмета по frame игрока; фактический домен см. таблицу. |
| `1811` | `D_sprite_frames_1811` | FrameSet[16] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `1892` | `D_tunnel_side_frames` | u8[128] | Frame бокового устройства по углу игрока. |
| `1912` | `D_template_tunnel_exit` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `1929` | `D_sprite_frames_1929` | FrameSet[24] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `19EA` | `D_tunnel_exit_frames` | u8[32] | Анимация выходного маркера по remaining distance. |
| `1A0A` | `D_tunnel_min_ring_radii` | u8[32] | Минимальный радиус колец по дистанции. |
| `1A2A` | `D_tunnel_pattern_pointers` | u16[4] | Четыре потока появления туннельных предметов. |
| `1A6A` | `D_tunnel_pickup_radii` | u8[48] | Радиус изображения по дистанции от spawn. |
| `1A9A` | `D_tunnel_pickup_layers` | u8[24] | Layer по кадру приближения предмета. |
| `1AB2` | `D_template_tunnel_pickup` | Entity template | Изменяемый шаблон 23 байта; структура см. entity.inc. |
| `1AC9` | `D_sprite_frames_1ac9` | FrameSet[24] | SoA: count, width[], height[], anchor_x[], anchor_y[], {bank, offset}[]. |
| `1B8A` | `D_entity_records` | Entity[48] | Пул сущностей: 48 ×23 байта; первая запись — игрок. |
| `1B8C` | `D_player_layer` | u8 | Поле+02 первой записи пула. |
| `1B8D` | `D_player_z` | i8 | Поле+03 первой записи; знаковая высота/проекционное смещение. |
| `1B8E` | `D_player_frame` | u8 | Поле+04 первой записи, также вариант рельефа. |
| `1B8F` | `D_player_x` | u16 | Поле+05 первой записи. |
| `1B91` | `D_player_y` | u16 | Поле+07 первой записи; modulo1024 на поверхности. |
| `1B9C` | `D_player_handler` | u8 | Поле+12 первой записи, состояние игрока. |
| `1B9E` | `D_player_map_row` | u8 | Поле+14 первой записи; в туннеле пересекается с иными state. |
| `1BA0` | `D_player_angle_or_age` | u8 | Поле+16: в туннеле угол, в иных состояниях счётчик. |
| `1FDC` | `D_tunnel_ring_radius_curve` | u8[64] | Радиус по младшим6 битам дистанции. |
| `201C` | `D_tunnel_edge_x_curve` | u8[13] | Дополнительные линии при phase<=12. |
| `2029` | `D_tunnel_edge_y_curve` | u8[13] | Вторая таблица дополнительных линий. |
| `62B6` | `D_level_01_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `6445` | `D_level_01_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `65D4` | `D_level_01_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `6755` | `D_level_01_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `678C` | `D_level_01_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `67C3` | `D_level_01_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `67F7` | `D_level_02_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `697F` | `D_level_02_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `6B3B` | `D_level_02_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `6CAC` | `D_level_02_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `6CE9` | `D_level_02_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `6D29` | `D_level_02_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `6D5D` | `D_level_03_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `6ED7` | `D_level_03_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7053` | `D_level_03_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `71DC` | `D_level_03_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `7228` | `D_level_03_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `725F` | `D_level_03_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `7293` | `D_level_04_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7416` | `D_level_04_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `75A8` | `D_level_04_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7738` | `D_level_04_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `7781` | `D_level_04_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `77B8` | `D_level_04_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `77FC` | `D_level_05_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7992` | `D_level_05_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7AEE` | `D_level_05_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7C86` | `D_level_05_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `7CB7` | `D_level_05_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `7CEE` | `D_level_05_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `7D2E` | `D_level_06_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7E7E` | `D_level_06_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `7FBC` | `D_level_06_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `8108` | `D_level_06_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `815A` | `D_level_06_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `818B` | `D_level_06_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `81E3` | `D_level_07_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `832D` | `D_level_07_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `8439` | `D_level_07_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `8596` | `D_level_07_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `85D9` | `D_level_07_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `8610` | `D_level_07_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `8659` | `D_level_08_part_1_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `87F7` | `D_level_08_part_2_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `89AD` | `D_level_08_part_3_tiles` | RLE stream | Точные RLE-токены; декодирование даёт12×64 клетки. |
| `8B0F` | `D_level_08_part_1_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `8B5B` | `D_level_08_part_2_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `8BA4` | `D_level_08_part_3_objects` | placement stream | Тройки kind,column,source_row; FFh завершает поток. |
| `8BE4` | `D_sound_descriptor_pointers` | u16[15] | Таблица 6-байтовых звуковых descriptor. |
| `8C02` | `D_sound_descriptor_01` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C08` | `D_sound_descriptor_02` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C0E` | `D_sound_descriptor_03` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C14` | `D_sound_descriptor_04` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C1A` | `D_sound_descriptor_05` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C20` | `D_sound_descriptor_06` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C26` | `D_sound_descriptor_07` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C2C` | `D_sound_descriptor_08` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C2E` | `D_charge_sound_divisor` | u16 | Динамически меняется по side_charge_units. |
| `8C32` | `D_sound_descriptor_09` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C34` | `D_angle_sound_divisor` | u16 | Динамически меняется по углу игрока. |
| `8C38` | `D_sound_descriptor_10` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C3E` | `D_sound_descriptor_11` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C44` | `D_sound_descriptor_12` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C4A` | `D_sound_descriptor_13` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C50` | `D_sound_descriptor_14` | SoundDescriptor | modulation u8, duration u8, divisor u16, step i16; 6 bytes. |
| `8C56` | `D_music_descriptor_modulation` | u8 | Первый байт изменяемого descriptor15. |
| `8C57` | `D_music_descriptor_duration` | u8 | Время изменяемого descriptor15. |
| `8C58` | `D_music_descriptor_divisor` | u16 | PIT-divisor выбранной ноты. |
| `8C5C` | `D_sound_modulation_pointers` | u16[4] | Указатели на циклы модуляции по16 слов. |
| `8C64` | `D_sound_modulation_1` | i16[16] | Циклическая добавка к делителю; индекс&15. |
| `8C84` | `D_sound_modulation_2` | i16[16] | Циклическая добавка к делителю; индекс&15. |
| `8CA4` | `D_sound_modulation_3` | i16[16] | Циклическая добавка к делителю; индекс&15. |
| `8CC4` | `D_sound_modulation_4` | i16[16] | Циклическая добавка к делителю; индекс&15. |
| `8CE4` | `D_note_divisors` | u16 table | Делитель PIT2 по note index. |
| `8E5D` | `D_music_fill_notes` | u8[32] | Ноты специальной нулевой команды музыки. |
| `8E7E` | `D_attract_script_pointer` | u16 | DS offset сценария заставки. |
| `8E80` | `D_attract_step_counter` | u16 | Счётчик кадров сценария; команда каждую8-ю итерацию. |
| `8E82` | `D_old_keyboard_segment` | u16 | Сегмент старого INT09h. |
| `8E84` | `D_old_keyboard_offset` | u16 | Offset старого INT09h. |
| `8E86` | `D_old_timer_segment` | u16 | Сегмент старого INT1Ch. |
| `8E88` | `D_old_timer_offset` | u16 | Offset старого INT1Ch. |
| `8E8A` | `D_keyboard_state` | u8[128] | 0 = отпущена, 1 = нажата; индекс — scan-code. |
| `8F0A` | `D_mouse_present` | u8 | Признак установленного драйвера мыши. |
| `8F0B` | `D_last_key_pointer` | u16 | DS offset последней клавиши при вводе имени. |
| `8F0D` | `D_key_left` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F0F` | `D_key_right` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F11` | `D_key_up` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F13` | `D_key_down` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F15` | `D_key_fire` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F17` | `D_key_action_n` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F19` | `D_key_action_m` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F1B` | `D_key_quit` | u16 | Настроенный scan-code; фактически используется младший байт. |
| `8F1E` | `D_file_handle` | u16 | DOS file handle. |
| `8F20` | `D_file_destination_segment` | u16 | Временный сегмент назначения DOS read. |
| `8F22` | `D_map_running_height` | u8 | Накопленная высота вдоль текущей строки тайлов. |
| `8F23` | `D_map_row_y` | u16 | Текущая y-строка карты при prerender. |
| `8F25` | `D_map_tile_cursor` | u16 | DS offset следующего декодированного тайла. |
| `8F29` | `D_decoded_map_tiles` | u8[768] | 12 столбцов ×64 строки; хранение обратное world_y. |
| `9229` | `D_map_spawn_rows` | u16[64] | 10 бит колонок +6 бит вида объекта строки. |
| `92A9` | `D_map_available_masks` | u16[3][64] | Сброшенный бит означает использованный объект этой части уровня. |
| `9429` | `D_collision_damage_tmp` | u8 | Временный результат query_player_map_collision. |
| `942C` | `D_collision_column_tmp` | u16 | Текущий столбец поиска препятствия. |
| `942E` | `D_sprite_dest_segment` | u16 | EGA segment для следующего blit. |
| `9430` | `D_sprite_clip_enabled` | u16 | 1 для игрового экрана, 0 для prerender карты. |
| `9432` | `D_player_forced_stop` | u8 | Запрос отдельного состояния игрока после выхода из туннеля. |
| `9433` | `D_player_history_x0` | u16 | Новейшая из4 сохранённых координат x. |
| `9435` | `D_player_history_x1` | u16 | История x, задержка1. |
| `9437` | `D_player_history_x2` | u16 | История x, задержка2. |
| `9439` | `D_player_history_x3` | u16 | История x, задержка3. |
| `943B` | `D_player_history_y0` | u16 | Новейшая y. |
| `943D` | `D_player_history_y1` | u16 | История y1. |
| `943F` | `D_player_history_y2` | u16 | История y2. |
| `9441` | `D_player_history_y3` | u16 | История y3. |
| `9443` | `D_player_history_frame0` | u8 | Новейший frame. |
| `9444` | `D_player_history_frame1` | u8 | История frame1. |
| `9445` | `D_player_history_frame2` | u8 | История frame2. |
| `9446` | `D_player_history_frame3` | u8 | История frame3. |
| `9447` | `D_player_history_z0` | i8 | Новейшая высота. |
| `9448` | `D_player_history_z1` | i8 | История z1. |
| `9449` | `D_player_history_z2` | i8 | История z2. |
| `944A` | `D_player_history_z3` | i8 | История z3. |
| `944C` | `D_hud_frame_counter` | u8 | Очки за этап начисляются при (counter&3)==0. |
| `944D` | `D_score_leading_zero` | u8 | Флаг подавления ведущих нулей. |
| `944E` | `D_text_draw_offset` | u16 | Offset в PODZ1 для текста таблицы рекордов. |
| `9450` | `D_pending_highscore_rank` | u8 | Строка редактируемого имени. |
| `9451` | `D_pending_name_pointer` | u16 | DS offset 12-байтового имени. |
| `9453` | `D_name_cursor` | u16 | Позиция0..12 при редактировании имени. |
| `9455` | `D_highscore_data` | u8[140] | HISCORES: 84 байта имён, 42 цифр, 7 scratch, 7 индексов. |
| `94A9` | `D_score_digit_100000` | u8 | Первая из6 цифр текущего счёта; начало7 результатов. |
| `94AA` | `D_score_digit_10000` | u8 | Десятичная цифра10000. |
| `94AB` | `D_score_digit_1000` | u8 | Десятичная цифра1000. |
| `94AC` | `D_score_digit_100` | u8 | Десятичная цифра100. |
| `94AD` | `D_score_digit_10` | u8 | Десятичная цифра10. |
| `94AE` | `D_score_digit_1` | u8 | Десятичная цифра1. |
| `94D3` | `D_highscore_sort_scratch0` | u8 | Первый из7 scratch индексов сортировки. |
| `94D4` | `D_highscore_sort_scratch1` | u8 | Scratch1. |
| `94D5` | `D_highscore_sort_scratch2` | u8 | Scratch2. |
| `94D6` | `D_highscore_sort_scratch3` | u8 | Scratch3. |
| `94D7` | `D_highscore_sort_scratch4` | u8 | Scratch4. |
| `94D8` | `D_highscore_sort_scratch5` | u8 | Scratch5. |
| `94D9` | `D_highscore_sort_scratch6` | u8 | Scratch6. |
| `94DA` | `D_highscore_order0` | u8 | Первый из7 отсортированных индексов. |
| `94DB` | `D_highscore_order1` | u8 | Order1. |
| `94DC` | `D_highscore_order2` | u8 | Order2. |
| `94DD` | `D_highscore_order3` | u8 | Order3. |
| `94DE` | `D_highscore_order4` | u8 | Order4. |
| `94DF` | `D_highscore_order5` | u8 | Order5. |
| `94E0` | `D_highscore_order6` | u8 | Последний индекс; 0 означает, что текущий результат не вошёл в6 лучших. |
| `94E2` | `D_tunnel_pattern_pointer` | u16 | DS offset потока объектов туннеля. |
| `94E4` | `D_tunnel_pattern_index` | u8 | Циклический выбор паттерна по &3. |
| `94E5` | `D_tunnel_angle_history0` | u8 | История угла0. |
| `94E6` | `D_tunnel_angle_history1` | u8 | История угла1. |
| `94E7` | `D_tunnel_angle_history2` | u8 | История угла2. |
| `94E8` | `D_segment_bird1` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94EA` | `D_segment_bird2` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94EC` | `D_segment_blox` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94EE` | `D_segment_newbird` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94F0` | `D_segment_podz1` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94F2` | `D_segment_intro` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94F4` | `D_segment_scratch0` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94F6` | `D_segment_scratch1` | u16 segment | Сегмент 32-KiB банка; индекс E_BANK — байтовое смещение в этой таблице. |
| `94F8` | `D_video_draw_segment` | u16 | Сегмент текущей экранной страницы для рисования. |
| `94FA` | `D_video_display_segment` | u16 | Сегмент второй видеостраницы. |
| `94FC` | `D_base_scroll_speed` | u16/mixed | Базовая скорость; start_game_cycle пишет только младший байт. |
| `94FE` | `D_stage_counter` | u8 | Число этапов, влияет на базовую скорость и очки. |
| `94FF` | `D_tunnel_mode` | u8 | 0=поверхность,1=туннель. |
| `9500` | `D_game_loop_end_flag` | u8 | Запрос завершить game_loop. |
| `9501` | `D_level_index` | u8 | Индекс уровня 0..7. |
| `9502` | `D_level_part` | u8 | Часть уровня 0..2. |
| `9503` | `D_attract_active` | u8 | Маркер заставки; не подменять им состояние главного цикла. |
| `9504` | `D_timer_ticks` | u8 | Счётчик INT1Ch; переполнение modulo256. |
| `9505` | `D_sound_toggle_state` | u8 | Автомат кнопки звука0/1/2/3. |
| `9506` | `D_sound_enabled` | u8 | Разрешение звуков/музыки. |
| `9507` | `D_last_spawn_row` | u16 | Последняя обработанная строка перед камерой. |
| `9509` | `D_dispatch_mode_snapshot` | u8 | Режим на входе в проход update_entities. |
| `950A` | `D_surface_resume_x` | u16 | X для создания игрока после смены режима. |
| `950C` | `D_surface_resume_y` | u16 | Y камеры при выходе из туннеля. |
| `950E` | `D_map_reload_pending` | u8 | Запрос rebuild после перехода между частями. |
| `950F` | `D_fuel_distance_accumulator` | u16 | Разные пороги расхода в surface/tunnel режимах. |
| `9511` | `D_ammo_use_accumulator` | u16 | Отложенный расход шкалы стрельбы. |
| `9513` | `D_player_respawn_pending` | u8 | Запрос смерти/respawn dispatcher. |
| `9514` | `D_player_death_age` | u8 | Таймер handler17. |
| `9515` | `D_player_zero_frame_age` | u8 | Срок пребывания в frame0 до смерти. |
| `9516` | `D_shot_variation_index` | u8 | Циклическая последовательность16 значений, не RNG. |
| `9517` | `D_side_impact_mask` | u8 | Занятые эффекты левого/правого ствола01h/80h. |
| `9518` | `D_spawn_projectile_gate` | u8 | Разрешает реальный снаряд вместе с эффектом. |
| `9519` | `D_respawn_effect_pending` | u8 | Создать эффект восстановления игрока. |
| `951A` | `D_left_device_state` | u8 | Автомат левой кнопки/устройства0..3. |
| `951B` | `D_right_device_state` | u8 | Автомат правой кнопки/устройства0..3. |
| `951C` | `D_tunnel_flash_state` | u8 | Вспышка:2→1→0. |
| `951D` | `D_tunnel_reward_kind` | u8 | 40 ammo,80 fuel,C0 shield,F0 переход уровня,прочее score. |
| `951E` | `D_tunnel_angular_velocity` | i16 | Инерция угла, clamp[-64,64]. |
| `9520` | `D_tunnel_passive_force` | i16 | Пассивная добавка угловой скорости. |
| `9522` | `D_tunnel_input_force` | i16 | Добавка от LEFT/RIGHT. |
| `9524` | `D_tunnel_exit_spawned` | u8 | Выходной marker уже создан. |
| `9525` | `D_tunnel_distance_remaining` | u16 | Дистанция до конца для маркера. |
| `9527` | `D_tunnel_last_section` | u8 | Последняя секция потока по distance>>6. |
| `9528` | `D_camera_y` | u16 | Кольцевая координата поверхности modulo1024. |
| `952A` | `D_scroll_speed` | u16 | Скорость прокрутки/движения. |
| `952C` | `D_player_x_velocity` | i16 | Горизонтальная инерция [-24,24] на поверхности. |
| `952E` | `D_collectible_0` | u8 | Collected flag0; подряд6 байтов. |
| `952F` | `D_collectible_1` | u8 | Collected flag1. |
| `9530` | `D_collectible_2` | u8 | Collected flag2. |
| `9531` | `D_collectible_3` | u8 | Collected flag3. |
| `9532` | `D_collectible_4` | u8 | Collected flag4. |
| `9533` | `D_collectible_5` | u8 | Collected flag5. |
| `9534` | `D_life_count` | u16 | Жизни0..3 на штатных путях. |
| `9536` | `D_side_charge_units` | u16 | Заряды боковых устройств0..6. |
| `9538` | `D_side_charge_delta` | i16 | Накопленное изменение зарядов; применяется HUD. |
| `953A` | `D_fuel_units` | u16 | Шкала движения0..44; имя fuel — вывод по использованию. |
| `953C` | `D_fuel_delta` | i16 | Отложенное изменение топлива; отрицательное значит один decrement. |
| `953E` | `D_ammo_units` | u16 | Шкала стрельбы0..48. |
| `9540` | `D_ammo_delta` | i16 | Отложенное изменение ammo; отрицательное значит один decrement. |
| `9542` | `D_shield_units` | u16 | Шкала урона0..56; имя shield условное. |
| `9544` | `D_shield_delta` | i16 | Накопленное изменение прочности. |
| `9546` | `D_pending_name_entry` | u8 | Нужно ввести имя для рекорда; выбирает музыку заставки. |
| `9547` | `D_tunnel_distance` | u16 | Пройденная дистанция0..896 на штатном пути. |
| `9549` | `D_tunnel_min_radius` | u8 | Порог уменьшения колец. |
| `954C` | `D_control_bits` | u16 | Маска управления; смысл битов см. constants.inc. |
| `954E` | `D_tunnel_ring_phase` | u16 | distance&63 для построения bitmap. |
| `9550` | `D_sound_active_id` | u8 | Активный sound ID;0=нет. |
| `9551` | `D_sound_pending_id` | u8 | Первый ожидающий эффект. |
| `9552` | `D_sound_priority_id` | u8 | Второй ожидающий; хранит максимальный ID из запросов. |
| `9553` | `D_sound_modulation_id` | u8 | 0=без модуляции; иначе индекс таблицы+1. |
| `9554` | `D_sound_modulation_phase` | u8 | Индекс последовательности по &15. |
| `9555` | `D_sound_modulation_pointer` | u16 | DS offset16 слов модуляции. |
| `9557` | `D_sound_ticks_left` | u8 | Тики до завершения эффекта. |
| `9558` | `D_sound_divisor` | u16 | Основной делитель PIT2. |
| `955A` | `D_sound_divisor_step` | i16 | Приращение делителя на тик (16-битный wrap). |
| `955C` | `D_sound_output_divisor` | u16 | Сумма основного делителя и модуляции. |
| `955E` | `D_music_active` | u8 | Запрещает обычные sound_trigger пока активен. |
| `955F` | `D_music_cursor` | u16 | Позиция в потоке команд музыки. |
| `9561` | `D_music_tick_divider` | u8 | Шаг последовательности раз в8 IRQ-тиков. |
| `9562` | `D_music_sequence_pointer` | u16 | DS offset выбранной последовательности. |
