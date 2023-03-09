.text
.align 4
#include <asm_setup.S>

.set infiniteLivesEnabled, 0x4

.global InfiniteLives
InfiniteLives:
#ifdef TARGET_TRICKY
    blr
#else
    SaveVolatileRegisters

    CheckSaveMember infiniteLivesEnabled

    // Otherwise restore replaced instruction and continue the function
    RestoreVolatileRegisters
    mflr    r0
    stwu    sp, -0x4(sp)
    stw     r3, 0x8(sp)
    lis     r3, InfiniteLivesContinue@ha
    addi    r3, r3, InfiniteLivesContinue@l
    mtctr   r3
    lwz     r3, 0x8(sp)
    addi    sp, sp, 0x4
    bctr    #0x24C005C (No, it's a NSMBU code)
#endif