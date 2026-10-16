/* RLE карты, рельеф, размещение и столкновения
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * prepare_level_maps  |  CS:069A
 * Назначение: Декодирует поток тайлов 12×64 и 64 маски размещения; рисует карту в кольцевой фон A800h.
 * Вход: level_index, level_part; RLE-таблицы в DS.
 * Эффект/ограничения: RLE: нижние 6 бит = tile, верхние 2 = длина минус 1. Ровно 768 выходных байтов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type prepare_level_maps, @function
prepare_level_maps:

    # level_index*3 + level_part выбирает и карту тайлов, и размещение объектов.
    mov al, ds:[D_level_index]                                       # CS:069A
    {load} mov bl, al                                                # CS:069D
    shl bl, 1                                                        # CS:069F
    {load} add bl, al                                                # CS:06A1
    add bl, BYTE PTR ds:[D_level_part]                               # CS:06A3
    mov bh, 0x0                                                      # CS:06A7
    shl bx, 1                                                        # CS:06A9
    mov si, WORD PTR [bx+D_level_object_streams]                     # CS:06AB
    mov di, 0x9229                                                   # CS:06AF
    mov cx, 0x40                                                     # CS:06B2
    mov ax, ds                                                       # CS:06B5
    mov es, ax                                                       # CS:06B7
    mov ax, 0x0                                                      # CS:06B9
    cld                                                              # CS:06BC
    rep stos WORD PTR es:[di], ax                                    # CS:06BD
    mov di, 0x9229                                                   # CS:06BF
L_map_next_placement:

    # Тройка kind/column/source_row. В stored row OR-ятся kind<<10 и бит колонки.
    lods al, BYTE PTR ds:[si]                                        # CS:06C2
    cmp al, 0xff                                                     # CS:06C3
    je L_map_decode_tile_stream                                      # CS:06C5
    shl al, 1                                                        # CS:06C7
    shl al, 1                                                        # CS:06C9
    {load} mov ah, al                                                # CS:06CB
    lods al, BYTE PTR ds:[si]                                        # CS:06CD
    shl al, 1                                                        # CS:06CE
    {load} mov bl, al                                                # CS:06D0
    mov bh, 0x0                                                      # CS:06D2
    mov al, 0x0                                                      # CS:06D4
    or ax, WORD PTR [bx+D_map_column_bits]                           # CS:06D6
    {load} mov cx, ax                                                # CS:06DA
    lods al, BYTE PTR ds:[si]                                        # CS:06DC
    neg al                                                           # CS:06DD
    add al, 0x3f                                                     # CS:06DF
    shl al, 1                                                        # CS:06E1
    {load} mov bl, al                                                # CS:06E3
    or WORD PTR [bx+D_map_spawn_rows], cx                            # CS:06E5
    jmp L_map_next_placement                                         # CS:06E9
L_map_decode_tile_stream:

    # RLE: младшие 6 бит — tile; старшие 2 — число повторений минус один.
    mov al, ds:[D_level_index]                                       # CS:06EB
    {load} mov bl, al                                                # CS:06EE
    shl bl, 1                                                        # CS:06F0
    {load} add bl, al                                                # CS:06F2
    add bl, BYTE PTR ds:[D_level_part]                               # CS:06F4
    mov bh, 0x0                                                      # CS:06F8
    shl bx, 1                                                        # CS:06FA
    mov si, WORD PTR [bx+D_level_tile_streams]                       # CS:06FC
    mov di, 0x8f29                                                   # CS:0700
    mov cx, 0x300                                                    # CS:0703
    mov bl, 0x0                                                      # CS:0706
L_map_next_decoded_tile:

    # BL — оставшиеся повторы. Декодируем ровно 12*64 клеток, без отдельного EOF.
    dec bl                                                           # CS:0708
    jns L_map_write_decoded_tile                                     # CS:070A
    lods al, BYTE PTR ds:[si]                                        # CS:070C
    {load} mov dl, al                                                # CS:070D
    and dl, 0x3f                                                     # CS:070F
    and al, 0xc0                                                     # CS:0712
    jne L_prepare_level_maps_071a                                    # CS:0714
    mov bl, 0x0                                                      # CS:0716
    jmp L_map_write_decoded_tile                                     # CS:0718
L_prepare_level_maps_071a:
    cmp al, 0x40                                                     # CS:071A
    jne L_prepare_level_maps_0722                                    # CS:071C
    mov bl, 0x1                                                      # CS:071E
    jmp L_map_write_decoded_tile                                     # CS:0720
L_prepare_level_maps_0722:
    cmp al, 0x80                                                     # CS:0722
    jne L_prepare_level_maps_072a                                    # CS:0724
    mov bl, 0x2                                                      # CS:0726
    jmp L_map_write_decoded_tile                                     # CS:0728
L_prepare_level_maps_072a:
    mov bl, 0x3                                                      # CS:072A
L_map_write_decoded_tile:
    mov BYTE PTR [di], dl                                            # CS:072C
    inc di                                                           # CS:072E
    loop L_map_next_decoded_tile                                     # CS:072F

    # Рисуем кольцевую карту в A800. Это video-memory cache, а не логическая карта столкновений.
    mov di, 0x0                                                      # CS:0731
    mov ax, 0xa800                                                   # CS:0734
    mov es, ax                                                       # CS:0737
    mov dx, 0x3ce                                                    # CS:0739
    mov ah, 0x0                                                      # CS:073C
    mov al, 0x0                                                      # CS:073E
    out dx, ax                                                       # CS:0740
    mov ax, 0xf01                                                    # CS:0741
    out dx, ax                                                       # CS:0744
    mov cx, 0x4000                                                   # CS:0745
    rep stos WORD PTR es:[di], ax                                    # CS:0748
    mov ax, 0x8f29                                                   # CS:074A
    mov ds:[D_map_tile_cursor], ax                                   # CS:074D
    mov WORD PTR ds:[D_map_row_y], 0x3f0                             # CS:0750
    mov cx, 0x40                                                     # CS:0756
L_map_prerender_row:

    # Каждая строка: 8 px левого края + 10*24 px середины + 8 px правого края = 256 px.
    push cx                                                          # CS:0759
    mov BYTE PTR ds:[D_map_running_height], 0x0                      # CS:075A
    call prepare_next_map_tile                                       # CS:075F
    mov WORD PTR ds:[D_map_tile_template_x], 0x0                     # CS:0762
    mov BYTE PTR ds:[D_map_tile_template_width], 0x8                 # CS:0768
    add WORD PTR ds:[D_map_tile_template_sprite], 0x8                # CS:076D
    mov si, OFFSET D_template_map_tile                               # CS:0772
    call render_entity_to_a800                                       # CS:0775
    mov WORD PTR ds:[D_map_tile_template_x], 0x8                     # CS:0778
    mov BYTE PTR ds:[D_map_tile_template_width], 0x18                # CS:077E
    mov cx, 0xa                                                      # CS:0783
L_map_prerender_middle_tile:
    push cx                                                          # CS:0786
    call prepare_next_map_tile                                       # CS:0787
    mov si, OFFSET D_template_map_tile                               # CS:078A
    call render_entity_to_a800                                       # CS:078D
    add WORD PTR ds:[D_map_tile_template_x], 0x18                    # CS:0790
    pop cx                                                           # CS:0795
    loop L_map_prerender_middle_tile                                 # CS:0796
    mov BYTE PTR ds:[D_map_tile_template_width], 0x8                 # CS:0798
    call prepare_next_map_tile                                       # CS:079D
    mov si, OFFSET D_template_map_tile                               # CS:07A0
    call render_entity_to_a800                                       # CS:07A3
    sub WORD PTR ds:[D_map_row_y], 0x10                              # CS:07A6
    pop cx                                                           # CS:07AB
    loop L_map_prerender_row                                         # CS:07AC
    ret                                                              # CS:07AE

/* ========================================================================== 
 * prepare_next_map_tile  |  CS:07AF
 * Назначение: Берёт следующий tile, обновляет высотный профиль строки и параметры шаблона отрисовки.
 * Вход: map_tile_cursor, map_row_y, map_running_height.
 * Эффект/ограничения: Изменяет шаблон DS:0274; bank=2 или 4, sprite offset берётся из таблицы.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type prepare_next_map_tile, @function
prepare_next_map_tile:

    # Шаблон map_tile обновляется прямо на месте; предыдущая высота строки сохраняется в E_Z.
    mov al, ds:[D_map_running_height]                                # CS:07AF
    mov ds:[D_map_tile_template_z], al                               # CS:07B2
    mov bx, WORD PTR ds:[D_map_tile_cursor]                          # CS:07B5
    mov al, BYTE PTR [bx]                                            # CS:07B9
    inc bx                                                           # CS:07BB
    mov WORD PTR ds:[D_map_tile_cursor], bx                          # CS:07BC
    mov bh, 0x0                                                      # CS:07C0
    {load} mov bl, al                                                # CS:07C2
    mov al, BYTE PTR [bx+D_tile_heights]                             # CS:07C4
    mov ds:[D_map_tile_template_height], al                          # CS:07C8
    mov al, BYTE PTR [bx+D_tile_height_delta]                        # CS:07CB
    add BYTE PTR ds:[D_map_running_height], al                       # CS:07CF
    mov al, BYTE PTR [bx+D_tile_y_adjust]                            # CS:07D3
    cbw                                                              # CS:07D7
    add ax, WORD PTR ds:[D_map_row_y]                                # CS:07D8
    mov ds:[D_map_tile_template_y], ax                               # CS:07DC
    cmp bx, 0x26                                                     # CS:07DF
    jb L_prepare_next_map_tile_07ec                                  # CS:07E2
    mov WORD PTR ds:[D_map_tile_template_bank], 0x2                  # CS:07E4
    jmp L_prepare_next_map_tile_07f2                                 # CS:07EA
L_prepare_next_map_tile_07ec:
    mov WORD PTR ds:[D_map_tile_template_bank], 0x4                  # CS:07EC
L_prepare_next_map_tile_07f2:
    shl bx, 1                                                        # CS:07F2
    mov bx, WORD PTR [bx+D_tile_sprite_offsets]                      # CS:07F4
    mov WORD PTR ds:[D_map_tile_template_sprite], bx                 # CS:07F8
    ret                                                              # CS:07FC

/* ========================================================================== 
 * sample_terrain  |  CS:07FD
 * Назначение: Ищет строку/колонку тайла, читает его микропрофиль 8×8 и суммирует перепады предыдущих тайлов.
 * Вход: Стек: [BP+4]=world_y, [BP+6]=x.
 * Эффект/ограничения: AX = знаковая высота; BX = variant из одной из двух таблиц; CX = world_y/16. RET 4.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sample_terrain, @function
sample_terrain:

    # Запрос профиля рельефа: одна клетка 24x16 содержит профиль 8x8 с шагом 3x2.
    push bp                                                          # CS:07FD
    {load} mov bp, sp                                                # CS:07FE
    sub sp, 0x8                                                      # CS:0800
    mov ax, WORD PTR [bp+TERRAIN_X]                                  # CS:0803
    ACC16 add, 0x10                                                  # CS:0806
    mov cl, 0x18                                                     # CS:0809
    div cl                                                           # CS:080B
    push ax                                                          # CS:080D
    mov ah, 0x0                                                      # CS:080E
    mov WORD PTR [bp+TERRAIN_TILE_COL], ax                           # CS:0810
    pop ax                                                           # CS:0813
    {load} mov al, ah                                                # CS:0814
    mov ah, 0x0                                                      # CS:0816
    mov cl, 0x3                                                      # CS:0818
    div cl                                                           # CS:081A
    mov ah, 0x0                                                      # CS:081C
    mov WORD PTR [bp+TERRAIN_SUB_COL], ax                            # CS:081E
    mov ax, WORD PTR [bp+TERRAIN_Y]                                  # CS:0821
    ACC16 and, WORLD_Y_MASK                                          # CS:0824
    shr ax, 1                                                        # CS:0827
    shr ax, 1                                                        # CS:0829
    shr ax, 1                                                        # CS:082B
    shr ax, 1                                                        # CS:082D
    neg ax                                                           # CS:082F
    ACC16 add, 0x3f                                                  # CS:0831
    mov WORD PTR [bp+TERRAIN_STORED_ROW], ax                         # CS:0834
    mov ax, WORD PTR [bp+TERRAIN_Y]                                  # CS:0837
    ACC16 and, 0xf                                                   # CS:083A
    shr ax, 1                                                        # CS:083D
    mov WORD PTR [bp+TERRAIN_SUB_ROW], ax                            # CS:083F
    mov ax, WORD PTR [bp+TERRAIN_STORED_ROW]                         # CS:0842
    shl ax, 1                                                        # CS:0845
    shl ax, 1                                                        # CS:0847
    {load} mov bx, ax                                                # CS:0849
    shl ax, 1                                                        # CS:084B
    {load} add bx, ax                                                # CS:084D
    add bx, 0x8f29                                                   # CS:084F
    {load} mov si, bx                                                # CS:0853
    add bx, WORD PTR [bp+TERRAIN_TILE_COL]                           # CS:0855
    mov al, BYTE PTR [bx]                                            # CS:0858
    mov ah, 0x0                                                      # CS:085A
    push ax                                                          # CS:085C
    shl ax, 1                                                        # CS:085D
    {load} mov bx, ax                                                # CS:085F
    mov bx, WORD PTR [bx+D_tile_profile_offsets]                     # CS:0861
    mov ax, WORD PTR [bp+TERRAIN_SUB_ROW]                            # CS:0865
    shl ax, 1                                                        # CS:0868
    shl ax, 1                                                        # CS:086A
    shl ax, 1                                                        # CS:086C
    {load} add bx, ax                                                # CS:086E
    add bx, WORD PTR [bp+TERRAIN_SUB_COL]                            # CS:0870
    mov cl, BYTE PTR [bx]                                            # CS:0873
    push cx                                                          # CS:0875
    and cl, 0x1f                                                     # CS:0876
    test cl, 0x10                                                    # CS:0879
    je L_sample_terrain_0881                                         # CS:087C
    or cl, 0xe0                                                      # CS:087E
L_sample_terrain_0881:
    mov dx, WORD PTR [bp+TERRAIN_TILE_COL]                           # CS:0881
    cld                                                              # CS:0884
L_sample_terrain_0885:
    cmp dx, 0x0                                                      # CS:0885
    je L_sample_terrain_0896                                         # CS:0888
    lods al, BYTE PTR ds:[si]                                        # CS:088A
    mov bh, 0x0                                                      # CS:088B
    {load} mov bl, al                                                # CS:088D
    add cl, BYTE PTR [bx+D_tile_height_delta]                        # CS:088F
    dec dx                                                           # CS:0893
    jmp L_sample_terrain_0885                                        # CS:0894
L_sample_terrain_0896:
    {load} mov al, cl                                                # CS:0896
    cbw                                                              # CS:0898
    pop cx                                                           # CS:0899
    pop bx                                                           # CS:089A
    test cl, 0x80                                                    # CS:089B
    jne L_sample_terrain_08a6                                        # CS:089E
    mov bl, BYTE PTR [bx+D_tile_surface_variant_a]                   # CS:08A0
    jmp L_sample_terrain_08aa                                        # CS:08A4
L_sample_terrain_08a6:
    mov bl, BYTE PTR [bx+D_tile_surface_variant_b]                   # CS:08A6
L_sample_terrain_08aa:
    mov bh, 0x0                                                      # CS:08AA
    mov cx, WORD PTR [bp+TERRAIN_STORED_ROW]                         # CS:08AC
    neg cx                                                           # CS:08AF
    add cx, 0x3f                                                     # CS:08B1
    {load} mov sp, bp                                                # CS:08B4
    pop bp                                                           # CS:08B6
    ret 0x4                                                          # CS:08B7

/* ========================================================================== 
 * spawn_map_row  |  CS:08BA
 * Назначение: Создаёт сущности для установленных битов одной строки; учитывает маски уже изменённых объектов.
 * Вход: Стек: [BP+4]=строка 0..63.
 * Эффект/ограничения: Поиск первых свободных слотов; для уже использованных объектов применяется иной шаблон состояния. RET 2.
 * Возвраты в физическом диапазоне: ret 0x2.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type spawn_map_row, @function
spawn_map_row:
    push bp                                                          # CS:08BA
    {load} mov bp, sp                                                # CS:08BB
    mov bx, WORD PTR [bp+0x4]                                  # CS:08BD
    shl bx, 1                                                        # CS:08C0
    mov ax, WORD PTR [bx+D_map_spawn_rows]                           # CS:08C2
    ACC16 cmp, 0x0                                                   # CS:08C6
    je L_spawn_map_row_0946                                          # CS:08C9
    push ax                                                          # CS:08CB
    shr ah, 1                                                        # CS:08CC
    shr ah, 1                                                        # CS:08CE
    {load} mov bl, ah                                                # CS:08D0
    mov bh, 0x0                                                      # CS:08D2
    shl bx, 1                                                        # CS:08D4
    mov si, WORD PTR [bx+D_map_entity_templates]                     # CS:08D6
    cmp ah, 0xf                                                      # CS:08DA
    jb L_spawn_map_row_0902                                          # CS:08DD
    cmp ah, 0x14                                                     # CS:08DF
    ja L_spawn_map_row_0902                                          # CS:08E2
    sub ah, 0xf                                                      # CS:08E4
    mov BYTE PTR [si+0x1], ah                                        # CS:08E7
    {load} mov bl, ah                                                # CS:08EA
    mov bh, 0x0                                                      # CS:08EC
    mov al, BYTE PTR [bx+D_collectible_0]                            # CS:08EE
    cmp al, 0x0                                                      # CS:08F2
    je L_spawn_map_row_08f9                                          # CS:08F4
    pop ax                                                           # CS:08F6
    jmp L_spawn_map_row_0946                                         # CS:08F7
L_spawn_map_row_08f9:
    shl bx, 1                                                        # CS:08F9
    mov ax, WORD PTR [bx+D_collectible_sprite_offsets]               # CS:08FB
    mov WORD PTR [si+0xf], ax                                        # CS:08FF
L_spawn_map_row_0902:
    mov BYTE PTR [si+0x13], 0xa                                      # CS:0902
    mov ax, WORD PTR [bp+0x4]                                        # CS:0906
    mov BYTE PTR [si+0x14], al                                       # CS:0909
    pop ax                                                           # CS:090C
    mov cx, 0xa                                                      # CS:090D
L_spawn_map_row_0910:
    shr ax, 1                                                        # CS:0910
    jae L_spawn_map_row_0941                                         # CS:0912
    push ax                                                          # CS:0914
    push cx                                                          # CS:0915
    call place_entity_on_map                                         # CS:0916
    mov al, BYTE PTR [si+0x13]                                       # CS:0919
    mov ah, 0x0                                                      # CS:091C
    push ax                                                          # CS:091E
    mov ax, WORD PTR [bp+0x4]                                        # CS:091F
    push ax                                                          # CS:0922
    call test_map_object_available                                   # CS:0923
    ACC16 cmp, 0x0                                                   # CS:0926
    je L_spawn_map_row_0932                                          # CS:0929
    push si                                                          # CS:092B
    call allocate_entity_copy                                        # CS:092C
    pop si                                                           # CS:092F
    jmp L_spawn_map_row_093f                                         # CS:0930
L_spawn_map_row_0932:
    push si                                                          # CS:0932
    call allocate_entity_copy                                        # CS:0933
    cmp si, 0x0                                                      # CS:0936
    je L_spawn_map_row_093e                                          # CS:0939
    call apply_entity_replacement                                    # CS:093B
L_spawn_map_row_093e:
    pop si                                                           # CS:093E
L_spawn_map_row_093f:
    pop cx                                                           # CS:093F
    pop ax                                                           # CS:0940
L_spawn_map_row_0941:
    dec BYTE PTR [si+0x13]                                           # CS:0941
    loop L_spawn_map_row_0910                                        # CS:0944
L_spawn_map_row_0946:
    pop bp                                                           # CS:0946
    ret 0x2                                                          # CS:0947

/* ========================================================================== 
 * place_entity_on_map  |  CS:094A
 * Назначение: Вычисляет x/y/высоту шаблона по его колонке и строке в сетке.
 * Вход: DS:SI = запись сущности; state_13=колонка, state_14=строка.
 * Эффект/ограничения: x=24*column-4; y=16*row; высота — сумма перепадов предшествующих тайлов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type place_entity_on_map, @function
place_entity_on_map:
    mov dl, BYTE PTR [si+0x13]                                       # CS:094A
    push si                                                          # CS:094D
    mov al, 0x3f                                                     # CS:094E
    sub al, BYTE PTR [si+0x14]                                       # CS:0950
    shl al, 1                                                        # CS:0953
    shl al, 1                                                        # CS:0955
    mov ah, 0x0                                                      # CS:0957
    {load} mov si, ax                                                # CS:0959
    shl ax, 1                                                        # CS:095B
    {load} add si, ax                                                # CS:095D
    add si, 0x8f29                                                   # CS:095F
    mov cl, 0x0                                                      # CS:0963
    cld                                                              # CS:0965
L_place_entity_on_map_0966:
    cmp dl, 0x0                                                      # CS:0966
    je L_place_entity_on_map_0978                                    # CS:0969
    lods al, BYTE PTR ds:[si]                                        # CS:096B
    mov bh, 0x0                                                      # CS:096C
    {load} mov bl, al                                                # CS:096E
    add cl, BYTE PTR [bx+D_tile_height_delta]                        # CS:0970
    dec dl                                                           # CS:0974
    jmp L_place_entity_on_map_0966                                   # CS:0976
L_place_entity_on_map_0978:
    pop si                                                           # CS:0978
    mov BYTE PTR [si+0x3], cl                                        # CS:0979
    mov al, BYTE PTR [si+0x14]                                       # CS:097C
    mov ah, 0x0                                                      # CS:097F
    shl ax, 1                                                        # CS:0981
    shl ax, 1                                                        # CS:0983
    shl ax, 1                                                        # CS:0985
    shl ax, 1                                                        # CS:0987
    mov WORD PTR [si+0x7], ax                                        # CS:0989
    mov al, BYTE PTR [si+0x13]                                       # CS:098C
    mov ah, 0x0                                                      # CS:098F
    shl ax, 1                                                        # CS:0991
    {load} mov cx, ax                                                # CS:0993
    shl ax, 1                                                        # CS:0995
    {load} add ax, cx                                                # CS:0997
    shl ax, 1                                                        # CS:0999
    shl ax, 1                                                        # CS:099B
    ACC16 sub, 0x4                                                   # CS:099D
    mov WORD PTR [si+0x5], ax                                        # CS:09A0
    ret                                                              # CS:09A3

/* ========================================================================== 
 * reset_map_object_masks  |  CS:09A4
 * Назначение: Заполняет FFh маски ещё доступных объектов для трёх частей уровня.
 * Вход: Нет аргументов.
 * Эффект/ограничения: 192 слова = 3×64 строк; ES=DS, DF=0.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_map_object_masks, @function
reset_map_object_masks:
    mov di, 0x92a9                                                   # CS:09A4
    mov cx, 0xc0                                                     # CS:09A7
    mov ax, ds                                                       # CS:09AA
    mov es, ax                                                       # CS:09AC
    mov ax, 0xffff                                                   # CS:09AE
    cld                                                              # CS:09B1
    rep stos WORD PTR es:[di], ax                                    # CS:09B2
    ret                                                              # CS:09B4

/* ========================================================================== 
 * consume_map_object  |  CS:09B5
 * Назначение: Меняет сущность по replacement-таблице и сбрасывает её бит в маске строки.
 * Вход: [BP+4]=row; [BP+6]=column; [BP+8]=entity offset.
 * Эффект/ограничения: Выбирает банк масок по level_part; RET 6.
 * Возвраты в физическом диапазоне: ret 0x6.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type consume_map_object, @function
consume_map_object:
    push bp                                                          # CS:09B5
    {load} mov bp, sp                                                # CS:09B6
    mov si, WORD PTR [bp+0x8]                                        # CS:09B8
    call apply_entity_replacement                                    # CS:09BB
    mov bx, WORD PTR [bp+0x6]                                        # CS:09BE
    shl bx, 1                                                        # CS:09C1
    mov cx, WORD PTR [bx+D_map_column_clear_masks]                   # CS:09C3
    mov bx, WORD PTR [bp+0x4]                                        # CS:09C7
    shl bx, 1                                                        # CS:09CA
    mov al, ds:[D_level_part]                                        # CS:09CC
    cmp al, 0x1                                                      # CS:09CF
    jne L_consume_map_object_09d9                                    # CS:09D1
    add bx, 0x80                                                     # CS:09D3
    jmp L_consume_map_object_09e1                                    # CS:09D7
L_consume_map_object_09d9:
    cmp al, 0x2                                                      # CS:09D9
    jne L_consume_map_object_09e1                                    # CS:09DB
    add bx, 0x100                                                    # CS:09DD
L_consume_map_object_09e1:
    and WORD PTR [bx+D_map_available_masks], cx                      # CS:09E1
    pop bp                                                           # CS:09E5
    ret 0x6                                                          # CS:09E6

/* ========================================================================== 
 * test_map_object_available  |  CS:09E9
 * Назначение: Проверяет бит объекта в выбранной части уровня.
 * Вход: [BP+4]=row; [BP+6]=column.
 * Эффект/ограничения: AX = исходный бит, не нормализованный bool; RET 4.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type test_map_object_available, @function
test_map_object_available:
    push bp                                                          # CS:09E9
    {load} mov bp, sp                                                # CS:09EA
    mov bx, WORD PTR [bp+0x6]                                        # CS:09EC
    shl bx, 1                                                        # CS:09EF
    mov cx, WORD PTR [bx+D_map_column_clear_masks]                   # CS:09F1
    mov bx, WORD PTR [bp+0x4]                                        # CS:09F5
    shl bx, 1                                                        # CS:09F8
    mov al, ds:[D_level_part]                                        # CS:09FA
    cmp al, 0x1                                                      # CS:09FD
    jne L_test_map_object_available_0a07                             # CS:09FF
    add bx, 0x80                                                     # CS:0A01
    jmp L_test_map_object_available_0a0f                             # CS:0A05
L_test_map_object_available_0a07:
    cmp al, 0x2                                                      # CS:0A07
    jne L_test_map_object_available_0a0f                             # CS:0A09
    add bx, 0x100                                                    # CS:0A0B
L_test_map_object_available_0a0f:
    not cx                                                           # CS:0A0F
    mov ax, WORD PTR [bx+D_map_available_masks]                      # CS:0A11
    {load} and ax, cx                                                # CS:0A15
    pop bp                                                           # CS:0A17
    ret 0x4                                                          # CS:0A18

/* ========================================================================== 
 * apply_entity_replacement  |  CS:0A1B
 * Назначение: Копирует подмножество полей из шаблона, выбранного по handler_id.
 * Вход: DS:SI = сущность.
 * Эффект/ограничения: Меняет flags, размеры/якорь, sprite offset и handler_id; координаты сохраняет.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type apply_entity_replacement, @function
apply_entity_replacement:
    mov al, BYTE PTR [si+0x12]                                       # CS:0A1B
    shl al, 1                                                        # CS:0A1E
    {load} mov bl, al                                                # CS:0A20
    mov bh, 0x0                                                      # CS:0A22
    mov di, WORD PTR [bx+D_entity_replacement_templates]             # CS:0A24
    mov al, BYTE PTR [di]                                            # CS:0A28
    mov BYTE PTR [si], al                                            # CS:0A2A
    mov al, BYTE PTR [di+0x9]                                        # CS:0A2C
    mov BYTE PTR [si+0x9], al                                        # CS:0A2F
    mov al, BYTE PTR [di+0xa]                                        # CS:0A32
    mov BYTE PTR [si+0xa], al                                        # CS:0A35
    mov al, BYTE PTR [di+0xb]                                        # CS:0A38
    mov BYTE PTR [si+0xb], al                                        # CS:0A3B
    mov al, BYTE PTR [di+0xc]                                        # CS:0A3E
    mov BYTE PTR [si+0xc], al                                        # CS:0A41
    mov ax, WORD PTR [di+0xf]                                        # CS:0A44
    mov WORD PTR [si+0xf], ax                                        # CS:0A47
    mov al, BYTE PTR [di+0x12]                                       # CS:0A4A
    mov BYTE PTR [si+0x12], al                                       # CS:0A4D
    ret                                                              # CS:0A50

/* ========================================================================== 
 * query_player_map_collision  |  CS:0A51
 * Назначение: Ищет препятствие текущей строки около x игрока по маскам и ширинам из таблиц.
 * Вход: player_map_row, player_x, level_part.
 * Эффект/ограничения: AX=код/величина столкновения либо 0; BX=колонка при попадании.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type query_player_map_collision, @function
query_player_map_collision:
    mov bl, BYTE PTR ds:[D_player_map_row]                           # CS:0A51
    mov bh, 0x0                                                      # CS:0A55
    shl bx, 1                                                        # CS:0A57
    {load} mov si, bx                                                # CS:0A59
    mov al, ds:[D_level_part]                                        # CS:0A5B
    cmp al, 0x1                                                      # CS:0A5E
    jne L_query_player_map_collision_0a68                            # CS:0A60
    add si, 0x80                                                     # CS:0A62
    jmp L_query_player_map_collision_0a70                            # CS:0A66
L_query_player_map_collision_0a68:
    cmp al, 0x2                                                      # CS:0A68
    jne L_query_player_map_collision_0a70                            # CS:0A6A
    add si, 0x100                                                    # CS:0A6C
L_query_player_map_collision_0a70:
    mov ax, WORD PTR [si+D_map_available_masks]                      # CS:0A70
    and ax, WORD PTR [bx+D_map_spawn_rows]                           # CS:0A74
    ACC16 cmp, 0x0                                                   # CS:0A78
    je L_query_player_map_collision_0ad1                             # CS:0A7B
    {load} mov bl, ah                                                # CS:0A7D
    shr bl, 1                                                        # CS:0A7F
    shr bl, 1                                                        # CS:0A81
    mov bh, 0x0                                                      # CS:0A83
    mov cl, BYTE PTR [bx+D_map_collision_damage]                     # CS:0A85
    cmp cl, 0x0                                                      # CS:0A89
    je L_query_player_map_collision_0ad1                             # CS:0A8C
    mov BYTE PTR ds:[D_collision_damage_tmp], cl                     # CS:0A8E
    mov WORD PTR ds:[D_collision_column_tmp], 0xa                    # CS:0A92
    shl bx, 1                                                        # CS:0A98
    mov dx, WORD PTR [bx+D_map_collision_halfwidth]                  # CS:0A9A
    mov bx, WORD PTR [bx+D_map_collision_rightmost_x]                # CS:0A9E
    mov cx, 0xa                                                      # CS:0AA2
L_query_player_map_collision_0aa5:
    shr ax, 1                                                        # CS:0AA5
    jae L_query_player_map_collision_0ac8                            # CS:0AA7
    push bx                                                          # CS:0AA9
    {load} sub bx, dx                                                # CS:0AAA
    cmp bx, WORD PTR ds:[D_player_x]                                 # CS:0AAC
    jg L_query_player_map_collision_0ac7                             # CS:0AB0
    {load} add bx, dx                                                # CS:0AB2
    {load} add bx, dx                                                # CS:0AB4
    cmp bx, WORD PTR ds:[D_player_x]                                 # CS:0AB6
    jl L_query_player_map_collision_0ac7                             # CS:0ABA
    pop ax                                                           # CS:0ABC
    mov al, ds:[D_collision_damage_tmp]                              # CS:0ABD
    mov ah, 0x0                                                      # CS:0AC0
    mov bx, WORD PTR ds:[D_collision_column_tmp]                     # CS:0AC2
    ret                                                              # CS:0AC6
L_query_player_map_collision_0ac7:
    pop bx                                                           # CS:0AC7
L_query_player_map_collision_0ac8:
    sub bx, 0x18                                                     # CS:0AC8
    dec WORD PTR ds:[D_collision_column_tmp]                         # CS:0ACB
    loop L_query_player_map_collision_0aa5                           # CS:0ACF
L_query_player_map_collision_0ad1:
    mov ax, 0x0                                                      # CS:0AD1
    ret                                                              # CS:0AD4

/* ========================================================================== 
 * query_map_collision_at  |  CS:0AD5
 * Назначение: Проверяет препятствие строки для переданного x, с уменьшением полуширины на 14.
 * Вход: [BP+4]=row; [BP+6]=x.
 * Эффект/ограничения: AX=0/1, BX=колонка при попадании; RET 4.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type query_map_collision_at, @function
query_map_collision_at:
    push bp                                                          # CS:0AD5
    {load} mov bp, sp                                                # CS:0AD6
    mov bx, WORD PTR [bp+0x4]                                        # CS:0AD8
    shl bx, 1                                                        # CS:0ADB
    {load} mov si, bx                                                # CS:0ADD
    mov al, ds:[D_level_part]                                        # CS:0ADF
    cmp al, 0x1                                                      # CS:0AE2
    jne L_query_map_collision_at_0aec                                # CS:0AE4
    add si, 0x80                                                     # CS:0AE6
    jmp L_query_map_collision_at_0af4                                # CS:0AEA
L_query_map_collision_at_0aec:
    cmp al, 0x2                                                      # CS:0AEC
    jne L_query_map_collision_at_0af4                                # CS:0AEE
    add si, 0x100                                                    # CS:0AF0
L_query_map_collision_at_0af4:
    mov ax, WORD PTR [si+D_map_available_masks]                      # CS:0AF4
    and ax, WORD PTR [bx+D_map_spawn_rows]                           # CS:0AF8
    ACC16 cmp, 0x0                                                   # CS:0AFC
    je L_query_map_collision_at_0b51                                 # CS:0AFF
    {load} mov bl, ah                                                # CS:0B01
    shr bl, 1                                                        # CS:0B03
    shr bl, 1                                                        # CS:0B05
    mov bh, 0x0                                                      # CS:0B07
    mov cl, BYTE PTR [bx+D_map_collision_damage]                     # CS:0B09
    cmp cl, 0x0                                                      # CS:0B0D
    je L_query_map_collision_at_0b51                                 # CS:0B10
    mov WORD PTR ds:[D_collision_column_tmp], 0xa                    # CS:0B12
    shl bx, 1                                                        # CS:0B18
    mov dx, WORD PTR [bx+D_map_collision_halfwidth]                  # CS:0B1A
    sub dx, 0xe                                                      # CS:0B1E
    mov bx, WORD PTR [bx+D_map_collision_rightmost_x]                # CS:0B21
    mov cx, 0xa                                                      # CS:0B25
L_query_map_collision_at_0b28:
    shr ax, 1                                                        # CS:0B28
    jae L_query_map_collision_at_0b48                                # CS:0B2A
    push bx                                                          # CS:0B2C
    {load} sub bx, dx                                                # CS:0B2D
    cmp bx, WORD PTR [bp+0x6]                                        # CS:0B2F
    jg L_query_map_collision_at_0b47                                 # CS:0B32
    {load} add bx, dx                                                # CS:0B34
    {load} add bx, dx                                                # CS:0B36
    cmp bx, WORD PTR [bp+0x6]                                        # CS:0B38
    jl L_query_map_collision_at_0b47                                 # CS:0B3B
    pop ax                                                           # CS:0B3D
    mov ax, 0x1                                                      # CS:0B3E
    mov bx, WORD PTR ds:[D_collision_column_tmp]                     # CS:0B41
    jmp L_query_map_collision_at_0b54                                # CS:0B45
L_query_map_collision_at_0b47:
    pop bx                                                           # CS:0B47
L_query_map_collision_at_0b48:
    sub bx, 0x18                                                     # CS:0B48
    dec WORD PTR ds:[D_collision_column_tmp]                         # CS:0B4B
    loop L_query_map_collision_at_0b28                               # CS:0B4F
L_query_map_collision_at_0b51:
    mov ax, 0x0                                                      # CS:0B51
L_query_map_collision_at_0b54:
    pop bp                                                           # CS:0B54
    ret 0x4                                                          # CS:0B55
