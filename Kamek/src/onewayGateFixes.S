#---

#include <asm_setup.S>
.text
.align 4

.global FixedOpeningSound
.extern Delete__7fBase_cFv
.extern PlaySoundWithFunctionB4
.extern SoundRelatedClass
.extern DetachSound__Q34nw4r3snd11SoundHandleFv
FixedOpeningSound:

	# create a SoundHandle
	li r3, 0
	stw r3, 0xC(r1)

	# call PSWFB4
	lis r3, SoundRelatedClass@h
	ori r3, r3, SoundRelatedClass@l
	lwz r3, 0(r3)
	addi r4, r1, 0xC
	li r5, 866 #SE_VOC_MA_NOTICE - Audio [724] from BANK 145
	li r6, 1
	bl PlaySoundWithFunctionB4

	# destroy SoundHandle
	addi r3, r1, 0xC
	bl DetachSound__Q34nw4r3snd11SoundHandleFv

	mr r3, r31
	lwz r31, 8(r1)
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	b Delete__7fBase_cFv
	
.global FixedShuttingSound
FixedShuttingSound:

	# create a SoundHandle
	li r3, 0
	stw r3, 0xC(r1)

	# call PSWFB4
	lis r3, SoundRelatedClass@h
	ori r3, r3, SoundRelatedClass@l
	lwz r3, 0(r3)
	addi r4, r1, 0xC
	li r5, 65 #SE_DEMO_OP_V_MA_WAIT - Audio [630 - 632] from BANK 145
	li r6, 1
	bl PlaySoundWithFunctionB4

	# destroy SoundHandle
	addi r3, r1, 0xC
	bl DetachSound__Q34nw4r3snd11SoundHandleFv

	mr r3, r31
	lwz r31, 8(r1)
	lwz r0, 0x14(r1)
	mtlr r0
	addi r1, r1, 0x10
	b Delete__7fBase_cFv