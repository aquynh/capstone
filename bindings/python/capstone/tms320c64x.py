# Capstone Python bindings, by Fotis Loukos <me@fotisl.com>

import ctypes, copy
from .tms320c64x_const import *

# define the API
class TMS320C64xOpMem(ctypes.Structure):
    _fields_ = (
        ('base', ctypes.c_int),
        ('disp', ctypes.c_uint16),
        ('unit', ctypes.c_uint8),
        ('scaled', ctypes.c_bool),
        ('disptype', ctypes.c_int),
        ('direction', ctypes.c_int),
        ('modify', ctypes.c_int),
    )

class TMS320C64xOpValue(ctypes.Union):
    _fields_ = (
        ('reg', ctypes.c_int),
        ('imm', ctypes.c_int32),
        ('mem', TMS320C64xOpMem),
    )

class TMS320C64xCondition(ctypes.Structure):
    _fields_ = (
        ('reg', ctypes.c_int),
        ('zero', ctypes.c_bool),
    )

class TMS320C64xFunctionalUnit(ctypes.Structure):
    _fields_ = (
        ('unit', ctypes.c_int),
        ('side', ctypes.c_uint8),
        ('crosspath', ctypes.c_int8),
    )

class TMS320C64xOp(ctypes.Structure):
    _fields_ = (
        ('type', ctypes.c_int),
        ('value', TMS320C64xOpValue),
    )

    @property
    def imm(self):
        return self.value.imm

    @property
    def reg(self):
        return self.value.reg

    @property
    def mem(self):
        return self.value.mem

class CsTMS320C64x(ctypes.Structure):
    _fields_ = (
        ('op_count', ctypes.c_uint8),
        ('operands', TMS320C64xOp * 8),
        ('condition', TMS320C64xCondition),
        ('funit', TMS320C64xFunctionalUnit),
        ('parallel', ctypes.c_int8),
    )

def get_arch_info(a):
    return (a.condition, a.funit, a.parallel, copy.deepcopy(a.operands[:a.op_count]))
