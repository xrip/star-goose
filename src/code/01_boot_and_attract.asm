/* Запуск, игровой цикл и заставка
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * entry  |  CS:0010
 * Назначение: Настраивает DS и восемь сегментов буферов; проверяет память, загружает ресурсы и запускает внешний цикл игры.
 * Вход: MZ entry, не обычный near CALL.
 * Эффект/ограничения: Не возвращается; последовательность инициализации и повторного старта сохранена.
 * Возвраты в физическом диапазоне: нет; переход/fall-through/невозвратный путь.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type entry, @function
entry:
    .set relocation_0011, . + 1

    # Загрузчик DOS передаёт управление сюда. Устанавливаем собственный DS; это не C runtime.
    mov ax, OFFSET DATA_SEGMENT                                      # CS:0010
    mov ds, ax                                                       # CS:0013

    # После данных и стека размещаем восемь банков по 0x800 параграфов = 32768 байт.
    mov bx, OFFSET DATA_AND_STACK_SIZE                               # CS:0015
    add bx, 0xf                                                      # CS:0018
    shr bx, 1                                                        # CS:001B
    shr bx, 1                                                        # CS:001D
    shr bx, 1                                                        # CS:001F
    shr bx, 1                                                        # CS:0021
    {load} add ax, bx                                                # CS:0023
    mov ds:[D_segment_bird1], ax                                     # CS:0025
    ACC16 add, 0x800                                                 # CS:0028
    mov ds:[D_segment_bird2], ax                                     # CS:002B
    ACC16 add, 0x800                                                 # CS:002E
    mov ds:[D_segment_blox], ax                                      # CS:0031
    ACC16 add, 0x800                                                 # CS:0034
    mov ds:[D_segment_newbird], ax                                   # CS:0037
    ACC16 add, 0x800                                                 # CS:003A
    mov ds:[D_segment_podz1], ax                                     # CS:003D
    ACC16 add, 0x800                                                 # CS:0040
    mov ds:[D_segment_intro], ax                                     # CS:0043
    ACC16 add, 0x800                                                 # CS:0046
    mov ds:[D_segment_scratch0], ax                                  # CS:0049
    ACC16 add, 0x800                                                 # CS:004C
    mov ds:[D_segment_scratch1], ax                                  # CS:004F

    # Проверяется общий объём conventional RAM, а не результат DOS malloc.
    int 0x12                                                         # CS:0052 | BIOS conventional-memory query (returns KiB in AX)
    ACC16 cmp, 0x200                                                 # CS:0054 | requires at least 0200h = 512 KiB
    jae L_entry_enough_memory                                        # CS:0057
    mov dx, OFFSET D_msg_insufficient_ram                            # CS:0059
    mov ah, 0x9                                                      # CS:005C
    int 0x21                                                         # CS:005E | DOS print $-terminated error string
    mov ax, 0x4c00                                                   # CS:0060
    int 0x21                                                         # CS:0063 | DOS process termination, exit status 0
    jmp L_entry_initialize                                           # CS:0065 | Не достигнуто текущим статическим CFG; не удалять.
L_entry_enough_memory:
    mov dx, OFFSET D_msg_loading                                     # CS:0067
    mov ah, 0x9                                                      # CS:006A
    int 0x21                                                         # CS:006C | DOS print loading message
L_entry_initialize:
    call load_assets                                                 # CS:006E
    call video_init_mode0d                                           # CS:0071
    call load_highscores                                             # CS:0074
    call critical_error_install                                      # CS:0077
    call keyboard_install                                            # CS:007A
    call mouse_reset                                                 # CS:007D
    call timer_install                                               # CS:0080
    call sound_init                                                  # CS:0083
    mov BYTE PTR ds:[D_level_index], 0x0                             # CS:0086
L_entry_session_loop:
    call start_game_cycle                                            # CS:008B
    call game_loop                                                   # CS:008E
    call mark_attract_seen                                           # CS:0091
    jmp L_entry_session_loop                                         # CS:0094

/* ========================================================================== 
 * exit_to_dos  |  CS:0096
 * Назначение: Останавливает звук, возвращает INT 1Ch/09h, включает текстовый режим и завершает процесс.
 * Вход: Вызывается переходом, не обязательно CALL.
 * Эффект/ограничения: Невозвратная процедура; PIT и INT 24h здесь явно не восстанавливаются.
 * Возвраты в физическом диапазоне: нет; переход/fall-through/невозвратный путь.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type exit_to_dos, @function
exit_to_dos:
    call sound_stop                                                  # CS:0096
    call timer_restore_vector                                        # CS:0099
    call keyboard_restore                                            # CS:009C
    mov ax, 0x3                                                      # CS:009F
    int 0x10                                                         # CS:00A2 | BIOS text mode 03h
    mov ax, 0x4c00                                                   # CS:00A4
    int 0x21                                                         # CS:00A7 | DOS process termination, exit status 0

/* ========================================================================== 
 * start_game_cycle  |  CS:00A9
 * Назначение: Показывает заставку/таблицу рекордов, сбрасывает состояние новой игры, готовит мир и HUD.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Не обнуляет level_index; нельзя добавлять такой сброс при буквальном переносе.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type start_game_cycle, @function
start_game_cycle:
    call run_attract_sequence                                        # CS:00A9
    mov BYTE PTR ds:[D_game_loop_end_flag], 0x0                      # CS:00AC
    mov BYTE PTR ds:[D_stage_counter], 0x1                           # CS:00B1

    # ВНИМАНИЕ: оригинал обнуляет/пишет только младший байт base_scroll_speed.
    mov BYTE PTR ds:[D_base_scroll_speed], 0x10                      # CS:00B6
    mov BYTE PTR ds:[D_tunnel_mode], 0x0                             # CS:00BB
    mov BYTE PTR ds:[D_level_part], 0x0                              # CS:00C0
    call start_level_state                                           # CS:00C5
    call init_playfield_pages                                        # CS:00C8
    call init_hud                                                    # CS:00CB
    ret                                                              # CS:00CE

/* ========================================================================== 
 * game_loop  |  CS:00CF
 * Назначение: Один игровой кадр: ввод → обновление → поверхность или туннель → сущности → HUD → показ.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Повторяет кадры до game_loop_end_flag. Порядок вызовов — часть поведения.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type game_loop, @function
game_loop:

    # Кадровый порядок важен: entity update выполняется до выбора renderer и HUD.
    call poll_controls                                               # CS:00CF
    call update_entities                                             # CS:00D2
    mov al, ds:[D_tunnel_mode]                                       # CS:00D5
    cmp al, 0x0                                                      # CS:00D8
    jne L_game_loop_00e4                                             # CS:00DA
    call draw_scrolling_background                                   # CS:00DC
    call render_entities                                             # CS:00DF
    jmp L_game_loop_00ed                                             # CS:00E2
L_game_loop_00e4:
    call build_tunnel_bitmap                                         # CS:00E4
    call present_tunnel_bitmap                                       # CS:00E7
    call render_entities                                             # CS:00EA
L_game_loop_00ed:
    call update_hud                                                  # CS:00ED
    call present_frame                                               # CS:00F0
    mov al, ds:[D_game_loop_end_flag]                                # CS:00F3
    cmp al, 0x0                                                      # CS:00F6
    jne L_game_loop_00fc                                             # CS:00F8
    jmp game_loop                                                    # CS:00FA
L_game_loop_00fc:
    ret                                                              # CS:00FC

/* ========================================================================== 
 * mark_attract_seen  |  CS:00FD
 * Назначение: Помечает, что первая игровая сессия уже закончилась.
 * Вход: Нет аргументов.
 * Эффект/ограничения: attract_seen = 1.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type mark_attract_seen, @function
mark_attract_seen:
    mov BYTE PTR ds:[D_attract_seen], 0x1                            # CS:00FD
    ret                                                              # CS:0102

/* ========================================================================== 
 * run_attract_sequence  |  CS:0103
 * Назначение: Выполняет сценарий заставки: каждые 8 итераций читает команду, создаёт надпись/сущность или запускает ввод имени.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: 0 = пауза; FF = конец; FE = ввод имени/ожидание; бит 7 = пара последовательных кадров.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type run_attract_sequence, @function
run_attract_sequence:

    # Сценарий заставки — отдельный автомат, использующий тот же пул сущностей.
    mov al, ds:[D_attract_seen]                                      # CS:0103
    cmp al, 0x0                                                      # CS:0106
    je L_run_attract_sequence_0115                                   # CS:0108
    mov WORD PTR ds:[D_attract_script_pointer], 0xda                 # CS:010A
    call insert_current_highscore                                    # CS:0110
    jmp L_run_attract_sequence_011e                                  # CS:0113
L_run_attract_sequence_0115:
    mov WORD PTR ds:[D_attract_script_pointer], 0xe9                 # CS:0115
    call render_highscore_table                                      # CS:011B
L_run_attract_sequence_011e:
    call music_start                                                 # CS:011E
    call prepare_attract_background                                  # CS:0121
    mov BYTE PTR ds:[D_attract_active], 0x1                          # CS:0124
    mov WORD PTR ds:[D_attract_step_counter], 0x0                    # CS:0129
    call reset_attract_entities                                      # CS:012F
L_run_attract_sequence_0132:
    call poll_controls                                               # CS:0132
    call update_entities                                             # CS:0135
    call restore_attract_background                                  # CS:0138
    call render_entities                                             # CS:013B
    call present_frame                                               # CS:013E
    mov ax, ds:[D_control_bits]                                      # CS:0141
    test ax, 0x80                                                    # CS:0144
    jne L_run_attract_sequence_0197                                  # CS:0147
    inc WORD PTR ds:[D_attract_step_counter]                         # CS:0149
    mov bx, WORD PTR ds:[D_attract_step_counter]                     # CS:014D
    test bx, 0x7                                                     # CS:0151
    jne L_run_attract_sequence_0195                                  # CS:0155
    shr bx, 1                                                        # CS:0157
    shr bx, 1                                                        # CS:0159
    shr bx, 1                                                        # CS:015B
    add bx, WORD PTR ds:[D_attract_script_pointer]                   # CS:015D
    mov al, BYTE PTR [bx]                                            # CS:0161
    cmp al, 0x0                                                      # CS:0163
    je L_run_attract_sequence_0195                                   # CS:0165
    cmp al, 0xff                                                     # CS:0167
    je L_run_attract_sequence_0197                                   # CS:0169
    cmp al, 0xfe                                                     # CS:016B
    jne L_run_attract_sequence_0180                                  # CS:016D
    mov al, ds:[D_pending_name_entry]                                # CS:016F
    cmp al, 0x0                                                      # CS:0172
    je L_run_attract_sequence_017b                                   # CS:0174
    call edit_highscore_name                                         # CS:0176
    jmp L_run_attract_sequence_017e                                  # CS:0179
L_run_attract_sequence_017b:
    call wait_attract_input                                          # CS:017B
L_run_attract_sequence_017e:
    jmp L_run_attract_sequence_0195                                  # CS:017E
L_run_attract_sequence_0180:
    test al, 0x80                                                    # CS:0180
    jne L_run_attract_sequence_0189                                  # CS:0182
    call spawn_attract_item                                          # CS:0184
    jmp L_run_attract_sequence_0195                                  # CS:0187
L_run_attract_sequence_0189:
    and al, 0x3f                                                     # CS:0189
    push ax                                                          # CS:018B
    call spawn_attract_item                                          # CS:018C
    pop ax                                                           # CS:018F
    inc al                                                           # CS:0190
    call spawn_attract_item                                          # CS:0192
L_run_attract_sequence_0195:
    jmp L_run_attract_sequence_0132                                  # CS:0195
L_run_attract_sequence_0197:
    mov BYTE PTR ds:[D_attract_active], 0x0                          # CS:0197
    ret                                                              # CS:019C

/* ========================================================================== 
 * spawn_attract_item  |  CS:019D
 * Назначение: Выбирает кадр шаблона заставки, применяет его параметры и помещает копию в пул.
 * Вход: AL = номер кадра; шаблон DS:0109.
 * Эффект/ограничения: SI = новый слот либо 0; состояние шаблона изменяется.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_attract_item, @function
spawn_attract_item:
    mov si, OFFSET D_template_attract                                # CS:019D
    mov BYTE PTR [si+0x4], al                                        # CS:01A0
    mov bx, OFFSET D_sprite_frames_0120                              # CS:01A3
    call apply_sprite_frame                                          # CS:01A6
    call allocate_entity_copy                                        # CS:01A9
    ret                                                              # CS:01AC

/* ========================================================================== 
 * wait_attract_input  |  CS:01AD
 * Назначение: До 8000 раз опрашивает управление; досрочно выходит по FIRE.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Это задержка числом опросов, не таймер в миллисекундах.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type wait_attract_input, @function
wait_attract_input:
    mov cx, 0x1f40                                                   # CS:01AD
L_wait_attract_input_01b0:
    push cx                                                          # CS:01B0
    call poll_controls                                               # CS:01B1
    pop cx                                                           # CS:01B4
    mov ax, ds:[D_control_bits]                                      # CS:01B5
    test al, 0x80                                                    # CS:01B8
    je L_wait_attract_input_01bd                                     # CS:01BA
    ret                                                              # CS:01BC
L_wait_attract_input_01bd:
    loop L_wait_attract_input_01b0                                   # CS:01BD
    ret                                                              # CS:01BF
