#include "menu_controller.h"
#include "ui.h"
#include "pattern_controller.h"
#include <stdio.h>

static lv_obj_t *items[PATTERN_COUNT];
static int selected = 0; // BLACK


// Grid previews are added to the SquareLine 34x34 indicator panels.
// Keep these outside generated SquareLine files so re-exporting UI is safe.
static void grid_bar(lv_obj_t *parent, int x, int y, int w, int h)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_remove_style_all(bar);
    lv_obj_set_size(bar, w, h);
    lv_obj_set_pos(bar, x, y);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

static void grid_previews_init(void)
{
    // Panels have 34x34 px and SquareLine's default padding/border.
    // Remove padding so preview coordinates refer to the entire panel.
    lv_obj_t *panels[] = {ui_GridVerticalPanel, ui_GridHorizontalPanel, ui_GridDiagonalPanel};
    for (int i = 0; i < 3; ++i) {
        lv_obj_set_style_pad_all(panels[i], 0, LV_PART_MAIN);
        lv_obj_set_style_radius(panels[i], 0, LV_PART_MAIN);
        lv_obj_remove_flag(panels[i], LV_OBJ_FLAG_SCROLLABLE);
    }
    for (int x = 4; x <= 28; x += 6) grid_bar(ui_GridVerticalPanel, x, 3, 2, 28);
    for (int y = 4; y <= 28; y += 6) grid_bar(ui_GridHorizontalPanel, 3, y, 28, 2);

    // Diagonal stripes, clipped by the 34x34 parent panel.
    static const lv_point_precise_t diagonals[5][2] = {
        {{0, 8}, {8, 0}},
        {{0, 20}, {20, 0}},
        {{0, 32}, {32, 0}},
        {{12, 33}, {33, 12}},
        {{24, 33}, {33, 24}},
    };
    for (int i = 0; i < 5; ++i) {
        lv_obj_t *line = lv_line_create(ui_GridDiagonalPanel);
        lv_line_set_points(line, diagonals[i], 2);
        lv_obj_set_style_line_color(line, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_style_line_width(line, 2, LV_PART_MAIN);
        lv_obj_remove_flag(line, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    }
}

static void refresh(void)
{
    // All item positions are relative to ActionCopntainer (166 x 236),
    // not to the 170 x 320 display.
    lv_obj_update_layout(ui_ActionCopntainer);
    const int32_t area_w = lv_obj_get_content_width(ui_ActionCopntainer);
    const int32_t area_h = lv_obj_get_content_height(ui_ActionCopntainer);
    const int32_t spacing = 47;

    for (int i = 0; i < PATTERN_COUNT; i++) {
        int d = (i - selected + PATTERN_COUNT) % PATTERN_COUNT;
        if (d > PATTERN_COUNT / 2) d -= PATTERN_COUNT;

        // Clear selection styles for every item, including hidden ones.
        const bool focused = (d == 0);
        lv_obj_set_style_bg_color(items[i], lv_color_hex(focused ? 0x384653 : 0x252F3A), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(items[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_color(items[i], lv_color_hex(focused ? 0xFFFFFF : 0x3C4854), LV_PART_MAIN);
        lv_obj_set_style_border_opa(items[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(items[i], 2, LV_PART_MAIN);
        lv_obj_set_style_radius(items[i], 5, LV_PART_MAIN);
        // Exactly the outline configured on GREEN in SquareLine.
        lv_obj_set_style_outline_color(items[i], lv_color_hex(0x666666), LV_PART_MAIN);
        lv_obj_set_style_outline_opa(items[i], focused ? LV_OPA_COVER : LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_outline_width(items[i], focused ? 1 : 0, LV_PART_MAIN);
        lv_obj_set_style_outline_pad(items[i], 1, LV_PART_MAIN);

        if (d < -2 || d > 2) {
            lv_obj_add_flag(items[i], LV_OBJ_FLAG_HIDDEN);
            continue;
        }
        lv_obj_remove_flag(items[i], LV_OBJ_FLAG_HIDDEN);
        // SquareLine set LV_ALIGN_CENTER on the items. Explicitly change it
        // so x/y mean coordinates from the container's top-left corner.
        lv_obj_set_align(items[i], LV_ALIGN_TOP_LEFT);
        const int32_t x = (area_w - lv_obj_get_width(items[i])) / 2;
        const int32_t y = (area_h - lv_obj_get_height(items[i])) / 2 + d * spacing;
        lv_obj_set_pos(items[i], x, y);
    }
    char info[80];
    snprintf(info, sizeof(info), "ACTIVE: %s", pattern_name(pattern_active()));
    lv_label_set_text(ui_InfoLable, info);
}

void menu_init(void)
{
    items[0] = ui_BLACKContainer;
    items[1] = ui_REDContainer;
    items[2] = ui_GREENContainer;
    items[3] = ui_BLUEContainer;
    items[4] = ui_WHITEContainer;
    items[5] = ui_GRIDContainerVertical;
    items[6] = ui_GRIDConatinerHorizontal;
    items[7] = ui_GRIDConatinerDiagonal;

    // Preserve SquareLine's 166x236 size and top-center position.
    lv_obj_set_layout(ui_ActionCopntainer, LV_LAYOUT_NONE);
    lv_obj_set_style_pad_all(ui_ActionCopntainer, 0, LV_PART_MAIN);
    lv_obj_update_layout(ui_ActionCopntainer);
    grid_previews_init();
    refresh();
}

void menu_rotate(int delta)
{
    if (!delta) return;
    selected = (selected + (delta % PATTERN_COUNT) + PATTERN_COUNT) % PATTERN_COUNT;
    refresh();
}

void menu_press(void)
{
    pattern_activate(selected);
    refresh();
}
