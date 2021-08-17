/*
 * Nuklear - 1.32.0 - public domain
 * no warrenty implied; use at your own risk.
 * authored from 2015-2016 by Micha Mettke
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#ifndef NK_SFML_GL2_H_
#define NK_SFML_GL2_H_

#include <SFML/Window.hpp>
#include "nuklear_def.h"

NK_API struct nk_context*   nk_sfml_init(sf::Window* window);
NK_API void                 nk_sfml_font_stash_begin(struct nk_font_atlas** atlas);
NK_API void                 nk_sfml_font_stash_end(void);
NK_API int                  nk_sfml_handle_event(sf::Event* event);
NK_API void                 nk_sfml_render(enum nk_anti_aliasing);
NK_API void                 nk_sfml_shutdown(void);

enum nk_theme { THEME_BLACK, THEME_WHITE, THEME_RED, THEME_BLUE, THEME_DARK };
int nk_overview(struct nk_context* ctx);
int nk_node_editor(struct nk_context* ctx);
void nk_calculator(struct nk_context* ctx);
void nk_set_style(struct nk_context* ctx, enum nk_theme theme);




#endif

