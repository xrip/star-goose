/* Десятичная арифметика счёта
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * reset_score  |  CS:264A
 * Назначение: Очищает шесть десятичных цифр текущего счёта и вызывает отрисовку.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Цифры хранятся отдельно, не ASCII и не packed BCD.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type reset_score, @function
reset_score:

    # Счёт — шесть отдельных десятичных цифр. Это не ASCII и не packed BCD.
    mov al, 0x0                                                      # CS:264A
    mov ds:[D_score_digit_100000], al                                # CS:264C
    mov ds:[D_score_digit_10000], al                                 # CS:264F
    mov ds:[D_score_digit_1000], al                                  # CS:2652
    mov ds:[D_score_digit_100], al                                   # CS:2655
    mov ds:[D_score_digit_10], al                                    # CS:2658
    mov ds:[D_score_digit_1], al                                     # CS:265B
    call draw_current_score                                          # CS:265E
    ret                                                              # CS:2661

/* ========================================================================== 
 * score_add_10  |  CS:2662
 * Назначение: Добавляет 10 через четыре десятичных разряда.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Стековая обёртка score_add_digits.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type score_add_10, @function
score_add_10:
    mov ax, 0x0                                                      # CS:2662
    push ax                                                          # CS:2665
    mov ax, 0x0                                                      # CS:2666
    push ax                                                          # CS:2669
    mov ax, 0x1                                                      # CS:266A
    push ax                                                          # CS:266D
    mov ax, 0x0                                                      # CS:266E
    push ax                                                          # CS:2671
    call score_add_digits                                            # CS:2672
    ret                                                              # CS:2675

/* ========================================================================== 
 * score_add_150  |  CS:2676
 * Назначение: Добавляет 150 через четыре десятичных разряда.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Используется при уничтожении/сборе объектов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type score_add_150, @function
score_add_150:
    mov ax, 0x0                                                      # CS:2676
    push ax                                                          # CS:2679
    mov ax, 0x1                                                      # CS:267A
    push ax                                                          # CS:267D
    mov ax, 0x5                                                      # CS:267E
    push ax                                                          # CS:2681
    mov ax, 0x0                                                      # CS:2682
    push ax                                                          # CS:2685
    call score_add_digits                                            # CS:2686
    ret                                                              # CS:2689

/* ========================================================================== 
 * score_add_2500  |  CS:268A
 * Назначение: Добавляет 2500 через четыре десятичных разряда.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Используется как один из туннельных бонусов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type score_add_2500, @function
score_add_2500:
    mov ax, 0x2                                                      # CS:268A
    push ax                                                          # CS:268D
    mov ax, 0x5                                                      # CS:268E
    push ax                                                          # CS:2691
    mov ax, 0x0                                                      # CS:2692
    push ax                                                          # CS:2695
    mov ax, 0x0                                                      # CS:2696
    push ax                                                          # CS:2699
    call score_add_digits                                            # CS:269A
    ret                                                              # CS:269D

/* ========================================================================== 
 * score_add_digits  |  CS:269E
 * Назначение: Добавляет units/tens/hundreds/thousands с переносами в массив из 6 цифр.
 * Вход: [BP+4]=units, +6=tens, +8=hundreds, +10=thousands; читаются младшие байты.
 * Эффект/ограничения: RET 8. Каждый разряд вычитает 10 максимум один раз; перенос 100000 даёт жизнь. Не нормализовать большие входы иначе.
 * Возвраты в физическом диапазоне: ret 0x8.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type score_add_digits, @function
score_add_digits:
    push bp                                                          # CS:269E
    {load} mov bp, sp                                                # CS:269F
    mov al, ds:[D_score_digit_1]                                     # CS:26A1
    add al, BYTE PTR [bp+0x4]                                        # CS:26A4
    cmp al, 0x9                                                      # CS:26A7
    jbe L_score_add_digits_26b1                                      # CS:26A9
    sub al, 0xa                                                      # CS:26AB
    inc BYTE PTR ds:[D_score_digit_10]                               # CS:26AD
L_score_add_digits_26b1:
    mov ds:[D_score_digit_1], al                                     # CS:26B1
    mov al, ds:[D_score_digit_10]                                    # CS:26B4
    add al, BYTE PTR [bp+0x6]                                        # CS:26B7
    cmp al, 0x9                                                      # CS:26BA
    jbe L_score_add_digits_26c4                                      # CS:26BC
    sub al, 0xa                                                      # CS:26BE
    inc BYTE PTR ds:[D_score_digit_100]                              # CS:26C0
L_score_add_digits_26c4:
    mov ds:[D_score_digit_10], al                                    # CS:26C4
    mov al, ds:[D_score_digit_100]                                   # CS:26C7
    add al, BYTE PTR [bp+0x8]                                        # CS:26CA
    cmp al, 0x9                                                      # CS:26CD
    jbe L_score_add_digits_26d7                                      # CS:26CF
    sub al, 0xa                                                      # CS:26D1
    inc BYTE PTR ds:[D_score_digit_1000]                             # CS:26D3
L_score_add_digits_26d7:
    mov ds:[D_score_digit_100], al                                   # CS:26D7
    mov al, ds:[D_score_digit_1000]                                  # CS:26DA
    add al, BYTE PTR [bp+0xa]                                        # CS:26DD
    cmp al, 0x9                                                      # CS:26E0
    jbe L_score_add_digits_26ea                                      # CS:26E2
    sub al, 0xa                                                      # CS:26E4
    inc BYTE PTR ds:[D_score_digit_10000]                            # CS:26E6
L_score_add_digits_26ea:
    mov ds:[D_score_digit_1000], al                                  # CS:26EA
    mov al, ds:[D_score_digit_10000]                                 # CS:26ED
    cmp al, 0x9                                                      # CS:26F0
    jbe L_score_add_digits_2701                                      # CS:26F2
    sub al, 0xa                                                      # CS:26F4
    inc BYTE PTR ds:[D_score_digit_100000]                           # CS:26F6
    push ax                                                          # CS:26FA
    push si                                                          # CS:26FB
    call add_life                                                    # CS:26FC
    pop si                                                           # CS:26FF
    pop ax                                                           # CS:2700
L_score_add_digits_2701:
    mov ds:[D_score_digit_10000], al                                 # CS:2701
    mov al, ds:[D_score_digit_100000]                                # CS:2704
    cmp al, 0x9                                                      # CS:2707
    jbe L_score_add_digits_270d                                      # CS:2709
    sub al, 0xa                                                      # CS:270B
L_score_add_digits_270d:
    mov ds:[D_score_digit_100000], al                                # CS:270D
    pop bp                                                           # CS:2710
    ret 0x8                                                          # CS:2711

/* ========================================================================== 
 * draw_current_score  |  CS:2714
 * Назначение: Рисует значащие цифры текущего счёта без ведущих нулей.
 * Вход: score_digits[6].
 * Эффект/ограничения: Позиция вывода сдвигается только при реально нарисованной цифре.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_current_score, @function
draw_current_score:
    mov BYTE PTR ds:[D_score_leading_zero], 0x1                      # CS:2714
    mov si, 0x94a9                                                   # CS:2719
    mov cx, 0x6                                                      # CS:271C
    mov dx, 0x0                                                      # CS:271F
L_draw_current_score_2722:
    push cx                                                          # CS:2722
    lods al, BYTE PTR ds:[si]                                        # CS:2723
    cmp al, 0x0                                                      # CS:2724
    je L_draw_current_score_272d                                     # CS:2726
    mov BYTE PTR ds:[D_score_leading_zero], 0x0                      # CS:2728
L_draw_current_score_272d:
    mov cl, BYTE PTR ds:[D_score_leading_zero]                       # CS:272D
    cmp cl, 0x0                                                      # CS:2731
    jne L_draw_current_score_275e                                    # CS:2734
    push dx                                                          # CS:2736
    push si                                                          # CS:2737
    mov bh, 0x0                                                      # CS:2738
    {load} mov bl, al                                                # CS:273A
    shl bx, 1                                                        # CS:273C
    mov ax, WORD PTR [bx+D_hud_digit_sprites]                        # CS:273E
    push ax                                                          # CS:2742
    {load} mov bx, dx                                                # CS:2743
    mov ax, WORD PTR [bx+D_hud_digit_destinations]                   # CS:2745
    push ax                                                          # CS:2749
    mov ax, 0x10                                                     # CS:274A
    push ax                                                          # CS:274D
    mov ax, 0x9                                                      # CS:274E
    push ax                                                          # CS:2751
    mov ax, 0x2                                                      # CS:2752
    push ax                                                          # CS:2755
    call blit_hud_sprite_both_pages                                  # CS:2756
    pop si                                                           # CS:2759
    pop dx                                                           # CS:275A
    add dx, 0x2                                                      # CS:275B
L_draw_current_score_275e:
    pop cx                                                           # CS:275E
    loop L_draw_current_score_2722                                   # CS:275F
    ret                                                              # CS:2761
