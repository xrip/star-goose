/* GNU as, Intel syntax. {load}, {disp16}, ACC16 сохраняют исходную форму кодирования.
 * Не заменять их обычными инструкциями без повторной byte-exact проверки. */
.intel_syntax noprefix
.code16
.arch i8086
.section .text
.global mz_header, image_start, entry, file_end
.macro ACC16 op:req, value:req
    \op ax, OFFSET FLAT:.Limm_\@
    .set .Limm_\@, \value
.endm
