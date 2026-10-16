#ifndef GOOSE_PICTURES_H
#define GOOSE_PICTURES_H

#include "surface.h"

/* Loads the converted PNG banks. Returns 0 when a file is missing. */
int pictures_load(void);
void pictures_draw_surface(const Surface *game);
void pictures_present(void);
void pictures_save(const char *path);
void pictures_shutdown(void);

#endif
