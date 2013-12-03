/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013> */

#ifndef CS_ARMDISASSEMBLER_H
#define CS_ARMDISASSEMBLER_H

#include "../../include/capstone.h"
#include "../../MCRegisterInfo.h"

void ARM_init(MCRegisterInfo *MRI);

bool ARM_getInstruction(csh handle, unsigned char *code, size_t code_len, MCInst *instr, uint16_t *size, uint64_t address, void *info);

bool Thumb_getInstruction(csh handle, unsigned char *code, size_t code_len, MCInst *instr, uint16_t *size, uint64_t address, void *info);

#endif
