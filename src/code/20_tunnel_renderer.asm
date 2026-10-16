/* Геометрия туннеля и целочисленные линии
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * build_tunnel_bitmap  |  CS:34A8
 * Назначение: Очищает 1bpp scratch, рисует диагонали и последовательно уменьшаемые восьмиугольники по дистанции.
 * Вход: tunnel_distance, radius tables.
 * Эффект/ограничения: Bitmap 256×256 с stride32; на экран берётся его центральная часть.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type build_tunnel_bitmap, @function
build_tunnel_bitmap:
    mov di, 0x0                                                      # CS:34A8
    mov ax, ds:[D_segment_scratch0]                                  # CS:34AB
    mov es, ax                                                       # CS:34AE
    mov ax, 0x0                                                      # CS:34B0
    mov cx, 0x1000                                                   # CS:34B3
    cld                                                              # CS:34B6
    rep stos WORD PTR es:[di], ax                                    # CS:34B7
    mov ax, 0x4f                                                     # CS:34B9
    push ax                                                          # CS:34BC
    mov ax, 0xff                                                     # CS:34BD
    push ax                                                          # CS:34C0
    mov ax, 0xb0                                                     # CS:34C1
    push ax                                                          # CS:34C4
    mov ax, 0x0                                                      # CS:34C5
    push ax                                                          # CS:34C8
    call draw_tunnel_line                                            # CS:34C9
    mov ax, 0x4f                                                     # CS:34CC
    push ax                                                          # CS:34CF
    mov ax, 0x0                                                      # CS:34D0
    push ax                                                          # CS:34D3
    mov ax, 0xb0                                                     # CS:34D4
    push ax                                                          # CS:34D7
    mov ax, 0xff                                                     # CS:34D8
    push ax                                                          # CS:34DB
    call draw_tunnel_line                                            # CS:34DC
    mov ax, 0x0                                                      # CS:34DF
    push ax                                                          # CS:34E2
    mov ax, 0x4f                                                     # CS:34E3
    push ax                                                          # CS:34E6
    mov ax, 0xff                                                     # CS:34E7
    push ax                                                          # CS:34EA
    mov ax, 0xb0                                                     # CS:34EB
    push ax                                                          # CS:34EE
    call draw_tunnel_line                                            # CS:34EF
    mov ax, 0x0                                                      # CS:34F2
    push ax                                                          # CS:34F5
    mov ax, 0xb0                                                     # CS:34F6
    push ax                                                          # CS:34F9
    mov ax, 0xff                                                     # CS:34FA
    push ax                                                          # CS:34FD
    mov ax, 0x4f                                                     # CS:34FE
    push ax                                                          # CS:3501
    call draw_tunnel_line                                            # CS:3502
    mov ax, ds:[D_tunnel_distance]                                   # CS:3505
    ACC16 and, 0x3f                                                  # CS:3508
    mov ds:[D_tunnel_ring_phase], ax                                 # CS:350B
    {load} mov bx, ax                                                # CS:350E
    ACC16 add, 0x40                                                  # CS:3510
    push ax                                                          # CS:3513
    mov al, BYTE PTR [bx+D_tunnel_ring_radius_curve]                 # CS:3514
    mov ah, 0x0                                                      # CS:3518
    push ax                                                          # CS:351A
    call draw_nested_tunnel_rings                                    # CS:351B
    mov ax, ds:[D_tunnel_ring_phase]                                 # CS:351E
    ACC16 cmp, 0xc                                                   # CS:3521
    jbe L_build_tunnel_bitmap_3527                                   # CS:3524
    ret                                                              # CS:3526
L_build_tunnel_bitmap_3527:
    mov bx, WORD PTR ds:[D_tunnel_ring_phase]                        # CS:3527
    mov al, BYTE PTR [bx+D_tunnel_edge_x_curve]                      # CS:352B
    mov ah, 0x0                                                      # CS:352F
    ACC16 add, 0x80                                                  # CS:3531
    push ax                                                          # CS:3534
    mov ax, 0x0                                                      # CS:3535
    push ax                                                          # CS:3538
    mov ax, 0xff                                                     # CS:3539
    push ax                                                          # CS:353C
    mov al, BYTE PTR [bx+D_tunnel_edge_y_curve]                      # CS:353D
    mov ah, 0x0                                                      # CS:3541
    neg ax                                                           # CS:3543
    ACC16 add, 0x7f                                                  # CS:3545
    push ax                                                          # CS:3548
    call draw_tunnel_line                                            # CS:3549
    mov bx, WORD PTR ds:[D_tunnel_ring_phase]                        # CS:354C
    mov al, BYTE PTR [bx+D_tunnel_edge_x_curve]                      # CS:3550
    mov ah, 0x0                                                      # CS:3554
    neg ax                                                           # CS:3556
    ACC16 add, 0x7f                                                  # CS:3558
    push ax                                                          # CS:355B
    mov ax, 0x0                                                      # CS:355C
    push ax                                                          # CS:355F
    mov ax, 0x0                                                      # CS:3560
    push ax                                                          # CS:3563
    mov al, BYTE PTR [bx+D_tunnel_edge_y_curve]                      # CS:3564
    mov ah, 0x0                                                      # CS:3568
    neg ax                                                           # CS:356A
    ACC16 add, 0x7f                                                  # CS:356C
    push ax                                                          # CS:356F
    call draw_tunnel_line                                            # CS:3570
    mov bx, WORD PTR ds:[D_tunnel_ring_phase]                        # CS:3573
    mov al, BYTE PTR [bx+D_tunnel_edge_x_curve]                      # CS:3577
    mov ah, 0x0                                                      # CS:357B
    ACC16 add, 0x80                                                  # CS:357D
    push ax                                                          # CS:3580
    mov ax, 0xff                                                     # CS:3581
    push ax                                                          # CS:3584
    mov ax, 0xff                                                     # CS:3585
    push ax                                                          # CS:3588
    mov al, BYTE PTR [bx+D_tunnel_edge_y_curve]                      # CS:3589
    mov ah, 0x0                                                      # CS:358D
    ACC16 add, 0x80                                                  # CS:358F
    push ax                                                          # CS:3592
    call draw_tunnel_line                                            # CS:3593
    mov bx, WORD PTR ds:[D_tunnel_ring_phase]                        # CS:3596
    mov al, BYTE PTR [bx+D_tunnel_edge_x_curve]                      # CS:359A
    mov ah, 0x0                                                      # CS:359E
    neg ax                                                           # CS:35A0
    ACC16 add, 0x7f                                                  # CS:35A2
    push ax                                                          # CS:35A5
    mov ax, 0xff                                                     # CS:35A6
    push ax                                                          # CS:35A9
    mov ax, 0x0                                                      # CS:35AA
    push ax                                                          # CS:35AD
    mov al, BYTE PTR [bx+D_tunnel_edge_y_curve]                      # CS:35AE
    mov ah, 0x0                                                      # CS:35B2
    ACC16 add, 0x80                                                  # CS:35B4
    push ax                                                          # CS:35B7
    call draw_tunnel_line                                            # CS:35B8
    ret                                                              # CS:35BB

/* ========================================================================== 
 * draw_nested_tunnel_rings  |  CS:35BC
 * Назначение: Рисует 8 сторон кольца около центра127/128; делит радиусы пополам до порога.
 * Вход: [BP+4]=radiusA, [BP+6]=radiusB.
 * Эффект/ограничения: RET4; цикл, а не рекурсивные CALL; после уменьшения сравнивает AL с минимальным радиусом.
 * Возвраты в физическом диапазоне: ret 0x4.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_nested_tunnel_rings, @function
draw_nested_tunnel_rings:
    push bp                                                          # CS:35BC
    {load} mov bp, sp                                                # CS:35BD
L_draw_nested_tunnel_rings_35bf:
    mov ax, 0x7f                                                     # CS:35BF
    sub ax, WORD PTR [bp+0x4]                                        # CS:35C2
    push ax                                                          # CS:35C5
    mov ax, 0x7f                                                     # CS:35C6
    sub ax, WORD PTR [bp+0x6]                                        # CS:35C9
    push ax                                                          # CS:35CC
    mov ax, 0x80                                                     # CS:35CD
    add ax, WORD PTR [bp+0x4]                                        # CS:35D0
    push ax                                                          # CS:35D3
    mov ax, 0x7f                                                     # CS:35D4
    sub ax, WORD PTR [bp+0x6]                                        # CS:35D7
    push ax                                                          # CS:35DA
    call draw_tunnel_line                                            # CS:35DB
    mov ax, 0x80                                                     # CS:35DE
    add ax, WORD PTR [bp+0x4]                                        # CS:35E1
    push ax                                                          # CS:35E4
    mov ax, 0x7f                                                     # CS:35E5
    sub ax, WORD PTR [bp+0x6]                                        # CS:35E8
    push ax                                                          # CS:35EB
    mov ax, 0x80                                                     # CS:35EC
    add ax, WORD PTR [bp+0x6]                                        # CS:35EF
    push ax                                                          # CS:35F2
    mov ax, 0x7f                                                     # CS:35F3
    sub ax, WORD PTR [bp+0x4]                                        # CS:35F6
    push ax                                                          # CS:35F9
    call draw_tunnel_line                                            # CS:35FA
    mov ax, 0x80                                                     # CS:35FD
    add ax, WORD PTR [bp+0x6]                                        # CS:3600
    push ax                                                          # CS:3603
    mov ax, 0x7f                                                     # CS:3604
    sub ax, WORD PTR [bp+0x4]                                        # CS:3607
    push ax                                                          # CS:360A
    mov ax, 0x80                                                     # CS:360B
    add ax, WORD PTR [bp+0x6]                                        # CS:360E
    push ax                                                          # CS:3611
    mov ax, 0x80                                                     # CS:3612
    add ax, WORD PTR [bp+0x4]                                        # CS:3615
    push ax                                                          # CS:3618
    call draw_tunnel_line                                            # CS:3619
    mov ax, 0x80                                                     # CS:361C
    add ax, WORD PTR [bp+0x4]                                        # CS:361F
    push ax                                                          # CS:3622
    mov ax, 0x80                                                     # CS:3623
    add ax, WORD PTR [bp+0x6]                                        # CS:3626
    push ax                                                          # CS:3629
    mov ax, 0x80                                                     # CS:362A
    add ax, WORD PTR [bp+0x6]                                        # CS:362D
    push ax                                                          # CS:3630
    mov ax, 0x80                                                     # CS:3631
    add ax, WORD PTR [bp+0x4]                                        # CS:3634
    push ax                                                          # CS:3637
    call draw_tunnel_line                                            # CS:3638
    mov ax, 0x7f                                                     # CS:363B
    sub ax, WORD PTR [bp+0x4]                                        # CS:363E
    push ax                                                          # CS:3641
    mov ax, 0x80                                                     # CS:3642
    add ax, WORD PTR [bp+0x6]                                        # CS:3645
    push ax                                                          # CS:3648
    mov ax, 0x80                                                     # CS:3649
    add ax, WORD PTR [bp+0x4]                                        # CS:364C
    push ax                                                          # CS:364F
    mov ax, 0x80                                                     # CS:3650
    add ax, WORD PTR [bp+0x6]                                        # CS:3653
    push ax                                                          # CS:3656
    call draw_tunnel_line                                            # CS:3657
    mov ax, 0x7f                                                     # CS:365A
    sub ax, WORD PTR [bp+0x6]                                        # CS:365D
    push ax                                                          # CS:3660
    mov ax, 0x80                                                     # CS:3661
    add ax, WORD PTR [bp+0x4]                                        # CS:3664
    push ax                                                          # CS:3667
    mov ax, 0x7f                                                     # CS:3668
    sub ax, WORD PTR [bp+0x4]                                        # CS:366B
    push ax                                                          # CS:366E
    mov ax, 0x80                                                     # CS:366F
    add ax, WORD PTR [bp+0x6]                                        # CS:3672
    push ax                                                          # CS:3675
    call draw_tunnel_line                                            # CS:3676
    mov ax, 0x7f                                                     # CS:3679
    sub ax, WORD PTR [bp+0x6]                                        # CS:367C
    push ax                                                          # CS:367F
    mov ax, 0x7f                                                     # CS:3680
    sub ax, WORD PTR [bp+0x4]                                        # CS:3683
    push ax                                                          # CS:3686
    mov ax, 0x7f                                                     # CS:3687
    sub ax, WORD PTR [bp+0x6]                                        # CS:368A
    push ax                                                          # CS:368D
    mov ax, 0x80                                                     # CS:368E
    add ax, WORD PTR [bp+0x4]                                        # CS:3691
    push ax                                                          # CS:3694
    call draw_tunnel_line                                            # CS:3695
    mov ax, 0x7f                                                     # CS:3698
    sub ax, WORD PTR [bp+0x6]                                        # CS:369B
    push ax                                                          # CS:369E
    mov ax, 0x7f                                                     # CS:369F
    sub ax, WORD PTR [bp+0x4]                                        # CS:36A2
    push ax                                                          # CS:36A5
    mov ax, 0x7f                                                     # CS:36A6
    sub ax, WORD PTR [bp+0x4]                                        # CS:36A9
    push ax                                                          # CS:36AC
    mov ax, 0x7f                                                     # CS:36AD
    sub ax, WORD PTR [bp+0x6]                                        # CS:36B0
    push ax                                                          # CS:36B3
    call draw_tunnel_line                                            # CS:36B4
    mov ax, WORD PTR [bp+0x6]                                        # CS:36B7
    shr ax, 1                                                        # CS:36BA
    cmp al, BYTE PTR ds:[D_tunnel_min_radius]                        # CS:36BC
    jbe L_draw_nested_tunnel_rings_36d0                              # CS:36C0
    mov WORD PTR [bp+0x6], ax                                        # CS:36C2
    mov ax, WORD PTR [bp+0x4]                                        # CS:36C5
    shr ax, 1                                                        # CS:36C8
    mov WORD PTR [bp+0x4], ax                                        # CS:36CA
    jmp L_draw_nested_tunnel_rings_35bf                              # CS:36CD
L_draw_nested_tunnel_rings_36d0:
    pop bp                                                           # CS:36D0
    ret 0x4                                                          # CS:36D1

/* ========================================================================== 
 * draw_tunnel_line  |  CS:36D4
 * Назначение: Целочисленная линия с accumulator; масштабирует y как (10*y+5)/12 и рисует включительно оба конца.
 * Вход: [BP+4]=y2,+6=x2,+8=y1,+10=x1; исходный домен 0..255.
 * Эффект/ограничения: RET8; порог накопителя строго >, не >=. Это влияет на пиксели и должно быть проверено при переносе.
 * Возвраты в физическом диапазоне: ret 0x8.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type draw_tunnel_line, @function
draw_tunnel_line:

    # Целочисленная линия; y сначала масштабируется (10*y+5)/12. Сохранять строгие сравнения аккумулятора.
    push bp                                                          # CS:36D4
    {load} mov bp, sp                                                # CS:36D5
    sub sp, 0xa                                                      # CS:36D7
    mov ax, ds:[D_segment_scratch0]                                  # CS:36DA
    mov es, ax                                                       # CS:36DD
    mov ax, WORD PTR [bp+LINE_Y1]                                    # CS:36DF
    ACC16 cmp, 0x0                                                   # CS:36E2
    je L_draw_tunnel_line_36fa                                       # CS:36E5
    {load} mov bx, ax                                                # CS:36E7
    shl ax, 1                                                        # CS:36E9
    shl ax, 1                                                        # CS:36EB
    {load} add ax, bx                                                # CS:36ED
    shl ax, 1                                                        # CS:36EF
    ACC16 add, 0x5                                                   # CS:36F1
    mov bl, 0xc                                                      # CS:36F4
    div bl                                                           # CS:36F6
    mov ah, 0x0                                                      # CS:36F8
L_draw_tunnel_line_36fa:
    mov WORD PTR [bp+LINE_Y1], ax                                    # CS:36FA
    mov ax, WORD PTR [bp+LINE_Y2]                                    # CS:36FD
    ACC16 cmp, 0x0                                                   # CS:3700
    je L_draw_tunnel_line_3718                                       # CS:3703
    {load} mov bx, ax                                                # CS:3705
    shl ax, 1                                                        # CS:3707
    shl ax, 1                                                        # CS:3709
    {load} add ax, bx                                                # CS:370B
    shl ax, 1                                                        # CS:370D
    ACC16 add, 0x5                                                   # CS:370F
    mov bl, 0xc                                                      # CS:3712
    div bl                                                           # CS:3714
    mov ah, 0x0                                                      # CS:3716
L_draw_tunnel_line_3718:
    mov WORD PTR [bp+LINE_Y2], ax                                    # CS:3718
    mov ax, WORD PTR [bp+LINE_X1]                                    # CS:371B
    mov bx, WORD PTR [bp+LINE_X2]                                    # CS:371E
    {load} cmp bx, ax                                                # CS:3721
    jae L_draw_tunnel_line_3737                                      # CS:3723
    mov WORD PTR [bp+LINE_X1], bx                                    # CS:3725
    mov WORD PTR [bp+LINE_X2], ax                                    # CS:3728
    mov ax, WORD PTR [bp+LINE_Y1]                                    # CS:372B
    mov bx, WORD PTR [bp+LINE_Y2]                                    # CS:372E
    mov WORD PTR [bp+LINE_Y1], bx                                    # CS:3731
    mov WORD PTR [bp+LINE_Y2], ax                                    # CS:3734
L_draw_tunnel_line_3737:
    mov ax, WORD PTR [bp+LINE_X2]                                    # CS:3737
    sub ax, WORD PTR [bp+LINE_X1]                                    # CS:373A
    mov WORD PTR [bp-0x2], ax                                        # CS:373D
    mov ax, WORD PTR [bp+LINE_Y2]                                    # CS:3740
    sub ax, WORD PTR [bp+LINE_Y1]                                    # CS:3743
    jns L_draw_tunnel_line_3751                                      # CS:3746
    neg ax                                                           # CS:3748
    mov WORD PTR [bp-0xa], 0xffe0                                    # CS:374A
    jmp L_draw_tunnel_line_3756                                      # CS:374F
L_draw_tunnel_line_3751:
    mov WORD PTR [bp-0xa], 0x20                                      # CS:3751
L_draw_tunnel_line_3756:
    mov WORD PTR [bp-0x4], ax                                        # CS:3756
    mov cx, WORD PTR [bp-0x2]                                        # CS:3759
    cmp cx, 0x0                                                      # CS:375C
    je L_draw_tunnel_line_37db                                       # CS:375F
    ACC16 cmp, 0x0                                                   # CS:3761
    jne L_draw_tunnel_line_3769                                      # CS:3764
    jmp L_draw_tunnel_line_37ed                                      # CS:3766
L_draw_tunnel_line_3769:
    cmp ax, WORD PTR [bp-0x2]                                        # CS:3769
    jae L_draw_tunnel_line_37a4                                      # CS:376C
    mov ax, WORD PTR [bp-0x2]                                        # CS:376E
    shr ax, 1                                                        # CS:3771
    mov WORD PTR [bp-0x6], ax                                        # CS:3773
    call plot_tunnel_line_start                                      # CS:3776
    mov cx, WORD PTR [bp-0x2]                                        # CS:3779
L_draw_tunnel_line_377c:
    shr al, 1                                                        # CS:377C
    jne L_draw_tunnel_line_3783                                      # CS:377E
    mov al, 0x80                                                     # CS:3780
    inc di                                                           # CS:3782
L_draw_tunnel_line_3783:
    mov bx, WORD PTR [bp-0x6]                                        # CS:3783
    add bx, WORD PTR [bp-0x4]                                        # CS:3786
    mov WORD PTR [bp-0x6], bx                                        # CS:3789
    cmp bx, WORD PTR [bp-0x2]                                        # CS:378C
    jbe L_draw_tunnel_line_379d                                      # CS:378F
    sub bx, WORD PTR [bp-0x2]                                        # CS:3791
    mov WORD PTR [bp-0x6], bx                                        # CS:3794
    mov bx, WORD PTR [bp-0xa]                                        # CS:3797
    add di, WORD PTR [bp-0xa]                                        # CS:379A
L_draw_tunnel_line_379d:
    or BYTE PTR es:[di], al                                          # CS:379D
    loop L_draw_tunnel_line_377c                                     # CS:37A0
    jmp L_draw_tunnel_line_37d5                                      # CS:37A2
L_draw_tunnel_line_37a4:
    mov ax, WORD PTR [bp-0x4]                                        # CS:37A4
    shr ax, 1                                                        # CS:37A7
    mov WORD PTR [bp-0x6], ax                                        # CS:37A9
    call plot_tunnel_line_start                                      # CS:37AC
    mov cx, WORD PTR [bp-0x4]                                        # CS:37AF
L_draw_tunnel_line_37b2:
    add di, WORD PTR [bp-0xa]                                        # CS:37B2
    mov bx, WORD PTR [bp-0x6]                                        # CS:37B5
    add bx, WORD PTR [bp-0x2]                                        # CS:37B8
    mov WORD PTR [bp-0x6], bx                                        # CS:37BB
    cmp bx, WORD PTR [bp-0x4]                                        # CS:37BE
    jbe L_draw_tunnel_line_37d0                                      # CS:37C1
    sub bx, WORD PTR [bp-0x4]                                        # CS:37C3
    mov WORD PTR [bp-0x6], bx                                        # CS:37C6
    shr al, 1                                                        # CS:37C9
    jne L_draw_tunnel_line_37d0                                      # CS:37CB
    mov al, 0x80                                                     # CS:37CD
    inc di                                                           # CS:37CF
L_draw_tunnel_line_37d0:
    or BYTE PTR es:[di], al                                          # CS:37D0
    loop L_draw_tunnel_line_37b2                                     # CS:37D3
L_draw_tunnel_line_37d5:
    {load} mov sp, bp                                                # CS:37D5
    pop bp                                                           # CS:37D7
    ret 0x8                                                          # CS:37D8
L_draw_tunnel_line_37db:
    call plot_tunnel_line_start                                      # CS:37DB
    mov cx, WORD PTR [bp-0x4]                                        # CS:37DE
    jcxz L_draw_tunnel_line_37d5                                     # CS:37E1
L_draw_tunnel_line_37e3:
    add di, WORD PTR [bp-0xa]                                        # CS:37E3
    or BYTE PTR es:[di], al                                          # CS:37E6
    loop L_draw_tunnel_line_37e3                                     # CS:37E9
    jmp L_draw_tunnel_line_37d5                                      # CS:37EB
L_draw_tunnel_line_37ed:
    call plot_tunnel_line_start                                      # CS:37ED
    mov cx, WORD PTR [bp-0x2]                                        # CS:37F0
L_draw_tunnel_line_37f3:
    shr al, 1                                                        # CS:37F3
    jne L_draw_tunnel_line_37fa                                      # CS:37F5
    mov al, 0x80                                                     # CS:37F7
    inc di                                                           # CS:37F9
L_draw_tunnel_line_37fa:
    or BYTE PTR es:[di], al                                          # CS:37FA
    loop L_draw_tunnel_line_37f3                                     # CS:37FD
    jmp L_draw_tunnel_line_37d5                                      # CS:37FF

/* ========================================================================== 
 * plot_tunnel_line_start  |  CS:3801
 * Назначение: Вычисляет offset=y1*32+x1/8, mask=80h>>(x1&7), рисует первый пиксель.
 * Вход: НАСЛЕДУЕТ BP draw_tunnel_line; ES=bitmap. Собственного пролога нет.
 * Эффект/ограничения: DI=offset, AL=mask; не трактовать [BP+8/+10] как аргументы собственного CALL.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type plot_tunnel_line_start, @function
plot_tunnel_line_start:

    # ДОПОЛНИТЕЛЬНЫЙ ВХОД: использует BP вызывающего rasterize_tunnel_line, своего кадра не создаёт.
    mov ax, WORD PTR [bp+LINE_Y1]                                    # CS:3801
    shl ax, 1                                                        # CS:3804
    shl ax, 1                                                        # CS:3806
    shl ax, 1                                                        # CS:3808
    shl ax, 1                                                        # CS:380A
    shl ax, 1                                                        # CS:380C
    {load} mov di, ax                                                # CS:380E
    mov ax, WORD PTR [bp+LINE_X1]                                    # CS:3810
    shr ax, 1                                                        # CS:3813
    shr ax, 1                                                        # CS:3815
    shr ax, 1                                                        # CS:3817
    {load} add di, ax                                                # CS:3819
    mov cx, WORD PTR [bp+LINE_X1]                                    # CS:381B
    and cx, 0x7                                                      # CS:381E
    mov al, 0x80                                                     # CS:3821
    shr al, cl                                                       # CS:3823
    or BYTE PTR es:[di], al                                          # CS:3825
    ret                                                              # CS:3828

/* CS:3829: Inter-procedure alignment byte, not an instruction. */
padding_3829:
    .byte 0x00
