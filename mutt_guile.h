#ifndef MUTT_MUTT_GUILE_H
#define MUTT_MUTT_GUILE_H

#include <stdint.h>
#include <libguile.h>
#include "core/lib.h"

struct Buffer;

enum CommandResult mutt_guile_parse      (struct Buffer *tmp, struct Buffer *s, intptr_t data, struct Buffer *err);
enum CommandResult mutt_guile_source_file(struct Buffer *tmp, struct Buffer *s, intptr_t data, struct Buffer *err);

void mutt_guile_init(void);
bool mutt_guile_load_config(const char *path);
void mutt_guile_run_hook(char *name, SCM args);

#endif /* MUTT_MUTT_GUILE_H */
