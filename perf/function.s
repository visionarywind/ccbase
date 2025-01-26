	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 15, 0	sdk_version 15, 2
	.globl	__ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev ; -- Begin function _ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev
	.weak_def_can_be_hidden	__ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev
	.p2align	2
__ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev: ; @_ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev
	.cfi_startproc
; %bb.0:
	stp	x22, x21, [sp, #-48]!           ; 16-byte Folded Spill
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	mov	x19, x0
	ldr	x8, [x0, #16]
	cbz	x8, LBB0_2
; %bb.1:
	ldp	x8, x20, [x19]
	ldr	x8, [x8, #8]
	ldr	x9, [x20]
	str	x8, [x9, #8]
	str	x9, [x8]
	str	xzr, [x19, #16]
	cmp	x20, x19
	b.ne	LBB0_6
LBB0_2:
	mov	x0, x19
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp], #48             ; 16-byte Folded Reload
	ret
LBB0_3:                                 ;   in Loop: Header=BB0_6 Depth=1
	mov	w8, #32                         ; =0x20
LBB0_4:                                 ;   in Loop: Header=BB0_6 Depth=1
	ldr	x9, [x0]
	ldr	x8, [x9, x8]
	blr	x8
LBB0_5:                                 ;   in Loop: Header=BB0_6 Depth=1
	mov	x0, x20
	mov	w1, #48                         ; =0x30
	bl	__ZdlPvm
	mov	x20, x21
	cmp	x21, x19
	b.eq	LBB0_2
LBB0_6:                                 ; =>This Inner Loop Header: Depth=1
	ldr	x21, [x20, #8]
	add	x8, x20, #16
	ldr	x0, [x20, #40]
	cmp	x0, x8
	b.eq	LBB0_3
; %bb.7:                                ;   in Loop: Header=BB0_6 Depth=1
	cbz	x0, LBB0_5
; %bb.8:                                ;   in Loop: Header=BB0_6 Depth=1
	mov	w8, #40                         ; =0x28
	b	LBB0_4
	.cfi_endproc
                                        ; -- End function
	.globl	__Z8consumerv                   ; -- Begin function _Z8consumerv
	.p2align	2
__Z8consumerv:                          ; @_Z8consumerv
Lfunc_begin0:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception0
; %bb.0:
	sub	sp, sp, #144
	stp	x26, x25, [sp, #64]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #80]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
Lloh0:
	adrp	x19, _kMutex@PAGE
Lloh1:
	add	x19, x19, _kMutex@PAGEOFF
Lloh2:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh3:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh4:
	ldr	x8, [x8]
	str	x8, [sp, #56]
	mov	w22, #1                         ; =0x1
	adrp	x23, _kReady@PAGE
	adrp	x24, _kList@PAGE+8
Lloh5:
	adrp	x25, _kList@PAGE+8
Lloh6:
	add	x25, x25, _kList@PAGEOFF+8
	add	x26, sp, #24
Lloh7:
	adrp	x20, _cond_var@PAGE
Lloh8:
	add	x20, x20, _cond_var@PAGEOFF
LBB1_1:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_2 Depth 2
	str	x19, [sp, #8]
	strb	w22, [sp, #16]
	mov	x0, x19
	bl	__ZNSt3__15mutex4lockEv
	ldrb	w8, [x23, _kReady@PAGEOFF]
	tbnz	w8, #0, LBB1_3
LBB1_2:                                 ;   Parent Loop BB1_1 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x1, sp, #8
	mov	x0, x20
	bl	__ZNSt3__118condition_variable4waitERNS_11unique_lockINS_5mutexEEE
	ldrb	w8, [x23, _kReady@PAGEOFF]
	cmp	w8, #1
	b.ne	LBB1_2
LBB1_3:                                 ;   in Loop: Header=BB1_1 Depth=1
	ldr	x8, [x24, _kList@PAGEOFF+8]
	ldr	x0, [x8, #40]
	cbz	x0, LBB1_6
; %bb.4:                                ;   in Loop: Header=BB1_1 Depth=1
	add	x8, x8, #16
	cmp	x0, x8
	b.eq	LBB1_7
; %bb.5:                                ;   in Loop: Header=BB1_1 Depth=1
	ldr	x8, [x0]
	ldr	x8, [x8, #16]
Ltmp0:
	blr	x8
Ltmp1:
LBB1_6:                                 ;   in Loop: Header=BB1_1 Depth=1
	str	x0, [sp, #48]
	b	LBB1_8
LBB1_7:                                 ;   in Loop: Header=BB1_1 Depth=1
	str	x26, [sp, #48]
	ldr	x8, [x0]
	ldr	x8, [x8, #24]
Ltmp2:
	add	x1, sp, #24
	blr	x8
Ltmp3:
LBB1_8:                                 ;   in Loop: Header=BB1_1 Depth=1
	ldp	x21, x8, [x25]
	ldp	x10, x9, [x21]
	str	x9, [x10, #8]
	str	x10, [x9]
	sub	x8, x8, #1
	str	x8, [x25, #8]
	add	x8, x21, #16
	ldr	x0, [x21, #40]
	cmp	x0, x8
	b.eq	LBB1_11
; %bb.9:                                ;   in Loop: Header=BB1_1 Depth=1
	cbz	x0, LBB1_13
; %bb.10:                               ;   in Loop: Header=BB1_1 Depth=1
	mov	w8, #40                         ; =0x28
	b	LBB1_12
LBB1_11:                                ;   in Loop: Header=BB1_1 Depth=1
	mov	w8, #32                         ; =0x20
LBB1_12:                                ;   in Loop: Header=BB1_1 Depth=1
	ldr	x9, [x0]
	ldr	x8, [x9, x8]
	blr	x8
LBB1_13:                                ;   in Loop: Header=BB1_1 Depth=1
	mov	x0, x21
	mov	w1, #48                         ; =0x30
	bl	__ZdlPvm
	ldr	x0, [sp, #48]
	cbz	x0, LBB1_22
; %bb.14:                               ;   in Loop: Header=BB1_1 Depth=1
	ldr	x8, [x0]
	ldr	x8, [x8, #48]
Ltmp5:
	blr	x8
Ltmp6:
; %bb.15:                               ;   in Loop: Header=BB1_1 Depth=1
	strb	wzr, [x23, _kReady@PAGEOFF]
	ldr	x0, [sp, #48]
	cmp	x0, x26
	b.eq	LBB1_18
; %bb.16:                               ;   in Loop: Header=BB1_1 Depth=1
	cbz	x0, LBB1_20
; %bb.17:                               ;   in Loop: Header=BB1_1 Depth=1
	mov	w8, #40                         ; =0x28
	b	LBB1_19
LBB1_18:                                ;   in Loop: Header=BB1_1 Depth=1
	mov	w8, #32                         ; =0x20
LBB1_19:                                ;   in Loop: Header=BB1_1 Depth=1
	ldr	x9, [x0]
	ldr	x8, [x9, x8]
	blr	x8
LBB1_20:                                ;   in Loop: Header=BB1_1 Depth=1
	ldrb	w8, [sp, #16]
	cmp	w8, #1
	b.ne	LBB1_1
; %bb.21:                               ;   in Loop: Header=BB1_1 Depth=1
	ldr	x0, [sp, #8]
	bl	__ZNSt3__15mutex6unlockEv
	b	LBB1_1
LBB1_22:
Ltmp8:
	bl	__ZNSt3__125__throw_bad_function_callB8ne190107Ev
Ltmp9:
; %bb.23:
	brk	#0x1
LBB1_24:
Ltmp4:
	mov	x19, x0
	b	LBB1_32
LBB1_25:
Ltmp7:
	b	LBB1_27
LBB1_26:
Ltmp10:
LBB1_27:
	mov	x19, x0
	ldr	x0, [sp, #48]
	add	x8, sp, #24
	cmp	x0, x8
	b.ne	LBB1_29
; %bb.28:
	mov	w8, #32                         ; =0x20
	b	LBB1_31
LBB1_29:
	cbz	x0, LBB1_32
; %bb.30:
	mov	w8, #40                         ; =0x28
LBB1_31:
	ldr	x9, [x0]
	ldr	x8, [x9, x8]
	blr	x8
LBB1_32:
	ldrb	w8, [sp, #16]
	cmp	w8, #1
	b.ne	LBB1_34
; %bb.33:
	ldr	x0, [sp, #8]
	bl	__ZNSt3__15mutex6unlockEv
LBB1_34:
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh7, Lloh8
	.loh AdrpAdd	Lloh5, Lloh6
	.loh AdrpLdrGotLdr	Lloh2, Lloh3, Lloh4
	.loh AdrpAdd	Lloh0, Lloh1
Lfunc_end0:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table1:
Lexception0:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end0-Lcst_begin0
Lcst_begin0:
	.uleb128 Lfunc_begin0-Lfunc_begin0      ; >> Call Site 1 <<
	.uleb128 Ltmp0-Lfunc_begin0             ;   Call between Lfunc_begin0 and Ltmp0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp0-Lfunc_begin0             ; >> Call Site 2 <<
	.uleb128 Ltmp3-Ltmp0                    ;   Call between Ltmp0 and Ltmp3
	.uleb128 Ltmp4-Lfunc_begin0             ;     jumps to Ltmp4
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp3-Lfunc_begin0             ; >> Call Site 3 <<
	.uleb128 Ltmp5-Ltmp3                    ;   Call between Ltmp3 and Ltmp5
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp5-Lfunc_begin0             ; >> Call Site 4 <<
	.uleb128 Ltmp6-Ltmp5                    ;   Call between Ltmp5 and Ltmp6
	.uleb128 Ltmp7-Lfunc_begin0             ;     jumps to Ltmp7
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp6-Lfunc_begin0             ; >> Call Site 5 <<
	.uleb128 Ltmp8-Ltmp6                    ;   Call between Ltmp6 and Ltmp8
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp8-Lfunc_begin0             ; >> Call Site 6 <<
	.uleb128 Ltmp9-Ltmp8                    ;   Call between Ltmp8 and Ltmp9
	.uleb128 Ltmp10-Lfunc_begin0            ;     jumps to Ltmp10
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp9-Lfunc_begin0             ; >> Call Site 7 <<
	.uleb128 Lfunc_end0-Ltmp9               ;   Call between Ltmp9 and Lfunc_end0
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function _Z8producerv
lCPI2_0:
	.quad	0                               ; 0x0
	.quad	288230376151711744              ; 0x400000000000000
	.section	__TEXT,__text,regular,pure_instructions
	.globl	__Z8producerv
	.p2align	2


__Z8producerv:                          ; @_Z8producerv
Lfunc_begin1:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception1
; %bb.0:
	sub	sp, sp, #128
	stp	x28, x27, [sp, #32]             ; 16-byte Folded Spill
	stp	x26, x25, [sp, #48]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	.cfi_offset w27, -88
	.cfi_offset w28, -96
Lloh9:
	adrp	x19, _kMutex@PAGE
Lloh10:
	add	x19, x19, _kMutex@PAGEOFF
Lloh11:
	adrp	x22, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh12:
	ldr	x22, [x22, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
Lloh13:
	adrp	x27, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGE+16
Lloh14:
	add	x27, x27, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGEOFF+16
	mov	w28, #25959                     ; =0x6567
	movk	w28, #25959, lsl #16
Lloh15:
	adrp	x8, lCPI2_0@PAGE
Lloh16:
	ldr	q0, [x8, lCPI2_0@PAGEOFF]
	str	q0, [sp]                        ; 16-byte Folded Spill
Lloh17:
	adrp	x20, _kList@PAGE
Lloh18:
	add	x20, x20, _kList@PAGEOFF
	adrp	x21, _kReady@PAGE
	mov	w26, #1                         ; =0x1
Lloh19:
	adrp	x23, _cond_var@PAGE
Lloh20:
	add	x23, x23, _cond_var@PAGEOFF
LBB2_1:                                 ; =>This Inner Loop Header: Depth=1
	mov	w8, #51712                      ; =0xca00
	movk	w8, #15258, lsl #16
	str	x8, [sp, #24]
	add	x0, sp, #24
	bl	__ZNSt3__111this_thread9sleep_forERKNS_6chrono8durationIxNS_5ratioILl1ELl1000000000EEEEE
	mov	x0, x19
	bl	__ZNSt3__15mutex4lockEv
Ltmp11:
Lloh21:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh22:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
Lloh23:
	adrp	x1, l_.str@PAGE
Lloh24:
	add	x1, x1, l_.str@PAGEOFF
	mov	w2, #7                          ; =0x7
	bl	__ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Ltmp12:
; %bb.2:                                ;   in Loop: Header=BB2_1 Depth=1
	mov	x24, x0
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
Ltmp13:
	add	x8, sp, #24
	add	x0, x0, x9
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp14:
; %bb.3:                                ;   in Loop: Header=BB2_1 Depth=1
Ltmp15:
	add	x0, sp, #24
	mov	x1, x22
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp16:
; %bb.4:                                ;   in Loop: Header=BB2_1 Depth=1
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp17:
	mov	w1, #10                         ; =0xa
	blr	x8
Ltmp18:
; %bb.5:                                ;   in Loop: Header=BB2_1 Depth=1
	mov	x25, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
Ltmp20:
	mov	x0, x24
	mov	x1, x25
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
Ltmp21:
; %bb.6:                                ;   in Loop: Header=BB2_1 Depth=1
Ltmp22:
	mov	x0, x24
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
Ltmp23:
; %bb.7:                                ;   in Loop: Header=BB2_1 Depth=1
Ltmp25:
	mov	w0, #32                         ; =0x20
	bl	__Znwm
Ltmp26:
; %bb.8:                                ;   in Loop: Header=BB2_1 Depth=1
	mov	x24, x0
	stp	x27, x28, [x0]
	ldr	q0, [sp]                        ; 16-byte Folded Reload
	str	q0, [x0, #16]
Ltmp28:
	mov	w0, #48                         ; =0x30
	bl	__Znwm
Ltmp29:
; %bb.9:                                ;   in Loop: Header=BB2_1 Depth=1
	str	x24, [x0, #40]
	ldr	x8, [x20]
	stp	x8, x20, [x0]
	str	x0, [x8, #8]
	str	x0, [x20]
	ldr	x8, [x20, #16]
	add	x8, x8, #1
	str	x8, [x20, #16]
	strb	w26, [x21, _kReady@PAGEOFF]
	mov	x0, x23
	bl	__ZNSt3__118condition_variable10notify_oneEv
	mov	x0, x19
	bl	__ZNSt3__15mutex6unlockEv
	b	LBB2_1
LBB2_10:
Ltmp30:
	mov	x19, x0
	mov	x0, x24
	mov	w1, #32                         ; =0x20
	bl	__ZdlPvm
	b	LBB2_15
LBB2_11:
Ltmp27:
	b	LBB2_14
LBB2_12:
Ltmp19:
	mov	x19, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB2_15
LBB2_13:
Ltmp24:
LBB2_14:
	mov	x19, x0
LBB2_15:
Lloh25:
	adrp	x0, _kMutex@PAGE
Lloh26:
	add	x0, x0, _kMutex@PAGEOFF
	bl	__ZNSt3__15mutex6unlockEv
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh19, Lloh20
	.loh AdrpAdd	Lloh17, Lloh18
	.loh AdrpLdr	Lloh15, Lloh16
	.loh AdrpAdd	Lloh13, Lloh14
	.loh AdrpLdrGot	Lloh11, Lloh12
	.loh AdrpAdd	Lloh9, Lloh10
	.loh AdrpAdd	Lloh23, Lloh24
	.loh AdrpLdrGot	Lloh21, Lloh22
	.loh AdrpAdd	Lloh25, Lloh26
Lfunc_end1:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table2:
Lexception1:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end1-Lcst_begin1
Lcst_begin1:
	.uleb128 Lfunc_begin1-Lfunc_begin1      ; >> Call Site 1 <<
	.uleb128 Ltmp11-Lfunc_begin1            ;   Call between Lfunc_begin1 and Ltmp11
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp11-Lfunc_begin1            ; >> Call Site 2 <<
	.uleb128 Ltmp14-Ltmp11                  ;   Call between Ltmp11 and Ltmp14
	.uleb128 Ltmp24-Lfunc_begin1            ;     jumps to Ltmp24
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp15-Lfunc_begin1            ; >> Call Site 3 <<
	.uleb128 Ltmp18-Ltmp15                  ;   Call between Ltmp15 and Ltmp18
	.uleb128 Ltmp19-Lfunc_begin1            ;     jumps to Ltmp19
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp20-Lfunc_begin1            ; >> Call Site 4 <<
	.uleb128 Ltmp23-Ltmp20                  ;   Call between Ltmp20 and Ltmp23
	.uleb128 Ltmp24-Lfunc_begin1            ;     jumps to Ltmp24
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp25-Lfunc_begin1            ; >> Call Site 5 <<
	.uleb128 Ltmp26-Ltmp25                  ;   Call between Ltmp25 and Ltmp26
	.uleb128 Ltmp27-Lfunc_begin1            ;     jumps to Ltmp27
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp28-Lfunc_begin1            ; >> Call Site 6 <<
	.uleb128 Ltmp29-Ltmp28                  ;   Call between Ltmp28 and Ltmp29
	.uleb128 Ltmp30-Lfunc_begin1            ;     jumps to Ltmp30
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp29-Lfunc_begin1            ; >> Call Site 7 <<
	.uleb128 Lfunc_end1-Ltmp29              ;   Call between Ltmp29 and Lfunc_end1
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end1:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main                           ; -- Begin function main
	.p2align	2
_main:                                  ; @main
Lfunc_begin2:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception2
; %bb.0:
	sub	sp, sp, #48
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
Lloh27:
	adrp	x1, __Z8consumerv@PAGE
Lloh28:
	add	x1, x1, __Z8consumerv@PAGEOFF
	add	x0, sp, #8
	bl	__ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_
Ltmp31:
Lloh29:
	adrp	x1, __Z8producerv@PAGE
Lloh30:
	add	x1, x1, __Z8producerv@PAGEOFF
	mov	x0, sp
	bl	__ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_
Ltmp32:
; %bb.1:
Ltmp34:
	add	x0, sp, #8
	bl	__ZNSt3__16thread4joinEv
Ltmp35:
; %bb.2:
Ltmp36:
	mov	x0, sp
	bl	__ZNSt3__16thread4joinEv
Ltmp37:
; %bb.3:
	mov	x0, sp
	bl	__ZNSt3__16threadD1Ev
	add	x0, sp, #8
	bl	__ZNSt3__16threadD1Ev
	mov	w0, #1                          ; =0x1
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #48
	ret
LBB3_4:
Ltmp33:
	mov	x19, x0
	b	LBB3_6
LBB3_5:
Ltmp38:
	mov	x19, x0
	mov	x0, sp
	bl	__ZNSt3__16threadD1Ev
LBB3_6:
	add	x0, sp, #8
	bl	__ZNSt3__16threadD1Ev
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh29, Lloh30
	.loh AdrpAdd	Lloh27, Lloh28
Lfunc_end2:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table3:
Lexception2:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end2-Lcst_begin2
Lcst_begin2:
	.uleb128 Lfunc_begin2-Lfunc_begin2      ; >> Call Site 1 <<
	.uleb128 Ltmp31-Lfunc_begin2            ;   Call between Lfunc_begin2 and Ltmp31
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp31-Lfunc_begin2            ; >> Call Site 2 <<
	.uleb128 Ltmp32-Ltmp31                  ;   Call between Ltmp31 and Ltmp32
	.uleb128 Ltmp33-Lfunc_begin2            ;     jumps to Ltmp33
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp34-Lfunc_begin2            ; >> Call Site 3 <<
	.uleb128 Ltmp37-Ltmp34                  ;   Call between Ltmp34 and Ltmp37
	.uleb128 Ltmp38-Lfunc_begin2            ;     jumps to Ltmp38
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp37-Lfunc_begin2            ; >> Call Site 4 <<
	.uleb128 Lfunc_end2-Ltmp37              ;   Call between Ltmp37 and Lfunc_end2
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end2:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	___clang_call_terminate ; -- Begin function __clang_call_terminate
	.globl	___clang_call_terminate
	.weak_def_can_be_hidden	___clang_call_terminate
	.p2align	2
___clang_call_terminate:                ; @__clang_call_terminate
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	___cxa_begin_catch
	bl	__ZSt9terminatev
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__125__throw_bad_function_callB8ne190107Ev ; -- Begin function _ZNSt3__125__throw_bad_function_callB8ne190107Ev
	.globl	__ZNSt3__125__throw_bad_function_callB8ne190107Ev
	.weak_def_can_be_hidden	__ZNSt3__125__throw_bad_function_callB8ne190107Ev
	.p2align	2
__ZNSt3__125__throw_bad_function_callB8ne190107Ev: ; @_ZNSt3__125__throw_bad_function_callB8ne190107Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	mov	w0, #8                          ; =0x8
	bl	___cxa_allocate_exception
Lloh31:
	adrp	x8, __ZTVNSt3__117bad_function_callE@GOTPAGE
Lloh32:
	ldr	x8, [x8, __ZTVNSt3__117bad_function_callE@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
Lloh33:
	adrp	x1, __ZTINSt3__117bad_function_callE@GOTPAGE
Lloh34:
	ldr	x1, [x1, __ZTINSt3__117bad_function_callE@GOTPAGEOFF]
Lloh35:
	adrp	x2, __ZNSt3__117bad_function_callD1Ev@GOTPAGE
Lloh36:
	ldr	x2, [x2, __ZNSt3__117bad_function_callD1Ev@GOTPAGEOFF]
	bl	___cxa_throw
	.loh AdrpLdrGot	Lloh35, Lloh36
	.loh AdrpLdrGot	Lloh33, Lloh34
	.loh AdrpLdrGot	Lloh31, Lloh32
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m ; -- Begin function _ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.globl	__ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.weak_def_can_be_hidden	__ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	.p2align	2
__ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m: ; @_ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
Lfunc_begin3:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception3
; %bb.0:
	sub	sp, sp, #112
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x21, x2
	mov	x20, x1
	mov	x19, x0
Ltmp39:
	add	x0, sp, #8
	mov	x1, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
Ltmp40:
; %bb.1:
	ldrb	w8, [sp, #8]
	cmp	w8, #1
	b.ne	LBB6_10
; %bb.2:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x4, x19, x8
	ldr	x22, [x4, #40]
	ldr	w24, [x4, #8]
	ldr	w23, [x4, #144]
	cmn	w23, #1
	b.ne	LBB6_7
; %bb.3:
Ltmp42:
	add	x8, sp, #24
	mov	x25, x4
	mov	x0, x4
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp43:
; %bb.4:
Ltmp44:
Lloh37:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh38:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #24
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp45:
; %bb.5:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp46:
	mov	w1, #32                         ; =0x20
	blr	x8
Ltmp47:
; %bb.6:
	mov	x23, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	mov	x4, x25
	str	w23, [x25, #144]
LBB6_7:
	mov	w8, #176                        ; =0xb0
	and	w8, w24, w8
	add	x3, x20, x21
	cmp	w8, #32
	csel	x2, x3, x20, eq
Ltmp49:
	sxtb	w5, w23
	mov	x0, x22
	mov	x1, x20
	bl	__ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Ltmp50:
; %bb.8:
	cbnz	x0, LBB6_10
; %bb.9:
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
	add	x0, x19, x8
	ldr	w8, [x0, #32]
	mov	w9, #5                          ; =0x5
Ltmp52:
	orr	w1, w8, w9
	bl	__ZNSt3__18ios_base5clearEj
Ltmp53:
LBB6_10:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
LBB6_11:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB6_12:
Ltmp54:
	b	LBB6_15
LBB6_13:
Ltmp48:
	mov	x20, x0
	add	x0, sp, #24
	bl	__ZNSt3__16localeD1Ev
	b	LBB6_16
LBB6_14:
Ltmp51:
LBB6_15:
	mov	x20, x0
LBB6_16:
	add	x0, sp, #8
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
	b	LBB6_18
LBB6_17:
Ltmp41:
	mov	x20, x0
LBB6_18:
	mov	x0, x20
	bl	___cxa_begin_catch
	ldr	x8, [x19]
	ldur	x8, [x8, #-24]
Ltmp55:
	add	x0, x19, x8
	bl	__ZNSt3__18ios_base33__set_badbit_and_consider_rethrowEv
Ltmp56:
; %bb.19:
	bl	___cxa_end_catch
	b	LBB6_11
LBB6_20:
Ltmp57:
	mov	x19, x0
Ltmp58:
	bl	___cxa_end_catch
Ltmp59:
; %bb.21:
	mov	x0, x19
	bl	__Unwind_Resume
LBB6_22:
Ltmp60:
	bl	___clang_call_terminate
	.loh AdrpLdrGot	Lloh37, Lloh38
Lfunc_end3:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table6:
Lexception3:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	155                             ; @TType Encoding = indirect pcrel sdata4
	.uleb128 Lttbase0-Lttbaseref0
Lttbaseref0:
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end3-Lcst_begin3
Lcst_begin3:
	.uleb128 Ltmp39-Lfunc_begin3            ; >> Call Site 1 <<
	.uleb128 Ltmp40-Ltmp39                  ;   Call between Ltmp39 and Ltmp40
	.uleb128 Ltmp41-Lfunc_begin3            ;     jumps to Ltmp41
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp42-Lfunc_begin3            ; >> Call Site 2 <<
	.uleb128 Ltmp43-Ltmp42                  ;   Call between Ltmp42 and Ltmp43
	.uleb128 Ltmp51-Lfunc_begin3            ;     jumps to Ltmp51
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp44-Lfunc_begin3            ; >> Call Site 3 <<
	.uleb128 Ltmp47-Ltmp44                  ;   Call between Ltmp44 and Ltmp47
	.uleb128 Ltmp48-Lfunc_begin3            ;     jumps to Ltmp48
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp49-Lfunc_begin3            ; >> Call Site 4 <<
	.uleb128 Ltmp50-Ltmp49                  ;   Call between Ltmp49 and Ltmp50
	.uleb128 Ltmp51-Lfunc_begin3            ;     jumps to Ltmp51
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp52-Lfunc_begin3            ; >> Call Site 5 <<
	.uleb128 Ltmp53-Ltmp52                  ;   Call between Ltmp52 and Ltmp53
	.uleb128 Ltmp54-Lfunc_begin3            ;     jumps to Ltmp54
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp53-Lfunc_begin3            ; >> Call Site 6 <<
	.uleb128 Ltmp55-Ltmp53                  ;   Call between Ltmp53 and Ltmp55
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp55-Lfunc_begin3            ; >> Call Site 7 <<
	.uleb128 Ltmp56-Ltmp55                  ;   Call between Ltmp55 and Ltmp56
	.uleb128 Ltmp57-Lfunc_begin3            ;     jumps to Ltmp57
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp56-Lfunc_begin3            ; >> Call Site 8 <<
	.uleb128 Ltmp58-Ltmp56                  ;   Call between Ltmp56 and Ltmp58
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp58-Lfunc_begin3            ; >> Call Site 9 <<
	.uleb128 Ltmp59-Ltmp58                  ;   Call between Ltmp58 and Ltmp59
	.uleb128 Ltmp60-Lfunc_begin3            ;     jumps to Ltmp60
	.byte	1                               ;   On action: 1
	.uleb128 Ltmp59-Lfunc_begin3            ; >> Call Site 10 <<
	.uleb128 Lfunc_end3-Ltmp59              ;   Call between Ltmp59 and Lfunc_end3
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end3:
	.byte	1                               ; >> Action Record 1 <<
                                        ;   Catch TypeInfo 1
	.byte	0                               ;   No further actions
	.p2align	2, 0x0
                                        ; >> Catch TypeInfos <<
	.long	0                               ; TypeInfo 1
Lttbase0:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_ ; -- Begin function _ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.globl	__ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.weak_def_can_be_hidden	__ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
	.p2align	2
__ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_: ; @_ZNSt3__116__pad_and_outputB8ne190107IcNS_11char_traitsIcEEEENS_19ostreambuf_iteratorIT_T0_EES6_PKS4_S8_S8_RNS_8ios_baseES4_
Lfunc_begin4:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception4
; %bb.0:
	sub	sp, sp, #112
	stp	x26, x25, [sp, #32]             ; 16-byte Folded Spill
	stp	x24, x23, [sp, #48]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #64]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #80]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #96]             ; 16-byte Folded Spill
	add	x29, sp, #96
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w23, -56
	.cfi_offset w24, -64
	.cfi_offset w25, -72
	.cfi_offset w26, -80
	mov	x19, x0
	cbz	x0, LBB7_16
; %bb.1:
	mov	x24, x5
	mov	x20, x4
	mov	x22, x3
	mov	x21, x2
	ldr	x8, [x4, #24]
	sub	x9, x3, x1
	subs	x8, x8, x9
	csel	x23, x8, xzr, gt
	sub	x25, x2, x1
	cmp	x25, #1
	b.lt	LBB7_3
; %bb.2:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x2, x25
	blr	x8
	cmp	x0, x25
	b.ne	LBB7_15
LBB7_3:
	cmp	x23, #1
	b.lt	LBB7_12
; %bb.4:
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x23, x8
	b.hs	LBB7_17
; %bb.5:
	cmp	x23, #23
	b.hs	LBB7_7
; %bb.6:
	strb	w23, [sp, #31]
	add	x25, sp, #8
	b	LBB7_8
LBB7_7:
	orr	x8, x23, #0x7
	cmp	x8, #23
	mov	w9, #25                         ; =0x19
	csinc	x26, x9, x8, eq
	mov	x0, x26
	bl	__Znwm
	mov	x25, x0
	orr	x8, x26, #0x8000000000000000
	stp	x23, x8, [sp, #16]
	str	x0, [sp, #8]
LBB7_8:
	mov	x0, x25
	mov	x1, x24
	mov	x2, x23
	bl	_memset
	strb	wzr, [x25, x23]
	ldrsb	w8, [sp, #31]
	ldr	x9, [sp, #8]
	cmp	w8, #0
	add	x8, sp, #8
	csel	x1, x9, x8, lt
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
Ltmp61:
	mov	x0, x19
	mov	x2, x23
	blr	x8
Ltmp62:
; %bb.9:
	ldrsb	w8, [sp, #31]
	tbnz	w8, #31, LBB7_11
; %bb.10:
	cmp	x0, x23
	b.ne	LBB7_15
	b	LBB7_12
LBB7_11:
	ldr	x8, [sp, #8]
	ldr	x9, [sp, #24]
	and	x1, x9, #0x7fffffffffffffff
	mov	x24, x0
	mov	x0, x8
	bl	__ZdlPvm
	mov	x0, x24
	cmp	x0, x23
	b.ne	LBB7_15
LBB7_12:
	sub	x22, x22, x21
	cmp	x22, #1
	b.lt	LBB7_14
; %bb.13:
	ldr	x8, [x19]
	ldr	x8, [x8, #96]
	mov	x0, x19
	mov	x1, x21
	mov	x2, x22
	blr	x8
	cmp	x0, x22
	b.ne	LBB7_15
LBB7_14:
	str	xzr, [x20, #24]
	b	LBB7_16
LBB7_15:
	mov	x19, #0                         ; =0x0
LBB7_16:
	mov	x0, x19
	ldp	x29, x30, [sp, #96]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #80]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #64]             ; 16-byte Folded Reload
	ldp	x24, x23, [sp, #48]             ; 16-byte Folded Reload
	ldp	x26, x25, [sp, #32]             ; 16-byte Folded Reload
	add	sp, sp, #112
	ret
LBB7_17:
	add	x0, sp, #8
	bl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev
LBB7_18:
Ltmp63:
	mov	x19, x0
	ldrsb	w8, [sp, #31]
	tbz	w8, #31, LBB7_20
; %bb.19:
	ldr	x0, [sp, #8]
	ldr	x8, [sp, #24]
	and	x1, x8, #0x7fffffffffffffff
	bl	__ZdlPvm
LBB7_20:
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end4:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table7:
Lexception4:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end4-Lcst_begin4
Lcst_begin4:
	.uleb128 Lfunc_begin4-Lfunc_begin4      ; >> Call Site 1 <<
	.uleb128 Ltmp61-Lfunc_begin4            ;   Call between Lfunc_begin4 and Ltmp61
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp61-Lfunc_begin4            ; >> Call Site 2 <<
	.uleb128 Ltmp62-Ltmp61                  ;   Call between Ltmp61 and Ltmp62
	.uleb128 Ltmp63-Lfunc_begin4            ;     jumps to Ltmp63
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp62-Lfunc_begin4            ; >> Call Site 3 <<
	.uleb128 Lfunc_end4-Ltmp62              ;   Call between Ltmp62 and Lfunc_end4
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end4:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev ; -- Begin function _ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev
	.globl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev
	.weak_def_can_be_hidden	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev
	.p2align	2
__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev: ; @_ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
Lloh39:
	adrp	x0, l_.str.4@PAGE
Lloh40:
	add	x0, x0, l_.str.4@PAGEOFF
	bl	__ZNSt3__120__throw_length_errorB8ne190107EPKc
	.loh AdrpAdd	Lloh39, Lloh40
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__120__throw_length_errorB8ne190107EPKc ; -- Begin function _ZNSt3__120__throw_length_errorB8ne190107EPKc
	.globl	__ZNSt3__120__throw_length_errorB8ne190107EPKc
	.weak_def_can_be_hidden	__ZNSt3__120__throw_length_errorB8ne190107EPKc
	.p2align	2
__ZNSt3__120__throw_length_errorB8ne190107EPKc: ; @_ZNSt3__120__throw_length_errorB8ne190107EPKc
Lfunc_begin5:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception5
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x20, x0
	mov	w0, #16                         ; =0x10
	bl	___cxa_allocate_exception
	mov	x19, x0
Ltmp64:
	mov	x1, x20
	bl	__ZNSt12length_errorC1B8ne190107EPKc
Ltmp65:
; %bb.1:
Lloh41:
	adrp	x1, __ZTISt12length_error@GOTPAGE
Lloh42:
	ldr	x1, [x1, __ZTISt12length_error@GOTPAGEOFF]
Lloh43:
	adrp	x2, __ZNSt12length_errorD1Ev@GOTPAGE
Lloh44:
	ldr	x2, [x2, __ZNSt12length_errorD1Ev@GOTPAGEOFF]
	mov	x0, x19
	bl	___cxa_throw
LBB9_2:
Ltmp66:
	mov	x20, x0
	mov	x0, x19
	bl	___cxa_free_exception
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh43, Lloh44
	.loh AdrpLdrGot	Lloh41, Lloh42
Lfunc_end5:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table9:
Lexception5:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end5-Lcst_begin5
Lcst_begin5:
	.uleb128 Lfunc_begin5-Lfunc_begin5      ; >> Call Site 1 <<
	.uleb128 Ltmp64-Lfunc_begin5            ;   Call between Lfunc_begin5 and Ltmp64
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp64-Lfunc_begin5            ; >> Call Site 2 <<
	.uleb128 Ltmp65-Ltmp64                  ;   Call between Ltmp64 and Ltmp65
	.uleb128 Ltmp66-Lfunc_begin5            ;     jumps to Ltmp66
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp65-Lfunc_begin5            ; >> Call Site 3 <<
	.uleb128 Lfunc_end5-Ltmp65              ;   Call between Ltmp65 and Lfunc_end5
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end5:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt12length_errorC1B8ne190107EPKc ; -- Begin function _ZNSt12length_errorC1B8ne190107EPKc
	.globl	__ZNSt12length_errorC1B8ne190107EPKc
	.weak_def_can_be_hidden	__ZNSt12length_errorC1B8ne190107EPKc
	.p2align	2
__ZNSt12length_errorC1B8ne190107EPKc:   ; @_ZNSt12length_errorC1B8ne190107EPKc
	.cfi_startproc
; %bb.0:
	stp	x29, x30, [sp, #-16]!           ; 16-byte Folded Spill
	mov	x29, sp
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	bl	__ZNSt11logic_errorC2EPKc
Lloh45:
	adrp	x8, __ZTVSt12length_error@GOTPAGE
Lloh46:
	ldr	x8, [x8, __ZTVSt12length_error@GOTPAGEOFF]
	add	x8, x8, #16
	str	x8, [x0]
	ldp	x29, x30, [sp], #16             ; 16-byte Folded Reload
	ret
	.loh AdrpLdrGot	Lloh45, Lloh46
	.cfi_endproc
                                        ; -- End function
	.globl	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm ; -- Begin function _ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm
	.weak_def_can_be_hidden	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm
	.p2align	2
__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm: ; @_ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm
	.cfi_startproc
; %bb.0:
	stp	x22, x21, [sp, #-48]!           ; 16-byte Folded Spill
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	mov	x19, x2
	cmp	x2, #22
	b.hi	LBB11_2
; %bb.1:
	strb	w19, [x0, #23]
	b	LBB11_4
LBB11_2:
	mov	x20, x0
	mov	x8, #9223372036854775800        ; =0x7ffffffffffffff8
	cmp	x19, x8
	b.hs	LBB11_5
; %bb.3:
	mov	x22, x1
	orr	x8, x19, #0x7
	cmp	x8, #23
	mov	w9, #25                         ; =0x19
	csinc	x21, x9, x8, eq
	mov	x0, x21
	bl	__Znwm
	orr	x8, x21, #0x8000000000000000
	stp	x19, x8, [x20, #8]
	str	x0, [x20]
	mov	x1, x22
LBB11_4:
	add	x2, x19, #1
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp], #48             ; 16-byte Folded Reload
	b	_memmove
LBB11_5:
	mov	x0, x20
	bl	__ZNKSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE20__throw_length_errorB8ne190107Ev
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED1Ev
__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED1Ev: ; @"_ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED1Ev"
	.cfi_startproc
; %bb.0:
Lloh47:
	adrp	x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGE+16
Lloh48:
	add	x8, x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGEOFF+16
	str	x8, [x0]
	ldrsb	w8, [x0, #31]
	tbnz	w8, #31, LBB12_2
; %bb.1:
	ret
LBB12_2:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	ldr	x8, [x0, #8]
	ldr	x9, [x0, #24]
	and	x1, x9, #0x7fffffffffffffff
	mov	x19, x0
	mov	x0, x8
	bl	__ZdlPvm
	mov	x0, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
	.loh AdrpAdd	Lloh47, Lloh48
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED0Ev
__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED0Ev: ; @"_ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED0Ev"
	.cfi_startproc
; %bb.0:
Lloh49:
	adrp	x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGE+16
Lloh50:
	add	x8, x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGEOFF+16
	str	x8, [x0]
	ldrsb	w8, [x0, #31]
	tbnz	w8, #31, LBB13_2
; %bb.1:
	mov	w1, #32                         ; =0x20
	b	__ZdlPvm
LBB13_2:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	ldr	x8, [x0, #8]
	ldr	x9, [x0, #24]
	and	x1, x9, #0x7fffffffffffffff
	mov	x19, x0
	mov	x0, x8
	bl	__ZdlPvm
	mov	x0, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	mov	w1, #32                         ; =0x20
	b	__ZdlPvm
	.loh AdrpAdd	Lloh49, Lloh50
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEv
__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEv: ; @"_ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEv"
Lfunc_begin6:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception6
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x20, x0
	mov	w0, #32                         ; =0x20
	bl	__Znwm
	mov	x19, x0
Lloh51:
	adrp	x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGE+16
Lloh52:
	add	x8, x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGEOFF+16
	str	x8, [x0], #8
	ldrsb	w8, [x20, #31]
	tbnz	w8, #31, LBB14_2
; %bb.1:
	ldur	q0, [x20, #8]
	str	q0, [x0]
	ldur	x8, [x20, #24]
	str	x8, [x0, #16]
	b	LBB14_3
LBB14_2:
	ldp	x1, x2, [x20, #8]
Ltmp67:
	bl	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm
Ltmp68:
LBB14_3:
	mov	x0, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
LBB14_4:
Ltmp69:
	mov	x20, x0
	mov	x0, x19
	mov	w1, #32                         ; =0x20
	bl	__ZdlPvm
	mov	x0, x20
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh51, Lloh52
Lfunc_end6:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table14:
Lexception6:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end6-Lcst_begin6
Lcst_begin6:
	.uleb128 Lfunc_begin6-Lfunc_begin6      ; >> Call Site 1 <<
	.uleb128 Ltmp67-Lfunc_begin6            ;   Call between Lfunc_begin6 and Ltmp67
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp67-Lfunc_begin6            ; >> Call Site 2 <<
	.uleb128 Ltmp68-Ltmp67                  ;   Call between Ltmp67 and Ltmp68
	.uleb128 Ltmp69-Lfunc_begin6            ;     jumps to Ltmp69
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp68-Lfunc_begin6            ; >> Call Site 3 <<
	.uleb128 Lfunc_end6-Ltmp68              ;   Call between Ltmp68 and Lfunc_end6
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end6:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2                               ; -- Begin function _ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEPNS0_6__baseIS5_EE
__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEPNS0_6__baseIS5_EE: ; @"_ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEPNS0_6__baseIS5_EE"
	.cfi_startproc
; %bb.0:
Lloh53:
	adrp	x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGE+16
Lloh54:
	add	x8, x8, __ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE@PAGEOFF+16
	str	x8, [x1], #8
	ldrsb	w8, [x0, #31]
	tbnz	w8, #31, LBB15_2
; %bb.1:
	ldur	q0, [x0, #8]
	ldur	x8, [x0, #24]
	str	x8, [x1, #16]
	str	q0, [x1]
	ret
LBB15_2:
	ldp	x8, x2, [x0, #8]
	mov	x0, x1
	mov	x1, x8
	b	__ZNSt3__112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE25__init_copy_ctor_externalEPKcm
	.loh AdrpAdd	Lloh53, Lloh54
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7destroyEv
__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7destroyEv: ; @"_ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7destroyEv"
	.cfi_startproc
; %bb.0:
	ldrsb	w8, [x0, #31]
	tbnz	w8, #31, LBB16_2
; %bb.1:
	ret
LBB16_2:
	ldr	x8, [x0, #8]
	ldr	x9, [x0, #24]
	and	x1, x9, #0x7fffffffffffffff
	mov	x0, x8
	b	__ZdlPvm
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE18destroy_deallocateEv
__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE18destroy_deallocateEv: ; @"_ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE18destroy_deallocateEv"
	.cfi_startproc
; %bb.0:
	ldrsb	w8, [x0, #31]
	tbnz	w8, #31, LBB17_2
; %bb.1:
	mov	w1, #32                         ; =0x20
	b	__ZdlPvm
LBB17_2:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	ldr	x8, [x0, #8]
	ldr	x9, [x0, #24]
	and	x1, x9, #0x7fffffffffffffff
	mov	x19, x0
	mov	x0, x8
	bl	__ZdlPvm
	mov	x0, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	mov	w1, #32                         ; =0x20
	b	__ZdlPvm
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEclEv
__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEclEv: ; @"_ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEclEv"
Lfunc_begin7:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception7
; %bb.0:
	sub	sp, sp, #48
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	ldr	x8, [x0, #8]!
	ldrsb	x9, [x0, #23]
	cmp	x9, #0
	csel	x1, x8, x0, lt
	ldr	x8, [x0, #8]
	cmp	w9, #0
	csel	x2, x8, x9, lt
Lloh55:
	adrp	x0, __ZNSt3__14coutE@GOTPAGE
Lloh56:
	ldr	x0, [x0, __ZNSt3__14coutE@GOTPAGEOFF]
	bl	__ZNSt3__124__put_character_sequenceB8ne190107IcNS_11char_traitsIcEEEERNS_13basic_ostreamIT_T0_EES7_PKS4_m
	mov	x19, x0
	ldr	x8, [x0]
	ldur	x9, [x8, #-24]
	add	x8, sp, #8
	add	x0, x0, x9
	bl	__ZNKSt3__18ios_base6getlocEv
Ltmp70:
Lloh57:
	adrp	x1, __ZNSt3__15ctypeIcE2idE@GOTPAGE
Lloh58:
	ldr	x1, [x1, __ZNSt3__15ctypeIcE2idE@GOTPAGEOFF]
	add	x0, sp, #8
	bl	__ZNKSt3__16locale9use_facetERNS0_2idE
Ltmp71:
; %bb.1:
	ldr	x8, [x0]
	ldr	x8, [x8, #56]
Ltmp72:
	mov	w1, #10                         ; =0xa
	blr	x8
Ltmp73:
; %bb.2:
	mov	x20, x0
	add	x0, sp, #8
	bl	__ZNSt3__16localeD1Ev
	mov	x0, x19
	mov	x1, x20
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE3putEc
	mov	x0, x19
	bl	__ZNSt3__113basic_ostreamIcNS_11char_traitsIcEEE5flushEv
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #48
	ret
LBB18_3:
Ltmp74:
	mov	x19, x0
	add	x0, sp, #8
	bl	__ZNSt3__16localeD1Ev
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpLdrGot	Lloh57, Lloh58
	.loh AdrpLdrGot	Lloh55, Lloh56
Lfunc_end7:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table18:
Lexception7:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end7-Lcst_begin7
Lcst_begin7:
	.uleb128 Lfunc_begin7-Lfunc_begin7      ; >> Call Site 1 <<
	.uleb128 Ltmp70-Lfunc_begin7            ;   Call between Lfunc_begin7 and Ltmp70
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp70-Lfunc_begin7            ; >> Call Site 2 <<
	.uleb128 Ltmp73-Ltmp70                  ;   Call between Ltmp70 and Ltmp73
	.uleb128 Ltmp74-Lfunc_begin7            ;     jumps to Ltmp74
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp73-Lfunc_begin7            ; >> Call Site 3 <<
	.uleb128 Lfunc_end7-Ltmp73              ;   Call between Ltmp73 and Lfunc_end7
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end7:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.p2align	2                               ; -- Begin function _ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE6targetERKSt9type_info
__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE6targetERKSt9type_info: ; @"_ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE6targetERKSt9type_info"
	.cfi_startproc
; %bb.0:
	ldr	x8, [x1, #8]
Lloh59:
	adrp	x9, __ZTSZ8producervE3$_0@PAGE
Lloh60:
	add	x9, x9, __ZTSZ8producervE3$_0@PAGEOFF
	cmp	x8, x9
	b.ne	LBB19_2
LBB19_1:
	add	x0, x0, #8
	ret
LBB19_2:
	tst	x8, x9
	b.lt	LBB19_4
; %bb.3:
	mov	x0, #0                          ; =0x0
	ret
LBB19_4:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x19, x0
	and	x0, x8, #0x7fffffffffffffff
	and	x1, x9, #0x7fffffffffffffff
	bl	_strcmp
	mov	x8, x0
	mov	x0, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	cbz	w8, LBB19_1
; %bb.5:
	mov	x0, #0                          ; =0x0
	ret
	.loh AdrpAdd	Lloh59, Lloh60
	.cfi_endproc
                                        ; -- End function
	.p2align	2                               ; -- Begin function _ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE11target_typeEv
__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE11target_typeEv: ; @"_ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE11target_typeEv"
	.cfi_startproc
; %bb.0:
Lloh61:
	adrp	x0, __ZTIZ8producervE3$_0@PAGE
Lloh62:
	add	x0, x0, __ZTIZ8producervE3$_0@PAGEOFF
	ret
	.loh AdrpAdd	Lloh61, Lloh62
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_ ; -- Begin function _ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_
	.globl	__ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_
	.weak_def_can_be_hidden	__ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_
	.p2align	2
__ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_: ; @_ZNSt3__16threadC2IRFvvEJETnNS_9enable_ifIXntsr7is_sameIu14__remove_cvrefIT_ES0_EE5valueEiE4typeELi0EEEOS5_DpOT0_
Lfunc_begin8:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception8
; %bb.0:
	sub	sp, sp, #64
	stp	x22, x21, [sp, #16]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #32]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #48]             ; 16-byte Folded Spill
	add	x29, sp, #48
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	mov	x20, x1
	mov	x19, x0
	mov	w0, #8                          ; =0x8
	bl	__Znwm
	mov	x21, x0
Ltmp75:
	bl	__ZNSt3__115__thread_structC1Ev
Ltmp76:
; %bb.1:
	str	x21, [sp, #8]
Ltmp78:
	mov	w0, #16                         ; =0x10
	bl	__Znwm
Ltmp79:
; %bb.2:
	mov	x3, x0
	stp	x0, xzr, [sp]
	stp	x21, x20, [x0]
Ltmp81:
Lloh63:
	adrp	x2, __ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_@PAGE
Lloh64:
	add	x2, x2, __ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_@PAGEOFF
	mov	x0, x19
	mov	x1, #0                          ; =0x0
	bl	_pthread_create
Ltmp82:
; %bb.3:
	cbnz	w0, LBB21_5
; %bb.4:
	mov	x0, x19
	ldp	x29, x30, [sp, #48]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #32]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #64
	ret
LBB21_5:
Ltmp83:
Lloh65:
	adrp	x1, l_.str.5@PAGE
Lloh66:
	add	x1, x1, l_.str.5@PAGEOFF
	bl	__ZNSt3__120__throw_system_errorEiPKc
Ltmp84:
; %bb.6:
	brk	#0x1
LBB21_7:
Ltmp80:
	mov	x19, x0
	b	LBB21_10
LBB21_8:
Ltmp77:
	mov	x19, x0
	mov	x0, x21
	mov	w1, #8                          ; =0x8
	bl	__ZdlPvm
	mov	x0, x19
	bl	__Unwind_Resume
LBB21_9:
Ltmp85:
	mov	x19, x0
	mov	x0, sp
	bl	__ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev
LBB21_10:
	add	x0, sp, #8
	bl	__ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev
	mov	x0, x19
	bl	__Unwind_Resume
	.loh AdrpAdd	Lloh63, Lloh64
	.loh AdrpAdd	Lloh65, Lloh66
Lfunc_end8:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table21:
Lexception8:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end8-Lcst_begin8
Lcst_begin8:
	.uleb128 Lfunc_begin8-Lfunc_begin8      ; >> Call Site 1 <<
	.uleb128 Ltmp75-Lfunc_begin8            ;   Call between Lfunc_begin8 and Ltmp75
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp75-Lfunc_begin8            ; >> Call Site 2 <<
	.uleb128 Ltmp76-Ltmp75                  ;   Call between Ltmp75 and Ltmp76
	.uleb128 Ltmp77-Lfunc_begin8            ;     jumps to Ltmp77
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp78-Lfunc_begin8            ; >> Call Site 3 <<
	.uleb128 Ltmp79-Ltmp78                  ;   Call between Ltmp78 and Ltmp79
	.uleb128 Ltmp80-Lfunc_begin8            ;     jumps to Ltmp80
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp81-Lfunc_begin8            ; >> Call Site 4 <<
	.uleb128 Ltmp84-Ltmp81                  ;   Call between Ltmp81 and Ltmp84
	.uleb128 Ltmp85-Lfunc_begin8            ;     jumps to Ltmp85
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp84-Lfunc_begin8            ; >> Call Site 5 <<
	.uleb128 Lfunc_end8-Ltmp84              ;   Call between Ltmp84 and Lfunc_end8
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end8:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_ ; -- Begin function _ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_
	.globl	__ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_
	.weak_definition	__ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_
	.p2align	2
__ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_: ; @_ZNSt3__114__thread_proxyB8ne190107INS_5tupleIJNS_10unique_ptrINS_15__thread_structENS_14default_deleteIS3_EEEEPFvvEEEEEEPvSA_
Lfunc_begin9:
	.cfi_startproc
	.cfi_personality 155, ___gxx_personality_v0
	.cfi_lsda 16, Lexception9
; %bb.0:
	sub	sp, sp, #48
	stp	x20, x19, [sp, #16]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #32]             ; 16-byte Folded Spill
	add	x29, sp, #32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x19, x0
	str	x0, [sp, #8]
Ltmp86:
	bl	__ZNSt3__119__thread_local_dataEv
Ltmp87:
; %bb.1:
	ldr	x1, [x19]
	str	xzr, [x19]
	ldr	x0, [x0]
Ltmp88:
	bl	_pthread_setspecific
Ltmp89:
; %bb.2:
	ldr	x8, [x19, #8]
Ltmp90:
	blr	x8
Ltmp91:
; %bb.3:
	ldr	x0, [x19]
	str	xzr, [x19]
	cbz	x0, LBB22_5
; %bb.4:
	bl	__ZNSt3__115__thread_structD1Ev
	mov	w1, #8                          ; =0x8
	bl	__ZdlPvm
LBB22_5:
	mov	x0, x19
	mov	w1, #16                         ; =0x10
	bl	__ZdlPvm
	mov	x0, #0                          ; =0x0
	ldp	x29, x30, [sp, #32]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #16]             ; 16-byte Folded Reload
	add	sp, sp, #48
	ret
LBB22_6:
Ltmp92:
	mov	x19, x0
	add	x0, sp, #8
	bl	__ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev
	mov	x0, x19
	bl	__Unwind_Resume
Lfunc_end9:
	.cfi_endproc
	.section	__TEXT,__gcc_except_tab
	.p2align	2, 0x0
GCC_except_table22:
Lexception9:
	.byte	255                             ; @LPStart Encoding = omit
	.byte	255                             ; @TType Encoding = omit
	.byte	1                               ; Call site Encoding = uleb128
	.uleb128 Lcst_end9-Lcst_begin9
Lcst_begin9:
	.uleb128 Ltmp86-Lfunc_begin9            ; >> Call Site 1 <<
	.uleb128 Ltmp91-Ltmp86                  ;   Call between Ltmp86 and Ltmp91
	.uleb128 Ltmp92-Lfunc_begin9            ;     jumps to Ltmp92
	.byte	0                               ;   On action: cleanup
	.uleb128 Ltmp91-Lfunc_begin9            ; >> Call Site 2 <<
	.uleb128 Lfunc_end9-Ltmp91              ;   Call between Ltmp91 and Lfunc_end9
	.byte	0                               ;     has no landing pad
	.byte	0                               ;   On action: cleanup
Lcst_end9:
	.p2align	2, 0x0
                                        ; -- End function
	.section	__TEXT,__text,regular,pure_instructions
	.private_extern	__ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev ; -- Begin function _ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev
	.globl	__ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev
	.weak_def_can_be_hidden	__ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev
	.p2align	2
__ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev: ; @_ZNSt3__110unique_ptrINS_5tupleIJNS0_INS_15__thread_structENS_14default_deleteIS2_EEEEPFvvEEEENS3_IS8_EEED1B8ne190107Ev
	.cfi_startproc
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	ldr	x19, [x0]
	str	xzr, [x0]
	cbz	x19, LBB23_4
; %bb.1:
	mov	x20, x0
	ldr	x0, [x19]
	str	xzr, [x19]
	cbz	x0, LBB23_3
; %bb.2:
	bl	__ZNSt3__115__thread_structD1Ev
	mov	w1, #8                          ; =0x8
	bl	__ZdlPvm
LBB23_3:
	mov	x0, x19
	mov	w1, #16                         ; =0x10
	bl	__ZdlPvm
	mov	x0, x20
LBB23_4:
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	ret
	.cfi_endproc
                                        ; -- End function
	.private_extern	__ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev ; -- Begin function _ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev
	.globl	__ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev
	.weak_def_can_be_hidden	__ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev
	.p2align	2
__ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev: ; @_ZNSt3__110unique_ptrINS_15__thread_structENS_14default_deleteIS1_EEED1B8ne190107Ev
	.cfi_startproc
; %bb.0:
	mov	x8, x0
	ldr	x0, [x0]
	str	xzr, [x8]
	cbz	x0, LBB24_2
; %bb.1:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	mov	x19, x8
	bl	__ZNSt3__115__thread_structD1Ev
	mov	w1, #8                          ; =0x8
	bl	__ZdlPvm
	mov	x8, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
LBB24_2:
	mov	x0, x8
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__StaticInit,regular,pure_instructions
	.p2align	2                               ; -- Begin function _GLOBAL__sub_I_function.cc
__GLOBAL__sub_I_function.cc:            ; @_GLOBAL__sub_I_function.cc
	.cfi_startproc
; %bb.0:
	stp	x20, x19, [sp, #-32]!           ; 16-byte Folded Spill
	stp	x29, x30, [sp, #16]             ; 16-byte Folded Spill
	add	x29, sp, #16
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
Lloh67:
	adrp	x0, __ZNSt3__15mutexD1Ev@GOTPAGE
Lloh68:
	ldr	x0, [x0, __ZNSt3__15mutexD1Ev@GOTPAGEOFF]
Lloh69:
	adrp	x1, _kMutex@PAGE
Lloh70:
	add	x1, x1, _kMutex@PAGEOFF
Lloh71:
	adrp	x19, ___dso_handle@PAGE
Lloh72:
	add	x19, x19, ___dso_handle@PAGEOFF
	mov	x2, x19
	bl	___cxa_atexit
Lloh73:
	adrp	x0, __ZNSt3__118condition_variableD1Ev@GOTPAGE
Lloh74:
	ldr	x0, [x0, __ZNSt3__118condition_variableD1Ev@GOTPAGEOFF]
Lloh75:
	adrp	x1, _cond_var@PAGE
Lloh76:
	add	x1, x1, _cond_var@PAGEOFF
	mov	x2, x19
	bl	___cxa_atexit
Lloh77:
	adrp	x1, _kList@PAGE
Lloh78:
	add	x1, x1, _kList@PAGEOFF
	stp	x1, x1, [x1]
	str	xzr, [x1, #16]
Lloh79:
	adrp	x0, __ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev@GOTPAGE
Lloh80:
	ldr	x0, [x0, __ZNSt3__14listINS_8functionIFvvEEENS_9allocatorIS3_EEED1Ev@GOTPAGEOFF]
	mov	x2, x19
	ldp	x29, x30, [sp, #16]             ; 16-byte Folded Reload
	ldp	x20, x19, [sp], #32             ; 16-byte Folded Reload
	b	___cxa_atexit
	.loh AdrpLdrGot	Lloh79, Lloh80
	.loh AdrpAdd	Lloh77, Lloh78
	.loh AdrpAdd	Lloh75, Lloh76
	.loh AdrpLdrGot	Lloh73, Lloh74
	.loh AdrpAdd	Lloh71, Lloh72
	.loh AdrpAdd	Lloh69, Lloh70
	.loh AdrpLdrGot	Lloh67, Lloh68
	.cfi_endproc
                                        ; -- End function
	.section	__DATA,__data
	.globl	_kMutex                         ; @kMutex
	.p2align	3, 0x0
_kMutex:
	.quad	850045863                       ; 0x32aaaba7
	.space	56

	.globl	_cond_var                       ; @cond_var
	.p2align	3, 0x0
_cond_var:
	.quad	1018212795                      ; 0x3cb0b1bb
	.space	40

	.globl	_kReady                         ; @kReady
.zerofill __DATA,__common,_kReady,1,0
	.globl	_kList                          ; @kList
.zerofill __DATA,__common,_kList,24,3
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Produce"

l_.str.4:                               ; @.str.4
	.asciz	"basic_string"

	.section	__DATA,__const
	.p2align	3, 0x0                          ; @"_ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE"
__ZTVNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE:
	.quad	0
	.quad	__ZTINSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE
	.quad	__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED1Ev
	.quad	__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEED0Ev
	.quad	__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEv
	.quad	__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7__cloneEPNS0_6__baseIS5_EE
	.quad	__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE7destroyEv
	.quad	__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE18destroy_deallocateEv
	.quad	__ZNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEclEv
	.quad	__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE6targetERKSt9type_info
	.quad	__ZNKSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEE11target_typeEv

	.section	__TEXT,__const
__ZTSNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE: ; @"_ZTSNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE"
	.asciz	"NSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE"

	.private_extern	__ZTSNSt3__110__function6__baseIFvvEEE ; @_ZTSNSt3__110__function6__baseIFvvEEE
	.globl	__ZTSNSt3__110__function6__baseIFvvEEE
	.weak_definition	__ZTSNSt3__110__function6__baseIFvvEEE
__ZTSNSt3__110__function6__baseIFvvEEE:
	.asciz	"NSt3__110__function6__baseIFvvEEE"

	.private_extern	__ZTINSt3__110__function6__baseIFvvEEE ; @_ZTINSt3__110__function6__baseIFvvEEE
	.section	__DATA,__const
	.globl	__ZTINSt3__110__function6__baseIFvvEEE
	.weak_definition	__ZTINSt3__110__function6__baseIFvvEEE
	.p2align	3, 0x0
__ZTINSt3__110__function6__baseIFvvEEE:
	.quad	__ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	__ZTSNSt3__110__function6__baseIFvvEEE-9223372036854775808

	.p2align	3, 0x0                          ; @"_ZTINSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE"
__ZTINSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE:
	.quad	__ZTVN10__cxxabiv120__si_class_type_infoE+16
	.quad	__ZTSNSt3__110__function6__funcIZ8producervE3$_0NS_9allocatorIS2_EEFvvEEE
	.quad	__ZTINSt3__110__function6__baseIFvvEEE

	.section	__TEXT,__const
__ZTSZ8producervE3$_0:                  ; @"_ZTSZ8producervE3$_0"
	.asciz	"Z8producervE3$_0"

	.section	__DATA,__const
	.p2align	3, 0x0                          ; @"_ZTIZ8producervE3$_0"
__ZTIZ8producervE3$_0:
	.quad	__ZTVN10__cxxabiv117__class_type_infoE+16
	.quad	__ZTSZ8producervE3$_0

	.section	__TEXT,__cstring,cstring_literals
l_.str.5:                               ; @.str.5
	.asciz	"thread constructor failed"

	.section	__DATA,__mod_init_func,mod_init_funcs
	.p2align	3, 0x0
	.quad	__GLOBAL__sub_I_function.cc
.subsections_via_symbols
