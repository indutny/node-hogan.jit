#include "assembler.h"
#include "assembler-ia32.h"

#include <stdint.h> // uintXX_t

namespace hogan {

void Assembler::Push(int reg) {
  emit(0x50 | reg);
}


void Assembler::PushImm(uint32_t imm) {
  emit(0x68);
  Immediate(imm);
}


void Assembler::Pop(int reg) {
  emit(0x58 | reg);
}


void Assembler::Mov(int dst, int src) {
  emit(0x8b); // mov
  emit(0xc0 | dst << 3 | src);
}


void Assembler::MovToContext(uint8_t offset, int src) {
  emit(0x89); // mov [ebp+offset], src
  emit(0x45 | src << 3); // modrm
  Immediate(offset);
}


void Assembler::MovFromContext(int dst, uint8_t offset) {
  emit(0x8b); // mov dst, [ebp+offset]
  emit(0x45 | dst << 3); // modrm
  Immediate(offset);
}


void Assembler::MovImm(int dst, uint64_t imm) {
  emit(0xb8 | dst); // mov

  Immediate(static_cast<uint32_t>(imm));
}


void Assembler::AddImm(int dst, uint8_t imm) {
  if (imm == 0) return;

  emit(0x83);
  emit(0xc0 | dst);
  Immediate(imm);
}


void Assembler::AddImmToContext(int offset, uint32_t imm) {
  emit(0x81);
  emit(0x45); // modrm
  Immediate(static_cast<uint8_t>(offset));
  Immediate(imm);
}


void Assembler::AddToContext(int offset, int src) {
  emit(0x01);
  emit(0x45 | src << 3); // modrm
  Immediate(static_cast<uint8_t>(offset));
}


void Assembler::SubImm(int dst, uint8_t imm) {
  if (imm == 0) return;

  emit(0x83);
  emit(0xc0 | 0x05 << 3 | dst);
  Immediate(imm);
}


void Assembler::Inc(int dst) {
  emit(0xff); // xor
  emit(0xc0 | dst << 3);
}


void Assembler::Xor(int dst, int src) {
  emit(0x33); // xor
  emit(0xc0 | dst << 3 | src);
}


int Assembler::PreCall(int offset, int args) {
  int delta = 16 - (offset + args * 4) % 16;

  if (delta == 16) return args * 4;
  SubImm(esp, delta);

  return delta + args * 4;
}


void Assembler::Call(const void* addr) {
  MovImm(ecx, reinterpret_cast<const uint64_t>(addr));
  emit(0xff); // Call
  emit(0xc0 | 2 << 3 | ecx);
}


void Assembler::Leave() {
  emit(0xc9);
}


void Assembler::Return(uint16_t bytes) {
  if (bytes == 0) {
    emit(0xc3);
  } else {
    emit(0xc2);
    Immediate(bytes);
  }
}


void Assembler::Cmp(int src, uint32_t imm) {
  if (src == eax) {
    emit(0x3d);
    Immediate(imm);
  } else {
    assert(false && "Not implemented yet!");
  }
}


void Assembler::Je(Label* lbl) {
  emit(0x0f);
  emit(0x84);
  Offset(lbl, 4);
}


void Assembler::Jmp(Label* lbl) {
  emit(0xe9);
  Offset(lbl, 4);
}

} // namespace hogan
