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

}

// Return true if interrupt needs to be requested
// todo: take in cpu context and mark interrupts there?
bool LCD::increment_ly()
{
    bool interrupt = false;
    ly++;
    if (ly == ly_compare) {
        set_lyc_flag();
        if (get_interrupt_status(ISM_LYC)) {
            interrupt = true;
        }
    }
    else {
        reset_lyc_flag();
    }
    return interrupt;
}
