/* Рекорды, глифы и ввод имени
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * load_highscores  |  CS:2762
 * Назначение: Готовит начальные имена/цифры/порядок и читает 140 байт HISCORES.
 * Вход: Нет аргументов.
 * Эффект/ограничения: 7 имён×12 + 7 результатов×6 + 7 scratch + 7 order =140.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type load_highscores, @function
load_highscores:

    # 140 байт HISCORES содержат не только имена/счёт, но и два массива индексов по7 байт.
    mov cx, 0x12                                                     # CS:2762
    mov ax, ds                                                       # CS:2765
    mov es, ax                                                       # CS:2767
    mov di, 0x94af                                                   # CS:2769
    cld                                                              # CS:276C
    mov ax, 0x0                                                      # CS:276D
    rep stos WORD PTR es:[di], ax                                    # CS:2770
    mov cx, 0x2a                                                     # CS:2772
    mov di, 0x9455                                                   # CS:2775
    mov al, 0x2e                                                     # CS:2778
    mov ah, 0x2e                                                     # CS:277A
    rep stos WORD PTR es:[di], ax                                    # CS:277C
    mov BYTE PTR ds:[D_highscore_data], 0x5f                         # CS:277E
    mov al, 0x0                                                      # CS:2783
    mov cx, 0x7                                                      # CS:2785
    mov si, 0x94da                                                   # CS:2788
L_load_highscores_278b:
    mov BYTE PTR [si], al                                            # CS:278B
    inc al                                                           # CS:278D
    inc si                                                           # CS:278F
    loop L_load_highscores_278b                                      # CS:2790
    mov di, 0x9455                                                   # CS:2792
    mov dx, OFFSET D_filename_hiscores                               # CS:2795
    mov cx, 0x8c                                                     # CS:2798
    call load_file_to_ds                                             # CS:279B
    ret                                                              # CS:279E

/* ========================================================================== 
 * insert_current_highscore  |  CS:279F
 * Назначение: Сортирует 7 результатов, проверяет попадание текущего в шесть лучших, заменяет последний слот и готовит ввод имени.
 * Вход: score slot0 = текущий; order[7].
 * Эффект/ограничения: При отсутствии рекорда очищает pending_name_entry.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type insert_current_highscore, @function
insert_current_highscore:
    call sort_highscore_order                                        # CS:279F
    mov al, ds:[D_highscore_order6]                                  # CS:27A2
    cmp al, 0x0                                                      # CS:27A5
    jne L_insert_current_highscore_27af                              # CS:27A7
    mov BYTE PTR ds:[D_pending_name_entry], 0x0                      # CS:27A9
    ret                                                              # CS:27AE
L_insert_current_highscore_27af:
    mov BYTE PTR ds:[D_pending_name_entry], 0x1                      # CS:27AF
    {load} mov bl, al                                                # CS:27B4
    shl bl, 1                                                        # CS:27B6
    {load} add bl, al                                                # CS:27B8
    mov bh, 0x0                                                      # CS:27BA
    shl bx, 1                                                        # CS:27BC
    push bx                                                          # CS:27BE
    add bx, 0x94a9                                                   # CS:27BF
    mov si, 0x94a9                                                   # CS:27C3
    mov cx, 0x6                                                      # CS:27C6
L_insert_current_highscore_27c9:
    lods al, BYTE PTR ds:[si]                                        # CS:27C9
    mov BYTE PTR [bx], al                                            # CS:27CA
    inc bx                                                           # CS:27CC
    loop L_insert_current_highscore_27c9                             # CS:27CD
    pop bx                                                           # CS:27CF
    shl bx, 1                                                        # CS:27D0
    add bx, 0x9455                                                   # CS:27D2
    mov WORD PTR ds:[D_pending_name_pointer], bx                     # CS:27D6
    mov si, 0x9455                                                   # CS:27DA
    mov cx, 0xc                                                      # CS:27DD
L_insert_current_highscore_27e0:
    lods al, BYTE PTR ds:[si]                                        # CS:27E0
    mov BYTE PTR [bx], al                                            # CS:27E1
    inc bx                                                           # CS:27E3
    loop L_insert_current_highscore_27e0                             # CS:27E4
    mov si, 0x94da                                                   # CS:27E6
L_insert_current_highscore_27e9:
    mov al, BYTE PTR [si]                                            # CS:27E9
    cmp al, 0x0                                                      # CS:27EB
    je L_insert_current_highscore_27f2                               # CS:27ED
    inc si                                                           # CS:27EF
    jmp L_insert_current_highscore_27e9                              # CS:27F0
L_insert_current_highscore_27f2:
    mov al, ds:[D_highscore_order6]                                  # CS:27F2
    mov BYTE PTR [si], al                                            # CS:27F5
    mov BYTE PTR ds:[D_highscore_order6], 0x0                        # CS:27F7
    sub si, 0x94da                                                   # CS:27FC
    {load} mov ax, si                                                # CS:2800
    mov ds:[D_pending_highscore_rank], al                            # CS:2802
    call render_highscore_table                                      # CS:2805
    ret                                                              # CS:2808

/* ========================================================================== 
 * sort_highscore_order  |  CS:2809
 * Назначение: Выбором максимума сортирует 7 индексов по результату; использует FFh как вычеркнутый элемент.
 * Вход: order[7], score_digits[7][6].
 * Эффект/ограничения: Имена и результаты физически не переставляет.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sort_highscore_order, @function
sort_highscore_order:
    mov al, ds:[D_highscore_order0]                                  # CS:2809
    mov ds:[D_highscore_sort_scratch0], al                           # CS:280C
    mov al, ds:[D_highscore_order1]                                  # CS:280F
    mov ds:[D_highscore_sort_scratch1], al                           # CS:2812
    mov al, ds:[D_highscore_order2]                                  # CS:2815
    mov ds:[D_highscore_sort_scratch2], al                           # CS:2818
    mov al, ds:[D_highscore_order3]                                  # CS:281B
    mov ds:[D_highscore_sort_scratch3], al                           # CS:281E
    mov al, ds:[D_highscore_order4]                                  # CS:2821
    mov ds:[D_highscore_sort_scratch4], al                           # CS:2824
    mov al, ds:[D_highscore_order5]                                  # CS:2827
    mov ds:[D_highscore_sort_scratch5], al                           # CS:282A
    mov al, ds:[D_highscore_order6]                                  # CS:282D
    mov ds:[D_highscore_sort_scratch6], al                           # CS:2830
    mov si, 0x94d3                                                   # CS:2833
    mov di, 0x94da                                                   # CS:2836
    mov cx, 0x7                                                      # CS:2839
L_sort_highscore_order_283c:
    push cx                                                          # CS:283C
    push si                                                          # CS:283D
L_sort_highscore_order_283e:
    mov al, BYTE PTR [si]                                            # CS:283E
    cmp al, 0xff                                                     # CS:2840
    jne L_sort_highscore_order_2847                                  # CS:2842
    inc si                                                           # CS:2844
    jmp L_sort_highscore_order_283e                                  # CS:2845
L_sort_highscore_order_2847:
    cmp si, 0x94d9                                                   # CS:2847
    je L_sort_highscore_order_2865                                   # CS:284B
    {load} mov bx, si                                                # CS:284D
L_sort_highscore_order_284f:
    inc bx                                                           # CS:284F
    mov al, BYTE PTR [bx]                                            # CS:2850
    cmp al, 0xff                                                     # CS:2852
    je L_sort_highscore_order_285f                                   # CS:2854
    call compare_highscore_candidates                                # CS:2856
    cmp al, 0x0                                                      # CS:2859
    je L_sort_highscore_order_285f                                   # CS:285B
    {load} mov si, bx                                                # CS:285D
L_sort_highscore_order_285f:
    cmp bx, 0x94d9                                                   # CS:285F
    jne L_sort_highscore_order_284f                                  # CS:2863
L_sort_highscore_order_2865:
    mov al, BYTE PTR [si]                                            # CS:2865
    mov BYTE PTR [si], 0xff                                          # CS:2867
    mov BYTE PTR [di], al                                            # CS:286A
    inc di                                                           # CS:286C
    pop si                                                           # CS:286D
    pop cx                                                           # CS:286E
    loop L_sort_highscore_order_283c                                 # CS:286F
    ret                                                              # CS:2871

/* ========================================================================== 
 * compare_highscore_candidates  |  CS:2872
 * Назначение: Лексикографически сравнивает по 6 десятичных цифр два результата.
 * Вход: DS:SI и DS:BX указывают на байты индексов.
 * Эффект/ограничения: AL=1, если кандидат BX строго больше SI; при равенстве 0. SI/DI сохранены, AX изменён.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type compare_highscore_candidates, @function
compare_highscore_candidates:
    push si                                                          # CS:2872
    push di                                                          # CS:2873
    mov al, BYTE PTR [si]                                            # CS:2874
    {load} mov ah, al                                                # CS:2876
    shl al, 1                                                        # CS:2878
    {load} add al, ah                                                # CS:287A
    shl al, 1                                                        # CS:287C
    mov ah, 0x0                                                      # CS:287E
    ACC16 add, 0x94a9                                                # CS:2880
    {load} mov si, ax                                                # CS:2883
    mov al, BYTE PTR [bx]                                            # CS:2885
    {load} mov ah, al                                                # CS:2887
    shl al, 1                                                        # CS:2889
    {load} add al, ah                                                # CS:288B
    shl al, 1                                                        # CS:288D
    mov ah, 0x0                                                      # CS:288F
    ACC16 add, 0x94a9                                                # CS:2891
    {load} mov di, ax                                                # CS:2894
    mov al, BYTE PTR [di]                                            # CS:2896
    cmp al, BYTE PTR [si]                                            # CS:2898
    ja L_compare_highscore_candidates_28d5                           # CS:289A
    jb L_compare_highscore_candidates_28d0                           # CS:289C
    mov al, BYTE PTR [di+0x1]                                        # CS:289E
    cmp al, BYTE PTR [si+0x1]                                        # CS:28A1
    ja L_compare_highscore_candidates_28d5                           # CS:28A4
    jb L_compare_highscore_candidates_28d0                           # CS:28A6
    mov al, BYTE PTR [di+0x2]                                        # CS:28A8
    cmp al, BYTE PTR [si+0x2]                                        # CS:28AB
    ja L_compare_highscore_candidates_28d5                           # CS:28AE
    jb L_compare_highscore_candidates_28d0                           # CS:28B0
    mov al, BYTE PTR [di+0x3]                                        # CS:28B2
    cmp al, BYTE PTR [si+0x3]                                        # CS:28B5
    ja L_compare_highscore_candidates_28d5                           # CS:28B8
    jb L_compare_highscore_candidates_28d0                           # CS:28BA
    mov al, BYTE PTR [di+0x4]                                        # CS:28BC
    cmp al, BYTE PTR [si+0x4]                                        # CS:28BF
    ja L_compare_highscore_candidates_28d5                           # CS:28C2
    jb L_compare_highscore_candidates_28d0                           # CS:28C4
    mov al, BYTE PTR [di+0x5]                                        # CS:28C6
    cmp al, BYTE PTR [si+0x5]                                        # CS:28C9
    ja L_compare_highscore_candidates_28d5                           # CS:28CC
    jb L_compare_highscore_candidates_28d0                           # CS:28CE
L_compare_highscore_candidates_28d0:
    mov al, 0x0                                                      # CS:28D0
    pop di                                                           # CS:28D2
    pop si                                                           # CS:28D3
    ret                                                              # CS:28D4
L_compare_highscore_candidates_28d5:
    mov al, 0x1                                                      # CS:28D5
    pop di                                                           # CS:28D7
    pop si                                                           # CS:28D8
    ret                                                              # CS:28D9

/* ========================================================================== 
 * render_highscore_table  |  CS:28DA
 * Назначение: Рисует имена и результаты шести победителей в packed-буфер PODZ1.
 * Вход: order[0..5], таблица layout DS:01B1.
 * Эффект/ограничения: Не выводит непосредственно на EGA.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type render_highscore_table, @function
render_highscore_table:
    mov si, 0x94da                                                   # CS:28DA
    mov di, 0x1b1                                                    # CS:28DD
    mov cx, 0x6                                                      # CS:28E0
L_render_highscore_table_28e3:
    push cx                                                          # CS:28E3
    add di, 0x2                                                      # CS:28E4
    mov ax, WORD PTR [di]                                            # CS:28E7
    add di, 0x2                                                      # CS:28E9
    push si                                                          # CS:28EC
    push di                                                          # CS:28ED
    call draw_highscore_name                                         # CS:28EE
    pop di                                                           # CS:28F1
    pop si                                                           # CS:28F2
    add di, 0x2                                                      # CS:28F3
    mov ax, WORD PTR [di]                                            # CS:28F6
    add di, 0x2                                                      # CS:28F8
    push si                                                          # CS:28FB
    push di                                                          # CS:28FC
    call draw_highscore_digits                                       # CS:28FD
    pop di                                                           # CS:2900
    pop si                                                           # CS:2901
    inc si                                                           # CS:2902
    pop cx                                                           # CS:2903
    loop L_render_highscore_table_28e3                               # CS:2904
    ret                                                              # CS:2906

/* ========================================================================== 
 * draw_highscore_digits  |  CS:2907
 * Назначение: Рисует 6 цифр выбранного результата в текстовую область, заменяя ведущие нули пустым glyph.
 * Вход: DS:SI = указатель на индекс; AX=destination offset.
 * Эффект/ограничения: Destination хранится в text_draw_offset; исходные glyph берутся из BIRD2.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_highscore_digits, @function
draw_highscore_digits:
    mov ds:[D_text_draw_offset], ax                                  # CS:2907
    mov al, BYTE PTR [si]                                            # CS:290A
    {load} mov ah, al                                                # CS:290C
    shl al, 1                                                        # CS:290E
    {load} add al, ah                                                # CS:2910
    mov ah, 0x0                                                      # CS:2912
    shl ax, 1                                                        # CS:2914
    ACC16 add, 0x94a9                                                # CS:2916
    {load} mov si, ax                                                # CS:2919
    mov BYTE PTR ds:[D_score_leading_zero], 0x1                      # CS:291B
    mov cx, 0x6                                                      # CS:2920
    mov dx, 0x0                                                      # CS:2923
L_draw_highscore_digits_2926:
    push cx                                                          # CS:2926
    lods al, BYTE PTR ds:[si]                                        # CS:2927
    cmp al, 0x0                                                      # CS:2928
    je L_draw_highscore_digits_2931                                  # CS:292A
    mov BYTE PTR ds:[D_score_leading_zero], 0x0                      # CS:292C
L_draw_highscore_digits_2931:
    mov cl, BYTE PTR ds:[D_score_leading_zero]                       # CS:2931
    cmp cl, 0x0                                                      # CS:2935
    je L_draw_highscore_digits_293f                                  # CS:2938
    mov ax, 0x6f6c                                                   # CS:293A
    jmp L_draw_highscore_digits_2949                                 # CS:293D
L_draw_highscore_digits_293f:
    mov bh, 0x0                                                      # CS:293F
    {load} mov bl, al                                                # CS:2941
    shl bx, 1                                                        # CS:2943
    mov ax, WORD PTR [bx+D_highscore_digit_sprites]                  # CS:2945
L_draw_highscore_digits_2949:
    push dx                                                          # CS:2949
    push si                                                          # CS:294A
    push ax                                                          # CS:294B
    push dx                                                          # CS:294C
    call copy_text_glyph                                             # CS:294D
    pop si                                                           # CS:2950
    pop dx                                                           # CS:2951
    add dx, 0x6                                                      # CS:2952
    pop cx                                                           # CS:2955
    loop L_draw_highscore_digits_2926                                # CS:2956
    ret                                                              # CS:2958

/* ========================================================================== 
 * draw_highscore_name  |  CS:2959
 * Назначение: Выбирает 12-байтовое имя по индексу и проваливается в draw_name_12_chars.
 * Вход: DS:SI = указатель на индекс; AX=destination offset.
 * Эффект/ограничения: Общая функция с дополнительным входом 296F; между ними нет RET.
 * Возвраты в физическом диапазоне: нет; переход/fall-through/невозвратный путь.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_highscore_name, @function
draw_highscore_name:

    # Вход обёртки; продолжение переходит в общий участок. Не считать соседний entry новой независимой рамкой.
    mov ds:[D_text_draw_offset], ax                                  # CS:2959
    mov al, BYTE PTR [si]                                            # CS:295C
    {load} mov ah, al                                                # CS:295E
    shl al, 1                                                        # CS:2960
    {load} add al, ah                                                # CS:2962
    mov ah, 0x0                                                      # CS:2964
    shl ax, 1                                                        # CS:2966
    shl ax, 1                                                        # CS:2968
    ACC16 add, 0x9455                                                # CS:296A
    {load} mov si, ax                                                # CS:296D

/* ========================================================================== 
 * draw_name_12_chars  |  CS:296F
 * Назначение: Рисует 12 символов: A–Z, пробел, точка, подчёркивание по особым glyph.
 * Вход: DS:SI = 12 байт имени; text_draw_offset.
 * Эффект/ограничения: Дополнительный вход общей процедуры; вызывается также напрямую.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_name_12_chars, @function
draw_name_12_chars:
    mov cx, 0xc                                                      # CS:296F
    mov dx, 0x0                                                      # CS:2972
L_draw_name_12_chars_2975:
    push cx                                                          # CS:2975
    lods al, BYTE PTR ds:[si]                                        # CS:2976
    push si                                                          # CS:2977
    push dx                                                          # CS:2978
    cmp al, 0x20                                                     # CS:2979
    jne L_draw_name_12_chars_2982                                    # CS:297B
    mov ax, 0x6546                                                   # CS:297D
    jmp L_draw_name_12_chars_29a0                                    # CS:2980
L_draw_name_12_chars_2982:
    cmp al, 0x2e                                                     # CS:2982
    jne L_draw_name_12_chars_298b                                    # CS:2984
    mov ax, 0x6f6c                                                   # CS:2986
    jmp L_draw_name_12_chars_29a0                                    # CS:2989
L_draw_name_12_chars_298b:
    cmp al, 0x5f                                                     # CS:298B
    jne L_draw_name_12_chars_2994                                    # CS:298D
    mov ax, 0x764c                                                   # CS:298F
    jmp L_draw_name_12_chars_29a0                                    # CS:2992
L_draw_name_12_chars_2994:
    sub al, 0x41                                                     # CS:2994
    {load} mov ah, al                                                # CS:2996
    shl al, 1                                                        # CS:2998
    {load} add al, ah                                                # CS:299A
    shl al, 1                                                        # CS:299C
    mov ah, 0x0                                                      # CS:299E
L_draw_name_12_chars_29a0:
    push ax                                                          # CS:29A0
    push dx                                                          # CS:29A1
    call copy_text_glyph                                             # CS:29A2
    pop dx                                                           # CS:29A5
    add dx, 0x6                                                      # CS:29A6
    pop si                                                           # CS:29A9
    pop cx                                                           # CS:29AA
    loop L_draw_name_12_chars_2975                                   # CS:29AB
    ret                                                              # CS:29AD

/* ========================================================================== 
 * copy_text_glyph  |  CS:29AE
 * Назначение: Копирует glyph 12×11 пикселей из BIRD2 в PODZ1 (6 байт на строку, stride160).
 * Вход: [BP+4]=x offset bytes; [BP+6]=glyph source offset.
 * Эффект/ограничения: RET 4; DS сохраняется, ES становится сегментом PODZ1; ожидается DF=0.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type copy_text_glyph, @function
copy_text_glyph:
    push bp                                                          # CS:29AE
    {load} mov bp, sp                                                # CS:29AF
    mov ax, ds:[D_segment_podz1]                                     # CS:29B1
    mov es, ax                                                       # CS:29B4
    mov di, WORD PTR ds:[D_text_draw_offset]                         # CS:29B6
    add di, WORD PTR [bp+0x4]                                        # CS:29BA
    mov ax, ds:[D_segment_bird2]                                     # CS:29BD
    push ds                                                          # CS:29C0
    mov ds, ax                                                       # CS:29C1
    mov si, WORD PTR [bp+0x6]                                        # CS:29C3
    mov cx, 0xb                                                      # CS:29C6
L_copy_text_glyph_29c9:
    movs WORD PTR es:[di], WORD PTR ds:[si]                          # CS:29C9
    movs WORD PTR es:[di], WORD PTR ds:[si]                          # CS:29CA
    movs WORD PTR es:[di], WORD PTR ds:[si]                          # CS:29CB
    add di, 0x9a                                                     # CS:29CC
    add si, 0x9a                                                     # CS:29D0
    loop L_copy_text_glyph_29c9                                      # CS:29D4
    pop ds                                                           # CS:29D6
    pop bp                                                           # CS:29D7
    ret 0x4                                                          # CS:29D8

/* ========================================================================== 
 * edit_highscore_name  |  CS:29DB
 * Назначение: Вводит до 12 символов с backspace, перерисовывает заставку и сохраняет HISCORES.
 * Вход: pending name pointer/rank.
 * Эффект/ограничения: Нулевой результат poll_key_character завершает ввод; нет отдельного бесконечного ожидания Enter.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type edit_highscore_name, @function
edit_highscore_name:
    mov WORD PTR ds:[D_name_cursor], 0x0                             # CS:29DB
L_edit_highscore_name_29e1:
    mov ax, ds:[D_name_cursor]                                       # CS:29E1
    ACC16 cmp, 0xc                                                   # CS:29E4
    jae L_edit_highscore_name_2a39                                   # CS:29E7
    call poll_key_character                                          # CS:29E9
    cmp al, 0x0                                                      # CS:29EC
    je L_edit_highscore_name_2a39                                    # CS:29EE
    cmp al, 0x8                                                      # CS:29F0
    jne L_edit_highscore_name_2a0d                                   # CS:29F2
    mov bx, WORD PTR ds:[D_pending_name_pointer]                     # CS:29F4
    add bx, WORD PTR ds:[D_name_cursor]                              # CS:29F8
    mov BYTE PTR [bx], 0x2e                                          # CS:29FC
    dec WORD PTR ds:[D_name_cursor]                                  # CS:29FF
    jns L_edit_highscore_name_2a0b                                   # CS:2A03
    mov WORD PTR ds:[D_name_cursor], 0x0                             # CS:2A05
L_edit_highscore_name_2a0b:
    jmp L_edit_highscore_name_2a1b                                   # CS:2A0B
L_edit_highscore_name_2a0d:
    mov bx, WORD PTR ds:[D_pending_name_pointer]                     # CS:2A0D
    add bx, WORD PTR ds:[D_name_cursor]                              # CS:2A11
    mov BYTE PTR [bx], al                                            # CS:2A15
    inc WORD PTR ds:[D_name_cursor]                                  # CS:2A17
L_edit_highscore_name_2a1b:
    mov bx, WORD PTR ds:[D_name_cursor]                              # CS:2A1B
    cmp bx, 0xc                                                      # CS:2A1F
    jae L_edit_highscore_name_2a2b                                   # CS:2A22
    add bx, WORD PTR ds:[D_pending_name_pointer]                     # CS:2A24
    mov BYTE PTR [bx], 0x5f                                          # CS:2A28
L_edit_highscore_name_2a2b:
    call redraw_edited_name                                          # CS:2A2B
    call restore_attract_background                                  # CS:2A2E
    call render_entities                                             # CS:2A31
    call present_frame                                               # CS:2A34
    jmp L_edit_highscore_name_29e1                                   # CS:2A37
L_edit_highscore_name_2a39:
    mov bx, WORD PTR ds:[D_name_cursor]                              # CS:2A39
    cmp bx, 0xc                                                      # CS:2A3D
    jae L_edit_highscore_name_2a52                                   # CS:2A40
    add bx, WORD PTR ds:[D_pending_name_pointer]                     # CS:2A42
    mov al, BYTE PTR [bx]                                            # CS:2A46
    cmp al, 0x5f                                                     # CS:2A48
    jne L_edit_highscore_name_2a52                                   # CS:2A4A
    mov BYTE PTR [bx], 0x2e                                          # CS:2A4C
    call redraw_edited_name                                          # CS:2A4F
L_edit_highscore_name_2a52:
    mov di, 0x9455                                                   # CS:2A52
    mov dx, 0x1506                                                   # CS:2A55
    mov cx, 0x8c                                                     # CS:2A58
    call dos_create_write_close                                      # CS:2A5B
    mov BYTE PTR ds:[D_pending_name_entry], 0x0                      # CS:2A5E
    ret                                                              # CS:2A63

/* ========================================================================== 
 * redraw_edited_name  |  CS:2A64
 * Назначение: Перерисовывает только имя выбранной строки рекордов.
 * Вход: name pointer, pending rank.
 * Эффект/ограничения: Вызывает дополнительный вход draw_name_12_chars.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type redraw_edited_name, @function
redraw_edited_name:
    mov si, WORD PTR ds:[D_pending_name_pointer]                     # CS:2A64
    mov bl, BYTE PTR ds:[D_pending_highscore_rank]                   # CS:2A68
    mov bh, 0x0                                                      # CS:2A6C
    shl bx, 1                                                        # CS:2A6E
    shl bx, 1                                                        # CS:2A70
    shl bx, 1                                                        # CS:2A72
    mov ax, WORD PTR [bx+D_highscore_name_destinations]              # CS:2A74
    mov ds:[D_text_draw_offset], ax                                  # CS:2A78
    call draw_name_12_chars                                          # CS:2A7B
    ret                                                              # CS:2A7E

/* ========================================================================== 
 * reset_collectible_hud  |  CS:2A7F
 * Назначение: Обнуляет 6 collectible flags и рисует исходные значки.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Массив flags одновременно проверяется выходными воротами.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_collectible_hud, @function
reset_collectible_hud:
    mov BYTE PTR ds:[D_collectible_0], 0x0                           # CS:2A7F
    mov BYTE PTR ds:[D_collectible_1], 0x0                           # CS:2A84
    mov BYTE PTR ds:[D_collectible_2], 0x0                           # CS:2A89
    mov BYTE PTR ds:[D_collectible_3], 0x0                           # CS:2A8E
    mov BYTE PTR ds:[D_collectible_4], 0x0                           # CS:2A93
    mov BYTE PTR ds:[D_collectible_5], 0x0                           # CS:2A98
    mov ax, 0x1178                                                   # CS:2A9D
    push ax                                                          # CS:2AA0
    mov ax, 0x1b82                                                   # CS:2AA1
    push ax                                                          # CS:2AA4
    mov ax, 0x10                                                     # CS:2AA5
    push ax                                                          # CS:2AA8
    mov ax, 0x8                                                      # CS:2AA9
    push ax                                                          # CS:2AAC
    mov ax, 0x4                                                      # CS:2AAD
    push ax                                                          # CS:2AB0
    call blit_hud_sprite_both_pages                                  # CS:2AB1
    mov ax, 0x1678                                                   # CS:2AB4
    push ax                                                          # CS:2AB7
    mov ax, 0x1cc2                                                   # CS:2AB8
    push ax                                                          # CS:2ABB
    mov ax, 0x10                                                     # CS:2ABC
    push ax                                                          # CS:2ABF
    mov ax, 0x8                                                      # CS:2AC0
    push ax                                                          # CS:2AC3
    mov ax, 0x4                                                      # CS:2AC4
    push ax                                                          # CS:2AC7
    call blit_hud_sprite_both_pages                                  # CS:2AC8
    mov ax, 0x1b78                                                   # CS:2ACB
    push ax                                                          # CS:2ACE
    mov ax, 0x1e02                                                   # CS:2ACF
    push ax                                                          # CS:2AD2
    mov ax, 0x10                                                     # CS:2AD3
    push ax                                                          # CS:2AD6
    mov ax, 0x8                                                      # CS:2AD7
    push ax                                                          # CS:2ADA
    mov ax, 0x4                                                      # CS:2ADB
    push ax                                                          # CS:2ADE
    call blit_hud_sprite_both_pages                                  # CS:2ADF
    mov ax, 0x2078                                                   # CS:2AE2
    push ax                                                          # CS:2AE5
    mov ax, 0x1b85                                                   # CS:2AE6
    push ax                                                          # CS:2AE9
    mov ax, 0x10                                                     # CS:2AEA
    push ax                                                          # CS:2AED
    mov ax, 0x8                                                      # CS:2AEE
    push ax                                                          # CS:2AF1
    mov ax, 0x4                                                      # CS:2AF2
    push ax                                                          # CS:2AF5
    call blit_hud_sprite_both_pages                                  # CS:2AF6
    mov ax, 0x2578                                                   # CS:2AF9
    push ax                                                          # CS:2AFC
    mov ax, 0x1cc5                                                   # CS:2AFD
    push ax                                                          # CS:2B00
    mov ax, 0x10                                                     # CS:2B01
    push ax                                                          # CS:2B04
    mov ax, 0x8                                                      # CS:2B05
    push ax                                                          # CS:2B08
    mov ax, 0x4                                                      # CS:2B09
    push ax                                                          # CS:2B0C
    call blit_hud_sprite_both_pages                                  # CS:2B0D
    mov ax, 0x2a78                                                   # CS:2B10
    push ax                                                          # CS:2B13
    mov ax, 0x1e05                                                   # CS:2B14
    push ax                                                          # CS:2B17
    mov ax, 0x10                                                     # CS:2B18
    push ax                                                          # CS:2B1B
    mov ax, 0x8                                                      # CS:2B1C
    push ax                                                          # CS:2B1F
    mov ax, 0x4                                                      # CS:2B20
    push ax                                                          # CS:2B23
    call blit_hud_sprite_both_pages                                  # CS:2B24
    ret                                                              # CS:2B27

/* ========================================================================== 
 * collect_item_and_update_hud  |  CS:2B28
 * Назначение: Начисляет 150 очков, выставляет collectible[AL] и меняет его значок.
 * Вход: AL=0..5; значения вне диапазона всё равно сначала начисляют очки.
 * Эффект/ограничения: Не добавлять раннюю проверку индекса перед начислением при буквальном переносе.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type collect_item_and_update_hud, @function
collect_item_and_update_hud:
    push ax                                                          # CS:2B28
    mov ax, 0x0                                                      # CS:2B29
    push ax                                                          # CS:2B2C
    mov ax, 0x1                                                      # CS:2B2D
    push ax                                                          # CS:2B30
    mov ax, 0x5                                                      # CS:2B31
    push ax                                                          # CS:2B34
    mov ax, 0x0                                                      # CS:2B35
    push ax                                                          # CS:2B38
    call score_add_digits                                            # CS:2B39
    pop ax                                                           # CS:2B3C
    cmp al, 0x0                                                      # CS:2B3D
    je L_collect_item_and_update_hud_2b59                            # CS:2B3F
    cmp al, 0x1                                                      # CS:2B41
    je L_collect_item_and_update_hud_2b76                            # CS:2B43
    cmp al, 0x2                                                      # CS:2B45
    je L_collect_item_and_update_hud_2b93                            # CS:2B47
    cmp al, 0x3                                                      # CS:2B49
    je L_collect_item_and_update_hud_2bb0                            # CS:2B4B
    cmp al, 0x4                                                      # CS:2B4D
    je L_collect_item_and_update_hud_2bcd                            # CS:2B4F
    cmp al, 0x5                                                      # CS:2B51
    jne L_collect_item_and_update_hud_2b58                           # CS:2B53
    jmp L_collect_item_and_update_hud_2bea                           # CS:2B55
L_collect_item_and_update_hud_2b58:
    ret                                                              # CS:2B58
L_collect_item_and_update_hud_2b59:
    mov BYTE PTR ds:[D_collectible_0], 0x1                           # CS:2B59
    mov ax, 0x2f78                                                   # CS:2B5E
    push ax                                                          # CS:2B61
    mov ax, 0x1b82                                                   # CS:2B62
    push ax                                                          # CS:2B65
    mov ax, 0x10                                                     # CS:2B66
    push ax                                                          # CS:2B69
    mov ax, 0x8                                                      # CS:2B6A
    push ax                                                          # CS:2B6D
    mov ax, 0x4                                                      # CS:2B6E
    push ax                                                          # CS:2B71
    call blit_hud_sprite_both_pages                                  # CS:2B72
    ret                                                              # CS:2B75
L_collect_item_and_update_hud_2b76:
    mov BYTE PTR ds:[D_collectible_1], 0x1                           # CS:2B76
    mov ax, 0x2f78                                                   # CS:2B7B
    push ax                                                          # CS:2B7E
    mov ax, 0x1cc2                                                   # CS:2B7F
    push ax                                                          # CS:2B82
    mov ax, 0x10                                                     # CS:2B83
    push ax                                                          # CS:2B86
    mov ax, 0x8                                                      # CS:2B87
    push ax                                                          # CS:2B8A
    mov ax, 0x4                                                      # CS:2B8B
    push ax                                                          # CS:2B8E
    call blit_hud_sprite_both_pages                                  # CS:2B8F
    ret                                                              # CS:2B92
L_collect_item_and_update_hud_2b93:
    mov BYTE PTR ds:[D_collectible_2], 0x1                           # CS:2B93
    mov ax, 0x2f78                                                   # CS:2B98
    push ax                                                          # CS:2B9B
    mov ax, 0x1e02                                                   # CS:2B9C
    push ax                                                          # CS:2B9F
    mov ax, 0x10                                                     # CS:2BA0
    push ax                                                          # CS:2BA3
    mov ax, 0x8                                                      # CS:2BA4
    push ax                                                          # CS:2BA7
    mov ax, 0x4                                                      # CS:2BA8
    push ax                                                          # CS:2BAB
    call blit_hud_sprite_both_pages                                  # CS:2BAC
    ret                                                              # CS:2BAF
L_collect_item_and_update_hud_2bb0:
    mov BYTE PTR ds:[D_collectible_3], 0x1                           # CS:2BB0
    mov ax, 0x2f78                                                   # CS:2BB5
    push ax                                                          # CS:2BB8
    mov ax, 0x1b85                                                   # CS:2BB9
    push ax                                                          # CS:2BBC
    mov ax, 0x10                                                     # CS:2BBD
    push ax                                                          # CS:2BC0
    mov ax, 0x8                                                      # CS:2BC1
    push ax                                                          # CS:2BC4
    mov ax, 0x4                                                      # CS:2BC5
    push ax                                                          # CS:2BC8
    call blit_hud_sprite_both_pages                                  # CS:2BC9
    ret                                                              # CS:2BCC
L_collect_item_and_update_hud_2bcd:
    mov BYTE PTR ds:[D_collectible_4], 0x1                           # CS:2BCD
    mov ax, 0x2f78                                                   # CS:2BD2
    push ax                                                          # CS:2BD5
    mov ax, 0x1cc5                                                   # CS:2BD6
    push ax                                                          # CS:2BD9
    mov ax, 0x10                                                     # CS:2BDA
    push ax                                                          # CS:2BDD
    mov ax, 0x8                                                      # CS:2BDE
    push ax                                                          # CS:2BE1
    mov ax, 0x4                                                      # CS:2BE2
    push ax                                                          # CS:2BE5
    call blit_hud_sprite_both_pages                                  # CS:2BE6
    ret                                                              # CS:2BE9
L_collect_item_and_update_hud_2bea:
    mov BYTE PTR ds:[D_collectible_5], 0x1                           # CS:2BEA
    mov ax, 0x2f78                                                   # CS:2BEF
    push ax                                                          # CS:2BF2
    mov ax, 0x1e05                                                   # CS:2BF3
    push ax                                                          # CS:2BF6
    mov ax, 0x10                                                     # CS:2BF7
    push ax                                                          # CS:2BFA
    mov ax, 0x8                                                      # CS:2BFB
    push ax                                                          # CS:2BFE
    mov ax, 0x4                                                      # CS:2BFF
    push ax                                                          # CS:2C02
    call blit_hud_sprite_both_pages                                  # CS:2C03
    ret                                                              # CS:2C06
