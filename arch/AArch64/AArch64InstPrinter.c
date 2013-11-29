//==-- AArch64InstPrinter.cpp - Convert AArch64 MCInst to assembly syntax --==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an AArch64 MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

/* Capstone Disassembler Engine */
/* By Nguyen Anh Quynh <aquynh@gmail.com>, 2013> */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "AArch64InstPrinter.h"
#include "AArch64BaseInfo.h"
#include "../../MCInst.h"
#include "../../SStream.h"
#include "../../MCRegisterInfo.h"
#include "../../MathExtras.h"
#include "../../utils.h"

#include "mapping.h"

static char *getRegisterName(unsigned RegNo);
static void printOperand(MCInst *MI, unsigned OpNo, SStream *O);

// FIXME: make this status session's specific, not global like this
static bool doing_mem = false;
static void set_mem_access(MCInst *MI, bool status)
{
	doing_mem = status;
	if (doing_mem) {
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_MEM;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.base = ARM64_REG_INVALID;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.index = ARM64_REG_INVALID;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.disp = 0;
	} else {
		// done, create the next operand slot
		MI->pub_insn.arm64.op_count++;
	}
}

static int64_t unpackSignedImm(int BitWidth, uint64_t Value)
{
	//assert(!(Value & ~((1ULL << BitWidth)-1)) && "immediate not n-bit");
	if (Value & (1ULL <<  (BitWidth - 1)))
		return (int64_t)Value - (1LL << BitWidth);
	else
		return Value;
}

static void printOffsetSImm9Operand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MOImm = MCInst_getOperand(MI, OpNum);
	int32_t Imm = unpackSignedImm(9, MCOperand_getImm(MOImm));

	SStream_concat(O, "#0x%x", Imm);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Imm;
	MI->pub_insn.arm64.op_count++;
}

static void printAddrRegExtendOperand(MCInst *MI, unsigned OpNum,
		SStream *O, unsigned MemSize, unsigned RmSize)
{
	unsigned ExtImm = MCOperand_getImm(MCInst_getOperand(MI, OpNum));
	unsigned OptionHi = ExtImm >> 1;
	unsigned S = ExtImm & 1;
	bool IsLSL = OptionHi == 1 && RmSize == 64;

	char *Ext = 0;
	switch (OptionHi) {
		case 1:
			if (RmSize == 32) {
				Ext = "uxtw";
				MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].ext = ARM64_EXT_UXTW;
			} else {
				Ext = "lsl";
			}
			break;
		case 3:
			if (RmSize == 32) {
				Ext = "sxtw";
				MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].ext = ARM64_EXT_SXTW;
			} else {
				Ext = "sxtx";
				MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].ext = ARM64_EXT_SXTX;
			}
			break;
		default:
			break; //llvm_unreachable("Incorrect Option on load/store (reg offset)");
	}
	SStream_concat(O, Ext);

	if (S) {
		unsigned ShiftAmt = Log2_32(MemSize);
		SStream_concat(O, " #0x%x", ShiftAmt);
		if (doing_mem) {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].shift.type = ARM64_SFT_LSL;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].shift.value = ShiftAmt;
		} else {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_LSL;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = ShiftAmt;
		}
	} else if (IsLSL) {
		SStream_concat(O, " #0");
	}
}

static void printAddSubImmLSL0Operand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *Imm12Op = MCInst_getOperand(MI, OpNum);

	if (MCOperand_isImm(Imm12Op)) {
		int64_t Imm12 = MCOperand_getImm(Imm12Op);
		//assert(Imm12 >= 0 && "Invalid immediate for add/sub imm");
		SStream_concat(O, "#0x%"PRIx64, Imm12);
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Imm12;
		MI->pub_insn.arm64.op_count++;
	}
}

static void printAddSubImmLSL12Operand(MCInst *MI, unsigned OpNum, SStream *O)
{
	printAddSubImmLSL0Operand(MI, OpNum, O);

	SStream_concat(O, ", lsl #12");
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_LSL;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = 12;
}

static void printBareImmOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);
	SStream_concat(O, "0x%"PRIx64, MCOperand_getImm(MO));
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = MCOperand_getImm(MO);
	MI->pub_insn.arm64.op_count++;
}

static void printBFILSBOperand(MCInst *MI, unsigned OpNum,
		SStream *O, unsigned RegWidth)
{
	MCOperand *ImmROp = MCInst_getOperand(MI, OpNum);
	unsigned LSB = MCOperand_getImm(ImmROp) == 0 ? 0 : RegWidth - MCOperand_getImm(ImmROp);

	SStream_concat(O, "#0x%x", LSB);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = LSB;
	MI->pub_insn.arm64.op_count++;
}

static void printBFIWidthOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *ImmSOp = MCInst_getOperand(MI, OpNum);
	unsigned Width = MCOperand_getImm(ImmSOp) + 1;

	SStream_concat(O, "#0x%x", Width);
}

static void printBFXWidthOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *ImmSOp = MCInst_getOperand(MI, OpNum);
	MCOperand *ImmROp = MCInst_getOperand(MI, OpNum - 1);

	unsigned ImmR = MCOperand_getImm(ImmROp);
	unsigned ImmS = MCOperand_getImm(ImmSOp);

	//assert(ImmS >= ImmR && "Invalid ImmR, ImmS combination for bitfield extract");

	SStream_concat(O, "#0x%x", (ImmS - ImmR + 1));

	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = ImmS - ImmR + 1;
	MI->pub_insn.arm64.op_count++;
}

static void printCRxOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *CRx = MCInst_getOperand(MI, OpNum);
	SStream_concat(O, "c%"PRIu64, MCOperand_getImm(CRx));

	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_CIMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = MCOperand_getImm(CRx);
	MI->pub_insn.arm64.op_count++;
}

static void printCVTFixedPosOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *ScaleOp = MCInst_getOperand(MI, OpNum);

	SStream_concat(O, "#0x%"PRIx64, (64 - MCOperand_getImm(ScaleOp)));
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = 64 - MCOperand_getImm(ScaleOp);
	MI->pub_insn.arm64.op_count++;
}

static void printFPImmOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MOImm8 = MCInst_getOperand(MI, OpNum);

	//assert(MOImm8.isImm()
	//       && "Immediate operand required for floating-point immediate inst");

	uint32_t Imm8 = MCOperand_getImm(MOImm8);
	uint32_t Fraction = Imm8 & 0xf;
	uint32_t Exponent = (Imm8 >> 4) & 0x7;
	uint32_t Negative = (Imm8 >> 7) & 0x1;

	float Val = 1.0f + Fraction / 16.0f;

	// That is:
	// 000 -> 2^1,  001 -> 2^2,  010 -> 2^3,  011 -> 2^4,
	// 100 -> 2^-3, 101 -> 2^-2, 110 -> 2^-1, 111 -> 2^0
	if (Exponent & 0x4) {
		Val /= 1 << (7 - Exponent);
	} else {
		Val *= 1 << (Exponent + 1);
	}

	Val = Negative ? -Val : Val;

	//o << '#' << format("%.8f", Val);
	SStream_concat(O, "#%.8f", Val);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_FP;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].fp = Val;
	MI->pub_insn.arm64.op_count++;
}

static void printFPZeroOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	SStream_concat(O, "#0.0");
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_FP;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].fp = 0;
	MI->pub_insn.arm64.op_count++;
}

static void printCondCodeOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);
	SStream_concat(O, A64CondCodeToString((A64CC_CondCodes)(MCOperand_getImm(MO))));
	MI->pub_insn.arm64.cc = MCOperand_getImm(MO) + 1;
}

static void printLabelOperand(MCInst *MI, unsigned OpNum,
		SStream *O, unsigned field_width, unsigned scale)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);

	if (!MCOperand_isImm(MO)) {
		printOperand(MI, OpNum, O);
		return;
	}

	// The immediate of LDR (lit) instructions is a signed 19-bit immediate, which
	// is multiplied by 4 (because all A64 instructions are 32-bits wide).
	uint64_t UImm = MCOperand_getImm(MO);
	uint64_t Sign = UImm & (1LL << (field_width - 1));
	int64_t SImm = scale * ((UImm & ~Sign) - Sign);

	// this is a relative address, so add with the offset
	// of current instruction
	SImm += MI->pub_insn.address;

	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = SImm;
	MI->pub_insn.arm64.op_count++;

	SStream_concat(O, "#0x%"PRIx64, SImm);
}

static void printLogicalImmOperand(MCInst *MI, unsigned OpNum,
		SStream *O, unsigned RegWidth)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);
	uint64_t Val;
	A64Imms_isLogicalImmBits(RegWidth, MCOperand_getImm(MO), &Val);
	SStream_concat(O, "#0x%"PRIx64, Val);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Val;
	MI->pub_insn.arm64.op_count++;
}

static void printOffsetUImm12Operand(MCInst *MI, unsigned OpNum,
		SStream *O, int MemSize)
{
	MCOperand *MOImm = MCInst_getOperand(MI, OpNum);

	if (MCOperand_isImm(MOImm)) {
		uint32_t Imm = MCOperand_getImm(MOImm) * MemSize;

		SStream_concat(O, "#0x%x", Imm);

		if (doing_mem) {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.disp = Imm;
		} else {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Imm;
			MI->pub_insn.arm64.op_count++;
		}
	}
}

static void printShiftOperand(MCInst *MI,  unsigned OpNum,
		SStream *O, A64SE_ShiftExtSpecifiers Shift)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);

	// LSL #0 is not printed
	if (Shift == A64SE_LSL && MCOperand_isImm(MO) && MCOperand_getImm(MO) == 0)
		return;

	switch (Shift) {
		case A64SE_LSL: SStream_concat(O, "lsl"); break;
		case A64SE_LSR: SStream_concat(O, "lsr"); break;
		case A64SE_ASR: SStream_concat(O, "asr"); break;
		case A64SE_ROR: SStream_concat(O, "ror"); break;
		default: break; // llvm_unreachable("Invalid shift specifier in logical instruction");
	}

	SStream_concat(O, " #0x%"PRIx64, MCOperand_getImm(MO));
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = Shift + 1;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = MCOperand_getImm(MO);
}

static void printMoveWideImmOperand(MCInst *MI,  unsigned OpNum, SStream *O)
{
	MCOperand *UImm16MO = MCInst_getOperand(MI, OpNum);
	MCOperand *ShiftMO = MCInst_getOperand(MI, OpNum + 1);

	if (MCOperand_isImm(UImm16MO)) {
		SStream_concat(O, "#0x%"PRIx64, MCOperand_getImm(UImm16MO));
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = MCOperand_getImm(UImm16MO);
		MI->pub_insn.arm64.op_count++;

		if (MCOperand_getImm(ShiftMO) != 0) {
			SStream_concat(O, ", lsl #0x%"PRIx64, (MCOperand_getImm(ShiftMO) * 16));
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_LSL;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = MCOperand_getImm(ShiftMO) * 16;
		}

		return;
	}
}

static void printNamedImmOperand(NamedImmMapper *Mapper,
		MCInst *MI, unsigned OpNum, SStream *O)
{
	bool ValidName;
	MCOperand *MO = MCInst_getOperand(MI, OpNum);
	char *Name = NamedImmMapper_toString(Mapper, MCOperand_getImm(MO), &ValidName);

	if (ValidName)
		SStream_concat(O, Name);
	else
		SStream_concat(O, "#0x%"PRIx64, MCOperand_getImm(MO));

	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = MCOperand_getImm(MO);
	MI->pub_insn.arm64.op_count++;
}

static void printSysRegOperand(SysRegMapper *Mapper,
		MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);

	bool ValidName;
	char *Name = SysRegMapper_toString(Mapper, MCOperand_getImm(MO), &ValidName);
	if (ValidName) {
		SStream_concat(O, Name);
	}
	free(Name);
}

#define GET_REGINFO_ENUM
#include "AArch64GenRegisterInfo.inc"

static inline bool isStackReg(unsigned RegNo)
{
	return RegNo == AArch64_XSP || RegNo == AArch64_WSP;
}

static void printRegExtendOperand(MCInst *MI, unsigned OpNum, SStream *O,
		A64SE_ShiftExtSpecifiers Ext)
{
	// FIXME: In principle TableGen should be able to detect this itself far more
	// easily. We will only accumulate more of these hacks.
	unsigned Reg0 = MCOperand_getReg(MCInst_getOperand(MI, 0));
	unsigned Reg1 = MCOperand_getReg(MCInst_getOperand(MI, 1));

	if (isStackReg(Reg0) || isStackReg(Reg1)) {
		A64SE_ShiftExtSpecifiers LSLEquiv;

		if (Reg0 == AArch64_XSP || Reg1 == AArch64_XSP)
			LSLEquiv = A64SE_UXTX;
		else
			LSLEquiv = A64SE_UXTW;

		if (Ext == LSLEquiv) {
			SStream_concat(O, "lsl #0x%"PRIx64, MCOperand_getImm(MCInst_getOperand(MI, OpNum)));
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_LSL;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = MCOperand_getImm(MCInst_getOperand(MI, OpNum));
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].ext = Ext - 4;
			return;
		}
	}

	switch (Ext) {
		case A64SE_UXTB: SStream_concat(O, "uxtb"); break;
		case A64SE_UXTH: SStream_concat(O, "uxth"); break;
		case A64SE_UXTW: SStream_concat(O, "uxtw"); break;
		case A64SE_UXTX: SStream_concat(O, "uxtx"); break;
		case A64SE_SXTB: SStream_concat(O, "sxtb"); break;
		case A64SE_SXTH: SStream_concat(O, "sxth"); break;
		case A64SE_SXTW: SStream_concat(O, "sxtw"); break;
		case A64SE_SXTX: SStream_concat(O, "sxtx"); break;
		default: break; //llvm_unreachable("Unexpected shift type for printing");
	}

	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].ext = Ext - 4;
	MCOperand *MO = MCInst_getOperand(MI, OpNum);
	if (MCOperand_getImm(MO) != 0) {
		SStream_concat(O, " #0x%"PRIx64, MCOperand_getImm(MO));
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_LSL;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = MCOperand_getImm(MO);
	}
}

static void printSImm7ScaledOperand(MCInst *MI, unsigned OpNum,
		SStream *O, int MemScale)
{
	MCOperand *MOImm = MCInst_getOperand(MI, OpNum);
	int32_t Imm = unpackSignedImm(7, MCOperand_getImm(MOImm));

	SStream_concat(O, "#0x%x", (Imm * MemScale));
	if (doing_mem) {
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.disp = Imm * MemScale;
	} else {
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Imm * MemScale;
		MI->pub_insn.arm64.op_count++;
	}
}

// TODO: handle this Vd register??
static void printVPRRegister(MCInst *MI, unsigned OpNo, SStream *O)
{
	unsigned Reg = MCOperand_getReg(MCInst_getOperand(MI, OpNo));
	char *Name = strdup(getRegisterName(Reg));
	Name[0] = 'v';
	SStream_concat(O, "%s", Name);
	free(Name);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_REG;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].reg = Reg;
	MI->pub_insn.arm64.op_count++;
}

static void printOperand(MCInst *MI, unsigned OpNo, SStream *O)
{
	MCOperand *Op = MCInst_getOperand(MI, OpNo);
	if (MCOperand_isReg(Op)) {
		unsigned Reg = MCOperand_getReg(Op);
		SStream_concat(O, getRegisterName(Reg));
		if (doing_mem) {
			if (MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.base == ARM64_REG_INVALID) {
				MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.base = Reg;
			} else {
				MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.index = Reg;
			}
		} else {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_REG;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].reg = Reg;
			MI->pub_insn.arm64.op_count++;
		}
	} else if (MCOperand_isImm(Op)) {
		int64_t imm = MCOperand_getImm(Op);
		SStream_concat(O, "#0x%"PRIx64, imm);
		if (doing_mem) {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.disp = imm;
		} else {
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
			MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = imm;
			MI->pub_insn.arm64.op_count++;
		}
	}
}

#define GET_INSTRINFO_ENUM
#include "AArch64GenInstrInfo.inc"

static void printNeonMovImmShiftOperand(MCInst *MI, unsigned OpNum,
		SStream *O, A64SE_ShiftExtSpecifiers Ext, bool isHalf)
{
	MCOperand *MO = MCInst_getOperand(MI, OpNum);

	//assert(MO.isImm() &&
	//       "Immediate operand required for Neon vector immediate inst.");

	bool IsLSL = false;
	if (Ext == A64SE_LSL)
		IsLSL = true;
	else if (Ext != A64SE_MSL) {
		//llvm_unreachable("Invalid shift specifier in movi instruction");
	}

	int64_t Imm = MCOperand_getImm(MO);

	// MSL and LSLH accepts encoded shift amount 0 or 1.
	if ((!IsLSL || (IsLSL && isHalf)) && Imm != 0 && Imm != 1) {
		// llvm_unreachable("Invalid shift amount in movi instruction");
	}

	// LSH accepts encoded shift amount 0, 1, 2 or 3.
	if (IsLSL && (Imm < 0 || Imm > 3)) {
		//llvm_unreachable("Invalid shift amount in movi instruction");
	}

	// Print shift amount as multiple of 8 with MSL encoded shift amount
	// 0 and 1 printed as 8 and 16.
	if (!IsLSL)
		Imm++;
	Imm *= 8;

	// LSL #0 is not printed
	if (IsLSL) {
		if (Imm == 0)
			return;
		SStream_concat(O, ", lsl");
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_LSL;
	} else {
		SStream_concat(O, ", msl");
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.type = ARM64_SFT_MSL;
	}

	SStream_concat(O, " #0x%"PRIx64, Imm);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count - 1].shift.value = Imm;
}

static void printNeonUImm0Operand(MCInst *MI, unsigned OpNum, SStream *O)
{
	SStream_concat(O, "#0x0");
	// FIXME: vector ZERO
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = 0;
	MI->pub_insn.arm64.op_count++;
}

static void printNeonUImm8Operand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MOUImm = MCInst_getOperand(MI, OpNum);

	//assert(MOUImm.isImm() &&
	//       "Immediate operand required for Neon vector immediate inst.");

	unsigned Imm = MCOperand_getImm(MOUImm);

	SStream_concat(O, "#0x%"PRIx64, Imm);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Imm;
	MI->pub_insn.arm64.op_count++;
}

static void printNeonUImm8OperandBare(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MOUImm = MCInst_getOperand(MI, OpNum);

	//assert(MOUImm.isImm()
	//		&& "Immediate operand required for Neon vector immediate inst.");

	unsigned Imm = MCOperand_getImm(MOUImm);
	SStream_concat(O, "0x%x", Imm);

	if (doing_mem) {
		MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].mem.disp = Imm;
	} else {
		// FIXME: never has false branch??
	}
}

static void printNeonUImm64MaskOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	MCOperand *MOUImm8 = MCInst_getOperand(MI, OpNum);

	//assert(MOUImm8.isImm() &&
	//       "Immediate operand required for Neon vector immediate bytemask inst.");

	uint32_t UImm8 = MCOperand_getImm(MOUImm8);
	uint64_t Mask = 0;

	// Replicates 0x00 or 0xff byte in a 64-bit vector
	unsigned ByteNum;
	for (ByteNum = 0; ByteNum < 8; ++ByteNum) {
		if ((UImm8 >> ByteNum) & 1)
			Mask |= (uint64_t)0xff << (8 * ByteNum);
	}

	SStream_concat(O, "#0x%"PRIx64, Mask);
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].type = ARM64_OP_IMM;
	MI->pub_insn.arm64.operands[MI->pub_insn.arm64.op_count].imm = Mask;
	MI->pub_insn.arm64.op_count++;
}

static void printMRSOperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	printSysRegOperand(&AArch64_MRSMapper, MI, OpNum, O);
}

static void printMSROperand(MCInst *MI, unsigned OpNum, SStream *O)
{
	printSysRegOperand(&AArch64_MSRMapper, MI, OpNum, O);
}


#define PRINT_ALIAS_INSTR
#include "AArch64GenAsmWriter.inc"

void AArch64_post_printer(unsigned int insn, cs_insn *pub_insn, char *insn_asm)
{
	// check if this insn requests write-back
	if (strrchr(insn_asm, '!') != NULL)
		pub_insn->arm64.writeback = true;
}

void AArch64_printInst(MCInst *MI, SStream *O, void *Info)
{
	if (printAliasInstr(MI, O, Info)) {
		char *mnem = strdup(O->buffer);
		char *tab = strchr(mnem, '\t');
		if (tab) {
			*tab = '\0';
		}
		// reflect the new insn name (alias) in the opcode
		MCInst_setOpcodePub(MI, AArch64_map_insn(mnem));
		free(mnem);
	} else
		AArch64InstPrinter_printInstruction(MI, O);
}

