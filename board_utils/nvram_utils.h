#ifndef NVRAM_UTILS
#define NVRAM_UTILS
#include "commons.h"

void update_version(uint32_t version);
bool is_version_changed(uint32_t version);
void nvram_save_settings();
void nvram_load_settings(COLOR_HSV* current_color);

#endif