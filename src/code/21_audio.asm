/* PC speaker, приоритеты эффектов и музыка
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * sound_init  |  CS:382A
 * Назначение: Очищает активный/два ожидающих звука, включает звук и сбрасывает toggle latch.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Не инициализирует всю память audio, а только выбранные поля.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sound_init, @function
sound_init:

    # PC speaker/PIT2 изолированы здесь. Переносимый автомат звука должен отделяться от host audio API.
    mov al, 0x0                                                      # CS:382A
    mov ds:[D_sound_active_id], al                                   # CS:382C
    mov ds:[D_sound_pending_id], al                                  # CS:382F
    mov ds:[D_sound_priority_id], al                                 # CS:3832
    mov BYTE PTR ds:[D_sound_enabled], 0x1                           # CS:3835
    mov BYTE PTR ds:[D_sound_toggle_state], 0x0                      # CS:383A
    ret                                                              # CS:383F

/* ========================================================================== 
 * sound_trigger  |  CS:3840
 * Назначение: Запрашивает эффект: стартует сразу либо помещает в двухступенчатую очередь; последняя ячейка хранит максимум ID.
 * Вход: AL = sound ID.
 * Эффект/ограничения: Во время music_active запрос игнорируется. Это не FIFO произвольной длины.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sound_trigger, @function
sound_trigger:
    mov ah, BYTE PTR ds:[D_music_active]                             # CS:3840
    cmp ah, 0x0                                                      # CS:3844
    je L_sound_trigger_384a                                          # CS:3847
    ret                                                              # CS:3849
L_sound_trigger_384a:
    mov ah, BYTE PTR ds:[D_sound_enabled]                            # CS:384A
    cmp ah, 0x0                                                      # CS:384E
    jne L_sound_trigger_3854                                         # CS:3851
    ret                                                              # CS:3853
L_sound_trigger_3854:
    mov ah, BYTE PTR ds:[D_sound_active_id]                          # CS:3854
    cmp ah, 0x0                                                      # CS:3858
    jne L_sound_trigger_3866                                         # CS:385B
    push bx                                                          # CS:385D
    mov ds:[D_sound_active_id], al                                   # CS:385E
    call start_sound_descriptor                                      # CS:3861
    pop bx                                                           # CS:3864
    ret                                                              # CS:3865
L_sound_trigger_3866:
    mov ah, BYTE PTR ds:[D_sound_pending_id]                         # CS:3866
    cmp ah, 0x0                                                      # CS:386A
    jne L_sound_trigger_3873                                         # CS:386D
    mov ds:[D_sound_pending_id], al                                  # CS:386F
    ret                                                              # CS:3872
L_sound_trigger_3873:
    cmp al, BYTE PTR ds:[D_sound_priority_id]                        # CS:3873
    jbe L_sound_trigger_387c                                         # CS:3877
    mov ds:[D_sound_priority_id], al                                 # CS:3879
L_sound_trigger_387c:
    ret                                                              # CS:387C

/* ========================================================================== 
 * sound_tick  |  CS:387D
 * Назначение: Шаг музыки/эффекта: время, очередь, изменение делителя, модуляция и запись PIT2.
 * Вход: Состояние audio; вызывается из INT1Ch.
 * Эффект/ограничения: Счётчики 8-битные, делители16-битные; IRQ-контекст отделить от simulation.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sound_tick, @function
sound_tick:

    # Звук обслуживается по IRQ-тикам. Обычная очередь эффектов и музыка имеют разные приоритеты.
    mov al, ds:[D_music_active]                                      # CS:387D
    cmp al, 0x0                                                      # CS:3880
    je L_sound_tick_389b                                             # CS:3882
    mov al, ds:[D_sound_enabled]                                     # CS:3884
    cmp al, 0x0                                                      # CS:3887
    je L_sound_tick_3890                                             # CS:3889
    call music_tick                                                  # CS:388B
    jmp L_sound_tick_389b                                            # CS:388E
L_sound_tick_3890:
    mov WORD PTR ds:[D_music_cursor], 0x0                            # CS:3890
    mov BYTE PTR ds:[D_music_active], 0x0                            # CS:3896
L_sound_tick_389b:
    mov al, ds:[D_sound_active_id]                                   # CS:389B
    cmp al, 0x0                                                      # CS:389E
    jne L_sound_tick_38a3                                            # CS:38A0
    ret                                                              # CS:38A2
L_sound_tick_38a3:
    dec BYTE PTR ds:[D_sound_ticks_left]                             # CS:38A3
    jne L_sound_tick_38d4                                            # CS:38A7
    mov al, ds:[D_sound_enabled]                                     # CS:38A9
    cmp al, 0x0                                                      # CS:38AC
    jne L_sound_tick_38b4                                            # CS:38AE
    call sound_stop                                                  # CS:38B0
    ret                                                              # CS:38B3
L_sound_tick_38b4:
    mov al, ds:[D_sound_pending_id]                                  # CS:38B4
    cmp al, 0x0                                                      # CS:38B7
    je L_sound_tick_38d0                                             # CS:38B9
    mov ds:[D_sound_active_id], al                                   # CS:38BB
    mov bl, BYTE PTR ds:[D_sound_priority_id]                        # CS:38BE
    mov BYTE PTR ds:[D_sound_pending_id], bl                         # CS:38C2
    mov BYTE PTR ds:[D_sound_priority_id], 0x0                       # CS:38C6
    call start_sound_descriptor                                      # CS:38CB
    jmp L_sound_tick_38d3                                            # CS:38CE
L_sound_tick_38d0:
    call sound_stop                                                  # CS:38D0
L_sound_tick_38d3:
    ret                                                              # CS:38D3
L_sound_tick_38d4:
    mov ax, ds:[D_sound_divisor]                                     # CS:38D4
    add ax, WORD PTR ds:[D_sound_divisor_step]                       # CS:38D7
    mov ds:[D_sound_divisor], ax                                     # CS:38DB
    mov al, ds:[D_sound_modulation_id]                               # CS:38DE
    cmp al, 0x0                                                      # CS:38E1
    je L_sound_tick_38fd                                             # CS:38E3
    mov bl, BYTE PTR ds:[D_sound_modulation_phase]                   # CS:38E5
    inc BYTE PTR ds:[D_sound_modulation_phase]                       # CS:38E9
    and bx, 0xf                                                      # CS:38ED
    shl bx, 1                                                        # CS:38F0
    add bx, WORD PTR ds:[D_sound_modulation_pointer]                 # CS:38F2
    mov ax, WORD PTR [bx]                                            # CS:38F6
    mov ds:[D_sound_output_divisor], ax                              # CS:38F8
    jmp L_sound_tick_3903                                            # CS:38FB
L_sound_tick_38fd:
    mov WORD PTR ds:[D_sound_output_divisor], 0x0                    # CS:38FD
L_sound_tick_3903:
    mov ax, ds:[D_sound_divisor]                                     # CS:3903
    add WORD PTR ds:[D_sound_output_divisor], ax                     # CS:3906
    mov ax, ds:[D_sound_output_divisor]                              # CS:390A
    out 0x42, al                                                     # CS:390D
    {load} mov al, ah                                                # CS:390F
    out 0x42, al                                                     # CS:3911
    ret                                                              # CS:3913

/* ========================================================================== 
 * start_sound_descriptor  |  CS:3914
 * Назначение: Загружает 6-байтовый descriptor, подставляет динамические параметры двух эффектов и включает PIT2/speaker.
 * Вход: AL = sound ID1..15.
 * Эффект/ограничения: Один вариант pitch зависит от зарядов, другой от угла игрока; таблицы могут изменяться.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type start_sound_descriptor, @function
start_sound_descriptor:

    # 6-байтовый sound descriptor: modulation, duration, divisor, divisor_step.
    dec al                                                           # CS:3914
    {load} mov bl, al                                                # CS:3916
    mov bh, 0x0                                                      # CS:3918
    shl bx, 1                                                        # CS:391A
    mov bx, WORD PTR [bx+D_sound_descriptor_pointers]                # CS:391C
    mov al, BYTE PTR [bx+0x1]                                        # CS:3920
    mov ds:[D_sound_ticks_left], al                                  # CS:3923
    cmp bx, 0x8c2c                                                   # CS:3926
    jne L_start_sound_descriptor_3941                                # CS:392A
    mov ax, ds:[D_side_charge_units]                                 # CS:392C
    {load} mov ah, al                                                # CS:392F
    mov al, 0x0                                                      # CS:3931
    shl ax, 1                                                        # CS:3933
    shl ax, 1                                                        # CS:3935
    neg ax                                                           # CS:3937
    ACC16 add, 0x1f40                                                # CS:3939
    mov ds:[D_charge_sound_divisor], ax                              # CS:393C
    jmp L_start_sound_descriptor_3966                                # CS:393F
L_start_sound_descriptor_3941:
    cmp bx, 0x8c32                                                   # CS:3941
    jne L_start_sound_descriptor_3966                                # CS:3945
    mov al, ds:[D_player_angle_or_age]                               # CS:3947
    ACC16 and, 0x7f                                                  # CS:394A
    ACC16 cmp, 0x3f                                                  # CS:394D
    jbe L_start_sound_descriptor_3957                                # CS:3950
    ACC16 sub, 0x40                                                  # CS:3952
    jmp L_start_sound_descriptor_395c                                # CS:3955
L_start_sound_descriptor_3957:
    neg ax                                                           # CS:3957
    ACC16 add, 0x3f                                                  # CS:3959
L_start_sound_descriptor_395c:
    shl ax, 1                                                        # CS:395C
    shl ax, 1                                                        # CS:395E
    ACC16 add, 0x190                                                 # CS:3960
    mov ds:[D_angle_sound_divisor], ax                               # CS:3963
L_start_sound_descriptor_3966:
    mov ax, WORD PTR [bx+0x2]                                        # CS:3966
    mov ds:[D_sound_divisor], ax                                     # CS:3969
    mov ax, WORD PTR [bx+0x4]                                        # CS:396C
    mov ds:[D_sound_divisor_step], ax                                # CS:396F
    mov bl, BYTE PTR [bx]                                            # CS:3972
    mov BYTE PTR ds:[D_sound_modulation_id], bl                      # CS:3974
    cmp bl, 0x0                                                      # CS:3978
    jle L_start_sound_descriptor_398f                                # CS:397B
    dec bl                                                           # CS:397D
    mov bh, 0x0                                                      # CS:397F
    shl bx, 1                                                        # CS:3981
    mov ax, WORD PTR [bx+D_sound_modulation_pointers]                # CS:3983
    mov ds:[D_sound_modulation_pointer], ax                          # CS:3987
    mov BYTE PTR ds:[D_sound_modulation_phase], 0x0                  # CS:398A
L_start_sound_descriptor_398f:
    mov al, 0xb6                                                     # CS:398F
    out 0x43, al                                                     # CS:3991
    mov ax, ds:[D_sound_divisor]                                     # CS:3993
    out 0x42, al                                                     # CS:3996
    {load} mov al, ah                                                # CS:3998
    out 0x42, al                                                     # CS:399A
    in al, 0x61                                                      # CS:399C
    or al, 0x3                                                       # CS:399E
    out 0x61, al                                                     # CS:39A0
    ret                                                              # CS:39A2

/* ========================================================================== 
 * sound_stop  |  CS:39A3
 * Назначение: Очищает очередь/время эффекта и отключает speaker gate bits0/1.
 * Вход: Нет аргументов.
 * Эффект/ограничения: Не очищает все музыкальные указатели.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type sound_stop, @function
sound_stop:
    mov al, 0x0                                                      # CS:39A3
    mov ds:[D_sound_active_id], al                                   # CS:39A5
    mov ds:[D_sound_pending_id], al                                  # CS:39A8
    mov ds:[D_sound_priority_id], al                                 # CS:39AB
    mov ds:[D_sound_ticks_left], al                                  # CS:39AE
    in al, 0x61                                                      # CS:39B1
    and al, 0xfc                                                     # CS:39B3
    out 0x61, al                                                     # CS:39B5
    ret                                                              # CS:39B7

/* ========================================================================== 
 * music_start  |  CS:39B8
 * Назначение: Запускает одну из двух последовательностей, выбирая её по pending_name_entry.
 * Вход: Нет аргументов.
 * Эффект/ограничения: sequence cursor=0, music divider=0, active=1.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type music_start, @function
music_start:
    mov BYTE PTR ds:[D_music_tick_divider], 0x0                      # CS:39B8
    mov WORD PTR ds:[D_music_cursor], 0x0                            # CS:39BD
    mov BYTE PTR ds:[D_music_active], 0x1                            # CS:39C3
    mov al, ds:[D_pending_name_entry]                                # CS:39C8
    cmp al, 0x0                                                      # CS:39CB
    jne L_music_start_39d4                                           # CS:39CD
    mov ax, 0x8dec                                                   # CS:39CF
    jmp L_music_start_39d7                                           # CS:39D2
L_music_start_39d4:
    mov ax, 0x8d4c                                                   # CS:39D4
L_music_start_39d7:
    mov ds:[D_music_sequence_pointer], ax                            # CS:39D7
    ret                                                              # CS:39DA

/* ========================================================================== 
 * music_tick  |  CS:39DB
 * Назначение: Каждые 8 звуковых тиков читает команду: FF конец, FE пауза, 0 специальное заполнение, прочее транспонируется.
 * Вход: music sequence/cursor, sound state.
 * Эффект/ограничения: Пустая команда может взять note из отдельной таблицы; не простая нулевая пауза.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type music_tick, @function
music_tick:

    # Музыкальная последовательность исполняется раз в8 timer IRQ; не раз в8 кадров рендера.
    dec BYTE PTR ds:[D_music_tick_divider]                           # CS:39DB
    js L_music_tick_39e2                                             # CS:39DF
    ret                                                              # CS:39E1
L_music_tick_39e2:
    mov BYTE PTR ds:[D_music_tick_divider], 0x7                      # CS:39E2
    mov BYTE PTR ds:[D_music_descriptor_duration], 0xe               # CS:39E7
    mov BYTE PTR ds:[D_music_descriptor_modulation], 0x4             # CS:39EC
    mov bx, WORD PTR ds:[D_music_cursor]                             # CS:39F1
    mov si, WORD PTR ds:[D_music_sequence_pointer]                   # CS:39F5
    mov al, BYTE PTR [bx+si]                                         # CS:39F9
    cmp al, 0xff                                                     # CS:39FB
    jne L_music_tick_3a05                                            # CS:39FD
    mov BYTE PTR ds:[D_music_active], 0x0                            # CS:39FF
    ret                                                              # CS:3A04
L_music_tick_3a05:
    cmp al, 0x0                                                      # CS:3A05
    jne L_music_tick_3a29                                            # CS:3A07
    mov al, ds:[D_sound_active_id]                                   # CS:3A09
    cmp al, 0x0                                                      # CS:3A0C
    jne L_music_tick_3a25                                            # CS:3A0E
    mov BYTE PTR ds:[D_music_descriptor_modulation], 0x0             # CS:3A10
    mov BYTE PTR ds:[D_music_descriptor_duration], 0x6               # CS:3A15
    and bx, 0x1f                                                     # CS:3A1A
    mov al, BYTE PTR [bx+D_music_fill_notes]                         # CS:3A1D
    sub al, 0xc                                                      # CS:3A21
    jmp L_music_tick_3a27                                            # CS:3A23
L_music_tick_3a25:
    mov al, 0x0                                                      # CS:3A25
L_music_tick_3a27:
    jmp L_music_tick_3a33                                            # CS:3A27
L_music_tick_3a29:
    cmp al, 0xfe                                                     # CS:3A29
    jne L_music_tick_3a31                                            # CS:3A2B
    mov al, 0x0                                                      # CS:3A2D
    jmp L_music_tick_3a33                                            # CS:3A2F
L_music_tick_3a31:
    add al, 0xc                                                      # CS:3A31
L_music_tick_3a33:
    call music_play_note                                             # CS:3A33
    inc WORD PTR ds:[D_music_cursor]                                 # CS:3A36
    ret                                                              # CS:3A3A

/* ========================================================================== 
 * music_play_note  |  CS:3A3B
 * Назначение: Берёт PIT-делитель по note-1, сохраняет его в descriptor15 и стартует этот эффект.
 * Вход: AL=номер ноты; ноль возвращает без старта.
 * Эффект/ограничения: Именно конец этой процедуры физически перекрывает DS:0000..0005.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type music_play_note, @function
music_play_note:
    dec al                                                           # CS:3A3B
    jns L_music_play_note_3a40                                       # CS:3A3D
    ret                                                              # CS:3A3F
L_music_play_note_3a40:
    {load} mov bl, al                                                # CS:3A40
    mov bh, 0x0                                                      # CS:3A42
    shl bx, 1                                                        # CS:3A44
    mov ax, WORD PTR [bx+D_note_divisors]                            # CS:3A46
    mov ds:[D_music_descriptor_divisor], ax                          # CS:3A4A
    mov al, 0xf                                                      # CS:3A4D
    mov ds:[D_sound_active_id], al                                   # CS:3A4F
    call start_sound_descriptor                                      # CS:3A52
    ret                                                              # CS:3A55
