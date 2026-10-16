/* Пул, обход рендера и подготовка спрайтов
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * allocate_entity_copy  |  CS:0B58
 * Назначение: Ищет первый слот с очищенным битом RENDER и копирует в него ровно 23 байта.
 * Вход: DS:SI = шаблон записи.
 * Эффект/ограничения: SI=адрес копии либо 0. ES=сегмент игры; DI и CX изменяются. Не проверяет flags==0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type allocate_entity_copy, @function
allocate_entity_copy:
    .set relocation_0b59, . + 1

    # Свободный слот = EF_RENDER сброшен, не обязательно flags==0. Эта разница влияет на переиспользование.
    mov ax, OFFSET DATA_SEGMENT                                      # CS:0B58
    mov es, ax                                                       # CS:0B5B
    mov di, OFFSET D_entity_records                                  # CS:0B5D
L_allocate_find_slot:
    mov al, BYTE PTR [di+E_FLAGS]                                    # CS:0B60
    test al, EF_SENTINEL                                                    # CS:0B62
    jne L_allocate_pool_full                                         # CS:0B64
    test al, EF_RENDER                                                     # CS:0B66
    jne L_allocate_next_slot                                         # CS:0B68

    # Копируем все 23 байта шаблона. После REP SI возвращается на начало новой записи.
    push di                                                          # CS:0B6A
    mov cx, ENTITY_SIZE                                              # CS:0B6B
    cld                                                              # CS:0B6E
    rep movs BYTE PTR es:[di], BYTE PTR ds:[si]      # CS:0B6F
    pop si                                                           # CS:0B71
    ret                                                              # CS:0B72
L_allocate_next_slot:
    add di, ENTITY_SIZE                                              # CS:0B73
    jmp L_allocate_find_slot                                         # CS:0B76
L_allocate_pool_full:
    mov si, 0x0                                                      # CS:0B78
    ret                                                              # CS:0B7B

/* ========================================================================== 
 * clear_entities  |  CS:0B7C
 * Назначение: Очищает только flags каждой записи до sentinel.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Остальные 22 байта слота остаются прежними; не заменять memset всего массива без проверки.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type clear_entities, @function
clear_entities:

    # Очищается только flags каждого слота. Остальные 22 байта намеренно остаются прежними.
    mov si, OFFSET D_entity_records                                  # CS:0B7C
L_clear_next_record:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:0B7F
    test al, EF_SENTINEL                                                    # CS:0B81
    jne L_clear_done                                                 # CS:0B83
    mov BYTE PTR [si+E_FLAGS], 0x0                                   # CS:0B85
    add si, ENTITY_SIZE                                              # CS:0B88
    jmp L_clear_next_record                                          # CS:0B8B
L_clear_done:
    ret                                                              # CS:0B8D

/* ========================================================================== 
 * render_entities  |  CS:0B8E
 * Назначение: Семь проходов по пулу: рисует активные записи слоя 7,6,...,1.
 * Вход: DS = сегмент игры.
 * Эффект/ограничения: Слой 0 не рисуется; порядок внутри слоя — порядок слотов. Stride=40, clipping включён.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type render_entities, @function
render_entities:

    # Порядок слоёв 7→1 и порядок слотов внутри слоя фиксированы оригиналом.
    mov ax, ds:[D_video_draw_segment]                                # CS:0B8E
    mov ds:[D_sprite_dest_segment], ax                               # CS:0B91
    mov WORD PTR cs:[CS_sprite_stride], 0x28                         # CS:0B94 | CS-local DATA write, not an opcode patch
    mov WORD PTR ds:[D_sprite_clip_enabled], 0x1                     # CS:0B9B
    mov cx, 0x7                                                      # CS:0BA1
L_render_next_layer:
    mov si, OFFSET D_entity_records                                  # CS:0BA4
L_render_next_entity:
    mov al, BYTE PTR [si+E_FLAGS]                                    # CS:0BA7
    test al, EF_SENTINEL                                                    # CS:0BA9
    jne L_render_advance_layer                                       # CS:0BAB
    test al, EF_RENDER                                                     # CS:0BAD
    je L_render_advance_entity                                       # CS:0BAF
    cmp cl, BYTE PTR [si+E_LAYER]                                    # CS:0BB1
    jne L_render_advance_entity                                      # CS:0BB4
    push cx                                                          # CS:0BB6
    push si                                                          # CS:0BB7
    call prepare_sprite_blit                                         # CS:0BB8
    pop si                                                           # CS:0BBB
    pop cx                                                           # CS:0BBC
L_render_advance_entity:
    add si, ENTITY_SIZE                                              # CS:0BBD
    jmp L_render_next_entity                                         # CS:0BC0
L_render_advance_layer:
    loop L_render_next_layer                                         # CS:0BC2
    ret                                                              # CS:0BC4

/* ========================================================================== 
 * render_entity_to_a800  |  CS:0BC5
 * Назначение: Рисует одну запись в карту-фон A800h, а не в экранную страницу.
 * Вход: DS:SI = сущность/шаблон.
 * Эффект/ограничения: Stride=32, экранное отсечение отключено.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type render_entity_to_a800, @function
render_entity_to_a800:
    mov ax, 0xa800                                                   # CS:0BC5
    mov ds:[D_sprite_dest_segment], ax                               # CS:0BC8
    mov WORD PTR cs:[CS_sprite_stride], 0x20                         # CS:0BCB | CS-local DATA write, not an opcode patch
    mov WORD PTR ds:[D_sprite_clip_enabled], 0x0                     # CS:0BD2
    call prepare_sprite_blit                                         # CS:0BD8
    ret                                                              # CS:0BDB

/* ========================================================================== 
 * prepare_sprite_blit  |  CS:0BDC
 * Назначение: Из записи вычисляет верхний левый угол, размеры, банк изображения и прозрачный цвет.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: x'=x-signed(anchor_x); y'=y-signed8(z+anchor_y). Передаёт четыре слова через стек.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type prepare_sprite_blit, @function
prepare_sprite_blit:

    # Проекция anchor: X = x-sign8(anchor_x); Y = y-sign8(z+anchor_y). Сумма z+anchor сначала 8-битная.
    mov al, BYTE PTR [si+E_ANCHOR_X]                                 # CS:0BDC
    cbw                                                              # CS:0BDF
    mov bx, WORD PTR [si+E_X]                                        # CS:0BE0
    {load} sub bx, ax                                                # CS:0BE3
    push bx                                                          # CS:0BE5
    mov al, BYTE PTR [si+E_Z]                                        # CS:0BE6
    add al, BYTE PTR [si+E_ANCHOR_Y]                                 # CS:0BE9
    cbw                                                              # CS:0BEC
    neg ax                                                           # CS:0BED
    add ax, WORD PTR [si+E_Y]                                        # CS:0BEF
    push ax                                                          # CS:0BF2
    mov al, BYTE PTR [si+E_WIDTH]                                    # CS:0BF3
    mov ah, 0x0                                                      # CS:0BF6
    push ax                                                          # CS:0BF8
    mov al, BYTE PTR [si+E_HEIGHT]                                   # CS:0BF9
    mov ah, 0x0                                                      # CS:0BFC
    push ax                                                          # CS:0BFE

    # E_BANK уже является байтовым offset таблицы сегментов. Дополнительное умножение на 2 будет ошибкой.
    mov bx, WORD PTR [si+E_BANK]                                     # CS:0BFF
    mov bx, WORD PTR [bx+D_segment_bird1]                            # CS:0C02
    mov al, BYTE PTR [si+E_TRANSPARENT]                              # CS:0C06
    mov cs:[CS_transparent_color], al                                # CS:0C09
    {load} mov cl, al                                                # CS:0C0D
    shl cl, 1                                                        # CS:0C0F
    shl cl, 1                                                        # CS:0C11
    shl cl, 1                                                        # CS:0C13
    shl cl, 1                                                        # CS:0C15
    {load} or al, cl                                                 # CS:0C17
    mov cs:[CS_transparent_pair], al                                 # CS:0C19
    mov si, WORD PTR [si+E_SPRITE]                                   # CS:0C1D
    call blit_sprite                                                 # CS:0C20
    ret                                                              # CS:0C23
