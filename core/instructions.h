#pragma once
#include "utility/Common.h"

enum InstructionType {
	IT_NOP = 1,
	IT_LD = 2,
	IT_INC = 3,
	IT_DEC = 4,
    IT_LD16 = 5,
    IT_JR = 6,
    IT_ADD = 7,
    IT_RRCA = 8,
    IT_RRA = 9,
    IT_CPL = 10,
    IT_CCF = 11,
    IT_STOP = 12,
    IT_HALT = 13,
    IT_ADC = 14,
    IT_SUB = 15,
    IT_SBC = 16,
    IT_AND = 17,
    IT_XOR = 18,
    IT_OR = 19,
    IT_CP = 20,
    IT_RET = 21,
    IT_RETI = 22,
    IT_POP = 23,
    IT_JP = 24,
    IT_CALL = 25,
    IT_RST = 26,
    IT_PREFIX = 27,
    IT_PUSH = 28,
    IT_LDH = 29,
    IT_DI = 30,
    IT_EI = 31,
    IT_CB = 32,
    IT_RLCA = 33,
    IT_RLA = 34,
    IT_DAA = 35,
    IT_SCF = 36
};

enum CPUFlagType {
    CFT_Z = 7,
    CFT_N = 6,
    CFT_H = 5,
    CFT_C = 4
};

enum ConditionType {
    CT_NONE = 1,
    CT_NZ = 2,
    CT_Z = 3,
    CT_NC = 4,
    CT_C = 5
};

enum AddressingMode {
    AM_R_D16 = 1,
    AM_R_R = 2,
    AM_MR_R = 3,
    AM_R = 4,
    AM_R_D8 = 5,
    AM_R_MR = 6,
    AM_R_HLI = 7,
    AM_R_HLD = 8,
    AM_HLI_R = 9,
    AM_HLD_R = 10,
    AM_R_A8 = 11,
    AM_A8_R = 12,
    AM_HL_SPR = 13,
    AM_D16 = 14,
    AM_D8 = 15,
    AM_IMP = 16,
    AM_D16_R = 17,
    AM_MR_D8 = 18,
    AM_MR = 19,
    AM_A16_R = 20,
    AM_R_A16 = 21
};

enum RegisterType {
    RT_NONE = 1,
    RT_A = 2,
    RT_F = 3,
    RT_B = 4,
    RT_C = 5,
    RT_D = 6,
    RT_E = 7,
    RT_H = 8,
    RT_L = 9,
    RT_AF = 10,
    RT_BC = 11,
    RT_DE = 12,
    RT_HL = 13,
    RT_SP = 14,
    RT_PC = 15
};

class Instruction {
public:
    InstructionType type;
    AddressingMode mode;
    RegisterType reg1;
    RegisterType reg2;
    ConditionType cond;
    u8 param;

    Instruction(InstructionType t, AddressingMode m, RegisterType r1, RegisterType r2, ConditionType c, u8 p):
        type(t), mode(m), reg1(r1), reg2(r2), cond(c), param(p) {}

    Instruction() {
        type = IT_NOP;
        mode = AM_IMP;
        reg1 = RT_NONE;
        reg2 = RT_NONE;
        cond = CT_NONE;
        param = 0;
    }

};


