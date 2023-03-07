/***************************************************************************//**
 * @file
 * @brief Silicon Labs Graphics Library: Color Defines
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __GLIB_COLOR_H
#define __GLIB_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup glib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup glib_color GLIB Colors
 * @brief Predefined Colors
 * @{
 ******************************************************************************/
/* A Set of color defines. This is a subset of the X11 colors */
#define AliceBlue               (0x00F0F8FF)  /**< AliceBlue               */
#define AntiqueWhite            (0x00FAEBD7)  /**< AntiqueWhite            */
#define Aqua                    (0x0000FFFF)  /**< Aqua                    */
#define Aquamarine              (0x007FFFD4)  /**< Aquamarine              */
#define Azure                   (0x00F0FFFF)  /**< Azure                   */
#define Beige                   (0x00F5F5DC)  /**< Beige                   */
#define Bisque                  (0x00FFE4C4)  /**< Bisque                  */
#define Black                   (0x00000000)  /**< Black                   */
#define BlanchedAlmond          (0x00FFEBCD)  /**< BlanchedAlmond          */
#define Blue                    (0x000000FF)  /**< Blue                    */
#define BlueViolet              (0x008A2BE2)  /**< BlueViolet              */
#define Brown                   (0x00A52A2A)  /**< Brown                   */
#define BurlyWood               (0x00DEB887)  /**< BurlyWood               */
#define CadetBlue               (0x005F9EA0)  /**< CadetBlue               */
#define Chartreuse              (0x007FFF00)  /**< Chartreuse              */
#define Chocolate               (0x00D2691E)  /**< Chocolate               */
#define Coral                   (0x00FF7F50)  /**< Coral                   */
#define CornflowerBlue          (0x006495ED)  /**< CornflowerBlue          */
#define Cornsilk                (0x00FFF8DC)  /**< Cornsilk                */
#define Crimson                 (0x00DC143C)  /**< Crimson                 */
#define Cyan                    (0x0000FFFF)  /**< Cyan                    */
#define DarkBlue                (0x0000008B)  /**< DarkBlue                */
#define DarkCyan                (0x00008B8B)  /**< DarkCyan                */
#define DarkGoldenrod           (0x00B8860B)  /**< DarkGoldenrod           */
#define DarkGray                (0x00A9A9A9)  /**< DarkGray                */
#define DarkGreen               (0x00006400)  /**< DarkGreen               */
#define DarkKhaki               (0x00BDB76B)  /**< DarkKhaki               */
#define DarkMagenta             (0x008B008B)  /**< DarkMagenta             */
#define DarkOliveGreen          (0x00556B2F)  /**< DarkOliveGreen          */
#define DarkOrange              (0x00FF8C00)  /**< DarkOrange              */
#define DarkOrchid              (0x009932CC)  /**< DarkOrchid              */
#define DarkRed                 (0x008B0000)  /**< DarkRed                 */
#define DarkSalmon              (0x00E9967A)  /**< DarkSalmon              */
#define DarkSeaGreen            (0x008FBC8F)  /**< DarkSeaGreen            */
#define DarkSlateBlue           (0x00483D8B)  /**< DarkSlateBlue           */
#define DarkSlateGray           (0x002F4F4F)  /**< DarkSlateGray           */
#define DarkTurquoise           (0x0000CED1)  /**< DarkTurquoise           */
#define DarkViolet              (0x009400D3)  /**< DarkViolet              */
#define DeepPink                (0x00FF1493)  /**< DeepPink                */
#define DeepSkyBlue             (0x0000BFFF)  /**< DeepSkyBlue             */
#define DimGray                 (0x00696969)  /**< DimGray                 */
#define DodgerBlue              (0x001E90FF)  /**< DodgerBlue              */
#define FireBrick               (0x00B22222)  /**< FireBrick               */
#define FloralWhite             (0x00FFFAF0)  /**< FloralWhite             */
#define ForestGreen             (0x00228B22)  /**< ForestGreen             */
#define Fuchsia                 (0x00FF00FF)  /**< Fuchsia                 */
#define Gainsboro               (0x00DCDCDC)  /**< Gainsboro               */
#define GhostWhite              (0x00F8F8FF)  /**< GhostWhite              */
#define Gold                    (0x00FFD700)  /**< Gold                    */
#define Goldenrod               (0x00DAA520)  /**< Goldenrod               */
#define Gray                    (0x00808080)  /**< Gray                    */
#define Green                   (0x00008000)  /**< Green                   */
#define GreenYellow             (0x00ADFF2F)  /**< GreenYellow             */
#define Honeydew                (0x00F0FFF0)  /**< Honeydew                */
#define HotPink                 (0x00FF69B4)  /**< HotPink                 */
#define IndianRed               (0x00CD5C5C)  /**< IndianRed               */
#define Indigo                  (0x004B0082)  /**< Indigo                  */
#define Ivory                   (0x00FFFFF0)  /**< Ivory                   */
#define Khaki                   (0x00F0E68C)  /**< Khaki                   */
#define Lavender                (0x00E6E6FA)  /**< Lavender                */
#define LavenderBlush           (0x00FFF0F5)  /**< LavenderBlush           */
#define LawnGreen               (0x007CFC00)  /**< LawnGreen               */
#define LemonChiffon            (0x00FFFACD)  /**< LemonChiffon            */
#define LightBlue               (0x00ADD8E6)  /**< LightBlue               */
#define LightCoral              (0x00F08080)  /**< LightCoral              */
#define LightCyan               (0x00E0FFFF)  /**< LightCyan               */
#define LightGoldenrodYellow    (0x00FAFAD2)  /**< LightGoldenrodYellow    */
#define LightGreen              (0x0090EE90)  /**< LightGreen              */
#define LightGrey               (0x00D3D3D3)  /**< LightGrey               */
#define LightPink               (0x00FFB6C1)  /**< LightPink               */
#define LightSalmon             (0x00FFA07A)  /**< LightSalmon             */
#define LightSeaGreen           (0x0020B2AA)  /**< LightSeaGreen           */
#define LightSkyBlue            (0x0087CEFA)  /**< LightSkyBlue            */
#define LightSlateGray          (0x00778899)  /**< LightSlateGray          */
#define LightSteelBlue          (0x00B0C4DE)  /**< LightSteelBlue          */
#define LightYellow             (0x00FFFFE0)  /**< LightYellow             */
#define Lime                    (0x0000FF00)  /**< Lime                    */
#define LimeGreen               (0x0032CD32)  /**< LimeGreen               */
#define Linen                   (0x00FAF0E6)  /**< Linen                   */
#define Magenta                 (0x00FF00FF)  /**< Magenta                 */
#define Maroon                  (0x00800000)  /**< Maroon                  */
#define MediumAquamarine        (0x0066CDAA)  /**< MediumAquamarine        */
#define MediumBlue              (0x000000CD)  /**< MediumBlue              */
#define MediumOrchid            (0x00BA55D3)  /**< MediumOrchid            */
#define MediumPurple            (0x009370DB)  /**< MediumPurple            */
#define MediumSeaGreen          (0x003CB371)  /**< MediumSeaGreen          */
#define MediumSlateBlue         (0x007B68EE)  /**< MediumSlateBlue         */
#define MediumSpringGreen       (0x0000FA9A)  /**< MediumSpringGreen       */
#define MediumTurquoise         (0x0048D1CC)  /**< MediumTurquoise         */
#define MediumVioletRed         (0x00C71585)  /**< MediumVioletRed         */
#define MidnightBlue            (0x00191970)  /**< MidnightBlue            */
#define MintCream               (0x00F5FFFA)  /**< MintCream               */
#define MistyRose               (0x00FFE4E1)  /**< MistyRose               */
#define Moccasin                (0x00FFE4B5)  /**< Moccasin                */
#define NavajoWhite             (0x00FFDEAD)  /**< NavajoWhite             */
#define Navy                    (0x00000080)  /**< Navy                    */
#define OldLace                 (0x00FDF5E6)  /**< OldLace                 */
#define Olive                   (0x00808000)  /**< Olive                   */
#define OliveDrab               (0x006B8E23)  /**< OliveDrab               */
#define Orange                  (0x00FFA500)  /**< Orange                  */
#define OrangeRed               (0x00FF4500)  /**< OrangeRed               */
#define Orchid                  (0x00DA70D6)  /**< Orchid                  */
#define PaleGoldenrod           (0x00EEE8AA)  /**< PaleGoldenrod           */
#define PaleGreen               (0x0098FB98)  /**< PaleGreen               */
#define PaleTurquoise           (0x00AFEEEE)  /**< PaleTurquoise           */
#define PaleVioletRed           (0x00DB7093)  /**< PaleVioletRed           */
#define PaleVioletRed           (0x00DB7093)  /**< PaleVioletRed           */
#define PapayaWhip              (0x00FFEFD5)  /**< PapayaWhip              */
#define PeachPuff               (0x00FFDAB9)  /**< PeachPuff               */
#define Peru                    (0x00CD853F)  /**< Peru                    */
#define Pink                    (0x00FFC0CB)  /**< Pink                    */
#define Plum                    (0x00DDA0DD)  /**< Plum                    */
#define PowderBlue              (0x00B0E0E6)  /**< PowderBlue              */
#define Purple                  (0x00800080)  /**< Purple                  */
#define Red                     (0x00FF0000)  /**< Red                     */
#define RosyBrown               (0x00BC8F8F)  /**< RosyBrown               */
#define RoyalBlue               (0x004169E1)  /**< RoyalBlue               */
#define SaddleBrown             (0x008B4513)  /**< SaddleBrown             */
#define Salmon                  (0x00FA8072)  /**< Salmon                  */
#define SandyBrown              (0x00F4A460)  /**< SandyBrown              */
#define SeaGreen                (0x002E8B57)  /**< SeaGreen                */
#define Seashell                (0x00FFF5EE)  /**< Seashell                */
#define Sienna                  (0x00A0522D)  /**< Sienna                  */
#define Silver                  (0x00C0C0C0)  /**< Silver                  */
#define SkyBlue                 (0x0087CEEB)  /**< SkyBlue                 */
#define SlateBlue               (0x006A5ACD)  /**< SlateBlue               */
#define SlateGray               (0x00708090)  /**< SlateGray               */
#define Snow                    (0x00FFFAFA)  /**< Snow                    */
#define SpringGreen             (0x0000FF7F)  /**< SpringGreen             */
#define SteelBlue               (0x004682B4)  /**< SteelBlue               */
#define Tan                     (0x00D2B48C)  /**< Tan                     */
#define Teal                    (0x00008080)  /**< Teal                    */
#define Thistle                 (0x00D8BFD8)  /**< Thistle                 */
#define Tomato                  (0x00FF6347)  /**< Tomato                  */
#define Turquoise               (0x0040E0D0)  /**< Turquoise               */
#define Violet                  (0x00EE82EE)  /**< Violet                  */
#define Wheat                   (0x00F5DEB3)  /**< Wheat                   */
#define White                   (0x00FFFFFF)  /**< White                   */
#define WhiteSmoke              (0x00F5F5F5)  /**< WhiteSmoke              */
#define Yellow                  (0x00FFFF00)  /**< Yellow                  */
#define YellowGreen             (0x009ACD32)  /**< YellowGreen             */

/* Mask to help extract components */
#define RedMask                 (0x00FF0000) /**< Mask to extract red color component.  */
#define RedShift                16           /**< Shift to extract red color component. */
#define GreenMask               (0x0000FF00) /**< Mask to extract green color component.  */
#define GreenShift              8            /**< Shift to extract green color component.  */
#define BlueMask                (0x000000FF) /**< Mask to extract blue color component.  */
#define BlueShift               0            /**< Shift to extract blue color component.  */

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup glib_color) */
/** @} (end addtogroup glib) */

#endif
