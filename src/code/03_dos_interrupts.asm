/* Клавиатура, мышь, PIT и DOS vectors
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * keyboard_install  |  CS:0442
 * Назначение: Задаёт исходные scan-code, очищает 128 состояний и устанавливает свой INT 09h.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Сохраняет старый вектор; использует REP STOSW без локального CLD.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type keyboard_install, @function
keyboard_install:

    # Установщик IRQ и таблица клавиш относятся к платформенному слою будущего порта.
    mov WORD PTR ds:[D_last_key_pointer], 0x0                        # CS:0442
    mov WORD PTR ds:[D_key_left], 0x4b                               # CS:0448
    mov WORD PTR ds:[D_key_right], 0x4d                              # CS:044E
    mov WORD PTR ds:[D_key_up], 0x48                                 # CS:0454
    mov WORD PTR ds:[D_key_down], 0x50                               # CS:045A
    mov WORD PTR ds:[D_key_fire], 0x39                               # CS:0460
    mov WORD PTR ds:[D_key_action_n], 0x31                           # CS:0466
    mov WORD PTR ds:[D_key_action_m], 0x32                           # CS:046C
    mov WORD PTR ds:[D_key_quit], 0x10                               # CS:0472
    mov ax, ds                                                       # CS:0478
    mov es, ax                                                       # CS:047A
    mov ax, 0x0                                                      # CS:047C
    mov cx, 0x40                                                     # CS:047F
    mov di, 0x8e8a                                                   # CS:0482
    rep stos WORD PTR es:[di], ax                                    # CS:0485
    mov al, 0x9                                                      # CS:0487
    mov ah, 0x35                                                     # CS:0489
    int 0x21                                                         # CS:048B
    mov WORD PTR ds:[D_old_keyboard_offset], bx                      # CS:048D
    mov WORD PTR ds:[D_old_keyboard_segment], es                     # CS:0491
    push ds                                                          # CS:0495
    mov dx, OFFSET keyboard_irq1 - image_start                       # CS:0496
    mov ax, cs                                                       # CS:0499
    mov ds, ax                                                       # CS:049B
    mov al, 0x9                                                      # CS:049D
    mov ah, 0x25                                                     # CS:049F
    int 0x21                                                         # CS:04A1
    pop ds                                                           # CS:04A3
    ret                                                              # CS:04A4

/* ========================================================================== 
 * keyboard_restore  |  CS:04A5
 * Назначение: Возвращает сохранённый INT 09h через DOS.
 * Вход: Сохранённые offset/segment.
 * Эффект/ограничения: DS восстанавливается после временной подстановки сегмента обработчика.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type keyboard_restore, @function
keyboard_restore:
    mov dx, WORD PTR ds:[D_old_keyboard_offset]                      # CS:04A5
    mov ax, ds:[D_old_keyboard_segment]                              # CS:04A9
    push ds                                                          # CS:04AC
    mov ds, ax                                                       # CS:04AD
    mov al, 0x9                                                      # CS:04AF
    mov ah, 0x25                                                     # CS:04B1
    int 0x21                                                         # CS:04B3
    pop ds                                                           # CS:04B5
    ret                                                              # CS:04B6

/* ========================================================================== 
 * keyboard_irq1  |  CS:04B7
 * Назначение: Читает scan-code, подтверждает IRQ и записывает make/break как 1/0.
 * Вход: Аппаратный вход INT 09h.
 * Эффект/ограничения: IRET; AX/BX/DS сохранены. Расширенные префиксы не декодируются в отдельные клавиши.
 * Возвраты в физическом диапазоне: iret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type keyboard_irq1, @function
keyboard_irq1:

    # IRQ ABI: сохранять регистры и возвращаться через IRET, не через RET.
    sti                                                              # CS:04B7
    push ax                                                          # CS:04B8
    push bx                                                          # CS:04B9
    push ds                                                          # CS:04BA
    mov ax, 0x40                                                     # CS:04BB
    mov ds, ax                                                       # CS:04BE
    in al, 0x60                                                      # CS:04C0
    push ax                                                          # CS:04C2
    in al, 0x61                                                      # CS:04C3
    {load} mov ah, al                                                # CS:04C5
    or al, 0x80                                                      # CS:04C7
    out 0x61, al                                                     # CS:04C9
    xchg al, ah                                                      # CS:04CB
    out 0x61, al                                                     # CS:04CD
    pop bx                                                           # CS:04CF
L_keyboard_irq1_04d0:
    .set relocation_04d1, . + 1
    mov ax, OFFSET DATA_SEGMENT                                      # CS:04D0
    mov ds, ax                                                       # CS:04D3
    mov bh, 0x0                                                      # CS:04D5
    test bl, 0x80                                                    # CS:04D7
    jne L_keyboard_irq1_04e3                                         # CS:04DA
    mov BYTE PTR [bx+D_keyboard_state], 0x1                          # CS:04DC
    jmp L_keyboard_irq1_04eb                                         # CS:04E1
L_keyboard_irq1_04e3:
    and bl, 0x7f                                                     # CS:04E3
    mov BYTE PTR [bx+D_keyboard_state], 0x0                          # CS:04E6
L_keyboard_irq1_04eb:
    mov al, 0x20                                                     # CS:04EB
    out 0x20, al                                                     # CS:04ED
    pop ds                                                           # CS:04EF
    pop bx                                                           # CS:04F0
    pop ax                                                           # CS:04F1
    iret                                                             # CS:04F2

/* ========================================================================== 
 * wait_scancode  |  CS:04F3
 * Назначение: Ищет нажатую клавишу, кроме F1–F4 и обоих Shift; повторяет сканирование до находки.
 * Вход: Нет аргументов.
 * Эффект/ограничения: AX = scan-code. Сам по себе не ждёт отпускания предыдущей клавиши.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type wait_scancode, @function
wait_scancode:
    cld                                                              # CS:04F3
L_wait_scancode_04f4:
    mov si, 0x8e8a                                                   # CS:04F4
L_wait_scancode_04f7:
    lods al, BYTE PTR ds:[si]                                        # CS:04F7
    cmp al, 0x0                                                      # CS:04F8
    je L_wait_scancode_0520                                          # CS:04FA
    {load} mov ax, si                                                # CS:04FC
    ACC16 sub, 0x8e8b                                                # CS:04FE
    ACC16 cmp, 0x3b                                                  # CS:0501
    je L_wait_scancode_0520                                          # CS:0504
    ACC16 cmp, 0x3c                                                  # CS:0506
    je L_wait_scancode_0520                                          # CS:0509
    ACC16 cmp, 0x3d                                                  # CS:050B
    je L_wait_scancode_0520                                          # CS:050E
    ACC16 cmp, 0x3e                                                  # CS:0510
    je L_wait_scancode_0520                                          # CS:0513
    ACC16 cmp, 0x2a                                                  # CS:0515
    je L_wait_scancode_0520                                          # CS:0518
    ACC16 cmp, 0x36                                                  # CS:051A
    je L_wait_scancode_0520                                          # CS:051D
    ret                                                              # CS:051F
L_wait_scancode_0520:
    cmp si, 0x8f0a                                                   # CS:0520
    jne L_wait_scancode_04f7                                         # CS:0524
    jmp L_wait_scancode_04f4                                         # CS:0526
    ret                                                              # CS:0528 | Не достигнуто текущим статическим CFG; не удалять.

/* ========================================================================== 
 * wait_keys_released  |  CS:0529
 * Назначение: Повторяет проход по 128 байтам, пока все клавиши не отпущены.
 * Вход: DS = сегмент игры; ожидается DF = 0.
 * Эффект/ограничения: Блокирующий busy loop.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type wait_keys_released, @function
wait_keys_released:
    mov si, 0x8e8a                                                   # CS:0529
    mov ah, 0x0                                                      # CS:052C
L_wait_keys_released_052e:
    lods al, BYTE PTR ds:[si]                                        # CS:052E
    cmp al, 0x0                                                      # CS:052F
    je L_wait_keys_released_0535                                     # CS:0531
    inc ah                                                           # CS:0533
L_wait_keys_released_0535:
    cmp si, 0x8f0a                                                   # CS:0535
    jne L_wait_keys_released_052e                                    # CS:0539
    cmp ah, 0x0                                                      # CS:053B
    jne wait_keys_released                                           # CS:053E
    ret                                                              # CS:0540

/* ========================================================================== 
 * poll_key_character  |  CS:0541
 * Назначение: До 20000 проходов ищет новый scan-code и переводит его через таблицу символов.
 * Вход: last_key_pointer подавляет повтор того же нажатия.
 * Эффект/ограничения: AL = символ либо 0. Это ограниченное ожидание, а не мгновенный poll.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type poll_key_character, @function
poll_key_character:
    mov cx, 0x4e20                                                   # CS:0541
L_poll_key_character_0544:
    mov si, 0x8e8a                                                   # CS:0544
    mov ah, 0x0                                                      # CS:0547
    cld                                                              # CS:0549
L_poll_key_character_054a:
    lods al, BYTE PTR ds:[si]                                        # CS:054A
    cmp al, 0x0                                                      # CS:054B
    je L_poll_key_character_0557                                     # CS:054D
    mov ah, 0x1                                                      # CS:054F
    cmp si, WORD PTR ds:[D_last_key_pointer]                         # CS:0551
    jne L_poll_key_character_055f                                    # CS:0555
L_poll_key_character_0557:
    cmp si, 0x8f0a                                                   # CS:0557
    je L_poll_key_character_055f                                     # CS:055B
    jmp L_poll_key_character_054a                                    # CS:055D
L_poll_key_character_055f:
    je L_poll_key_character_0578                                     # CS:055F
    mov WORD PTR ds:[D_last_key_pointer], si                         # CS:0561
    sub si, 0x8e8b                                                   # CS:0565
    {disp16} mov al, BYTE PTR [si+D_key_to_character]                # CS:0569 | Original disp16=005Ah deliberately retained, although disp8 would fit
    cmp al, 0x0                                                      # CS:056D
    je L_poll_key_character_0576                                     # CS:056F
    jns L_poll_key_character_0575                                    # CS:0571
    mov al, 0x0                                                      # CS:0573
L_poll_key_character_0575:
    ret                                                              # CS:0575
L_poll_key_character_0576:
    jmp L_poll_key_character_0583                                    # CS:0576
L_poll_key_character_0578:
    cmp ah, 0x0                                                      # CS:0578
    jne L_poll_key_character_0583                                    # CS:057B
    mov WORD PTR ds:[D_last_key_pointer], 0x0                        # CS:057D
L_poll_key_character_0583:
    loop L_poll_key_character_0544                                   # CS:0583
    mov al, 0x0                                                      # CS:0585
    ret                                                              # CS:0587

/* ========================================================================== 
 * mouse_reset  |  CS:0588
 * Назначение: Сбрасывает драйвер мыши INT 33h и сохраняет признак его наличия.
 * Вход: Нет аргументов.
 * Эффект/ограничения: mouse_present = AL результата AX=0/INT 33h.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type mouse_reset, @function
mouse_reset:
    mov ax, 0x0                                                      # CS:0588
    int 0x33                                                         # CS:058B
    mov ds:[D_mouse_present], al                                     # CS:058D
    ret                                                              # CS:0590

/* ========================================================================== 
 * timer_install  |  CS:0591
 * Назначение: Сохраняет и заменяет INT 1Ch; программирует PIT0 делителем 4D0Eh.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Звук обновляется из обработчика, а не из главного игрового цикла.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type timer_install, @function
timer_install:

    # PIT обслуживает звук независимо от вертикальной синхронизации игрового кадра.
    mov al, 0x1c                                                     # CS:0591
    mov ah, 0x35                                                     # CS:0593
    int 0x21                                                         # CS:0595
    mov WORD PTR ds:[D_old_timer_offset], bx                         # CS:0597
    mov WORD PTR ds:[D_old_timer_segment], es                        # CS:059B
    push ds                                                          # CS:059F
    mov dx, OFFSET timer_int1c - image_start                         # CS:05A0
    mov ax, cs                                                       # CS:05A3
    mov ds, ax                                                       # CS:05A5
    mov al, 0x1c                                                     # CS:05A7
    mov ah, 0x25                                                     # CS:05A9
    int 0x21                                                         # CS:05AB | DOS installs INT 1Ch
    pop ds                                                           # CS:05AD
    mov al, 0x36                                                     # CS:05AE
    out 0x43, al                                                     # CS:05B0
    mov ax, 0x4d0e                                                   # CS:05B2
    out 0x40, al                                                     # CS:05B5
    {load} mov al, ah                                                # CS:05B7
    out 0x40, al                                                     # CS:05B9
    ret                                                              # CS:05BB

/* ========================================================================== 
 * timer_restore_vector  |  CS:05BC
 * Назначение: Возвращает только вектор INT 1Ch.
 * Вход: Сохранённые offset/segment.
 * Эффект/ограничения: Делитель PIT в этой процедуре не восстанавливается.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type timer_restore_vector, @function
timer_restore_vector:
    mov dx, WORD PTR ds:[D_old_timer_offset]                         # CS:05BC
    mov ax, ds:[D_old_timer_segment]                                 # CS:05C0
    push ds                                                          # CS:05C3
    mov ds, ax                                                       # CS:05C4
    mov al, 0x1c                                                     # CS:05C6
    mov ah, 0x25                                                     # CS:05C8
    int 0x21                                                         # CS:05CA
    pop ds                                                           # CS:05CC
    ret                                                              # CS:05CD

/* ========================================================================== 
 * timer_int1c  |  CS:05CE
 * Назначение: Увеличивает 8-битный счётчик тиков и обновляет звуковой автомат.
 * Вход: Вход прерывания; свой DS устанавливается явно.
 * Эффект/ограничения: IRET; tick переполняется по модулю 256. Общие регистры и DS сохранены, ES явно не сохраняется.
 * Возвраты в физическом диапазоне: iret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type timer_int1c, @function
timer_int1c:
    sti                                                              # CS:05CE
    push ax                                                          # CS:05CF
    push bx                                                          # CS:05D0
    push cx                                                          # CS:05D1
    push dx                                                          # CS:05D2
    push si                                                          # CS:05D3
    push di                                                          # CS:05D4
    push bp                                                          # CS:05D5
    push ds                                                          # CS:05D6
L_timer_int1c_05d7:
    .set relocation_05d8, . + 1
    mov ax, OFFSET DATA_SEGMENT                                      # CS:05D7
    mov ds, ax                                                       # CS:05DA
    inc BYTE PTR ds:[D_timer_ticks]                                  # CS:05DC
    call sound_tick                                                  # CS:05E0
    pop ds                                                           # CS:05E3
    pop bp                                                           # CS:05E4
    pop di                                                           # CS:05E5
    pop si                                                           # CS:05E6
    pop dx                                                           # CS:05E7
    pop cx                                                           # CS:05E8
    pop bx                                                           # CS:05E9
    pop ax                                                           # CS:05EA
    iret                                                             # CS:05EB

/* ========================================================================== 
 * critical_error_install  |  CS:05EC
 * Назначение: Устанавливает минимальный INT 24h.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Старый вектор здесь не сохраняется.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type critical_error_install, @function
critical_error_install:
    push ds                                                          # CS:05EC
    mov dx, OFFSET critical_error_int24 - image_start                # CS:05ED
    mov ax, cs                                                       # CS:05F0
    mov ds, ax                                                       # CS:05F2
    mov al, 0x24                                                     # CS:05F4
    mov ah, 0x25                                                     # CS:05F6
    int 0x21                                                         # CS:05F8
    pop ds                                                           # CS:05FA
    ret                                                              # CS:05FB

/* ========================================================================== 
 * critical_error_int24  |  CS:05FC
 * Назначение: Возвращает AL=0 обработчику критической ошибки DOS.
 * Вход: Вход прерывания.
 * Эффект/ограничения: IRET; не обычный RET и не логическая функция игры.
 * Возвраты в физическом диапазоне: iret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type critical_error_int24, @function
critical_error_int24:
    sti                                                              # CS:05FC
    mov al, 0x0                                                      # CS:05FD
    iret                                                             # CS:05FF
