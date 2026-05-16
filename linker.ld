OUTPUT_ARCH(arm)
MEMORY {
        /* 
                Overlay load address + offset to common area
                see https://docs.google.com/document/d/1Rs4icdYtiM6KYnWxMkdlw7jpWrH7qw5v6LOfDWIiYho
        */
        main    : ORIGIN = 0x23DB1B0, LENGTH = 0x4E50
        /* Change/Add memory locations here */
        /* NEW    : ORIGIN = 0x2000000, LENGTH = 0x800 */
        
        scene_selector : ORIGIN = 0x238A140, LENGTH = 0x24A0
        create_main_menu    : ORIGIN = 0x23310E8, LENGTH = 0x1A0
        update_main_menu    : ORIGIN = 0x2331788, LENGTH = 0x650
        data_main_menu      : ORIGIN = 0x233250C, LENGTH = 0x29C6
        displayui           : ORIGIN = 0x2335A14, LENGTH = 0x6E8
        helpers             : ORIGIN = 0x22DF814, LENGTH = 0x500
        chains              : ORIGIN = 0x2382820, LENGTH = 0x7A34
}

SECTIONS {
        /*
                Add more sections for the linker, each section using a different memory location
                This allows to spread your code on different overlays
                You can change the section of one symbol (function or variable) in your C code using: __attribute__((section(".YOURSECTIONNAME")))
                Note that section names are standardized and in the form ".text.OV.NB.SPECIAL"
                Where OV is between 'ov' (ARM9 Overlay) and 'arm' (Main ARM Binary)
                NB is the OV number (7 or 9 for 'arm', any ARM9 Overlay number for 'ov')
                SPECIAL is a distinct name, this is only to distinguish between 2 sections on the same overlay
        */
        /*.text.OV.NB.SPECIAL : {
                *(.YOURSECTIONNAME)
        } >NEW = 0xff*/
        
        .text.ov.24.playbill : {
                *(.text.playbill)
                *(.data.playbill)
        } >scene_selector = 0xff
        
        .text.ov.1.CREATETOPMENU : {
                *(.CREATE)
        } >create_main_menu = 0xff

        .text.ov.1.UPDATETOPMENU : {
                *(.UPDATE)
        } >update_main_menu = 0xff

        .text.ov.1.DATATOPMENU : {
                *(.PAIN)
        } >data_main_menu = 0xff
        
        .text.ov.29.displayui : {
                *(.text.dungeon)
        } >displayui = 0xff
        
        .text.ov.29.helpers : {
                *(.text.dungeon_helper)
        } >helpers = 0xff
        
        .text.ov.31.chains : {
                *(.data.chains)
        } >chains = 0xff
        
        
        /*
                Main Section (Default section, in ARM9 overlay 36)
                Note that you should add your special sections before that
        */
        .text.ov.36.main : {
                *(.init)
                *(.text)
                *(.text.*)
                *(.fini)
                *(.ctors)
                *(.dtors)
                *(.rodata)
                *(.rodata.*)
                *(.data)
                *(.data.*)
                *(COMMON)
                . = ALIGN(4);
                *(.bss)
        } >main = 0xff
}
