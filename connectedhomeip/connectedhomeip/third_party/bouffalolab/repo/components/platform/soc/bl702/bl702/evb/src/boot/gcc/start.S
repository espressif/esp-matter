// See LICENSE for license details.

/* This is defined in sifive/platform.h, but that can't be included from
 * assembly. */
#define CLINT_CTRL_ADDR 0x02000000
#define MSTATUS_FS      0x00006000
#define MSTATUS_MIE     0x00000008

	.section .init
	.globl bl702_start
	.type bl702_start,@function

bl702_start:
	.cfi_startproc
	.cfi_undefined ra
.option push
.option norelax
	la gp, __global_pointer$
.option pop
	la sp, _sp_main

	/*set em_sel to 0 before using sp*/
	li a0, 0x4000007C
	sw x0, (a0)

	/*disable IRQ*/
	li t0, MSTATUS_MIE
	csrc mstatus, t0

#if defined(ENABLE_SMP)
	smp_pause(t0, t1)
#endif

#ifndef RUN_IN_RAM
	call bl_sys_cache_config

	/* Load boot2 partition address */
	la a0, __boot2_pt_addr_src
	la a1, __boot2_pt_addr_start
	la a2, __boot2_pt_addr_end
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

#if 0
	/* Load boot2 flashCfg address */
	jal hal_boot2_get_flash_addr
	la a1, __boot2_flashCfg_start
	la a2, __boot2_flashCfg_end
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:
#endif

	/* Load tcm section */
	la a0, _tcm_load
	la a1, _tcm_run
	la a2, _tcm_run_end
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

	/* Load data section */
	la a0, _data_load
	la a1, _data_run
	la a2, _data_run_end
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:
#endif

	/* Load hbn section */
	la a0, _hbn_load
	la a1, _hbn_run
	la a2, _hbn_run_end
	bgeu a1, a2, 2f
1:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 1b
2:

	/* Fill system stack with 0x5a */
	la a0, __stack_size
	beq a0, x0, 2f
	la a1, _sp_main
	li t0, 0x5a5a5a5a
1:
	sw t0, -4(a1)
	addi a1, a1, -4
	addi a0, a0, -4
	bne a0, x0, 1b
2:

	/* Clear bss section */
	la a0, __bss_start
	la a1, __bss_end
	bgeu a0, a1, 3f
1:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 1b

3:

	/* Call global constructors */
#if 0
	la a0, __libc_fini_array
	call atexit
	call __libc_init_array
#endif

#ifndef __riscv_float_abi_soft
	/* Enable FPU */
	li t0, MSTATUS_FS
	csrs mstatus, t0
	csrr t1, mstatus
	and t1, t1, t0
	beqz t1, 1f
	fssr x0
1:
#endif

#if defined(ENABLE_SMP)
	smp_resume(t0, t1)

	csrr a0, mhartid
	bnez a0, 2f
#endif

	auipc ra, 0
	addi sp, sp, -16
#if __riscv_xlen == 32
	sw ra, 8(sp)
#else
	sd ra, 8(sp)
#endif

	call setup_heap

#if defined(CFG_ZIGBEE_ENABLE) || CFG_CPP_ENABLE
    la s1, __init_array_start
    la s2, __init_array_end
    li s3, 4
1:
    bgeu s1, s2, 2f
    lw a3, (s1)
    jalr a3
    add s1, s1, s3
    j 1b
2:
	li a0, 0
	li a1, 0
	call bl702_main
	
	tail exit
#else
	/* argc = argv = 0 */
	li a0, 0
	li a1, 0
	call bl702_main
#if 0
	tail exit
#endif
#endif
1:
	j 1b

#if defined(ENABLE_SMP)
2:
	la t0, trap_entry
	csrw mtvec, t0

	csrr a0, mhartid
	la t1, _sp_main
	slli t0, a0, 10
	sub sp, t1, t0

	auipc ra, 0
	addi sp, sp, -16
#if __riscv_xlen == 32
	sw ra, 8(sp)
#else
	sd ra, 8(sp)
#endif

	call secondary_main
	tail exit

1:
	j 1b
#endif
	.cfi_endproc
