#include "LCD.h"

u8 LCD::read(u16 address)
{

    switch (address) {
    case 0xFF40:
        return lcd_control;
    case 0xFF41:
        return lcd_status;
    case 0xFF42:
        return scroll_y;
    case 0xFF43:
        return scroll_x;
    case 0xFF44:
        return ly;
    case 0xFF45:
        return ly_compare;
    case 0xFF47:
        return bgp_palette;
    case 0xFF48:
        return obj_palette[0];
    case 0xFF49:
        return obj_palette[1];
    case 0xFF4A:
        return window_y;
    case 0xFF4B:
        return window_x;
    }
}

void LCD::write(u16 address, u8 value)
{
    switch (address) {
    case 0xFF40:
        lcd_control = value;
        break;
    case 0xFF41:
        lcd_status = value;
        break;
    case 0xFF42:
        scroll_y = value;
        break;
    case 0xFF43:
        scroll_x = value;
        break;
    case 0xFF44:
        break;
    case 0xFF45:
        ly_compare = value;
        break;
    case 0xFF47:
        update_palette(value, 0);
        break;
    case 0xFF48:
        update_palette(value & 0b11111100, 1);
        break;
    case 0xFF49:
        update_palette(value & 0b11111100, 2);
        break;
    case 0xFF4A:
        window_y = value;
        break;
    case 0xFF4B:
        window_x = value;
        break;
    }
}

void LCD::update_palette(u8 value, u8 palette_number)
{
    u32 colors[] = {colors_default[value & 0b11], colors_default[(value >> 2) & 0b11],
        colors_default[(value >> 4) & 0b11],colors_default[(value >> 6) & 0b11] };

    switch (palette_number)
    {
    case 0:
        for (int i = 0; i < 4; i++) {
            bg_colors[i] = colors[i];
        }
        
        break;
    case 1:
        for (int i = 0; i < 4; i++) {
            sp1_colors[i] = colors[i];
        }

        break;
    case 2:
        for (int i = 0; i < 4; i++) {
            sp2_colors[i] = colors[i];
        }

        break;
    }

}


void LCD::increment_ly(CPUContext* cpu)
{
    const int y_res = 144;
    if (window_visible() && ly >= window_y && ly < (window_y + y_res)) {
        window_line++;
    }


    ly++;
    if (ly == ly_compare) {
        set_lyc_flag();
        if (get_interrupt_status(ISM_LYC)) {
            cpu->ppu_interrupt_queue.push_back(IT_LCD_STAT);
        }
    }
    else {
        reset_lyc_flag();
    }
}

bool LCD::lcd_on()
{
    return lcd_control >> 7;
}

bool LCD::get_bgw_enable()
{
    return lcd_control & 1;
}

bool LCD::get_obj_enable()
{
    return (lcd_control >> 1) & 1;
}

u16 LCD::get_bg_tile_map()
{
    if ((lcd_control & 0b1000) >> 3) {
        return 0x9C00;
    }
    return 0x9800;
}

u16 LCD::get_bgw_tile_data_am()
{
    if ((lcd_control & 0b10000) >> 4) {
        return 0x8000;
    }
    return 0x8800;
}

u16 LCD::get_window_tile_map()
{
    if ((lcd_control & 0b1000000) >> 6) {
        return 0x9C00;
    }
    return 0x9800;
}

void LCD::set_lcd_mode(LCDMode mode)
{
    lcd_status = ((lcd_status >> 2) << 2) | mode;
}

LCDMode LCD::get_lcd_mode()
{
    return static_cast<LCDMode>(lcd_status & 0b11);
}

void LCD::set_lyc_flag()
{
    lcd_status |= 0b100;
}

void LCD::reset_lyc_flag()
{
    lcd_status &= 0b11111011;
}

bool LCD::get_interrupt_status(InterruptSourceMode interrupt_source)
{
    return lcd_status & interrupt_source;
}

u8 LCD::get_sprite_height()
{
    if ((lcd_control >> 2) & 1) {
        return 16;
    }
    return 8;
}

bool LCD::window_visible()
{
    const int y_res = 144;
    bool win_enable_bit = (lcd_control >> 5) & 1;
    bool win_on_screen = window_x <= 166 && window_y >= 9 &&
        window_y < y_res;
    return win_enable_bit && win_on_screen;
}

