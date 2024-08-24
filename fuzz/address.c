#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "mutt/lib.h"
#include "config/lib.h"
#include "email/lib.h"
#include "core/lib.h"
#include "globals.h"
#include "init.h"

/**
 * log_writer_null - Discard log lines
 */

static GLogWriterOutput log_writer_null(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data)
{
  return G_LOG_WRITER_HANDLED;
}


int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
  MuttLogWriter = log_writer_null;
  struct ConfigSet *cs = cs_new(16);
  NeoMutt = neomutt_new(cs);
  init_config(cs);
  OptNoCurses = true;
  char file[] = "/tmp/mutt-fuzz";
  FILE *fp = fopen(file, "wb");
  if (!fp)
    return -1;

  fwrite(data, 1, size, fp);
  fclose(fp);
  fp = fopen(file, "rb");
  if (!fp)
    return -1;

  struct Email *e = email_new();
  struct Envelope *env = mutt_rfc822_read_header(fp, e, 0, 0);
  mutt_parse_part(fp, e->body);
  email_free(&e);
  mutt_env_free(&env);
  fclose(fp);
  neomutt_free(&NeoMutt);
  cs_free(&cs);
  return 0;
}
