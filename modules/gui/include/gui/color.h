/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef _APP_GUI_COLOR_H
#define _APP_GUI_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_COLOR_BASIC_GREEN               LV_COLOR_MAKE(0x73, 0xbf, 0x69)
#define GUI_COLOR_DARK_GREEN                LV_COLOR_MAKE(0x37, 0x87, 0x2d)
#define GUI_COLOR_SEMIDARK_GREEN            LV_COLOR_MAKE(0x56, 0xa6, 0x4b)
#define GUI_COLOR_LIGHT_GREEN               LV_COLOR_MAKE(0x96, 0xd9, 0x8d)
#define GUI_COLOR_SUPERLIGHT_GREEN          LV_COLOR_MAKE(0xc8, 0xf2, 0xc2)

#define GUI_COLOR_BASIC_YELLOW              LV_COLOR_MAKE(0xfa, 0xde, 0x2a)
#define GUI_COLOR_DARK_YELLOW               LV_COLOR_MAKE(0xe0, 0xb4, 0x00)
#define GUI_COLOR_SEMIDARK_YELLOW           LV_COLOR_MAKE(0xf2, 0xcc, 0x0c)
#define GUI_COLOR_LIGHT_YELLOW              LV_COLOR_MAKE(0xff, 0xee, 0x52)
#define GUI_COLOR_SUPERLIGHT_YELLOW         LV_COLOR_MAKE(0xff, 0xf8, 0x99)

#define GUI_COLOR_BASIC_RED                 LV_COLOR_MAKE(0xf2, 0x49, 0x5c)
#define GUI_COLOR_DARK_RED                  LV_COLOR_MAKE(0xc4, 0x16, 0x2a)
#define GUI_COLOR_SEMIDARK_RED              LV_COLOR_MAKE(0xe0, 0x2f, 0x44)
#define GUI_COLOR_LIGHT_RED                 LV_COLOR_MAKE(0xff, 0x73, 0x83)
#define GUI_COLOR_SUPERLIGHT_RED            LV_COLOR_MAKE(0xff, 0xa6, 0xb0)

#define GUI_COLOR_BASIC_BLUE                LV_COLOR_MAKE(0x57, 0x94, 0xf2)
#define GUI_COLOR_DARK_BLUE                 LV_COLOR_MAKE(0x1f, 0x60, 0xc4)
#define GUI_COLOR_SEMIDARK_BLUE             LV_COLOR_MAKE(0x32, 0x74, 0xd9)
#define GUI_COLOR_LIGHT_BLUE                LV_COLOR_MAKE(0x8a, 0xb8, 0xff)
#define GUI_COLOR_SUPERLIGHT_BLUE           LV_COLOR_MAKE(0xc0, 0xd8, 0xff)

#define GUI_COLOR_BASIC_ORANGE              LV_COLOR_MAKE(0xff, 0x98, 0x30)
#define GUI_COLOR_DARK_ORANGE               LV_COLOR_MAKE(0xfa, 0x64, 0x00)
#define GUI_COLOR_SEMIDARK_ORANGE           LV_COLOR_MAKE(0xff, 0x78, 0x0a)
#define GUI_COLOR_LIGHT_ORANGE              LV_COLOR_MAKE(0xff, 0xb3, 0x57)
#define GUI_COLOR_SUPERLIGHT_ORANGE         LV_COLOR_MAKE(0xff, 0xcb, 0x7d)

#define GUI_COLOR_BASIC_PURPLE              LV_COLOR_MAKE(0xb8, 0x77, 0xd9)
#define GUI_COLOR_DARK_PURPLE               LV_COLOR_MAKE(0x8f, 0x3b, 0xb8)
#define GUI_COLOR_SEMIDARK_PURPLE           LV_COLOR_MAKE(0xa3, 0x52, 0xcc)
#define GUI_COLOR_LIGHT_PURPLE              LV_COLOR_MAKE(0xca, 0x95, 0xe5)
#define GUI_COLOR_SUPERLIGHT_PURPLE         LV_COLOR_MAKE(0xde, 0xb6, 0xf2)

#define GUI_COLOR_BASIC_GREY                LV_COLOR_MAKE(0x8e, 0x8e, 0x8e)
#define GUI_COLOR_DARK_GREY                 LV_COLOR_MAKE(0x16, 0x17, 0x19)
#define GUI_COLOR_SEMIDARK_GREY             LV_COLOR_MAKE(0x21, 0x21, 0x24)
#define GUI_COLOR_LIGHT_GREY                LV_COLOR_MAKE(0x62, 0x62, 0x63)
#define GUI_COLOR_SUPERLIGHT_GREY           LV_COLOR_MAKE(0xd8, 0xd9, 0xda)

#define GUI_COLOR_LBL_BASIC_GREEN           "#73bf69"
#define GUI_COLOR_LBL_DARK_GREEN            "#37872d"
#define GUI_COLOR_LBL_SEMIDARK_GREEN        "#56a64b"
#define GUI_COLOR_LBL_LIGHT_GREEN           "#96d98d"
#define GUI_COLOR_LBL_SUPERLIGHT_GREEN      "#c8f2c2"

#define GUI_COLOR_LBL_BASIC_YELLOW          "#fade2a"
#define GUI_COLOR_LBL_DARK_YELLOW           "#e0b400"
#define GUI_COLOR_LBL_SEMIDARK_YELLOW       "#f2cc0c"
#define GUI_COLOR_LBL_LIGHT_YELLOW          "#ffee52"
#define GUI_COLOR_LBL_SUPERLIGHT_YELLOW     "#fff899"

#define GUI_COLOR_LBL_BASIC_RED             "#f2495c"
#define GUI_COLOR_LBL_DARK_RED              "#c4162a"
#define GUI_COLOR_LBL_SEMIDARK_RED          "#e02f44"
#define GUI_COLOR_LBL_LIGHT_RED             "#ff7383"
#define GUI_COLOR_LBL_SUPERLIGHT_RED        "#ffa6b0"

#define GUI_COLOR_LBL_BASIC_BLUE            "#5794f2"
#define GUI_COLOR_LBL_DARK_BLUE             "#1f60c4"
#define GUI_COLOR_LBL_SEMIDARK_BLUE         "#3274d9"
#define GUI_COLOR_LBL_LIGHT_BLUE            "#8ab8ff"
#define GUI_COLOR_LBL_SUPERLIGHT_BLUE       "#c0d8ff"

#define GUI_COLOR_LBL_BASIC_ORANGE          "#ff9830"
#define GUI_COLOR_LBL_DARK_ORANGE           "#fa6400"
#define GUI_COLOR_LBL_SEMIDARK_ORANGE       "#ff780a"
#define GUI_COLOR_LBL_LIGHT_ORANGE          "#ffb357"
#define GUI_COLOR_LBL_SUPERLIGHT_ORANGE     "#ffcb7d"

#define GUI_COLOR_LBL_BASIC_PURPLE          "#b877d9"
#define GUI_COLOR_LBL_DARK_PURPLE           "#8f3bb8"
#define GUI_COLOR_LBL_SEMIDARK_PURPLE       "#a352cc"
#define GUI_COLOR_LBL_LIGHT_PURPLE          "#ca95e5"
#define GUI_COLOR_LBL_SUPERLIGHT_PURPLE     "#deb6f2"

#define GUI_COLOR_LBL_BASIC_GREY            "#8e8e8e"
#define GUI_COLOR_LBL_DARK_GREY             "#161719"
#define GUI_COLOR_LBL_SEMIDARK_GREY         "#212124"
#define GUI_COLOR_LBL_LIGHT_GREY            "#626263"
#define GUI_COLOR_LBL_SUPERLIGHT_GREY       "#d8d9da"

#ifdef __cplusplus
}
#endif

#endif /* _APP_GUI_COLOR_H */

