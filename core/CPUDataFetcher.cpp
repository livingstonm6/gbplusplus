#include "CPUDataFetcher.h"

void CPUDataFetcher::fetch_instruction(CPUContext* ctx, Bus* bus) {
	u16 pc = ctx->reg.read(RT_PC);
	ctx->current_opcode = bus->read(pc);
	ctx->current_instruction = INSTRUCTIONS.at(ctx->current_opcode);
	ctx->reg.write(RT_PC, pc + 1);
}

void CPUDataFetcher::fetch_data(CPUContext* ctx, Bus* bus) {
	ctx->memory_destination = 0;
	ctx->writing_to_memory = false;
	(this->*fetch_functions[ctx->current_instruction.mode])(ctx, bus);
}

void CPUDataFetcher::d16_to_reg(CPUContext* ctx, Bus* bus) {
	d16(ctx, bus);
}
void CPUDataFetcher::reg_to_reg(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg2);
}
void CPUDataFetcher::reg_to_mr(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg2);
	ctx->memory_destination = ctx->reg.read(ctx->current_instruction.reg1);
	ctx->writing_to_memory = true;

	if (ctx->current_instruction.reg1 == RT_C) {
		ctx->memory_destination |= 0xFF00;
	}
}
void CPUDataFetcher::reg(CPUContext* ctx, Bus*) {
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg1);
}
void CPUDataFetcher::d8_to_reg(CPUContext* ctx, Bus* bus) {
	u16 pc = ctx->reg.read(RT_PC);
	ctx->fetched_data = bus->read(pc);
	ctx->cycles_to_increment++;
	ctx->reg.write(RT_PC, pc + 1);
}
void CPUDataFetcher::mr_to_reg(CPUContext* ctx, Bus* bus) {
	u16 address = ctx->reg.read(ctx->current_instruction.reg2);
	if (ctx->current_instruction.reg2 == RT_C) {
		address |= 0xFF00;
	}
	ctx->fetched_data = bus->read(address);
	ctx->cycles_to_increment++;
}
void CPUDataFetcher::hli_to_reg(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = bus->read(ctx->reg.read(ctx->current_instruction.reg2));
	ctx->cycles_to_increment++;
	ctx->reg.add(RT_HL, 1);
}
void CPUDataFetcher::hld_to_reg(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = bus->read(ctx->reg.read(ctx->current_instruction.reg2));
	ctx->cycles_to_increment++;
	ctx->reg.add(RT_HL, -1);
}
void CPUDataFetcher::reg_to_hli(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg2);
	ctx->memory_destination = ctx->reg.read(ctx->current_instruction.reg1);
	ctx->writing_to_memory = true;
	ctx->reg.add(RT_HL, 1);
}
void CPUDataFetcher::reg_to_hld(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg2);
	ctx->memory_destination = ctx->reg.read(ctx->current_instruction.reg1);
	ctx->writing_to_memory = true;
	ctx->reg.add(RT_HL, -1);
}
void CPUDataFetcher::a8_to_reg(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = bus->read(ctx->reg.read(RT_PC)) | 0xFF00;
	ctx->cycles_to_increment++;
	ctx->reg.add(RT_PC, 1);
}
void CPUDataFetcher::reg_to_a8(CPUContext* ctx, Bus* bus) {
	ctx->memory_destination = bus->read(ctx->reg.read(RT_PC)) | 0xFF00;
	ctx->writing_to_memory = true;
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg2);
	ctx->cycles_to_increment++;
	ctx->reg.add(RT_PC, 1);
}
void CPUDataFetcher::spr_to_hl(CPUContext* ctx, Bus* bus) {
	d8(ctx, bus);
}
void CPUDataFetcher::d16(CPUContext* ctx, Bus* bus) {
	u16 pc = ctx->reg.read(RT_PC);
	u8 low = bus->read(pc);
	ctx->cycles_to_increment++;
	u8 high = bus->read(pc + 1);
	ctx->cycles_to_increment++;
	ctx->fetched_data = (high << 8) | low;
	ctx->reg.write(RT_PC, pc + 2);
}
void CPUDataFetcher::d8(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = bus->read(ctx->reg.read(RT_PC));
	ctx->cycles_to_increment++;
	ctx->reg.add(RT_PC, 1);
}
void CPUDataFetcher::imp(CPUContext*, Bus*) {
	// Do nothing
}
void CPUDataFetcher::d8_to_mr(CPUContext* ctx, Bus* bus) {
	ctx->fetched_data = bus->read(ctx->reg.read(RT_PC));
	ctx->cycles_to_increment++;
	ctx->reg.add(RT_PC, 1);
	ctx->memory_destination = ctx->reg.read(ctx->current_instruction.reg1);
	ctx->writing_to_memory = true;
}
void CPUDataFetcher::mr(CPUContext* ctx, Bus* bus) {
	ctx->memory_destination = ctx->reg.read(ctx->current_instruction.reg1);
	ctx->writing_to_memory = true;
	ctx->fetched_data = bus->read(ctx->reg.read(ctx->current_instruction.reg1));
	ctx->cycles_to_increment++;
}
void CPUDataFetcher::reg_to_a16(CPUContext* ctx, Bus* bus) {
	u8 low = bus->read(ctx->reg.read(RT_PC));
	ctx->cycles_to_increment++;
	u8 high = bus->read(ctx->reg.read(RT_PC) + 1);
	ctx->cycles_to_increment++;
	ctx->memory_destination = (high << 8) | low;
	ctx->writing_to_memory = true;
	ctx->reg.add(RT_PC, 2);
	ctx->fetched_data = ctx->reg.read(ctx->current_instruction.reg2);
}
void CPUDataFetcher::a16_to_r(CPUContext* ctx, Bus* bus) {
	u8 low = bus->read(ctx->reg.read(RT_PC));
	ctx->cycles_to_increment++;
	u8 high = bus->read(ctx->reg.read(RT_PC) + 1);
	ctx->cycles_to_increment++;
	u16 address = (high << 8) | low;
	ctx->reg.add(RT_PC, 2);
	ctx->fetched_data = bus->read(address);
	ctx->cycles_to_increment++;
}

void CPUDataFetcher::init_map()
{
	fetch_functions[AM_R_D16] = &CPUDataFetcher::d16_to_reg;
	fetch_functions[AM_R_R] = &CPUDataFetcher::reg_to_reg;
	fetch_functions[AM_MR_R] = &CPUDataFetcher::reg_to_mr;
	fetch_functions[AM_R] = &CPUDataFetcher::reg;
	fetch_functions[AM_R_D8] = &CPUDataFetcher::d8_to_reg;
	fetch_functions[AM_R_MR] = &CPUDataFetcher::mr_to_reg;
	fetch_functions[AM_R_HLI] = &CPUDataFetcher::hli_to_reg;
	fetch_functions[AM_R_HLD] = &CPUDataFetcher::hld_to_reg;
	fetch_functions[AM_HLI_R] = &CPUDataFetcher::reg_to_hli;
	fetch_functions[AM_HLD_R] = &CPUDataFetcher::reg_to_hld;
	fetch_functions[AM_R_A8] = &CPUDataFetcher::a8_to_reg;
	fetch_functions[AM_A8_R] = &CPUDataFetcher::reg_to_a8;
	fetch_functions[AM_HL_SPR] = &CPUDataFetcher::spr_to_hl;
	fetch_functions[AM_D16] = &CPUDataFetcher::d16;
	fetch_functions[AM_D8] = &CPUDataFetcher::d8;
	fetch_functions[AM_IMP] = &CPUDataFetcher::imp;
	fetch_functions[AM_MR_D8] = &CPUDataFetcher::d8_to_mr;
	fetch_functions[AM_MR] = &CPUDataFetcher::mr;
	fetch_functions[AM_A16_R] = &CPUDataFetcher::reg_to_a16;
	fetch_functions[AM_R_A16] = &CPUDataFetcher::a16_to_r;

}
