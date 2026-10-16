# raylib-nx 5.5 always opens a 1280x720 view, so a docked 1080p panel
# keeps the picture in one corner. Size the window before EGL creates it.
from pathlib import Path
import sys

path = Path(sys.argv[1])
text = path.read_text(encoding="utf-8").replace("\r\n", "\n")
old = """    romfsInit();

    CORE.Window.screen.width = 1280;
    CORE.Window.screen.height = 720;
"""
new = """    romfsInit();

    u32 screen_w = 1280;
    u32 screen_h = 720;
    if (appletGetOperationMode() == AppletOperationMode_Console) {
        screen_w = 1920;
        screen_h = 1080;
    }
    nwindowSetDimensions(nwindowGetDefault(), screen_w, screen_h);
    CORE.Window.screen.width = (int)screen_w;
    CORE.Window.screen.height = (int)screen_h;
"""
old = old.replace("\r\n", "\n")
new = new.replace("\r\n", "\n")
if old not in text:
    raise SystemExit("raylib-nx screen size block was not found")
path.write_text(text.replace(old, new, 1), encoding="utf-8")
