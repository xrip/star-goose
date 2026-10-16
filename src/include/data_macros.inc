/* Метаданные, не инструкции. Сохраняют каждый исходный токен, без повторного сжатия. */
.macro MAP_RUN tile:req, count:req
    .byte ((\count - 1) << 6) | \tile
.endm
.macro MAP_OBJECT kind:req, column:req, source_row:req
    .byte \kind, \column, \source_row
.endm
