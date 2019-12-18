#include "cpu/decode.h"
#include "rtl/rtl.h"

// decode operand helper
#define make_DopHelper(name) void concat(decode_op_, name) (Operand *op, uint32_t val, bool load_val)

static inline make_DopHelper(i) {
  op->type = OP_TYPE_IMM;
  op->imm = val;
  rtl_li(&op->val, op->imm);

  print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline make_DopHelper(r) {
  op->type = OP_TYPE_REG;
  op->reg = val;
  if (load_val) {
    rtl_lr(&op->val, op->reg, 4);
  }

  print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg, 4));
}

make_DHelper(U) {
  decode_op_i(id_src, decinfo.isa.instr.imm31_12 << 12, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);

  print_Dop(id_src->str, OP_STR_SIZE, "0x%x", decinfo.isa.instr.imm31_12);
}

make_DHelper(ld) {
  Instr instr = decinfo.isa.instr;
  s0 = (instr.funct3>>2) ? (unsigned)(instr.simm11_0): (signed)(instr.simm11_0);
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_i(id_src2, s0, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(st) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  int32_t simm = (decinfo.isa.instr.simm11_5 << 5) | decinfo.isa.instr.imm4_0;
  decode_op_i(id_src2, simm, true);

  print_Dop(id_src->str, OP_STR_SIZE, "%d(%s)", id_src2->val, reg_name(id_src->reg, 4));

  rtl_add(&id_src->addr, &id_src->val, &id_src2->val);

  decode_op_r(id_dest, decinfo.isa.instr.rs2, true);
}

make_DHelper(CALUI) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
  //TODO: maybe need add signed extend
  //TODO: ADD comment for all new DHelper

}

make_DHelper(AUP) {
  decode_op_i(id_src, decinfo.isa.instr.imm31_12 << 12, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(CALU) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(JAL) {
  Instr instr = decinfo.isa.instr;
  s0 = (instr.simm20<<20) + (instr.imm19_12<<12) + (instr.imm11_<<11) + (instr.imm10_1<<1);
  decode_op_i(id_src, s0, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(JALR) {
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_dest, decinfo.isa.instr.rd, false);
}

make_DHelper(BEQ) {
  Instr instr = decinfo.isa.instr;
  s0 = (instr.simm12<<12) + (instr.imm11<<11) + (instr.imm10_5<<5) + (instr.imm4_1<<1);
  decode_op_i(id_dest, s0, true);
  decode_op_r(id_src, decinfo.isa.instr.rs1, true);
  decode_op_r(id_src2, decinfo.isa.instr.rs2, true);
}

