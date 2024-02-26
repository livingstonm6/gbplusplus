#pragma once
#include "CPUContext.h"
#include "Bus.h"
#include <map>
#include "instructions.h"
class InstructionExecutor
{
public:

	std::map<InstructionType, void (InstructionExecutor::*)(CPUContext*, Bus*)> processor_functions = {
		{IT_LD,   &InstructionExecutor::proc_ld   },
		{IT_JP,   &InstructionExecutor::proc_jp   },
		{IT_JR,   &InstructionExecutor::proc_jr   },
		{IT_CALL, &InstructionExecutor::proc_call },
		{IT_NOP , &InstructionExecutor::proc_nop	 },
		{IT_ADC,  &InstructionExecutor::proc_adc	 },
		{IT_DI,   &InstructionExecutor::proc_di	 },
		{IT_XOR,  &InstructionExecutor::proc_xor	 },
		{IT_LDH,  &InstructionExecutor::proc_ldh	 },
		{IT_POP,  &InstructionExecutor::proc_pop	 },
		{IT_PUSH, &InstructionExecutor::proc_push },
		{IT_RET,  &InstructionExecutor::proc_ret	 },
		{IT_RETI, &InstructionExecutor::proc_reti },
		{IT_RST,  &InstructionExecutor::proc_rst	},
		{IT_INC,  &InstructionExecutor::proc_inc	 },
		{IT_DEC,  &InstructionExecutor::proc_dec	 },
		{IT_ADD,  &InstructionExecutor::proc_add	 },
		{IT_SUB,  &InstructionExecutor::proc_sub	 },
		{IT_SBC,  &InstructionExecutor::proc_sbc	 },
		{IT_AND,  &InstructionExecutor::proc_and	 },
		{IT_OR,   &InstructionExecutor::proc_or   },
		{IT_CP,   &InstructionExecutor::proc_cp	 },
		{IT_CB,   &InstructionExecutor::proc_cb	 },
		{IT_RRCA, &InstructionExecutor::proc_rrca },
		{IT_RLCA, &InstructionExecutor::proc_rlca },
		{IT_RLA,  &InstructionExecutor::proc_rla  },
		{IT_RRA,  &InstructionExecutor::proc_rra  },
		{IT_STOP, &InstructionExecutor::proc_stop },
		{IT_HALT, &InstructionExecutor::proc_halt },
		{IT_DAA,  &InstructionExecutor::proc_daa	 },
		{IT_SCF,  &InstructionExecutor::proc_scf	 },
		{IT_CCF,  &InstructionExecutor::proc_ccf  },
		{IT_EI,   &InstructionExecutor::proc_ei	 },
		{IT_CPL,  &InstructionExecutor::proc_cpl  },
	};

	void execute_instruction(CPUContext* cpu, Bus* bus);


	void stack_push(CPUContext* cpu, Bus* bus, u8 data);
	void stack_push16(CPUContext* cpu, Bus* bus, u16 data);

	u8 stack_pop(CPUContext* cpu, Bus* bus);
	u16 stack_pop16(CPUContext* cpu, Bus* bus);

	bool check_condition(CPUContext* cpu);
	void go_to(CPUContext* cpu, Bus* bus, u16 address, bool push_pc);

	void proc_ld(CPUContext* cpu, Bus* bus);
	void proc_jp(CPUContext* cpu, Bus* bus);
	void proc_jr(CPUContext* cpu, Bus* bus);
	void proc_call(CPUContext* cpu, Bus* bus);
	void proc_nop(CPUContext* cpu, Bus* bus);
	void proc_adc(CPUContext* cpu, Bus* bus);
	void proc_di(CPUContext* cpu, Bus* bus);
	void proc_xor(CPUContext* cpu, Bus* bus);
	void proc_ldh(CPUContext* cpu, Bus* bus);
	void proc_pop(CPUContext* cpu, Bus* bus);
	void proc_push(CPUContext* cpu, Bus* bus);
	void proc_ret(CPUContext* cpu, Bus* bus);
	void proc_reti(CPUContext* cpu, Bus* bus);
	void proc_rst(CPUContext* cpu, Bus* bus);
	void proc_inc(CPUContext* cpu, Bus* bus);
	void proc_dec(CPUContext* cpu, Bus* bus);
	void proc_add(CPUContext* cpu, Bus* bus);
	void proc_sub(CPUContext* cpu, Bus* bus);
	void proc_sbc(CPUContext* cpu, Bus* bus);
	void proc_and(CPUContext* cpu, Bus* bus);
	void proc_or(CPUContext* cpu, Bus* bus);
	void proc_cp(CPUContext* cpu, Bus* bus);
	void proc_cb(CPUContext* cpu, Bus* bus);
	void proc_rrca(CPUContext* cpu, Bus* bus);
	void proc_rlca(CPUContext* cpu, Bus* bus);
	void proc_rla(CPUContext* cpu, Bus* bus);
	void proc_rra(CPUContext* cpu, Bus* bus);
	void proc_stop(CPUContext* cpu, Bus* bus);
	void proc_halt(CPUContext* cpu, Bus* bus);
	void proc_daa(CPUContext* cpu, Bus* bus);
	void proc_scf(CPUContext* cpu, Bus* bus);
	void proc_ccf(CPUContext* cpu, Bus* bus);
	void proc_ei(CPUContext* cpu, Bus* bus);
	void proc_cpl(CPUContext* cpu, Bus* bus);

	InstructionExecutor(){}
};

RegisterType u8_to_rt(u8 num);