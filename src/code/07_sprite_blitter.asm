/* Packed4bpp → EGA и отсечение
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * blit_sprite  |  CS:0C24
 * Назначение: Отсекает и выводит packed-4bpp sprite через EGA write mode 2; сохраняет прозрачный цвет.
 * Вход: [BP+4]=height, +6=width, +8=y, +10=x; BX=segment; SI=source offset.
 * Эффект/ограничения: RET 8; DS восстанавливается. Горизонтальная граница сдвигает x целиком, не обрезает источник; ширина обрабатывается парами пикселей.
 * Возвраты в физическом диапазоне: ret, ret 0x8.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type blit_sprite, @function
blit_sprite:

    # Stack ABI: height,width,y,x по BP+4,+6,+8,+10. Процедура сама снимает 8 байт аргументов.
    push bp                                                          # CS:0C24
    {load} mov bp, sp                                                # CS:0C25
    mov ax, ds:[D_sprite_clip_enabled]                               # CS:0C27
    ACC16 cmp, 0x0                                                   # CS:0C2A
    jne L_blit_clip_to_viewport                                      # CS:0C2D
    jmp L_blit_setup_ega                                             # CS:0C2F
L_blit_clip_to_viewport:

    # Разность Y нормализуется в кольце 1024 и интерпретируется как диапазон [-512,511].
    mov ax, WORD PTR [bp+BLIT_Y]                                     # CS:0C32
    sub ax, WORD PTR ds:[D_camera_y]                                 # CS:0C35
    ACC16 and, WORLD_Y_MASK                                          # CS:0C39
    test ax, 0x200                                                   # CS:0C3C
    je L_blit_y_signed                                               # CS:0C3F
    ACC16 or, 0xfc00                                                 # CS:0C41
L_blit_y_signed:
    mov WORD PTR [bp+BLIT_Y], ax                                     # CS:0C44
    ACC16 cmp, 0x0                                                   # CS:0C47
    jge L_blit_clip_bottom                                           # CS:0C4A
    add WORD PTR [bp+BLIT_HEIGHT], ax                                # CS:0C4C
    jns L_blit_sprite_0c54                                           # CS:0C4F
    jmp L_blit_return                                                # CS:0C51
L_blit_sprite_0c54:
    jne L_blit_skip_source_rows                                      # CS:0C54
    jmp L_blit_return                                                # CS:0C56
L_blit_skip_source_rows:

    # Срез сверху: сместить packed source на число пропущенных строк *160 байт.
    neg ax                                                           # CS:0C59
    shl ax, 1                                                        # CS:0C5B
    shl ax, 1                                                        # CS:0C5D
    shl ax, 1                                                        # CS:0C5F
    {load} mov cx, ax                                                # CS:0C61
    shl ax, 1                                                        # CS:0C63
    shl ax, 1                                                        # CS:0C65
    {load} add cx, ax                                                # CS:0C67
    shl cx, 1                                                        # CS:0C69
    shl cx, 1                                                        # CS:0C6B
    {load} add si, cx                                                # CS:0C6D
    mov ax, 0x0                                                      # CS:0C6F
    mov WORD PTR [bp+BLIT_Y], ax                                     # CS:0C72
    jmp L_blit_add_viewport_origin                                   # CS:0C75
L_blit_clip_bottom:
    add ax, WORD PTR [bp+BLIT_HEIGHT]                                # CS:0C77
    ACC16 sub, 0xa0                                                  # CS:0C7A
    js L_blit_add_viewport_origin                                    # CS:0C7D
    neg ax                                                           # CS:0C7F
    add ax, WORD PTR [bp+BLIT_HEIGHT]                                # CS:0C81
    ACC16 cmp, 0x0                                                   # CS:0C84
    jg L_blit_sprite_0c8c                                            # CS:0C87
    jmp L_blit_return                                                # CS:0C89
L_blit_sprite_0c8c:
    mov WORD PTR [bp+BLIT_HEIGHT], ax                                # CS:0C8C
L_blit_add_viewport_origin:

    # Экранное окно начинается в (32,8). Горизонталь прижимает весь спрайт к краю, а не срезает его!
    mov ax, 0x20                                                     # CS:0C8F
    add WORD PTR [bp+BLIT_X], ax                                     # CS:0C92
    mov ax, 0x8                                                      # CS:0C95
    add WORD PTR [bp+BLIT_Y], ax                                     # CS:0C98
    mov ax, WORD PTR [bp+BLIT_X]                                     # CS:0C9B
    ACC16 cmp, 0x20                                                  # CS:0C9E
    jge L_blit_sprite_0cab                                           # CS:0CA1
    mov ax, 0x20                                                     # CS:0CA3
    mov WORD PTR [bp+BLIT_X], ax                                     # CS:0CA6
    jmp L_blit_setup_ega                                             # CS:0CA9
L_blit_sprite_0cab:
    add ax, WORD PTR [bp+BLIT_WIDTH]                                 # CS:0CAB
    ACC16 cmp, 0x120                                                 # CS:0CAE
    jle L_blit_setup_ega                                             # CS:0CB1
    mov ax, 0x120                                                    # CS:0CB3
    sub ax, WORD PTR [bp+BLIT_WIDTH]                                 # CS:0CB6
    mov WORD PTR [bp+BLIT_X], ax                                     # CS:0CB9
L_blit_setup_ega:

    # Ниже DS временно меняется на банк пикселей. Операнды [si] здесь НЕ являются Entity.
    push ds                                                          # CS:0CBC
    push bx                                                          # CS:0CBD
    mov dx, 0x3ce                                                    # CS:0CBE
    mov ax, 0xa05                                                    # CS:0CC1
    out dx, ax                                                       # CS:0CC4
    mov ax, 0x7                                                      # CS:0CC5
    out dx, ax                                                       # CS:0CC8
    mov ax, WORD PTR [bp+BLIT_X]                                     # CS:0CC9
    mov bx, WORD PTR [bp+BLIT_Y]                                     # CS:0CCC
    push cx                                                          # CS:0CCF
    shl bx, 1                                                        # CS:0CD0
    shl bx, 1                                                        # CS:0CD2
    shl bx, 1                                                        # CS:0CD4
    mov cx, WORD PTR cs:[CS_sprite_stride]                           # CS:0CD6
    cmp cx, 0x28                                                     # CS:0CDB
    jne L_blit_sprite_0ce4                                           # CS:0CDE
    {load} mov di, bx                                                # CS:0CE0
    jmp L_blit_sprite_0ce7                                           # CS:0CE2
L_blit_sprite_0ce4:
    mov di, 0x0                                                      # CS:0CE4
L_blit_sprite_0ce7:
    shl bx, 1                                                        # CS:0CE7
    shl bx, 1                                                        # CS:0CE9
    {load} add di, bx                                                # CS:0CEB
    {load} mov cl, al                                                # CS:0CED
    shr ax, 1                                                        # CS:0CEF
    shr ax, 1                                                        # CS:0CF1
    shr ax, 1                                                        # CS:0CF3
    {load} add di, ax                                                # CS:0CF5
    and cl, 0x7                                                      # CS:0CF7
    mov ax, 0x80                                                     # CS:0CFA
    shr ax, cl                                                       # CS:0CFD
    {load} mov ah, al                                                # CS:0CFF
    mov bx, WORD PTR ds:[D_sprite_dest_segment]                      # CS:0D01
    mov es, bx                                                       # CS:0D05
    pop cx                                                           # CS:0D07

    # Обрабатываются width>>1 пар пикселей. Нечётную ширину нельзя молча дорисовать в C.
    mov cx, WORD PTR [bp+BLIT_WIDTH]                                 # CS:0D08
    shr cx, 1                                                        # CS:0D0B
    mov al, 0x8                                                      # CS:0D0D
    pop ds                                                           # CS:0D0F
    mov bx, WORD PTR [bp+BLIT_HEIGHT]                                # CS:0D10
L_blit_next_row:
    push ax                                                          # CS:0D13
    push bx                                                          # CS:0D14
    push cx                                                          # CS:0D15
    push di                                                          # CS:0D16
    push si                                                          # CS:0D17
L_blit_pixel_pair:

    # Packed4bpp: верхняя тетрада идёт первой. Быстрый путь пропускает сразу пару прозрачных пикселей.
    mov bh, BYTE PTR cs:[CS_transparent_pair]                        # CS:0D18
L_blit_sprite_0d1d:
    mov bl, BYTE PTR [si]                                            # CS:0D1D
    inc si                                                           # CS:0D1F
    {load} cmp bh, bl                                                # CS:0D20
    jne L_blit_nontransparent_pair                                   # CS:0D22
    shr ah, 1                                                        # CS:0D24
    jne L_blit_sprite_0d2f                                           # CS:0D26
    mov ah, 0x40                                                     # CS:0D28
    inc di                                                           # CS:0D2A
    loop L_blit_sprite_0d1d                                          # CS:0D2B
    jmp L_blit_advance_row                                           # CS:0D2D
L_blit_sprite_0d2f:
    shr ah, 1                                                        # CS:0D2F
    jne L_blit_sprite_0d36                                           # CS:0D31
    mov ah, 0x80                                                     # CS:0D33
    inc di                                                           # CS:0D35
L_blit_sprite_0d36:
    loop L_blit_sprite_0d1d                                          # CS:0D36
    jmp L_blit_advance_row                                           # CS:0D38
L_blit_nontransparent_pair:
    {load} mov bh, bl                                                # CS:0D3A
    shr bl, 1                                                        # CS:0D3C
    shr bl, 1                                                        # CS:0D3E
    shr bl, 1                                                        # CS:0D40
    shr bl, 1                                                        # CS:0D42
    cmp bl, BYTE PTR cs:[CS_transparent_color]                       # CS:0D44
    je L_blit_sprite_0d4f                                            # CS:0D49
    out dx, ax                                                       # CS:0D4B
    and BYTE PTR es:[di], bl                                         # CS:0D4C
L_blit_sprite_0d4f:
    shr ah, 1                                                        # CS:0D4F
    jne L_blit_sprite_0d56                                           # CS:0D51
    mov ah, 0x80                                                     # CS:0D53
    inc di                                                           # CS:0D55
L_blit_sprite_0d56:
    and bh, 0xf                                                      # CS:0D56
    cmp bh, BYTE PTR cs:[CS_transparent_color]                       # CS:0D59
    je L_blit_sprite_0d64                                            # CS:0D5E
    out dx, ax                                                       # CS:0D60
    and BYTE PTR es:[di], bh                                         # CS:0D61
L_blit_sprite_0d64:
    shr ah, 1                                                        # CS:0D64
    jne L_blit_sprite_0d6b                                           # CS:0D66
    mov ah, 0x80                                                     # CS:0D68
    inc di                                                           # CS:0D6A
L_blit_sprite_0d6b:
    loop L_blit_pixel_pair                                           # CS:0D6B
L_blit_advance_row:

    # Следующая source-строка всегда через160 байт; destination заворачивается по &0x7FFF.
    pop si                                                           # CS:0D6D
    add si, 0xa0                                                     # CS:0D6E
    pop di                                                           # CS:0D72
    add di, WORD PTR cs:[CS_sprite_stride]                           # CS:0D73
    and di, 0x7fff                                                   # CS:0D78
    pop cx                                                           # CS:0D7C
    pop bx                                                           # CS:0D7D
    pop ax                                                           # CS:0D7E
    dec bx                                                           # CS:0D7F
    jne L_blit_next_row                                              # CS:0D80
    pop ds                                                           # CS:0D82
    mov ax, 0xff08                                                   # CS:0D83
    out dx, ax                                                       # CS:0D86
    mov ax, 0x5                                                      # CS:0D87
    out dx, ax                                                       # CS:0D8A
    mov ax, 0xf07                                                    # CS:0D8B
    out dx, ax                                                       # CS:0D8E
L_blit_return:
    pop bp                                                           # CS:0D8F
    ret 0x8                                                          # CS:0D90

/* CS:0D93: Writable CS-local row stride (word); read/written by the sprite renderer. */
sprite_stride:
    .word 40


/* CS:0D95: Writable CS-local transparent color (byte). */
transparent_color:
    .byte 0x00


/* CS:0D96: Writable CS-local duplicated transparent nibble (byte). */
transparent_pair:
    .byte 0x00

    ret                                                              # CS:0D97 | Не достигнуто текущим статическим CFG; не удалять.
