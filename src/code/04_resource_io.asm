/* Загрузка ресурсов и файловый I/O
 * Физический порядок EXE сохранён. Контракты получены анализом, не авторскими символами.
 * docs/FUNCTIONS_RU.md — индекс; docs/PORTING_TO_C_RU.md — ограничения переноса. */


/* ========================================================================== 
 * load_assets  |  CS:0600
 * Назначение: Загружает BIRD1, BIRD2, BLOX, NEWBIRD, PODZ1 и INTRO в отдельные буферы.
 * Вход: Сегменты буферов уже назначены entry.
 * Эффект/ограничения: Шесть чтений по 32000 байт. Формат использования не одинаков у всех .X.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type load_assets, @function
load_assets:

    # Шесть чтений по 32000 байт. Это исходные форматы ресурсов, не шесть одинаковых PNG.
    mov dx, OFFSET D_filename_bird1                                  # CS:0600
    mov ax, ds:[D_segment_bird1]                                     # CS:0603
    call load_32000_bytes                                            # CS:0606
    mov dx, OFFSET D_filename_bird2                                  # CS:0609
    mov ax, ds:[D_segment_bird2]                                     # CS:060C
    call load_32000_bytes                                            # CS:060F
    mov dx, OFFSET D_filename_blox                                   # CS:0612
    mov ax, ds:[D_segment_blox]                                      # CS:0615
    call load_32000_bytes                                            # CS:0618
    mov dx, OFFSET D_filename_newbird                                # CS:061B
    mov ax, ds:[D_segment_newbird]                                   # CS:061E
    call load_32000_bytes                                            # CS:0621
    mov dx, OFFSET D_filename_podz1                                  # CS:0624
    mov ax, ds:[D_segment_podz1]                                     # CS:0627
    call load_32000_bytes                                            # CS:062A
    mov dx, OFFSET D_filename_intro                                  # CS:062D
    mov ax, ds:[D_segment_intro]                                     # CS:0630
    call load_32000_bytes                                            # CS:0633
    ret                                                              # CS:0636

/* ========================================================================== 
 * load_32000_bytes  |  CS:0637
 * Назначение: Обёртка чтения полного ресурса в segment:0000.
 * Вход: AX = сегмент назначения; DS:DX = имя.
 * Эффект/ограничения: DI=0, CX=32000; затем файловый reader.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type load_32000_bytes, @function
load_32000_bytes:
    mov ds:[D_file_destination_segment], ax                          # CS:0637
    mov di, 0x0                                                      # CS:063A
    mov cx, 0x7d00                                                   # CS:063D
    call dos_open_read_close                                         # CS:0640
    ret                                                              # CS:0643

/* ========================================================================== 
 * load_file_to_ds  |  CS:0644
 * Назначение: Выбирает текущий DS как сегмент назначения и читает файл.
 * Вход: DS:DX = имя; DI = адрес назначения; CX = длина.
 * Эффект/ограничения: Используется для HISCORES.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type load_file_to_ds, @function
load_file_to_ds:
    mov ax, ds                                                       # CS:0644
    mov ds:[D_file_destination_segment], ax                          # CS:0646
    call dos_open_read_close                                         # CS:0649
    ret                                                              # CS:064C

/* ========================================================================== 
 * dos_open_read_close  |  CS:064D
 * Назначение: Открывает файл, читает блок и закрывает handle.
 * Вход: DS:DX = имя; DI = offset; CX = count; file_destination_segment = сегмент.
 * Эффект/ограничения: Ошибка open возвращается; результат read не проверяется до close. Не считать AX надёжным числом прочитанных байтов.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type dos_open_read_close, @function
dos_open_read_close:
    push di                                                          # CS:064D
    push cx                                                          # CS:064E
    mov ax, 0x3d00                                                   # CS:064F
    int 0x21                                                         # CS:0652
    pop cx                                                           # CS:0654
    pop dx                                                           # CS:0655
    jae L_dos_open_read_close_0659                                   # CS:0656
    ret                                                              # CS:0658
L_dos_open_read_close_0659:
    mov ds:[D_file_handle], ax                                       # CS:0659
    {load} mov bx, ax                                                # CS:065C
    mov ax, ds:[D_file_destination_segment]                          # CS:065E
    push ds                                                          # CS:0661
    mov ds, ax                                                       # CS:0662
    mov ax, 0x3f00                                                   # CS:0664
    int 0x21                                                         # CS:0667
    pop ds                                                           # CS:0669
    mov ax, 0x3e00                                                   # CS:066A
    mov bx, WORD PTR ds:[D_file_handle]                              # CS:066D
    int 0x21                                                         # CS:0671
    ret                                                              # CS:0673

/* ========================================================================== 
 * dos_create_write_close  |  CS:0674
 * Назначение: Создаёт/обнуляет файл и записывает блок.
 * Вход: DS:DX = имя; DS:DI = данные; CX = длина.
 * Эффект/ограничения: При ошибке write выходит без close; сохранено, а не исправлено.
 * Возвраты в физическом диапазоне: ret.
 * Регистровый ABI не является C ABI; сохранность регистров проверять вместе с callees.
 * ========================================================================== */
.type dos_create_write_close, @function
dos_create_write_close:
    push di                                                          # CS:0674
    push cx                                                          # CS:0675
    mov cx, 0x0                                                      # CS:0676
    mov ax, 0x3c00                                                   # CS:0679
    int 0x21                                                         # CS:067C
    pop cx                                                           # CS:067E
    pop dx                                                           # CS:067F
    jae L_dos_create_write_close_0683                                # CS:0680
    ret                                                              # CS:0682
L_dos_create_write_close_0683:
    mov ds:[D_file_handle], ax                                       # CS:0683
    {load} mov bx, ax                                                # CS:0686
    mov ax, 0x4000                                                   # CS:0688
    int 0x21                                                         # CS:068B
    jae L_dos_create_write_close_0690                                # CS:068D
    ret                                                              # CS:068F
L_dos_create_write_close_0690:
    mov ax, 0x3e00                                                   # CS:0690
    mov bx, WORD PTR ds:[D_file_handle]                              # CS:0693
    int 0x21                                                         # CS:0697
    ret                                                              # CS:0699
