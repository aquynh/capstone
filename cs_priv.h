/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013> */

#ifndef __CS_PRIV_H__
#define __CS_PRIV_H__

#include <capstone.h>

#include "MCInst.h"
#include "SStream.h"

typedef void (*Printer_t)(MCInst *MI, SStream *OS, void *info);

// function to be called after Printer_t
// this is the best time to gather insn's characteristics
typedef void (*PostPrinter_t)(unsigned int insn, cs_insn *, char *mnem);

typedef bool (*Disasm_t)(csh handle, unsigned char *code, size_t code_len, MCInst *instr, uint16_t *size, size_t address, void *info);

typedef char *(*GetName_t)(unsigned int reg);

typedef void (*GetID_t)(cs_insn *insn, unsigned int id);

typedef struct cs_struct {
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
	bool micro_mips;	// for Mips only
	cs_err errnum;
} cs_struct;

#endif
