ENTRY(hypercalltable)
    .quad do_set_trap_table /* 0 */
    .quad do_mmu_update
    .quad do_set_gdt
    .quad do_stack_switch
    .quad do_set_callbacks
    .quad do_fpu_task_switch /* 5 */
    // skip...
    .quad do_ni_hypercall /* reservedforXenClient */
    .quad do_xenpmu_op /* 40 */
    .rept __HYPERVISOR_arch_0−((.−hypercall_table)/8)
    .quad do_ni_hypercall
    .endr
    .quad do_mca /* 48 */
    .quad paging_domctl_continuation
    .rept NR hypercalls−((.−hypercalltable)/8)
    .quad do_ni_hypercall
    .endr
