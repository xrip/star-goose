/* Star Goose: readable byte-exact reconstruction, v2.
 * Читайте docs/ARCHITECTURE_RU.md и src/include/entity.inc.
 * Изменяются имена/комментарии/границы include; порядок и байты EXE неизменны.
 * Общие хвосты и дополнительные входы не превращены в новые CALL/RET.
 */
.include "src/include/encoding.inc"
.include "src/include/constants.inc"
.include "src/include/entity.inc"
.include "src/include/globals.inc"
.include "src/include/data_macros.inc"

.include "src/data/00_mz_header.inc"

.include "src/code/01_boot_and_attract.asm"    # Запуск, игровой цикл и заставка
.include "src/code/02_controls.asm"    # Управление и переназначение клавиш
.include "src/code/03_dos_interrupts.asm"    # Клавиатура, мышь, PIT и DOS vectors
.include "src/code/04_resource_io.asm"    # Загрузка ресурсов и файловый I/O
.include "src/code/05_world_map.asm"    # RLE карты, рельеф, размещение и столкновения
.include "src/code/06_entity_pool.asm"    # Пул, обход рендера и подготовка спрайтов
.include "src/code/07_sprite_blitter.asm"    # Packed4bpp → EGA и отсечение
.include "src/code/08_game_lifecycle.asm"    # Смена уровней, respawn, вход/выход туннеля
.include "src/code/09_entity_dispatch.asm"    # Таблица35 обработчиков и culling
.include "src/code/10_player_surface.asm"    # Движение игрока и кнопочные автоматы
.include "src/code/11_entity_behaviors.asm"    # Обработчики врагов, снарядов и эффектов
.include "src/code/12_player_world_state.asm"    # Камера, скорость, рельеф и смерть игрока
.include "src/code/13_weapons_and_frames.asm"    # Оружие, столкновения сущностей, кадры и прицеливание
.include "src/code/14_hud.asm"    # Шкалы ресурсов и индикаторы
.include "src/code/15_score.asm"    # Десятичная арифметика счёта
.include "src/code/16_highscores.asm"    # Рекорды, глифы и ввод имени
.include "src/code/17_ui_primitives.asm"    # Диалоги, подсветка и primitives HUD
.include "src/code/18_tunnel_gameplay.asm"    # Физика туннеля, предметы, проекция
.include "src/code/19_ega_video.asm"    # Видеостраницы, копирование, CRTC, синхронизация
.include "src/code/20_tunnel_renderer.asm"    # Геометрия туннеля и целочисленные линии
.include "src/code/21_audio.asm"    # PC speaker, приоритеты эффектов и музыка

/* DS начинается на 6 байт раньше: CS:3A50..3A55 остаются кодом. */
initialized_data:
.include "src/data/01_strings_templates_tables.inc"
.include "src/data/02_embedded_unclassified.inc"
.include "src/data/03_levels.inc"
.include "src/data/04_sound_tables.inc"
.include "src/data/05_runtime_state.inc"
file_end:
