/* Capstone Disassembly Engine */
/* BPF Backend by david942j <david942j@gmail.com>, 2019 */

#ifndef CAPSTONE_BPF_H
#define CAPSTONE_BPF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"

#ifdef _MSC_VER
#pragma warning(disable:4201)
#endif

/// Operand type for instruction's operands
typedef enum bpf_op_type {
	BPF_OP_INVALID = 0,

	BPF_OP_REG,
	BPF_OP_IMM,
	BPF_OP_MEM
} bpf_op_type;

/// BPF registers
typedef enum bpf_reg {
	BPF_REG_INVALID = 0,

	///< cBPF
	BPF_REG_A,
	BPF_REG_X,

	///< eBPF
	BPF_REG_R0,
	BPF_REG_R1,
	BPF_REG_R2,
	BPF_REG_R3,
	BPF_REG_R4,
	BPF_REG_R5,
	BPF_REG_R6,
	BPF_REG_R7,
	BPF_REG_R8,
	BPF_REG_R9,
	BPF_REG_R10,

	BPF_REG_ENDING,		// <-- mark the end of the list or registers
} bpf_reg;

/// Instruction's operand referring to memory
/// This is associated with BPF_OP_MEM operand type above
typedef struct bpf_op_mem {
	bpf_reg base;	///< base register
	int32_t disp;	///< offset value
} bpf_op_mem;

/// Instruction operand
typedef struct cs_bpf_op {
	bpf_op_type type;
	union {
		uint8_t reg;	///< register value for REG operand
		uint64_t imm;	///< immediate value IMM operand
		bpf_op_mem mem;	///< base/index/scale/disp value for MEM operand
	};
} cs_bpf_op;

/// Instruction structure
typedef struct cs_bpf {
	uint8_t op_count;
	cs_bpf_op *operands;
} cs_bpf;

/// BPF instruction
typedef enum bpf_insn {
	BPF_INS_INVALID = 0,

	///< ALU
	BPF_INS_ADD,
	BPF_INS_SUB,
	BPF_INS_MUL,
	BPF_INS_DIV,
	BPF_INS_OR,
	BPF_INS_AND,
	BPF_INS_LSH,
	BPF_INS_RSH,
	BPF_INS_NEG,
	BPF_INS_MOD,
	BPF_INS_XOR,
	BPF_INS_MOV,	///< eBPF only
	BPF_INS_ARSH,	///< eBPF only

	///< ALU64, eBPF only
	BPF_INS_ADD64,
	BPF_INS_SUB64,
	BPF_INS_MUL64,
	BPF_INS_DIV64,
	BPF_INS_OR64,
	BPF_INS_AND64,
	BPF_INS_LSH64,
	BPF_INS_RSH64,
	BPF_INS_NEG64,
	BPF_INS_MOD64,
	BPF_INS_XOR64,
	BPF_INS_MOV64,
	BPF_INS_ARSH64,

	///< Byteswap, eBPF only
	BPF_INS_LE16,
	BPF_INS_LE32,
	BPF_INS_LE64,
	BPF_INS_BE16,
	BPF_INS_BE32,
	BPF_INS_BE64,

	///< Load
	BPF_INS_LDDW,		///< eBPF only, load 64-bit imm
	BPF_INS_LDABSW,
	BPF_INS_LDABSH,
	BPF_INS_LDABSB,
	BPF_INS_LDABSDW,	///< eBPF only
	BPF_INS_LDINDW,
	BPF_INS_LDINDH,
	BPF_INS_LDINDB,
	BPF_INS_LDINDDW,	///< eBPF only
	BPF_INS_LDXW,
	BPF_INS_LDXH,
	BPF_INS_LDXB,
	BPF_INS_LDXDW,		///< eBPF only

	///< Store
	BPF_INS_STW,
	BPF_INS_STH,
	BPF_INS_STB,
	BPF_INS_STDW,	///< eBPF only
	BPF_INS_STXW,
	BPF_INS_STXH,
	BPF_INS_STXB,
	BPF_INS_STXDW,	///< eBPF only

	///< Jump
	BPF_INS_JMP,
	BPF_INS_JEQ,
	BPF_INS_JGT,
	BPF_INS_JGE,
	BPF_INS_JSET,
	BPF_INS_JNE,	///< eBPF only
	BPF_INS_JSGT,	///< eBPF only
	BPF_INS_JSGE,	///< eBPF only
	BPF_INS_CALL,	///< eBPF only
	BPF_INS_EXIT,	///< eBPF only
	BPF_INS_JLT,	///< eBPF only
	BPF_INS_JLE,	///< eBPF only
	BPF_INS_JSLT,	///< eBPF only
	BPF_INS_JSLE,	///< eBPF only

	///< Return, cBPF only
	BPF_INS_RET,

	///< Misc, cBPF only
	BPF_INS_TAX,
	BPF_INS_TXA,

	BPF_INS_ENDING,
} bpf_insn;

/// Group of BPF instructions
typedef enum bpf_insn_group {
	BPF_GRP_INVALID = 0, ///< = CS_GRP_INVALID

	BPF_GRP_LOAD,
	BPF_GRP_STORE,
	BPF_GRP_ALU,
	BPF_GRP_JUMP,
	BPF_GRP_CALL, ///< eBPF only
	BPF_GRP_RETURN,
	BPF_GRP_MISC, ///< cBPF only

	BPF_GRP_ENDING,   ///< <-- mark the end of the list of groups
} bpf_insn_group;

#ifdef __cplusplus
}
#endif

#endif
