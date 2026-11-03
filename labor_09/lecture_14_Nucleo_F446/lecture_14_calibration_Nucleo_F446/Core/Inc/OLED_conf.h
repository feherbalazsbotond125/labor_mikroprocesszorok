#ifndef __OLED_CONF_H__
#define __OLED_CONF_H__

// I2C Configuration
#define OLED_I2C_PORT        hi2c2
#define OLED_I2C_ADDR_1      (0x3C << 1)
#define OLED_I2C_ADDR_2      (0x3D << 1)

// Mirror the screen if needed
// #define OLED_MIRROR_VERT
// #define OLED_MIRROR_HORIZ

// Set inverse color if needed
// # define OLED_INVERSE_COLOR

// Include only needed fonts
#define OLED_INCLUDE_FONT_6x8
#define OLED_INCLUDE_FONT_7x10
#define OLED_INCLUDE_FONT_11x18
#define OLED_INCLUDE_FONT_16x26

#endif /* __OLED_CONF_H__ */
