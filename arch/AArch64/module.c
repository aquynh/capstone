/* Capstone Disassembler Engine */
/* By Dang Hoang Vu <danghvu@gmail.com> 2013 */

#include "../../cs_priv.h"
#include "../../MCRegisterInfo.h"
#include "AArch64Disassembler.h"
#include "AArch64InstPrinter.h"
#include "mapping.h"


static cs_err init(cs_struct *ud)
{
	MCRegisterInfo *mri = malloc(sizeof(*mri));

	AArch64_init(mri);
	ud->printer = AArch64_printInst;
	ud->printer_info = mri;
	ud->getinsn_info = mri;
	ud->disasm = AArch64_getInstruction;
	ud->reg_name = AArch64_reg_name;
	ud->insn_id = AArch64_get_insn_id;
	ud->insn_name = AArch64_insn_name;
	ud->post_printer = AArch64_post_printer;

	return CS_ERR_OK;
}

static cs_err option(cs_struct *handle, cs_opt_type type, size_t value)
{
	return CS_ERR_OK;
}

static void __attribute__ ((constructor)) __init_arm64__()
{
	arch_init[CS_ARCH_ARM64] = init;
	arch_option[CS_ARCH_ARM] = option;

	// support this arch
	all_arch |= (1 << CS_ARCH_ARM64);
}
