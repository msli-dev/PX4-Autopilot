/****************************************************************************
 * scripts/script.ld
 *
 *   Copyright (C) 2016, 2019 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* The Pogo CANFCU Mini uses an STM32H743VI with 2048KB of main FLASH.
 * The external high-speed oscillator (HSE) frequency is 8 MHz.
 *
 * The flash memory is partitioned into User Flash memory and System Flash
 * memory. The User Flash memory has two banks:
 *
 *   1) User Flash memory:
 *
 *      Bank 1: Start address 0x0800:0000 to 0x080F:FFFF
 *              8 sectors, 128KB each
 *
 *      Bank 2: Start address 0x0810:0000 to 0x081F:FFFF
 *              8 sectors, 128KB each
 *
 *   2) System Flash memory:
 *
 *      Bank 1: Start address 0x1FF0:0000 to 0x1FF1:FFFF
 *              1 sector, 128KB
 *
 *      Bank 2: Start address 0x1FF4:0000 to 0x1FF5:FFFF
 *              1 sector, 128KB
 *
 *   3) User option bytes for boot and device configuration.
 *
 * The STM32H743VI can select different boot spaces through the BOOT pin and
 * the boot base addresses programmed in the BOOT_ADD0 and BOOT_ADD1 option
 * bytes:
 *
 *   1) BOOT=0: Boot address defined by BOOT_ADD0[15:0].
 *      The default ST-programmed value maps to User Flash memory at
 *      0x0800:0000.
 *
 *   2) BOOT=1: Boot address defined by BOOT_ADD1[15:0].
 *      The default ST-programmed value maps to the System bootloader at
 *      0x1FF0:0000.
 *
 * On the Pogo CANFCU Mini, BOOT0 is normally held low, so the MCU boots from
 * User Flash at 0x0800:0000. If BOOT0 is asserted high during reset, the MCU
 * boots from the STM32 System bootloader.
 *
 * The Pogo CANFCU Mini bootloader occupies the first User Flash sector:
 *
 *      0x0800:0000 to 0x0801:FFFF, 128KB
 *
 * The PX4 application image starts after the bootloader:
 *
 *      0x0802:0000 to 0x081F:FFFF, 1920KB
 *
 * The STM32H743VI has 1024KB of data SRAM. SRAM is split into several blocks
 * across multiple power domains:
 *
 *   1) TCM SRAMs are dedicated to the Cortex-M7 and are accessible with
 *      zero wait states by the Cortex-M7 and by MDMA through the AHBS slave
 *      bus.
 *
 *      1.1) 128KB of DTCM-RAM beginning at address 0x2000:0000
 *
 *           The DTCM-RAM is organized as two 64KB blocks on two 32-bit DTCM
 *           ports. It can be used for critical real-time data, such as
 *           interrupt service routines, stacks, or heap memory.
 *
 *      1.2) 64KB of ITCM-RAM beginning at address 0x0000:0000
 *
 *           This RAM is connected to the ITCM 64-bit interface and can be
 *           used for timing-critical code execution.
 *
 *   2) AXI SRAM, D1 domain:
 *
 *      512KB beginning at address 0x2400:0000
 *
 *   3) AHB SRAM, D2 domain:
 *
 *      SRAM1: 128KB beginning at address 0x3000:0000
 *      SRAM2: 128KB beginning at address 0x3002:0000
 *      SRAM3:  32KB beginning at address 0x3004:0000
 *
 *      SRAM1 through SRAM3 form a contiguous 288KB memory region beginning
 *      at address 0x3000:0000.
 *
 *   4) AHB SRAM, D3 domain:
 *
 *      SRAM4:  64KB beginning at address 0x3800:0000
 *      BKPRAM:  4KB beginning at address 0x3880:0000
 *
 * When booting from User Flash, Flash memory is aliased to address
 * 0x0000:0000. The reset vector is fetched from the aliased address space and
 * execution then continues in the 0x0800:0000 Flash address range.
 */

MEMORY
{
	itcm  (rwx) : ORIGIN = 0x00000000, LENGTH =   64K
	flash (rx)  : ORIGIN = 0x08000000, LENGTH =  128K
	dtcm1 (rwx) : ORIGIN = 0x20000000, LENGTH =   64K
	dtcm2 (rwx) : ORIGIN = 0x20010000, LENGTH =   64K
	sram  (rwx) : ORIGIN = 0x24000000, LENGTH =  512K
	sram1 (rwx) : ORIGIN = 0x30000000, LENGTH =  128K
	sram2 (rwx) : ORIGIN = 0x30020000, LENGTH =  128K
	sram3 (rwx) : ORIGIN = 0x30040000, LENGTH =   32K
	sram4 (rwx) : ORIGIN = 0x38000000, LENGTH =   64K
	bbram (rwx) : ORIGIN = 0x38800000, LENGTH =    4K
}

OUTPUT_ARCH(arm)
EXTERN(_vectors)
ENTRY(_stext)

/*
 * Ensure that abort() is present in the final object.  The exception handling
 * code pulled in by libgcc.a requires it (and that code cannot be easily avoided).
 */
EXTERN(abort)
SECTIONS
{
	.text : {
		_stext = ABSOLUTE(.);
		*(.vectors)
		. = ALIGN(32);
		*(.text .text.*)
		*(.fixup)
		*(.gnu.warning)
		*(.rodata .rodata.*)
		*(.gnu.linkonce.t.*)
		*(.glue_7)
		*(.glue_7t)
		*(.got)
		*(.gcc_except_table)
		*(.gnu.linkonce.r.*)
		_etext = ABSOLUTE(.);

	} > flash

	/*
	 * Init functions (static constructors and the like)
	 */
	.init_section : {
		_sinit = ABSOLUTE(.);
		KEEP(*(.init_array .init_array.*))
		_einit = ABSOLUTE(.);
	} > flash


	.ARM.extab : {
		*(.ARM.extab*)
	} > flash

	__exidx_start = ABSOLUTE(.);
	.ARM.exidx : {
		*(.ARM.exidx*)
	} > flash
	__exidx_end = ABSOLUTE(.);

	_eronly = ABSOLUTE(.);

	.data : {
		_sdata = ABSOLUTE(.);
		*(.data .data.*)
		*(.gnu.linkonce.d.*)
		CONSTRUCTORS
		_edata = ABSOLUTE(.);
	} > sram AT > flash

	.bss : {
		_sbss = ABSOLUTE(.);
		*(.bss .bss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = ABSOLUTE(.);
	} > sram

	/* Stabs debugging sections. */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
	.debug_abbrev 0 : { *(.debug_abbrev) }
	.debug_info 0 : { *(.debug_info) }
	.debug_line 0 : { *(.debug_line) }
	.debug_pubnames 0 : { *(.debug_pubnames) }
	.debug_aranges 0 : { *(.debug_aranges) }
}
