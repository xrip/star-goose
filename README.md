# Star Goose — readable byte-exact ASM

[Полное описание и сборка](README_RU.md) · [Архитектура](docs/ARCHITECTURE_RU.md) · [Каталог процедур](docs/FUNCTIONS_RU.md) · [C-порт](docs/PORTING_TO_C_RU.md)

Исходник `src/GOOSE.asm` объединяет 21 модуль кода. 204 восстановленных входа снабжены именами и контрактами; данные и Entity размечены отдельно. Сборка сохраняет все53 684 байта исходного EXE.

```sh
python3 build.py
python3 tests/test_project.py
python3 tests/test_c_models.py
```

Имена/описания получены анализом, а не восстановлены из авторских symbols. Полный запуск игры и эквивалентность C-порта не заявляются; см. `docs/OPEN_QUESTIONS_RU.md`.
