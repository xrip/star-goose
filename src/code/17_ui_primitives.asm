/* Диалоги, подсветка и primitives HUD
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * draw_redefine_dialog  |  CS:2C07
 * Назначение: Рисует подложку и заголовок переназначения клавиш.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Фиксированные прямоугольники/ресурсы в обеих страницах.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_redefine_dialog, @function
draw_redefine_dialog:
    mov ax, 0xa0b                                                    # CS:2C07
    push ax                                                          # CS:2C0A
    mov ax, 0x13                                                     # CS:2C0B
    push ax                                                          # CS:2C0E
    mov ax, 0x30                                                     # CS:2C0F
    push ax                                                          # CS:2C12
    mov ax, 0x5                                                      # CS:2C13
    push ax                                                          # CS:2C16
    call fill_hud_rect_both_pages                                    # CS:2C17
    mov ax, 0xaac                                                    # CS:2C1A
    push ax                                                          # CS:2C1D
    mov ax, 0x11                                                     # CS:2C1E
    push ax                                                          # CS:2C21
    mov ax, 0x28                                                     # CS:2C22
    push ax                                                          # CS:2C25
    mov ax, 0x2                                                      # CS:2C26
    push ax                                                          # CS:2C29
    call fill_hud_rect_both_pages                                    # CS:2C2A
    mov ax, 0x5a5c                                                   # CS:2C2D
    push ax                                                          # CS:2C30
    mov ax, 0xb4d                                                    # CS:2C31
    push ax                                                          # CS:2C34
    mov ax, 0x78                                                     # CS:2C35
    push ax                                                          # CS:2C38
    mov ax, 0xf                                                      # CS:2C39
    push ax                                                          # CS:2C3C
    mov ax, 0x4                                                      # CS:2C3D
    push ax                                                          # CS:2C40
    call blit_hud_sprite_both_pages                                  # CS:2C41
    ret                                                              # CS:2C44

/* ========================================================================== 
 * draw_quit_dialog  |  CS:2C45
 * Назначение: Рисует диалог подтверждения выхода.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Опрос Y/N находится в poll_controls, не здесь.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_quit_dialog, @function
draw_quit_dialog:
    mov ax, 0xaac                                                    # CS:2C45
    push ax                                                          # CS:2C48
    mov ax, 0x10                                                     # CS:2C49
    push ax                                                          # CS:2C4C
    mov ax, 0x28                                                     # CS:2C4D
    push ax                                                          # CS:2C50
    mov ax, 0x5                                                      # CS:2C51
    push ax                                                          # CS:2C54
    call fill_hud_rect_both_pages                                    # CS:2C55
    mov ax, 0xb4d                                                    # CS:2C58
    push ax                                                          # CS:2C5B
    mov ax, 0xe                                                      # CS:2C5C
    push ax                                                          # CS:2C5F
    mov ax, 0x20                                                     # CS:2C60
    push ax                                                          # CS:2C63
    mov ax, 0x2                                                      # CS:2C64
    push ax                                                          # CS:2C67
    call fill_hud_rect_both_pages                                    # CS:2C68
    mov ax, 0x7592                                                   # CS:2C6B
    push ax                                                          # CS:2C6E
    mov ax, 0xc8f                                                    # CS:2C6F
    push ax                                                          # CS:2C72
    mov ax, 0x28                                                     # CS:2C73
    push ax                                                          # CS:2C76
    mov ax, 0xc                                                      # CS:2C77
    push ax                                                          # CS:2C7A
    mov ax, 0x2                                                      # CS:2C7B
    push ax                                                          # CS:2C7E
    call blit_hud_sprite_both_pages                                  # CS:2C7F
    mov ax, 0x1ab8                                                   # CS:2C82
    push ax                                                          # CS:2C85
    mov ax, 0xce4                                                    # CS:2C86
    push ax                                                          # CS:2C89
    mov ax, 0x20                                                     # CS:2C8A
    push ax                                                          # CS:2C8D
    mov ax, 0xd                                                      # CS:2C8E
    push ax                                                          # CS:2C91
    mov ax, 0x4                                                      # CS:2C92
    push ax                                                          # CS:2C95
    call blit_hud_sprite_both_pages                                  # CS:2C96
    ret                                                              # CS:2C99

/* ========================================================================== 
 * draw_control_prompt  |  CS:2C9A
 * Назначение: Очищает строку и рисует запрос очередной клавиши; боковые действия состоят из двух частей.
 * Вход: AL = control index 0..7.
 * Эффект/ограничения: Может проваливаться в draw_control_prompt_piece; общий хвост.
 * Возвраты в физическом диапазоне: нет; переход/fall-through/невозвратный путь.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_control_prompt, @function
draw_control_prompt:

    # После подготовки параметров происходит fall-through в общий растровый участок.
    push ax                                                          # CS:2C9A
    mov ax, 0xdcc                                                    # CS:2C9B
    push ax                                                          # CS:2C9E
    mov ax, 0x11                                                     # CS:2C9F
    push ax                                                          # CS:2CA2
    mov ax, 0xf                                                      # CS:2CA3
    push ax                                                          # CS:2CA6
    mov ax, 0x2                                                      # CS:2CA7
    push ax                                                          # CS:2CAA
    call fill_hud_rect_both_pages                                    # CS:2CAB
    pop ax                                                           # CS:2CAE
    cmp al, 0x6                                                      # CS:2CAF
    jne L_draw_control_prompt_2cbc                                   # CS:2CB1
    mov al, 0x0                                                      # CS:2CB3
    call draw_control_prompt_piece                                   # CS:2CB5
    mov al, 0x6                                                      # CS:2CB8
    jmp draw_control_prompt_piece                                    # CS:2CBA
L_draw_control_prompt_2cbc:
    cmp al, 0x7                                                      # CS:2CBC
    jne draw_control_prompt_piece                                    # CS:2CBE
    mov al, 0x1                                                      # CS:2CC0
    call draw_control_prompt_piece                                   # CS:2CC2
    mov al, 0x7                                                      # CS:2CC5

/* ========================================================================== 
 * draw_control_prompt_piece  |  CS:2CC7
 * Назначение: По индексу берёт bank/source/destination/width/height из пяти таблиц и рисует фрагмент.
 * Вход: AL = индекс.
 * Эффект/ограничения: Также самостоятельная CALL-цель и хвост предыдущей процедуры.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_control_prompt_piece, @function
draw_control_prompt_piece:
    {load} mov bl, al                                                # CS:2CC7
    mov bh, 0x0                                                      # CS:2CC9
    shl bx, 1                                                        # CS:2CCB
    mov ax, WORD PTR [bx+D_control_prompt_sources]                   # CS:2CCD
    push ax                                                          # CS:2CD1
    mov ax, WORD PTR [bx+D_control_prompt_destinations]              # CS:2CD2
    push ax                                                          # CS:2CD6
    mov ax, WORD PTR [bx+D_control_prompt_widths]                    # CS:2CD7
    push ax                                                          # CS:2CDB
    mov ax, WORD PTR [bx+D_control_prompt_heights]                   # CS:2CDC
    push ax                                                          # CS:2CE0
    mov ax, WORD PTR [bx+D_control_prompt_banks]                     # CS:2CE1
    push ax                                                          # CS:2CE5
    call blit_hud_sprite_both_pages                                  # CS:2CE6
    ret                                                              # CS:2CE9

/* ========================================================================== 
 * blit_hud_sprite_both_pages  |  CS:2CEA
 * Назначение: Выводит packed-4bpp ресурс с прозрачным цветом 2 сразу в A000 и A200 страницы.
 * Вход: [BP+4]=bank offset, +6=height, +8=width, +10=dest byte offset, +12=source offset.
 * Эффект/ограничения: RET 10; width обрабатывается парами. Source stride160, destination stride40.
 * Возвраты в физическом диапазоне: ret 0xa.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type blit_hud_sprite_both_pages, @function
blit_hud_sprite_both_pages:
    push bp                                                          # CS:2CEA
    {load} mov bp, sp                                                # CS:2CEB
    mov ax, 0xa000                                                   # CS:2CED
    mov es, ax                                                       # CS:2CF0
    mov di, WORD PTR [bp+0xa]                                        # CS:2CF2
    mov bx, WORD PTR [bp+0x4]                                        # CS:2CF5
    mov bx, WORD PTR [bx+D_segment_bird1]                            # CS:2CF8
    mov si, WORD PTR [bp+0xc]                                        # CS:2CFC
    push ds                                                          # CS:2CFF
    push bx                                                          # CS:2D00
    mov dx, 0x3ce                                                    # CS:2D01
    mov ax, 0xa05                                                    # CS:2D04
    out dx, ax                                                       # CS:2D07
    mov ax, 0x7                                                      # CS:2D08
    out dx, ax                                                       # CS:2D0B
    mov cx, WORD PTR [bp+0x8]                                        # CS:2D0C
    shr cx, 1                                                        # CS:2D0F
    mov ah, 0x80                                                     # CS:2D11
    mov al, 0x8                                                      # CS:2D13
    pop ds                                                           # CS:2D15
    mov bx, WORD PTR [bp+0x6]                                        # CS:2D16
L_blit_hud_sprite_both_pages_2d19:
    push ax                                                          # CS:2D19
    push bx                                                          # CS:2D1A
    push cx                                                          # CS:2D1B
    push di                                                          # CS:2D1C
    push si                                                          # CS:2D1D
L_blit_hud_sprite_both_pages_2d1e:
    mov bh, 0x22                                                     # CS:2D1E
L_blit_hud_sprite_both_pages_2d20:
    mov bl, BYTE PTR [si]                                            # CS:2D20
    inc si                                                           # CS:2D22
    {load} cmp bh, bl                                                # CS:2D23
    jne L_blit_hud_sprite_both_pages_2d3d                            # CS:2D25
    shr ah, 1                                                        # CS:2D27
    jne L_blit_hud_sprite_both_pages_2d32                            # CS:2D29
    mov ah, 0x40                                                     # CS:2D2B
    inc di                                                           # CS:2D2D
    loop L_blit_hud_sprite_both_pages_2d20                           # CS:2D2E
    jmp L_blit_hud_sprite_both_pages_2d76                            # CS:2D30
L_blit_hud_sprite_both_pages_2d32:
    shr ah, 1                                                        # CS:2D32
    jne L_blit_hud_sprite_both_pages_2d39                            # CS:2D34
    mov ah, 0x80                                                     # CS:2D36
    inc di                                                           # CS:2D38
L_blit_hud_sprite_both_pages_2d39:
    loop L_blit_hud_sprite_both_pages_2d20                           # CS:2D39
    jmp L_blit_hud_sprite_both_pages_2d76                            # CS:2D3B
L_blit_hud_sprite_both_pages_2d3d:
    {load} mov bh, bl                                                # CS:2D3D
    shr bl, 1                                                        # CS:2D3F
    shr bl, 1                                                        # CS:2D41
    shr bl, 1                                                        # CS:2D43
    shr bl, 1                                                        # CS:2D45
    cmp bl, 0x2                                                      # CS:2D47
    je L_blit_hud_sprite_both_pages_2d55                             # CS:2D4A
    out dx, ax                                                       # CS:2D4C
    and BYTE PTR es:[di], bl                                         # CS:2D4D
    and BYTE PTR es:[di+0x2000], bl                                  # CS:2D50
L_blit_hud_sprite_both_pages_2d55:
    shr ah, 1                                                        # CS:2D55
    jne L_blit_hud_sprite_both_pages_2d5c                            # CS:2D57
    mov ah, 0x80                                                     # CS:2D59
    inc di                                                           # CS:2D5B
L_blit_hud_sprite_both_pages_2d5c:
    and bh, 0xf                                                      # CS:2D5C
    cmp bh, 0x2                                                      # CS:2D5F
    je L_blit_hud_sprite_both_pages_2d6d                             # CS:2D62
    out dx, ax                                                       # CS:2D64
    and BYTE PTR es:[di], bh                                         # CS:2D65
    and BYTE PTR es:[di+0x2000], bh                                  # CS:2D68
L_blit_hud_sprite_both_pages_2d6d:
    shr ah, 1                                                        # CS:2D6D
    jne L_blit_hud_sprite_both_pages_2d74                            # CS:2D6F
    mov ah, 0x80                                                     # CS:2D71
    inc di                                                           # CS:2D73
L_blit_hud_sprite_both_pages_2d74:
    loop L_blit_hud_sprite_both_pages_2d1e                           # CS:2D74
L_blit_hud_sprite_both_pages_2d76:
    pop si                                                           # CS:2D76
    add si, 0xa0                                                     # CS:2D77
    pop di                                                           # CS:2D7B
    add di, 0x28                                                     # CS:2D7C
    and di, 0x7fff                                                   # CS:2D7F
    pop cx                                                           # CS:2D83
    pop bx                                                           # CS:2D84
    pop ax                                                           # CS:2D85
    dec bx                                                           # CS:2D86
    jne L_blit_hud_sprite_both_pages_2d19                            # CS:2D87
    pop ds                                                           # CS:2D89
    mov ax, 0xff08                                                   # CS:2D8A
    out dx, ax                                                       # CS:2D8D
    mov ax, 0x5                                                      # CS:2D8E
    out dx, ax                                                       # CS:2D91
    mov ax, 0xf07                                                    # CS:2D92
    out dx, ax                                                       # CS:2D95
    pop bp                                                           # CS:2D96
    ret 0xa                                                          # CS:2D97

/* ========================================================================== 
 * fill_hud_rect_both_pages  |  CS:2D9A
 * Назначение: Заливает прямоугольник через EGA set/reset одновременно в обеих страницах.
 * Вход: [BP+4]=color, +6=height, +8=width BYTES, +10=destination byte offset.
 * Эффект/ограничения: RET 8; ширина в байтах плоскости, не в пикселях.
 * Возвраты в физическом диапазоне: ret 0x8.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type fill_hud_rect_both_pages, @function
fill_hud_rect_both_pages:
    push bp                                                          # CS:2D9A
    {load} mov bp, sp                                                # CS:2D9B
    mov ax, 0xa000                                                   # CS:2D9D
    mov es, ax                                                       # CS:2DA0
    mov di, WORD PTR [bp+0xa]                                        # CS:2DA2
    mov dx, 0x3ce                                                    # CS:2DA5
    mov ax, WORD PTR [bp+0x4]                                        # CS:2DA8
    {load} mov ah, al                                                # CS:2DAB
    mov al, 0x0                                                      # CS:2DAD
    out dx, ax                                                       # CS:2DAF
    mov ax, 0xf01                                                    # CS:2DB0
    out dx, ax                                                       # CS:2DB3
    cld                                                              # CS:2DB4
    mov bx, WORD PTR [bp+0x6]                                        # CS:2DB5
L_fill_hud_rect_both_pages_2db8:
    push di                                                          # CS:2DB8
    mov cx, WORD PTR [bp+0x8]                                        # CS:2DB9
    rep stos BYTE PTR es:[di], al                                    # CS:2DBC
    pop di                                                           # CS:2DBE
    push di                                                          # CS:2DBF
    add di, 0x2000                                                   # CS:2DC0
    mov cx, WORD PTR [bp+0x8]                                        # CS:2DC4
    rep stos BYTE PTR es:[di], al                                    # CS:2DC7
    pop di                                                           # CS:2DC9
    add di, 0x28                                                     # CS:2DCA
    dec bx                                                           # CS:2DCD
    jne L_fill_hud_rect_both_pages_2db8                              # CS:2DCE
    mov ax, 0x0                                                      # CS:2DD0
    out dx, ax                                                       # CS:2DD3
    inc al                                                           # CS:2DD4
    out dx, ax                                                       # CS:2DD6
    pop bp                                                           # CS:2DD7
    ret 0x8                                                          # CS:2DD8

/* CS:2DDB: Inter-procedure alignment byte, not an instruction. */
padding_2ddb:
    .byte 0x00
