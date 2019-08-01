#pragma once

#include <types.h>

/*
 * Api - IDT
 */
extern void ih_zero();
extern void ih_opcode();
extern void ih_double_fault();
extern void ih_general_protect();
extern void ih_page_fault();
extern void ih_alignment_check();
extern void asm_ih_zero();
extern void asm_ih_opcode();
extern void asm_ih_double_fault();
extern void asm_ih_general_protect();
extern void asm_ih_page_fault();
extern void asm_ih_alignment_check();

/*
 * Api - IRQ
 */
extern void ih_timer(size_t *ret_addr, size_t *reg_addr);
extern void ih_keyboard();
extern void asm_ih_keyboard();
extern void asm_ih_timer(size_t *ret_addr);
