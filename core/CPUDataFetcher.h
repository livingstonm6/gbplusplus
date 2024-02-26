#pragma once
#include "CPUContext.h"
#include "Bus.h"
#include "instructions.h"
#include "utility/Common.h"
#include <map>
class CPUDataFetcher {
public:

	CPUDataFetcher() { init_map(); }

	void fetch_instruction(CPUContext* ctx, Bus* bus);

	/**
	* Fetch data for the CPU if needed given the current 
	* instruction's addressing mode.
	*
	* @param cpu: pointer to the CPUContext member of the CPU
	* @param bus: pointer to the memory bus
	*/
	void fetch_data(CPUContext* ctx, Bus* bus);
private:
	std::map <AddressingMode, void (CPUDataFetcher::*)(CPUContext*, Bus*)> fetch_functions;

	void d16_to_reg(CPUContext* ctx, Bus* bus);
	void reg_to_reg(CPUContext* ctx, Bus* bus);
	void reg_to_mr(CPUContext* ctx, Bus* bus);
	void reg(CPUContext* ctx, Bus* bus);
	void d8_to_reg(CPUContext* ctx, Bus* bus);
	void mr_to_reg(CPUContext* ctx, Bus* bus);
	void hli_to_reg(CPUContext* ctx, Bus* bus);
	void hld_to_reg(CPUContext* ctx, Bus* bus);
	void reg_to_hli(CPUContext* ctx, Bus* bus);
	void reg_to_hld(CPUContext* ctx, Bus* bus);
	void a8_to_reg(CPUContext* ctx, Bus* bus);
	void reg_to_a8(CPUContext* ctx, Bus* bus);
	void spr_to_hl(CPUContext* ctx, Bus* bus);
	void d16(CPUContext* ctx, Bus* bus);
	void d8(CPUContext* ctx, Bus* bus);
	void imp(CPUContext* ctx, Bus* bus);
	void d8_to_mr(CPUContext* ctx, Bus* bus);
	void mr(CPUContext* ctx, Bus* bus);
	void reg_to_a16(CPUContext* ctx, Bus* bus);
	void a16_to_r(CPUContext* ctx, Bus* bus);

	void init_map();

	
};