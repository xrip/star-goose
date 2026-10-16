/* Видеостраницы, копирование, CRTC, синхронизация
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * present_tunnel_bitmap  |  CS:31A4
 * Назначение: Копирует 256×160 участок 1bpp туннеля в draw page с выбором EGA-цветовых плоскостей.
 * Вход: scratch0 с bitmap; tunnel_reward_kind и flash state.
 * Эффект/ограничения: Источник stride32, назначение40; это не packed4bpp sprite.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type present_tunnel_bitmap, @function
present_tunnel_bitmap:
    mov ax, ds:[D_video_draw_segment]                                # CS:31A4
    mov es, ax                                                       # CS:31A7
    mov di, 0x144                                                    # CS:31A9
    mov bx, WORD PTR ds:[D_segment_scratch0]                         # CS:31AC
    mov si, 0x340                                                    # CS:31B0
    call tunnel_plane_color                                          # CS:31B3
    not ax                                                           # CS:31B6
    ACC16 and, 0xf                                                   # CS:31B8
    {load} mov ah, al                                                # CS:31BB
    mov al, 0x1                                                      # CS:31BD
    mov dx, 0x3ce                                                    # CS:31BF
    out dx, ax                                                       # CS:31C2
    call tunnel_flash_color                                          # CS:31C3
    {load} mov ah, al                                                # CS:31C6
    mov al, 0x0                                                      # CS:31C8
    out dx, ax                                                       # CS:31CA
    push ds                                                          # CS:31CB
    mov ds, bx                                                       # CS:31CC
    cld                                                              # CS:31CE
    mov cx, 0xa0                                                     # CS:31CF
L_present_tunnel_bitmap_31d2:
    push cx                                                          # CS:31D2
    mov cx, 0x20                                                     # CS:31D3
    rep movs BYTE PTR es:[di], BYTE PTR ds:[si]                      # CS:31D6
    add di, 0x8                                                      # CS:31D8
    pop cx                                                           # CS:31DB
    loop L_present_tunnel_bitmap_31d2                                # CS:31DC
    pop ds                                                           # CS:31DE
    mov ax, 0x0                                                      # CS:31DF
    out dx, ax                                                       # CS:31E2
    inc al                                                           # CS:31E3
    out dx, ax                                                       # CS:31E5
    ret                                                              # CS:31E6

/* ========================================================================== 
 * tunnel_plane_color  |  CS:31E7
 * Назначение: Выбирает маску/цвет туннеля по типу награды.
 * Вход: tunnel_reward_kind.
 * Эффект/ограничения: AX: 40h→2, 80h→4, C0h→1, иначе3.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_plane_color, @function
tunnel_plane_color:
    mov al, ds:[D_tunnel_reward_kind]                                # CS:31E7
    cmp al, 0x40                                                     # CS:31EA
    jne L_tunnel_plane_color_31f3                                    # CS:31EC
    mov ax, 0x2                                                      # CS:31EE
    jmp L_tunnel_plane_color_3208                                    # CS:31F1
L_tunnel_plane_color_31f3:
    cmp al, 0x80                                                     # CS:31F3
    jne L_tunnel_plane_color_31fc                                    # CS:31F5
    mov ax, 0x4                                                      # CS:31F7
    jmp L_tunnel_plane_color_3208                                    # CS:31FA
L_tunnel_plane_color_31fc:
    cmp al, 0xc0                                                     # CS:31FC
    jne L_tunnel_plane_color_3205                                    # CS:31FE
    mov ax, 0x1                                                      # CS:3200
    jmp L_tunnel_plane_color_3208                                    # CS:3203
L_tunnel_plane_color_3205:
    mov ax, 0x3                                                      # CS:3205
L_tunnel_plane_color_3208:
    ret                                                              # CS:3208

/* ========================================================================== 
 * tunnel_flash_color  |  CS:3209
 * Назначение: Выдаёт цвет вспышки и переводит двухшаговое состояние к нулю.
 * Вход: tunnel_flash_state.
 * Эффект/ограничения: 0→color0; 1→color7/state0; другое→color15/state1.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type tunnel_flash_color, @function
tunnel_flash_color:
    mov al, ds:[D_tunnel_flash_state]                                # CS:3209
    cmp al, 0x0                                                      # CS:320C
    jne L_tunnel_flash_color_3215                                    # CS:320E
    mov ax, 0x0                                                      # CS:3210
    jmp L_tunnel_flash_color_322b                                    # CS:3213
L_tunnel_flash_color_3215:
    cmp al, 0x1                                                      # CS:3215
    jne L_tunnel_flash_color_3223                                    # CS:3217
    mov ax, 0x7                                                      # CS:3219
    mov BYTE PTR ds:[D_tunnel_flash_state], 0x0                      # CS:321C
    jmp L_tunnel_flash_color_322b                                    # CS:3221
L_tunnel_flash_color_3223:
    mov ax, 0xf                                                      # CS:3223
    mov BYTE PTR ds:[D_tunnel_flash_state], 0x1                      # CS:3226
L_tunnel_flash_color_322b:
    ret                                                              # CS:322B

/* ========================================================================== 
 * unused_planar_blit  |  CS:322C
 * Назначение: Сохранённый, статически недостижимый blit четырёх плоскостей с шагом источника 8000.
 * Вход: [BP+4]=bank,+6=height,+8=width bytes,+10=dest,+12=source.
 * Эффект/ограничения: RET10; не удалять из byte-exact сборки. Не доказано, что никогда не выполняется.
 * Возвраты в физическом диапазоне: ret 0xa.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type unused_planar_blit, @function
unused_planar_blit:
    push bp                                                          # CS:322C | Не достигнуто текущим статическим CFG; не удалять.
    {load} mov bp, sp                                                # CS:322D | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, ds:[D_video_draw_segment]                                # CS:322F | Не достигнуто текущим статическим CFG; не удалять.
    mov es, ax                                                       # CS:3232 | Не достигнуто текущим статическим CFG; не удалять.
    mov di, WORD PTR [bp+0xa]                                        # CS:3234 | Не достигнуто текущим статическим CFG; не удалять.
    mov bx, WORD PTR [bp+0x4]                                        # CS:3237 | Не достигнуто текущим статическим CFG; не удалять.
    mov bx, WORD PTR [bx+D_segment_bird1]                            # CS:323A | Не достигнуто текущим статическим CFG; не удалять.
    mov si, WORD PTR [bp+0xc]                                        # CS:323E | Не достигнуто текущим статическим CFG; не удалять.
    mov dx, 0x3ce                                                    # CS:3241 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0x805                                                    # CS:3244 | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:3247 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0x7                                                      # CS:3248 | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:324B | Не достигнуто текущим статическим CFG; не удалять.
    mov dl, 0xc4                                                     # CS:324C | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0x802                                                    # CS:324E | Не достигнуто текущим статическим CFG; не удалять.
    push ds                                                          # CS:3251 | Не достигнуто текущим статическим CFG; не удалять.
    mov ds, bx                                                       # CS:3252 | Не достигнуто текущим статическим CFG; не удалять.
L_unused_planar_blit_3254:
    out dx, ax                                                       # CS:3254 | Не достигнуто текущим статическим CFG; не удалять.
    push ax                                                          # CS:3255 | Не достигнуто текущим статическим CFG; не удалять.
    push di                                                          # CS:3256 | Не достигнуто текущим статическим CFG; не удалять.
    push si                                                          # CS:3257 | Не достигнуто текущим статическим CFG; не удалять.
    mov cx, WORD PTR [bp+0x6]                                        # CS:3258 | Не достигнуто текущим статическим CFG; не удалять.
L_unused_planar_blit_325b:
    push di                                                          # CS:325B | Не достигнуто текущим статическим CFG; не удалять.
    push si                                                          # CS:325C | Не достигнуто текущим статическим CFG; не удалять.
    push cx                                                          # CS:325D | Не достигнуто текущим статическим CFG; не удалять.
    mov cx, WORD PTR [bp+0x8]                                        # CS:325E | Не достигнуто текущим статическим CFG; не удалять.
L_unused_planar_blit_3261:
    lods al, BYTE PTR ds:[si]                                        # CS:3261 | Не достигнуто текущим статическим CFG; не удалять.
    and BYTE PTR es:[di], al                                         # CS:3262 | Не достигнуто текущим статическим CFG; не удалять.
    inc di                                                           # CS:3265 | Не достигнуто текущим статическим CFG; не удалять.
    loop L_unused_planar_blit_3261                                   # CS:3266 | Не достигнуто текущим статическим CFG; не удалять.
    pop cx                                                           # CS:3268 | Не достигнуто текущим статическим CFG; не удалять.
    pop si                                                           # CS:3269 | Не достигнуто текущим статическим CFG; не удалять.
    pop di                                                           # CS:326A | Не достигнуто текущим статическим CFG; не удалять.
    add si, 0x28                                                     # CS:326B | Не достигнуто текущим статическим CFG; не удалять.
    add di, 0x28                                                     # CS:326E | Не достигнуто текущим статическим CFG; не удалять.
    loop L_unused_planar_blit_325b                                   # CS:3271 | Не достигнуто текущим статическим CFG; не удалять.
    pop si                                                           # CS:3273 | Не достигнуто текущим статическим CFG; не удалять.
    pop di                                                           # CS:3274 | Не достигнуто текущим статическим CFG; не удалять.
    pop ax                                                           # CS:3275 | Не достигнуто текущим статическим CFG; не удалять.
    add si, 0x1f40                                                   # CS:3276 | Не достигнуто текущим статическим CFG; не удалять.
    shr ah, 1                                                        # CS:327A | Не достигнуто текущим статическим CFG; не удалять.
    jne L_unused_planar_blit_3254                                    # CS:327C | Не достигнуто текущим статическим CFG; не удалять.
    pop ds                                                           # CS:327E | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0xf02                                                    # CS:327F | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:3282 | Не достигнуто текущим статическим CFG; не удалять.
    mov dl, 0xce                                                     # CS:3283 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0x5                                                      # CS:3285 | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:3288 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0xf07                                                    # CS:3289 | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:328C | Не достигнуто текущим статическим CFG; не удалять.
    pop bp                                                           # CS:328D | Не достигнуто текущим статическим CFG; не удалять.
    ret 0xa                                                          # CS:328E | Не достигнуто текущим статическим CFG; не удалять.

/* ========================================================================== 
 * copy_playfield_between_pages  |  CS:3291
 * Назначение: Копирует видимую область 256×160 между EGA-сегментами через latch copy.
 * Вход: [BP+4]=destination segment; [BP+6]=source segment.
 * Эффект/ограничения: RET4; начальный offset=0144h, stride40, 32 байта на строку.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type copy_playfield_between_pages, @function
copy_playfield_between_pages:
    push bp                                                          # CS:3291
    {load} mov bp, sp                                                # CS:3292
    mov ax, WORD PTR [bp+0x4]                                        # CS:3294
    mov es, ax                                                       # CS:3297
    mov di, 0x144                                                    # CS:3299
    mov bx, WORD PTR [bp+0x6]                                        # CS:329C
    mov si, 0x144                                                    # CS:329F
    mov dx, 0x3ce                                                    # CS:32A2
    mov ax, 0x105                                                    # CS:32A5
    out dx, ax                                                       # CS:32A8
    push ds                                                          # CS:32A9
    mov ds, bx                                                       # CS:32AA
    cld                                                              # CS:32AC
    mov cx, 0xa0                                                     # CS:32AD
L_copy_playfield_between_pages_32b0:
    push cx                                                          # CS:32B0
    mov cx, 0x20                                                     # CS:32B1
    rep movs BYTE PTR es:[di], BYTE PTR ds:[si]                      # CS:32B4
    add si, 0x8                                                      # CS:32B6
    add di, 0x8                                                      # CS:32B9
    pop cx                                                           # CS:32BC
    loop L_copy_playfield_between_pages_32b0                         # CS:32BD
    pop ds                                                           # CS:32BF
    mov ax, 0x5                                                      # CS:32C0
    out dx, ax                                                       # CS:32C3
    pop bp                                                           # CS:32C4
    ret 0x4                                                          # CS:32C5

/* ========================================================================== 
 * unused_rectangle_fill  |  CS:32C8
 * Назначение: Сохранённая недостижимая заливка прямоугольника draw page.
 * Вход: [BP+4]=color,+6=height,+8=width bytes,+10=dest.
 * Эффект/ограничения: RET8; отсутствие статического пути не доказательство неиспользования при любых условиях.
 * Возвраты в физическом диапазоне: ret 0x8.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type unused_rectangle_fill, @function
unused_rectangle_fill:
    push bp                                                          # CS:32C8 | Не достигнуто текущим статическим CFG; не удалять.
    {load} mov bp, sp                                                # CS:32C9 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, ds:[D_video_draw_segment]                                # CS:32CB | Не достигнуто текущим статическим CFG; не удалять.
    mov es, ax                                                       # CS:32CE | Не достигнуто текущим статическим CFG; не удалять.
    mov di, WORD PTR [bp+0xa]                                        # CS:32D0 | Не достигнуто текущим статическим CFG; не удалять.
    mov dx, 0x3ce                                                    # CS:32D3 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, WORD PTR [bp+0x4]                                        # CS:32D6 | Не достигнуто текущим статическим CFG; не удалять.
    {load} mov ah, al                                                # CS:32D9 | Не достигнуто текущим статическим CFG; не удалять.
    mov al, 0x0                                                      # CS:32DB | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:32DD | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0xf01                                                    # CS:32DE | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:32E1 | Не достигнуто текущим статическим CFG; не удалять.
    cld                                                              # CS:32E2 | Не достигнуто текущим статическим CFG; не удалять.
    mov bx, WORD PTR [bp+0x6]                                        # CS:32E3 | Не достигнуто текущим статическим CFG; не удалять.
L_unused_rectangle_fill_32e6:
    push di                                                          # CS:32E6 | Не достигнуто текущим статическим CFG; не удалять.
    mov cx, WORD PTR [bp+0x8]                                        # CS:32E7 | Не достигнуто текущим статическим CFG; не удалять.
    rep stos BYTE PTR es:[di], al                                    # CS:32EA | Не достигнуто текущим статическим CFG; не удалять.
    pop di                                                           # CS:32EC | Не достигнуто текущим статическим CFG; не удалять.
    add di, 0x28                                                     # CS:32ED | Не достигнуто текущим статическим CFG; не удалять.
    dec bx                                                           # CS:32F0 | Не достигнуто текущим статическим CFG; не удалять.
    jne L_unused_rectangle_fill_32e6                                 # CS:32F1 | Не достигнуто текущим статическим CFG; не удалять.
    mov ax, 0x0                                                      # CS:32F3 | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:32F6 | Не достигнуто текущим статическим CFG; не удалять.
    inc al                                                           # CS:32F7 | Не достигнуто текущим статическим CFG; не удалять.
    out dx, ax                                                       # CS:32F9 | Не достигнуто текущим статическим CFG; не удалять.
    pop bp                                                           # CS:32FA | Не достигнуто текущим статическим CFG; не удалять.
    ret 0x8                                                          # CS:32FB | Не достигнуто текущим статическим CFG; не удалять.

/* ========================================================================== 
 * draw_scrolling_background  |  CS:32FE
 * Назначение: Копирует 160 строк из кольцевого фона A800h, начиная с camera_y*32, на draw page.
 * Вход: camera_y.
 * Эффект/ограничения: Source address маскируется 7FFFh после каждой строки; viewport offset0144h.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_scrolling_background, @function
draw_scrolling_background:
    mov dx, 0x3ce                                                    # CS:32FE
    mov ax, 0x105                                                    # CS:3301
    out dx, ax                                                       # CS:3304
    mov ax, ds:[D_video_draw_segment]                                # CS:3305
    mov es, ax                                                       # CS:3308
    mov di, 0x144                                                    # CS:330A
    mov ax, ds:[D_camera_y]                                          # CS:330D
    shl ax, 1                                                        # CS:3310
    shl ax, 1                                                        # CS:3312
    shl ax, 1                                                        # CS:3314
    shl ax, 1                                                        # CS:3316
    shl ax, 1                                                        # CS:3318
    {load} mov si, ax                                                # CS:331A
    push ds                                                          # CS:331C
    mov ax, 0xa800                                                   # CS:331D
    mov ds, ax                                                       # CS:3320
    cld                                                              # CS:3322
    mov cx, 0xa0                                                     # CS:3323
L_draw_scrolling_background_3326:
    push cx                                                          # CS:3326
    mov cx, 0x20                                                     # CS:3327
    rep movs BYTE PTR es:[di], BYTE PTR ds:[si]                      # CS:332A
    and si, 0x7fff                                                   # CS:332C
    add di, 0x8                                                      # CS:3330
    pop cx                                                           # CS:3333
    loop L_draw_scrolling_background_3326                            # CS:3334
    pop ds                                                           # CS:3336
    mov ax, 0x5                                                      # CS:3337
    out dx, ax                                                       # CS:333A
    ret                                                              # CS:333B

/* ========================================================================== 
 * video_init_mode0d  |  CS:333C
 * Назначение: Включает режим 0Dh, задаёт A000/A200 страницы и начальный CRTC start.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Обращается к BIOS и EGA, в C-порте заменяется backend, а не DOS-эмуляцией в игровой логике.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type video_init_mode0d, @function
video_init_mode0d:

    # Платформенная граница: EGA mode0Dh. В C-порте заменяется backend, не игровой логикой.
    mov ax, 0xd                                                      # CS:333C
    int 0x10                                                         # CS:333F | BIOS video mode 0Dh
    mov ax, 0xa000                                                   # CS:3341
    mov ds:[D_video_display_segment], ax                             # CS:3344
    ACC16 add, 0x200                                                 # CS:3347
    mov ds:[D_video_draw_segment], ax                                # CS:334A
    mov ax, 0x0                                                      # CS:334D
    call clear_draw_page                                             # CS:3350
    mov ax, ds:[D_video_draw_segment]                                # CS:3353
    mov bx, WORD PTR ds:[D_video_display_segment]                    # CS:3356
    mov WORD PTR ds:[D_video_draw_segment], bx                       # CS:335A
    mov ds:[D_video_display_segment], ax                             # CS:335E
    call wait_retrace_cycle                                          # CS:3361
    mov bx, WORD PTR ds:[D_video_display_segment]                    # CS:3364
    shl bx, 1                                                        # CS:3368
    shl bx, 1                                                        # CS:336A
    shl bx, 1                                                        # CS:336C
    shl bx, 1                                                        # CS:336E
    cli                                                              # CS:3370
    mov dx, 0x3d4                                                    # CS:3371
    {load} mov ah, bh                                                # CS:3374
    mov al, 0xc                                                      # CS:3376
    out dx, ax                                                       # CS:3378
    {load} mov ah, bl                                                # CS:3379
    inc al                                                           # CS:337B
    out dx, ax                                                       # CS:337D
    sti                                                              # CS:337E
    call wait_retrace_cycle                                          # CS:337F
    ret                                                              # CS:3382

/* ========================================================================== 
 * present_frame  |  CS:3383
 * Назначение: Меняет местами draw/display сегменты, пишет CRTC start high и ждёт retrace.
 * Вход: Две страницы.
 * Эффект/ограничения: Не использует timer_ticks для шага игровой логики; frame pacing привязан к retrace.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type present_frame, @function
present_frame:

    # Показ кадра меняет страницы и CRTC. Логический кадр привязан к этому пути, не напрямую к IRQ1Ch.
    mov ax, ds:[D_video_draw_segment]                                # CS:3383
    mov bx, WORD PTR ds:[D_video_display_segment]                    # CS:3386
    mov WORD PTR ds:[D_video_draw_segment], bx                       # CS:338A
    mov ds:[D_video_display_segment], ax                             # CS:338E
    mov bx, WORD PTR ds:[D_video_display_segment]                    # CS:3391
    shl bx, 1                                                        # CS:3395
    shl bx, 1                                                        # CS:3397
    shl bx, 1                                                        # CS:3399
    shl bx, 1                                                        # CS:339B
    cli                                                              # CS:339D
    mov dx, 0x3d4                                                    # CS:339E
    {load} mov ah, bh                                                # CS:33A1
    mov al, 0xc                                                      # CS:33A3
    out dx, ax                                                       # CS:33A5
    sti                                                              # CS:33A6
    mov dx, 0x3da                                                    # CS:33A7
L_present_frame_33aa:
    in al, dx                                                        # CS:33AA
    and al, 0x8                                                      # CS:33AB
    jne L_present_frame_33aa                                         # CS:33AD
    call wait_retrace_cycle                                          # CS:33AF
    ret                                                              # CS:33B2

/* ========================================================================== 
 * clear_draw_page  |  CS:33B3
 * Назначение: Заливает текущую страницу указанным EGA-цветом.
 * Вход: AL = цвет.
 * Эффект/ограничения: 8000 байт адресов плоскости, ES=draw segment, DF=0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type clear_draw_page, @function
clear_draw_page:
    push ax                                                          # CS:33B3
    mov ax, ds:[D_video_draw_segment]                                # CS:33B4
    mov es, ax                                                       # CS:33B7
    mov di, 0x0                                                      # CS:33B9
    mov dx, 0x3ce                                                    # CS:33BC
    pop ax                                                           # CS:33BF
    {load} mov ah, al                                                # CS:33C0
    mov al, 0x0                                                      # CS:33C2
    out dx, ax                                                       # CS:33C4
    mov ax, 0xf01                                                    # CS:33C5
    out dx, ax                                                       # CS:33C8
    cld                                                              # CS:33C9
    mov cx, 0xfa0                                                    # CS:33CA
    rep stos WORD PTR es:[di], ax                                    # CS:33CD
    mov ax, 0x0                                                      # CS:33CF
    out dx, ax                                                       # CS:33D2
    inc al                                                           # CS:33D3
    out dx, ax                                                       # CS:33D5
    ret                                                              # CS:33D6

/* ========================================================================== 
 * wait_retrace_cycle  |  CS:33D7
 * Назначение: Ждёт установки и затем снятия бита3 порта3DAh.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Два busy loop; нельзя заменять частоту звукового PIT частотой кадров без измерения.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type wait_retrace_cycle, @function
wait_retrace_cycle:

    # Ожидание бит3 порта3DAh: busy wait. В host-порте нужен явный scheduler, не этот цикл IN.
    mov dx, 0x3da                                                    # CS:33D7
L_wait_retrace_cycle_33da:
    in al, dx                                                        # CS:33DA
    and al, 0x8                                                      # CS:33DB
    je L_wait_retrace_cycle_33da                                     # CS:33DD
L_wait_retrace_cycle_33df:
    in al, dx                                                        # CS:33DF
    and al, 0x8                                                      # CS:33E0
    jne L_wait_retrace_cycle_33df                                    # CS:33E2
    ret                                                              # CS:33E4

/* ========================================================================== 
 * blit_planar_fullscreen  |  CS:33E5
 * Назначение: Выводит 4 плоскости по 8000 байт из последовательного буфера.
 * Вход: BX=source segment, SI=source offset.
 * Эффект/ограничения: Цикл масок sequencer 8,4,2,1; используется для INTRO.X, не для packed sprites.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type blit_planar_fullscreen, @function
blit_planar_fullscreen:
    mov ax, ds:[D_video_draw_segment]                                # CS:33E5
    mov es, ax                                                       # CS:33E8
    mov di, 0x0                                                      # CS:33EA
    mov dx, 0x3ce                                                    # CS:33ED
    mov ax, 0x805                                                    # CS:33F0
    out dx, ax                                                       # CS:33F3
    mov ax, 0x7                                                      # CS:33F4
    out dx, ax                                                       # CS:33F7
    mov dl, 0xc4                                                     # CS:33F8
    mov ax, 0x802                                                    # CS:33FA
    push ds                                                          # CS:33FD
    mov ds, bx                                                       # CS:33FE
L_blit_planar_fullscreen_3400:
    out dx, ax                                                       # CS:3400
    push ax                                                          # CS:3401
    push di                                                          # CS:3402
    mov cx, 0x1f40                                                   # CS:3403
L_blit_planar_fullscreen_3406:
    lods al, BYTE PTR ds:[si]                                        # CS:3406
    and BYTE PTR es:[di], al                                         # CS:3407
    inc di                                                           # CS:340A
    loop L_blit_planar_fullscreen_3406                               # CS:340B
    pop di                                                           # CS:340D
    pop ax                                                           # CS:340E
    shr ah, 1                                                        # CS:340F
    jne L_blit_planar_fullscreen_3400                                # CS:3411
    pop ds                                                           # CS:3413
    mov ax, 0xf02                                                    # CS:3414
    out dx, ax                                                       # CS:3417
    mov dl, 0xce                                                     # CS:3418
    mov ax, 0x5                                                      # CS:341A
    out dx, ax                                                       # CS:341D
    mov ax, 0xf07                                                    # CS:341E
    out dx, ax                                                       # CS:3421
    ret                                                              # CS:3422

/* ========================================================================== 
 * init_playfield_pages  |  CS:3423
 * Назначение: Очищает обе видеостраницы и рисует рамку.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Между страницами выполняется present_frame.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type init_playfield_pages, @function
init_playfield_pages:
    mov ax, 0x0                                                      # CS:3423
    call clear_draw_page                                             # CS:3426
    call draw_playfield_border                                       # CS:3429
    call present_frame                                               # CS:342C
    mov ax, 0x0                                                      # CS:342F
    call clear_draw_page                                             # CS:3432
    call draw_playfield_border                                       # CS:3435
    ret                                                              # CS:3438

/* ========================================================================== 
 * draw_playfield_border  |  CS:3439
 * Назначение: Рисует рамку вокруг игрового поля прямыми записями в video RAM.
 * Вход: draw segment, ожидаемое состояние EGA и DF.
 * Эффект/ограничения: Ширина поля 256; аппаратное состояние важно, не обычный массив RGB.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_playfield_border, @function
draw_playfield_border:
    mov ax, ds:[D_video_draw_segment]                                # CS:3439
    mov es, ax                                                       # CS:343C
    mov di, 0x11c                                                    # CS:343E
    mov ax, 0xffff                                                   # CS:3441
    mov cx, 0x10                                                     # CS:3444
    rep stos WORD PTR es:[di], ax                                    # CS:3447
    mov di, 0x1a44                                                   # CS:3449
    mov cx, 0x10                                                     # CS:344C
    rep stos WORD PTR es:[di], ax                                    # CS:344F
    mov di, 0x11b                                                    # CS:3451
    mov al, 0x1                                                      # CS:3454
    mov cx, 0xa2                                                     # CS:3456
L_draw_playfield_border_3459:
    mov BYTE PTR es:[di], al                                         # CS:3459
    add di, 0x28                                                     # CS:345C
    loop L_draw_playfield_border_3459                                # CS:345F
    mov di, 0x13c                                                    # CS:3461
    mov al, 0x80                                                     # CS:3464
    mov cx, 0xa2                                                     # CS:3466
L_draw_playfield_border_3469:
    mov BYTE PTR es:[di], al                                         # CS:3469
    add di, 0x28                                                     # CS:346C
    loop L_draw_playfield_border_3469                                # CS:346F
    ret                                                              # CS:3471

/* ========================================================================== 
 * prepare_attract_background  |  CS:3472
 * Назначение: Выводит INTRO в обе страницы и сохраняет область игры в A400h.
 * Вход: segment_intro.
 * Эффект/ограничения: A400 — фоновый backup для заставки, не третий игровой world buffer.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type prepare_attract_background, @function
prepare_attract_background:
    mov si, 0x0                                                      # CS:3472
    mov bx, WORD PTR ds:[D_segment_intro]                            # CS:3475
    call blit_planar_fullscreen                                      # CS:3479
    call present_frame                                               # CS:347C
    mov si, 0x0                                                      # CS:347F
    mov bx, WORD PTR ds:[D_segment_intro]                            # CS:3482
    call blit_planar_fullscreen                                      # CS:3486
    mov ax, ds:[D_video_draw_segment]                                # CS:3489
    push ax                                                          # CS:348C
    mov ax, 0xa000                                                   # CS:348D
    ACC16 add, 0x400                                                 # CS:3490
    push ax                                                          # CS:3493
    call copy_playfield_between_pages                                # CS:3494
    ret                                                              # CS:3497

/* ========================================================================== 
 * restore_attract_background  |  CS:3498
 * Назначение: Восстанавливает playfield из A400h в текущую draw page.
 * Вход: Backup уже создан.
 * Эффект/ограничения: Сущности рисуются после восстановления фона.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type restore_attract_background, @function
restore_attract_background:
    mov ax, 0xa000                                                   # CS:3498
    ACC16 add, 0x400                                                 # CS:349B
    push ax                                                          # CS:349E
    mov ax, ds:[D_video_draw_segment]                                # CS:349F
    push ax                                                          # CS:34A2
    call copy_playfield_between_pages                                # CS:34A3
    ret                                                              # CS:34A6

/* CS:34A7: Inter-procedure alignment byte, not an instruction. */
padding_34a7:
    .byte 0x00
