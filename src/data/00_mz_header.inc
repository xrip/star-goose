/* MZ header, including the nonstandard word at offset 001Ch. */
mz_header:
    .word 0x5a4d                         # signature
    .word 0x01b4                         # bytes_in_last_page
    .word 0x0069                         # pages_in_file
    .word 0x0004                         # relocation_count
    .word 0x0020                         # header_paragraphs
    .word 0x0011                         # min_extra_paragraphs
    .word 0xffff                         # max_extra_paragraphs
    .word 0x0cfc                         # initial_ss
    .word 0x0100                         # initial_sp
    .word 0xe069                         # original_checksum
    .word entry - image_start             # initial_ip = 0010h
    .word 0x0000                          # initial_cs
    .word mz_relocations - mz_header      # relocation_table_offset = 001Eh
    .word 0x0000                          # overlay_number
    .word 0x0001                          # preserved extra header word at 001Ch
mz_relocations:
    .word relocation_0011 - image_start, 0x0000
    .word relocation_04d1 - image_start, 0x0000
    .word relocation_05d8 - image_start, 0x0000
    .word relocation_0b59 - image_start, 0x0000
    .fill 0x0200 - (. - mz_header), 1, 0
image_start:
    .fill 16, 1, 0                       # unexecuted image prefix; entry is CS:0010


