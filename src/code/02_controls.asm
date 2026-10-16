/* Управление и переназначение клавиш
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * poll_controls  |  CS:01C0
 * Назначение: Собирает маску управления, обрабатывает паузу, переназначение, выход и переключение звука, затем мышь.
 * Вход: Таблица клавиш и состояние мыши.
 * Эффект/ограничения: control_bits; LEFT имеет приоритет над RIGHT, UP над DOWN. Движение мыши учитывается только без клавиатурного направления.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type poll_controls, @function
poll_controls:

    # Сначала горячие клавиши; затем игровая маска. Не заменять блокирующие ветви одним poll.
    mov bx, 0x8e8a                                                   # CS:01C0
    mov si, 0x3d                                                     # CS:01C3
    mov al, BYTE PTR [bx+si]                                         # CS:01C6
    cmp al, 0x0                                                      # CS:01C8
    je L_poll_controls_01d5                                          # CS:01CA
    mov si, 0x3e                                                     # CS:01CC
L_poll_controls_01cf:
    mov al, BYTE PTR [bx+si]                                         # CS:01CF
    cmp al, 0x0                                                      # CS:01D1
    je L_poll_controls_01cf                                          # CS:01D3
L_poll_controls_01d5:
    mov si, 0x3b                                                     # CS:01D5
    mov al, BYTE PTR [bx+si]                                         # CS:01D8
    cmp al, 0x0                                                      # CS:01DA
    je L_poll_controls_01e1                                          # CS:01DC
    call redefine_keys                                               # CS:01DE
L_poll_controls_01e1:
    mov bx, 0x8e8a                                                   # CS:01E1
    mov si, WORD PTR ds:[D_key_left]                                 # CS:01E4
    mov al, BYTE PTR [bx+si]                                         # CS:01E8
    cmp al, 0x0                                                      # CS:01EA
    je L_poll_controls_01f6                                          # CS:01EC
    mov WORD PTR ds:[D_control_bits], 0x1                            # CS:01EE
    jmp L_poll_controls_020e                                         # CS:01F4
L_poll_controls_01f6:
    mov si, WORD PTR ds:[D_key_right]                                # CS:01F6
    mov al, BYTE PTR [bx+si]                                         # CS:01FA
    cmp al, 0x0                                                      # CS:01FC
    je L_poll_controls_0208                                          # CS:01FE
    mov WORD PTR ds:[D_control_bits], 0x2                            # CS:0200
    jmp L_poll_controls_020e                                         # CS:0206
L_poll_controls_0208:
    mov WORD PTR ds:[D_control_bits], 0x0                            # CS:0208
L_poll_controls_020e:
    mov si, WORD PTR ds:[D_key_up]                                   # CS:020E
    mov al, BYTE PTR [bx+si]                                         # CS:0212
    cmp al, 0x0                                                      # CS:0214
    je L_poll_controls_021f                                          # CS:0216
    or WORD PTR ds:[D_control_bits], 0x4                             # CS:0218
    jmp L_poll_controls_022e                                         # CS:021D
L_poll_controls_021f:
    mov si, WORD PTR ds:[D_key_down]                                 # CS:021F
    mov al, BYTE PTR [bx+si]                                         # CS:0223
    cmp al, 0x0                                                      # CS:0225
    je L_poll_controls_022e                                          # CS:0227
    or WORD PTR ds:[D_control_bits], 0x8                             # CS:0229
L_poll_controls_022e:
    mov si, WORD PTR ds:[D_key_fire]                                 # CS:022E
    mov al, BYTE PTR [bx+si]                                         # CS:0232
    cmp al, 0x0                                                      # CS:0234
    je L_poll_controls_023e                                          # CS:0236
    or WORD PTR ds:[D_control_bits], 0x80                            # CS:0238
L_poll_controls_023e:
    mov si, WORD PTR ds:[D_key_action_n]                             # CS:023E
    mov al, BYTE PTR [bx+si]                                         # CS:0242
    cmp al, 0x0                                                      # CS:0244
    je L_poll_controls_024d                                          # CS:0246
    or WORD PTR ds:[D_control_bits], 0x40                            # CS:0248
L_poll_controls_024d:
    mov si, WORD PTR ds:[D_key_action_m]                             # CS:024D
    mov al, BYTE PTR [bx+si]                                         # CS:0251
    cmp al, 0x0                                                      # CS:0253
    je L_poll_controls_025c                                          # CS:0255
    or WORD PTR ds:[D_control_bits], 0x20                            # CS:0257
L_poll_controls_025c:
    mov si, WORD PTR ds:[D_key_quit]                                 # CS:025C
    mov al, BYTE PTR [bx+si]                                         # CS:0260
    cmp al, 0x0                                                      # CS:0262
    je L_poll_controls_028c                                          # CS:0264
    call draw_quit_dialog                                            # CS:0266
    mov bx, 0x8e8a                                                   # CS:0269
L_poll_controls_026c:
    mov al, BYTE PTR [bx+0x15]                                       # CS:026C
    cmp al, 0x0                                                      # CS:026F
    jne L_poll_controls_027c                                         # CS:0271
    mov al, BYTE PTR [bx+0x31]                                       # CS:0273
    cmp al, 0x0                                                      # CS:0276
    jne L_poll_controls_027c                                         # CS:0278
    jmp L_poll_controls_026c                                         # CS:027A
L_poll_controls_027c:
    mov bx, 0x8e8a                                                   # CS:027C
    mov al, BYTE PTR [bx+0x15]                                       # CS:027F
    cmp al, 0x0                                                      # CS:0282
    je L_poll_controls_0289                                          # CS:0284
    jmp exit_to_dos                                                  # CS:0286
L_poll_controls_0289:
    call wait_keys_released                                          # CS:0289
L_poll_controls_028c:
    mov bx, 0x8e8a                                                   # CS:028C
    mov si, 0x3c                                                     # CS:028F
    mov al, BYTE PTR [bx+si]                                         # CS:0292
    cmp al, 0x0                                                      # CS:0294
    je L_poll_controls_02c5                                          # CS:0296
    mov al, ds:[D_sound_toggle_state]                                # CS:0298
    cmp al, 0x0                                                      # CS:029B
    jne L_poll_controls_02b0                                         # CS:029D
    mov al, 0xe                                                      # CS:029F
    call sound_trigger                                               # CS:02A1
    mov BYTE PTR ds:[D_sound_toggle_state], 0x1                      # CS:02A4
    mov BYTE PTR ds:[D_sound_enabled], 0x0                           # CS:02A9
    jmp L_poll_controls_02c3                                         # CS:02AE
L_poll_controls_02b0:
    cmp al, 0x2                                                      # CS:02B0
    jne L_poll_controls_02c3                                         # CS:02B2
    mov BYTE PTR ds:[D_sound_toggle_state], 0x3                      # CS:02B4
    mov BYTE PTR ds:[D_sound_enabled], 0x1                           # CS:02B9
    mov al, 0xd                                                      # CS:02BE
    call sound_trigger                                               # CS:02C0
L_poll_controls_02c3:
    jmp L_poll_controls_02dc                                         # CS:02C3
L_poll_controls_02c5:
    mov al, ds:[D_sound_toggle_state]                                # CS:02C5
    cmp al, 0x1                                                      # CS:02C8
    jne L_poll_controls_02d3                                         # CS:02CA
    mov BYTE PTR ds:[D_sound_toggle_state], 0x2                      # CS:02CC
    jmp L_poll_controls_02dc                                         # CS:02D1
L_poll_controls_02d3:
    cmp al, 0x3                                                      # CS:02D3
    jne L_poll_controls_02dc                                         # CS:02D5
    mov BYTE PTR ds:[D_sound_toggle_state], 0x0                      # CS:02D7
L_poll_controls_02dc:
    mov al, ds:[D_mouse_present]                                     # CS:02DC
    cmp al, 0x0                                                      # CS:02DF
    jne L_poll_controls_02e4                                         # CS:02E1
    ret                                                              # CS:02E3
L_poll_controls_02e4:
    mov ax, 0x3                                                      # CS:02E4
    int 0x33                                                         # CS:02E7
    test bx, 0x1                                                     # CS:02E9
    je L_poll_controls_02f5                                          # CS:02ED
    or WORD PTR ds:[D_control_bits], 0x80                            # CS:02EF
L_poll_controls_02f5:
    test bx, 0x2                                                     # CS:02F5
    je L_poll_controls_0300                                          # CS:02F9
    or WORD PTR ds:[D_control_bits], 0x20                            # CS:02FB
L_poll_controls_0300:
    mov ax, ds:[D_control_bits]                                      # CS:0300
    ACC16 and, 0xf                                                   # CS:0303
    je L_poll_controls_0309                                          # CS:0306
    ret                                                              # CS:0308
L_poll_controls_0309:
    mov ax, 0xb                                                      # CS:0309
    int 0x33                                                         # CS:030C
    cmp cx, 0xfffb                                                   # CS:030E
    jge L_poll_controls_031a                                         # CS:0311
    or WORD PTR ds:[D_control_bits], 0x1                             # CS:0313
    jmp L_poll_controls_0324                                         # CS:0318
L_poll_controls_031a:
    cmp cx, 0x5                                                      # CS:031A
    jle L_poll_controls_0324                                         # CS:031D
    or WORD PTR ds:[D_control_bits], 0x2                             # CS:031F
L_poll_controls_0324:
    cmp dx, 0xfff6                                                   # CS:0324
    jge L_poll_controls_0330                                         # CS:0327
    or WORD PTR ds:[D_control_bits], 0x4                             # CS:0329
    jmp L_poll_controls_033a                                         # CS:032E
L_poll_controls_0330:
    cmp dx, 0xa                                                      # CS:0330
    jle L_poll_controls_033a                                         # CS:0333
    or WORD PTR ds:[D_control_bits], 0x8                             # CS:0335
L_poll_controls_033a:
    ret                                                              # CS:033A

/* ========================================================================== 
 * redefine_keys  |  CS:033B
 * Назначение: Запрашивает 8 различных клавиш в порядке FIRE, LEFT, RIGHT, UP, DOWN, N, M, QUIT.
 * Вход: Блокирующий интерфейс, DS = сегмент игры.
 * Эффект/ограничения: Записывает scan-code в таблицу настроек; дубли отклоняются.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type redefine_keys, @function
redefine_keys:
    call draw_redefine_dialog                                        # CS:033B
    mov al, 0x4                                                      # CS:033E
    call draw_control_prompt                                         # CS:0340
    call wait_scancode                                               # CS:0343
    mov ds:[D_key_fire], ax                                          # CS:0346
    mov al, 0x0                                                      # CS:0349
    call draw_control_prompt                                         # CS:034B
L_redefine_keys_034e:
    call wait_scancode                                               # CS:034E
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:0351
    je L_redefine_keys_034e                                          # CS:0355
    mov ds:[D_key_left], ax                                          # CS:0357
    mov al, 0x1                                                      # CS:035A
    call draw_control_prompt                                         # CS:035C
L_redefine_keys_035f:
    call wait_scancode                                               # CS:035F
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:0362
    je L_redefine_keys_035f                                          # CS:0366
    cmp ax, WORD PTR ds:[D_key_left]                                 # CS:0368
    je L_redefine_keys_035f                                          # CS:036C
    mov ds:[D_key_right], ax                                         # CS:036E
    mov al, 0x2                                                      # CS:0371
    call draw_control_prompt                                         # CS:0373
L_redefine_keys_0376:
    call wait_scancode                                               # CS:0376
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:0379
    je L_redefine_keys_0376                                          # CS:037D
    cmp ax, WORD PTR ds:[D_key_left]                                 # CS:037F
    je L_redefine_keys_0376                                          # CS:0383
    cmp ax, WORD PTR ds:[D_key_right]                                # CS:0385
    je L_redefine_keys_0376                                          # CS:0389
    mov ds:[D_key_up], ax                                            # CS:038B
    mov al, 0x3                                                      # CS:038E
    call draw_control_prompt                                         # CS:0390
L_redefine_keys_0393:
    call wait_scancode                                               # CS:0393
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:0396
    je L_redefine_keys_0393                                          # CS:039A
    cmp ax, WORD PTR ds:[D_key_left]                                 # CS:039C
    je L_redefine_keys_0393                                          # CS:03A0
    cmp ax, WORD PTR ds:[D_key_right]                                # CS:03A2
    je L_redefine_keys_0393                                          # CS:03A6
    cmp ax, WORD PTR ds:[D_key_up]                                   # CS:03A8
    je L_redefine_keys_0393                                          # CS:03AC
    mov ds:[D_key_down], ax                                          # CS:03AE
    mov al, 0x6                                                      # CS:03B1
    call draw_control_prompt                                         # CS:03B3
L_redefine_keys_03b6:
    call wait_scancode                                               # CS:03B6
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:03B9
    je L_redefine_keys_03b6                                          # CS:03BD
    cmp ax, WORD PTR ds:[D_key_left]                                 # CS:03BF
    je L_redefine_keys_03b6                                          # CS:03C3
    cmp ax, WORD PTR ds:[D_key_right]                                # CS:03C5
    je L_redefine_keys_03b6                                          # CS:03C9
    cmp ax, WORD PTR ds:[D_key_up]                                   # CS:03CB
    je L_redefine_keys_03b6                                          # CS:03CF
    cmp ax, WORD PTR ds:[D_key_down]                                 # CS:03D1
    je L_redefine_keys_03b6                                          # CS:03D5
    mov ds:[D_key_action_n], ax                                      # CS:03D7
    mov al, 0x7                                                      # CS:03DA
    call draw_control_prompt                                         # CS:03DC
L_redefine_keys_03df:
    call wait_scancode                                               # CS:03DF
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:03E2
    je L_redefine_keys_03df                                          # CS:03E6
    cmp ax, WORD PTR ds:[D_key_left]                                 # CS:03E8
    je L_redefine_keys_03df                                          # CS:03EC
    cmp ax, WORD PTR ds:[D_key_right]                                # CS:03EE
    je L_redefine_keys_03df                                          # CS:03F2
    cmp ax, WORD PTR ds:[D_key_up]                                   # CS:03F4
    je L_redefine_keys_03df                                          # CS:03F8
    cmp ax, WORD PTR ds:[D_key_down]                                 # CS:03FA
    je L_redefine_keys_03df                                          # CS:03FE
    cmp ax, WORD PTR ds:[D_key_action_n]                             # CS:0400
    je L_redefine_keys_03df                                          # CS:0404
    mov ds:[D_key_action_m], ax                                      # CS:0406
    mov al, 0x5                                                      # CS:0409
    call draw_control_prompt                                         # CS:040B
L_redefine_keys_040e:
    call wait_scancode                                               # CS:040E
    cmp ax, WORD PTR ds:[D_key_fire]                                 # CS:0411
    je L_redefine_keys_040e                                          # CS:0415
    cmp ax, WORD PTR ds:[D_key_left]                                 # CS:0417
    je L_redefine_keys_040e                                          # CS:041B
    cmp ax, WORD PTR ds:[D_key_right]                                # CS:041D
    je L_redefine_keys_040e                                          # CS:0421
    cmp ax, WORD PTR ds:[D_key_up]                                   # CS:0423
    je L_redefine_keys_040e                                          # CS:0427
    cmp ax, WORD PTR ds:[D_key_down]                                 # CS:0429
    je L_redefine_keys_040e                                          # CS:042D
    cmp ax, WORD PTR ds:[D_key_action_n]                             # CS:042F
    je L_redefine_keys_040e                                          # CS:0433
    cmp ax, WORD PTR ds:[D_key_action_m]                             # CS:0435
    je L_redefine_keys_040e                                          # CS:0439
    mov ds:[D_key_quit], ax                                          # CS:043B
    call wait_keys_released                                          # CS:043E
    ret                                                              # CS:0441
