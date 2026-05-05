#pragma once
#include <compositor.h>

void init_term(context_t *ctx);
void write_buf(context_t *ctx, char c);
const char *submit_buf(context_t *ctx);
int handle_command(context_t *ctx, const char *cmd);