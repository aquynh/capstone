#ifndef CAPSTONE_PRIV_H_CB1B104E570B477A82CE5CF84602B41C
#define CAPSTONE_PRIV_H_CB1B104E570B477A82CE5CF84602B41C

/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013> */

#include <capstone.h>

#include "MCInst.h"
#include "SStream.h"

typedef void (*Printer_t)(MCInst *MI, SStream *OS, void *info);

// function to be called after Printer_t
// this is the best time to gather insn's characteristics
typedef void (*PostPrinter_t)(csh handle, cs_insn *, char *mnem);

typedef bool (*Disasm_t)(csh handle, const uint8_t *code, size_t code_len, MCInst *instr, uint16_t *size, uint64_t address, void *info);

typedef const char *(*GetName_t)(csh handle, unsigned int reg);

typedef void (*GetID_t)(cs_insn *insn, unsigned int id, int detail);

// for ARM only
typedef struct ARM_ITStatus {
	unsigned char ITStates[128];	// FIXME
	unsigned int size;
} ARM_ITStatus;

struct cs_struct {
	cs_arch arch;
	cs_mode mode;
	Printer_t printer;	// asm printer
	void *printer_info; // aux info for printer
	Disasm_t disasm;	// disassembler
	void *getinsn_info; // auxiliary info for printer
	bool big_endian;
	GetName_t reg_name;
	GetName_t insn_name;
	GetID_t insn_id;
	PostPrinter_t post_printer;
	cs_err errnum;
	ARM_ITStatus ITBlock;	// for Arm only
	cs_opt_value detail;
	int syntax;	// asm syntax for simple printer such as PPC
	bool doing_mem;	// handling memory operand in InstPrinter code
};

#define MAX_ARCH 8

// constructor initialization for all archs
extern cs_err (*arch_init[MAX_ARCH]) (cs_struct *);

// support cs_option() for all archs
extern cs_err (*arch_option[MAX_ARCH]) (cs_struct*, cs_opt_type, size_t value);

extern unsigned int all_arch;

#endif
