#pragma once
#include "Registers.h"
#include "utility/Common.h"
#include "Bus.h"
#include "InstructionExecutor.h"
#include "CPUContext.h"
#include "Timer.h"
#include "PPU.h"
#include "LCD.h"

class CPU
{
    
private:
	Bus* bus;
    Timer* timer;
    PPU* ppu;
    LCD* lcd;
	//bool enabling_ime = false;
	int ticks = 0;
	int step_count = 0;
	//bool ime = false;
	InstructionExecutor executor;
    void tick_ppu();
    void fetch_sprite_tile();
    void fetch_window_tile();
    void fetch_sprite_data(bool);
    void check_ppu_interrupts();

    

    

    std::map<u8, Instruction> INSTRUCTIONS {
        {u8 {0x00}, Instruction(IT_NOP, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0)},
        { u8 {0x01}, Instruction(IT_LD, AM_R_D16, RT_BC, RT_NONE, CT_NONE, 0) },
        { u8 {0x02}, Instruction(IT_LD, AM_MR_R, RT_BC, RT_A, CT_NONE, 0) },
        { u8 {0x03}, Instruction(IT_INC, AM_R, RT_BC, RT_NONE, CT_NONE, 0) },
        { u8 {0x04}, Instruction(IT_INC, AM_R, RT_B, RT_NONE, CT_NONE, 0) },
        { u8 {0x05}, Instruction(IT_DEC, AM_R, RT_B, RT_NONE, CT_NONE, 0) },
        { u8 {0x06}, Instruction(IT_LD, AM_R_D8, RT_B, RT_NONE, CT_NONE, 0) },
        { u8 {0x07}, Instruction(IT_RLCA, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0x08}, Instruction(IT_LD, AM_A16_R, RT_NONE, RT_SP, CT_NONE, 0) },
        { u8 {0x09}, Instruction(IT_ADD, AM_R_R, RT_HL, RT_BC, CT_NONE, 0) },
        { u8 {0x0A}, Instruction(IT_LD, AM_R_MR, RT_A, RT_BC, CT_NONE, 0) },
        { u8 {0x0B}, Instruction(IT_DEC, AM_R, RT_BC, RT_NONE, CT_NONE, 0) },
        { u8 {0x0C}, Instruction(IT_INC, AM_R, RT_C, RT_NONE, CT_NONE, 0) },
        { u8 {0x0D}, Instruction(IT_DEC, AM_R, RT_C, RT_NONE, CT_NONE, 0) },
        { u8 {0x0E}, Instruction(IT_LD, AM_R_D8, RT_C, RT_NONE, CT_NONE, 0) },
        { u8 {0x0F}, Instruction(IT_RRCA, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },

        { u8 {0x10}, Instruction(IT_STOP, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0x11}, Instruction(IT_LD, AM_R_D16, RT_DE, RT_NONE, CT_NONE, 0) },
        { u8 {0x12}, Instruction(IT_LD, AM_MR_R, RT_DE, RT_A, CT_NONE, 0) },
        { u8 {0x13}, Instruction(IT_INC, AM_R, RT_DE, RT_NONE, CT_NONE, 0) },
        { u8 {0x14}, Instruction(IT_INC, AM_R, RT_D, RT_NONE, CT_NONE, 0) },
        { u8 {0x15}, Instruction(IT_DEC, AM_R, RT_D, RT_NONE, CT_NONE, 0) },
        { u8 {0x16}, Instruction(IT_LD, AM_R_D8, RT_D, RT_NONE, CT_NONE, 0) },
        { u8 {0x17}, Instruction(IT_RLA, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0x18}, Instruction(IT_JR, AM_D8, RT_NONE, RT_SP, CT_NONE, 0) },
        { u8 {0x19}, Instruction(IT_ADD, AM_R_R, RT_HL, RT_DE, CT_NONE, 0) },
        { u8 {0x1A}, Instruction(IT_LD, AM_R_MR, RT_A, RT_DE, CT_NONE, 0) },
        { u8 {0x1B}, Instruction(IT_DEC, AM_R, RT_DE, RT_NONE, CT_NONE, 0) },
        { u8 {0x1C}, Instruction(IT_INC, AM_R, RT_E, RT_NONE, CT_NONE, 0) },
        { u8 {0x1D}, Instruction(IT_DEC, AM_R, RT_E, RT_NONE, CT_NONE, 0) },
        { u8 {0x1E}, Instruction(IT_LD, AM_R_D8, RT_E, RT_NONE, CT_NONE, 0) },
        { u8 {0x1F}, Instruction(IT_RRA, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },

        { u8 {0x20}, Instruction(IT_JR, AM_D8, RT_NONE, RT_NONE, CT_NZ, 0) },
        { u8 {0x21}, Instruction(IT_LD, AM_R_D16, RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0x22}, Instruction(IT_LD, AM_HLI_R, RT_HL, RT_A, CT_NONE, 0) },
        { u8 {0x23}, Instruction(IT_INC, AM_R, RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0x24}, Instruction(IT_INC, AM_R, RT_H, RT_NONE, CT_NONE, 0) },
        { u8 {0x25}, Instruction(IT_DEC, AM_R, RT_H, RT_NONE, CT_NONE, 0) },
        { u8 {0x26}, Instruction(IT_LD, AM_R_D8, RT_H, RT_NONE, CT_NONE, 0) },
        { u8 {0x27}, Instruction(IT_DAA, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0x28}, Instruction(IT_JR, AM_D8, RT_NONE, RT_NONE, CT_Z, 0) },
        { u8 {0x29}, Instruction(IT_ADD, AM_R_R, RT_HL, RT_HL, CT_NONE, 0) },
        { u8 {0x2A}, Instruction(IT_LD, AM_R_HLI, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x2B}, Instruction(IT_DEC, AM_R, RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0x2C}, Instruction(IT_INC, AM_R, RT_L, RT_NONE, CT_NONE, 0) },
        { u8 {0x2D}, Instruction(IT_DEC, AM_R, RT_L, RT_NONE, CT_NONE, 0) },
        { u8 {0x2E}, Instruction(IT_LD, AM_R_D8, RT_L, RT_NONE, CT_NONE, 0) },
        { u8 {0x2F}, Instruction(IT_CPL, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },

        { u8 {0x30}, Instruction(IT_JR, AM_D8, RT_NONE, RT_NONE, CT_NC, 0) },
        { u8 {0x31}, Instruction(IT_LD, AM_R_D16, RT_SP, RT_NONE, CT_NONE, 0) },
        { u8 {0x32}, Instruction(IT_LD, AM_HLD_R, RT_HL, RT_A, CT_NONE, 0) },
        { u8 {0x33}, Instruction(IT_INC, AM_R, RT_SP, RT_NONE, CT_NONE, 0) },
        { u8 {0x34}, Instruction(IT_INC, AM_MR, RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0x35}, Instruction(IT_DEC, AM_MR, RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0x36}, Instruction(IT_LD, AM_MR_D8, RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0x37}, Instruction(IT_SCF, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0x38}, Instruction(IT_JR, AM_D8, RT_NONE, RT_NONE, CT_C, 0) },
        { u8 {0x39}, Instruction(IT_ADD, AM_R_R, RT_HL, RT_SP, CT_NONE, 0) },
        { u8 {0x3A}, Instruction(IT_LD, AM_R_HLD, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x3B}, Instruction(IT_DEC, AM_R, RT_SP, RT_NONE, CT_NONE, 0) },
        { u8 {0x3C}, Instruction(IT_INC, AM_R, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0x3D}, Instruction(IT_DEC, AM_R, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0x3E}, Instruction(IT_LD, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0x3F}, Instruction(IT_CCF, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },

        { u8 {0x40}, Instruction(IT_LD, AM_R_R,  RT_B, RT_B, CT_NONE, 0) },
        { u8 {0x41}, Instruction(IT_LD, AM_R_R,  RT_B, RT_C, CT_NONE, 0) },
        { u8 {0x42}, Instruction(IT_LD, AM_R_R,  RT_B, RT_D, CT_NONE, 0) },
        { u8 {0x43}, Instruction(IT_LD, AM_R_R,  RT_B, RT_E, CT_NONE, 0) },
        { u8 {0x44}, Instruction(IT_LD, AM_R_R,  RT_B, RT_H, CT_NONE, 0) },
        { u8 {0x45}, Instruction(IT_LD, AM_R_R,  RT_B, RT_L, CT_NONE, 0) },
        { u8 {0x46}, Instruction(IT_LD, AM_R_MR, RT_B, RT_HL, CT_NONE, 0) },
        { u8 {0x47}, Instruction(IT_LD, AM_R_R,  RT_B, RT_A, CT_NONE, 0) },
        { u8 {0x48}, Instruction(IT_LD, AM_R_R,  RT_C, RT_B, CT_NONE, 0) },
        { u8 {0x49}, Instruction(IT_LD, AM_R_R,  RT_C, RT_C, CT_NONE, 0) },
        { u8 {0x4A}, Instruction(IT_LD, AM_R_R,  RT_C, RT_D, CT_NONE, 0) },
        { u8 {0x4B}, Instruction(IT_LD, AM_R_R,  RT_C, RT_E, CT_NONE, 0) },
        { u8 {0x4C}, Instruction(IT_LD, AM_R_R,  RT_C, RT_H, CT_NONE, 0) },
        { u8 {0x4D}, Instruction(IT_LD, AM_R_R,  RT_C, RT_L, CT_NONE, 0) },
        { u8 {0x4E}, Instruction(IT_LD, AM_R_MR, RT_C, RT_HL, CT_NONE, 0) },
        { u8 {0x4F}, Instruction(IT_LD, AM_R_R,  RT_C, RT_A, CT_NONE, 0) },

        { u8 {0x50}, Instruction(IT_LD, AM_R_R,  RT_D, RT_B, CT_NONE, 0) },
        { u8 {0x51}, Instruction(IT_LD, AM_R_R,  RT_D, RT_C, CT_NONE, 0) },
        { u8 {0x52}, Instruction(IT_LD, AM_R_R,  RT_D, RT_D, CT_NONE, 0) },
        { u8 {0x53}, Instruction(IT_LD, AM_R_R,  RT_D, RT_E, CT_NONE, 0) },
        { u8 {0x54}, Instruction(IT_LD, AM_R_R,  RT_D, RT_H, CT_NONE, 0) },
        { u8 {0x55}, Instruction(IT_LD, AM_R_R,  RT_D, RT_L, CT_NONE, 0) },
        { u8 {0x56}, Instruction(IT_LD, AM_R_MR, RT_D, RT_HL, CT_NONE, 0) },
        { u8 {0x57}, Instruction(IT_LD, AM_R_R,  RT_D, RT_A, CT_NONE, 0) },
        { u8 {0x58}, Instruction(IT_LD, AM_R_R,  RT_E, RT_B, CT_NONE, 0) },
        { u8 {0x59}, Instruction(IT_LD, AM_R_R,  RT_E, RT_C, CT_NONE, 0) },
        { u8 {0x5A}, Instruction(IT_LD, AM_R_R,  RT_E, RT_D, CT_NONE, 0) },
        { u8 {0x5B}, Instruction(IT_LD, AM_R_R,  RT_E, RT_E, CT_NONE, 0) },
        { u8 {0x5C}, Instruction(IT_LD, AM_R_R,  RT_E, RT_H, CT_NONE, 0) },
        { u8 {0x5D}, Instruction(IT_LD, AM_R_R,  RT_E, RT_L, CT_NONE, 0) },
        { u8 {0x5E}, Instruction(IT_LD, AM_R_MR, RT_E, RT_HL, CT_NONE, 0) },
        { u8 {0x5F}, Instruction(IT_LD, AM_R_R,  RT_E, RT_A, CT_NONE, 0) },

        { u8 {0x60}, Instruction(IT_LD, AM_R_R,  RT_H, RT_B, CT_NONE, 0) },
        { u8 {0x61}, Instruction(IT_LD, AM_R_R,  RT_H, RT_C, CT_NONE, 0) },
        { u8 {0x62}, Instruction(IT_LD, AM_R_R,  RT_H, RT_D, CT_NONE, 0) },
        { u8 {0x63}, Instruction(IT_LD, AM_R_R,  RT_H, RT_E, CT_NONE, 0) },
        { u8 {0x64}, Instruction(IT_LD, AM_R_R,  RT_H, RT_H, CT_NONE, 0) },
        { u8 {0x65}, Instruction(IT_LD, AM_R_R,  RT_H, RT_L, CT_NONE, 0) },
        { u8 {0x66}, Instruction(IT_LD, AM_R_MR, RT_H, RT_HL, CT_NONE, 0) },
        { u8 {0x67}, Instruction(IT_LD, AM_R_R,  RT_H, RT_A, CT_NONE, 0) },
        { u8 {0x68}, Instruction(IT_LD, AM_R_R,  RT_L, RT_B, CT_NONE, 0) },
        { u8 {0x69}, Instruction(IT_LD, AM_R_R,  RT_L, RT_C, CT_NONE, 0) },
        { u8 {0x6A}, Instruction(IT_LD, AM_R_R,  RT_L, RT_D, CT_NONE, 0) },
        { u8 {0x6B}, Instruction(IT_LD, AM_R_R,  RT_L, RT_E, CT_NONE, 0) },
        { u8 {0x6C}, Instruction(IT_LD, AM_R_R,  RT_L, RT_H, CT_NONE, 0) },
        { u8 {0x6D}, Instruction(IT_LD, AM_R_R,  RT_L, RT_L, CT_NONE, 0) },
        { u8 {0x6E}, Instruction(IT_LD, AM_R_MR, RT_L, RT_HL, CT_NONE, 0) },
        { u8 {0x6F}, Instruction(IT_LD, AM_R_R,  RT_L, RT_A, CT_NONE, 0) },

        { u8 {0x70}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_B, CT_NONE, 0) },
        { u8 {0x71}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_C, CT_NONE, 0) },
        { u8 {0x72}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_D, CT_NONE, 0) },
        { u8 {0x73}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_E, CT_NONE, 0) },
        { u8 {0x74}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_H, CT_NONE, 0) },
        { u8 {0x75}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_L, CT_NONE, 0) },
        { u8 {0x76}, Instruction(IT_HALT, AM_IMP, RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0x77}, Instruction(IT_LD, AM_MR_R,  RT_HL, RT_A, CT_NONE, 0) },
        { u8 {0x78}, Instruction(IT_LD, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0x79}, Instruction(IT_LD, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0x7A}, Instruction(IT_LD, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0x7B}, Instruction(IT_LD, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0x7C}, Instruction(IT_LD, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0x7D}, Instruction(IT_LD, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0x7E}, Instruction(IT_LD, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x7F}, Instruction(IT_LD, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },

        { u8 {0x80}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0x81}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0x82}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0x83}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0x84}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0x85}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0x86}, Instruction(IT_ADD, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x87}, Instruction(IT_ADD, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },
        { u8 {0x88}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0x89}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0x8A}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0x8B}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0x8C}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0x8D}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0x8E}, Instruction(IT_ADC, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x8F}, Instruction(IT_ADC, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },

        { u8 {0x90}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0x91}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0x92}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0x93}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0x94}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0x95}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0x96}, Instruction(IT_SUB, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x97}, Instruction(IT_SUB, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },
        { u8 {0x98}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0x99}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0x9A}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0x9B}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0x9C}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0x9D}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0x9E}, Instruction(IT_SBC, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0x9F}, Instruction(IT_SBC, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },

        { u8 {0xA0}, Instruction(IT_AND, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0xA1}, Instruction(IT_AND, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0xA2}, Instruction(IT_AND, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0xA3}, Instruction(IT_AND, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0xA4}, Instruction(IT_AND, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0xA5}, Instruction(IT_AND, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0xA6}, Instruction(IT_AND, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0xA7}, Instruction(IT_AND, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },
        { u8 {0xA8}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0xA9}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0xAA}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0xAB}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0xAC}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0xAD}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0xAE}, Instruction(IT_XOR, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0xAF}, Instruction(IT_XOR, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },

        { u8 {0xB0}, Instruction(IT_OR, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0xB1}, Instruction(IT_OR, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0xB2}, Instruction(IT_OR, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0xB3}, Instruction(IT_OR, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0xB4}, Instruction(IT_OR, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0xB5}, Instruction(IT_OR, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0xB6}, Instruction(IT_OR, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0xB7}, Instruction(IT_OR, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },
        { u8 {0xB8}, Instruction(IT_CP, AM_R_R,  RT_A, RT_B, CT_NONE, 0) },
        { u8 {0xB9}, Instruction(IT_CP, AM_R_R,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0xBA}, Instruction(IT_CP, AM_R_R,  RT_A, RT_D, CT_NONE, 0) },
        { u8 {0xBB}, Instruction(IT_CP, AM_R_R,  RT_A, RT_E, CT_NONE, 0) },
        { u8 {0xBC}, Instruction(IT_CP, AM_R_R,  RT_A, RT_H, CT_NONE, 0) },
        { u8 {0xBD}, Instruction(IT_CP, AM_R_R,  RT_A, RT_L, CT_NONE, 0) },
        { u8 {0xBE}, Instruction(IT_CP, AM_R_MR, RT_A, RT_HL, CT_NONE, 0) },
        { u8 {0xBF}, Instruction(IT_CP, AM_R_R,  RT_A, RT_A, CT_NONE, 0) },

        { u8 {0xC0}, Instruction(IT_RET, AM_IMP,  RT_NONE, RT_NONE, CT_NZ, 0) },
        { u8 {0xC1}, Instruction(IT_POP, AM_IMP,  RT_BC, RT_NONE, CT_NONE, 0) },
        { u8 {0xC2}, Instruction(IT_JP, AM_D16,  RT_NONE, RT_NONE, CT_NZ, 0) },
        { u8 {0xC3}, Instruction(IT_JP, AM_D16,  RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0xC4}, Instruction(IT_CALL, AM_D16,  RT_NONE, RT_NONE, CT_NZ, 0) },
        { u8 {0xC5}, Instruction(IT_PUSH, AM_R,  RT_BC, RT_NONE, CT_NONE, 0) },
        { u8 {0xC6}, Instruction(IT_ADD, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xC7}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x00) },
        { u8 {0xC8}, Instruction(IT_RET, AM_IMP,  RT_NONE, RT_NONE, CT_Z, 0) },
        { u8 {0xC9}, Instruction(IT_RET, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0xCA}, Instruction(IT_JP, AM_D16,  RT_NONE, RT_NONE, CT_Z, 0) },
        { u8 {0xCB}, Instruction(IT_CB, AM_D8,  RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0xCC}, Instruction(IT_CALL, AM_D16,  RT_NONE, RT_NONE, CT_Z, 0) },
        { u8 {0xCD}, Instruction(IT_CALL, AM_D16,  RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0xCE}, Instruction(IT_ADC, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xCF}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x08) },

        { u8 {0xD0}, Instruction(IT_RET, AM_IMP,  RT_NONE, RT_NONE, CT_NC, 0) },
        { u8 {0xD1}, Instruction(IT_POP, AM_IMP,  RT_DE, RT_NONE, CT_NONE, 0) },
        { u8 {0xD2}, Instruction(IT_JP, AM_D16,  RT_NONE, RT_NONE, CT_NC, 0) },
        { u8 {0xD4}, Instruction(IT_CALL, AM_D16,  RT_NONE, RT_NONE, CT_NC, 0) },
        { u8 {0xD5}, Instruction(IT_PUSH, AM_R,  RT_DE, RT_NONE, CT_NONE, 0) },
        { u8 {0xD6}, Instruction(IT_SUB, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xD7}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x10) },
        { u8 {0xD8}, Instruction(IT_RET, AM_IMP,  RT_NONE, RT_NONE, CT_C, 0) },
        { u8 {0xD9}, Instruction(IT_RETI, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0xDA}, Instruction(IT_JP, AM_D16,  RT_NONE, RT_NONE, CT_C, 0) },
        { u8 {0xDC}, Instruction(IT_CALL, AM_D16,  RT_NONE, RT_NONE, CT_C, 0) },
        { u8 {0xDE}, Instruction(IT_SBC, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xDF}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x18) },

        { u8 {0xE0}, Instruction(IT_LDH, AM_A8_R, RT_NONE, RT_A, CT_NONE, 0) },
        { u8 {0xE1}, Instruction(IT_POP, AM_IMP,  RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0xE2}, Instruction(IT_LD, AM_MR_R,  RT_C, RT_A, CT_NONE, 0) },
        { u8 {0xE5}, Instruction(IT_PUSH, AM_R,  RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0xE6}, Instruction(IT_AND, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xE7}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x20) },
        { u8 {0xE8}, Instruction(IT_ADD, AM_R_D8,  RT_SP, RT_NONE, CT_NONE, 0) },
        { u8 {0xE9}, Instruction(IT_JP, AM_R,  RT_HL, RT_NONE, CT_NONE, 0) },
        { u8 {0xEA}, Instruction(IT_LD, AM_A16_R,  RT_NONE, RT_A, CT_NONE, 0) },
        { u8 {0xEE}, Instruction(IT_XOR, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xEF}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x28) },

        { u8 {0xF0}, Instruction(IT_LDH, AM_R_A8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xF1}, Instruction(IT_POP, AM_IMP,  RT_AF, RT_NONE, CT_NONE, 0) },
        { u8 {0xF2}, Instruction(IT_LD, AM_R_MR,  RT_A, RT_C, CT_NONE, 0) },
        { u8 {0xF3}, Instruction(IT_DI, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0) },
        { u8 {0xF5}, Instruction(IT_PUSH, AM_R,  RT_AF, RT_NONE, CT_NONE, 0) },
        { u8 {0xF6}, Instruction(IT_OR, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xF7}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x30) },
        { u8 {0xF8}, Instruction(IT_LD, AM_HL_SPR,  RT_HL, RT_SP, CT_NONE, 0) },
        { u8 {0xF9}, Instruction(IT_LD, AM_R_R,  RT_SP, RT_HL, CT_NONE, 0) },
        { u8 {0xFA}, Instruction(IT_LD, AM_R_A16,  RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xFB}, Instruction(IT_EI, AM_IMP,  RT_NONE, RT_A, CT_NONE, 0) },
        { u8 {0xFE}, Instruction(IT_CP, AM_R_D8, RT_A, RT_NONE, CT_NONE, 0) },
        { u8 {0xFF}, Instruction(IT_RST, AM_IMP,  RT_NONE, RT_NONE, CT_NONE, 0x38) },



    };
	

public:
	CPUContext ctx;

    bool stepping = false;
    bool debug_logging = false;
    void update_logfile();

	void init();
	void connect(Bus* b, Timer* t, PPU* p, LCD* l);
	void step();
    void print_pc();
    void print_context();
	void fetch_instruction();
	void fetch_data();
	void execute();

	void increment_cycles(int cycles);

	void int_handle(u16 address);
	void request_interrupt(InterruptType);
    bool interrupt_check(u16 address, InterruptType type);
	void handle_interrupts();

	CPU() { init(); }
};

