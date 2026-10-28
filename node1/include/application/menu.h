#ifndef MENU_H
#define MENU_H

#include <stdint.h>

void menu_framebuffer_draw(const char *items[], uint8_t num_items, uint8_t selected_item);
uint8_t menu_navigate(uint8_t num_items, const char *items[]);
void home_menu(void);
void game_menu(void);
void settings_menu(void);
#endif // MENU_H