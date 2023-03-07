define freertos_fault

  printf "==========================================================================================\n"

  set $fe_stack = (uint32_t *)(((TCB_t *)pxCurrentTCB)->pxTopOfStack)

  set $fe_mepc = $fe_stack[0]
  set $fe_ra   = $fe_stack[1]
  set $fe_x5   = $fe_stack[2]
  set $fe_x6   = $fe_stack[3]
  set $fe_x7   = $fe_stack[4]
  set $fe_x8   = $fe_stack[5]
  set $fe_x9   = $fe_stack[6]
  set $fe_x10  = $fe_stack[7]

  set $fe_x11  = $fe_stack[8]
  set $fe_x12  = $fe_stack[9]
  set $fe_x13  = $fe_stack[10]
  set $fe_x14  = $fe_stack[11]
  set $fe_x15  = $fe_stack[12]
  set $fe_x16  = $fe_stack[13]
  set $fe_x17  = $fe_stack[14]
  set $fe_x18  = $fe_stack[15]

  set $fe_x19  = $fe_stack[16]
  set $fe_x20  = $fe_stack[17]
  set $fe_x21  = $fe_stack[18]
  set $fe_x22  = $fe_stack[19]
  set $fe_x23  = $fe_stack[20]
  set $fe_x24  = $fe_stack[21]
  set $fe_x25  = $fe_stack[22]
  set $fe_x26  = $fe_stack[23]

  set $fe_x27  = $fe_stack[24]
  set $fe_x28  = $fe_stack[25]
  set $fe_x29  = $fe_stack[26]
  set $fe_x30  = $fe_stack[27]
  set $fe_x31  = $fe_stack[28]
  set $fe_msts = $fe_stack[29]

  printf "\ncurrent task: %s\n", ((TCB_t *)pxCurrentTCB)->pcTaskName
  printf "        task SP: 0x%08x\n", ((TCB_t *)pxCurrentTCB)->pxTopOfStack
  printf "        Context: pxCode   RA       x5       x6       x7       x8       x9       x10     \n"
  printf "               : %08x %08x %08x %08x %08x %08x %08x %08x\n", $fe_mepc, $fe_ra, $fe_x5, $fe_x6, $fe_x7, $fe_x8, $fe_x9, $fe_x10
  printf "               : x11      x12      x13      x14      x15      x16      x17      x18     \n"
  printf "               : %08x %08x %08x %08x %08x %08x %08x %08x\n", $fe_x11, $fe_x12, $fe_x13, $fe_x14, $fe_x15, $fe_x16, $fe_x17, $fe_x18
  printf "               : x19      x20      x21      x22      x23      x24      x25      x26     \n"
  printf "               : %08x %08x %08x %08x %08x %08x %08x %08x\n", $fe_x19, $fe_x20, $fe_x21, $fe_x22, $fe_x23, $fe_x24, $fe_x25, $fe_x26
  printf "               : x27      x28      x29      x30      x31      mstatus\n"
  printf "               : %08x %08x %08x %08x %08x %08x\n", $fe_x27, $fe_x28, $fe_x29, $fe_x30, $fe_x31, $fe_msts,

  printf "==========================================================================================\n"
  printf "Try backing to the exception point:\n"

  printf "    from context:\n"
  info line *($fe_mepc)
  info line *($fe_ra)

  set $pc = $fe_mepc
  set $ra = $fe_ra
  set $sp = ($fe_stack + 30)

  printf "\n    from GDB backtrace:\n"
  set backtrace limit 16
  bt
  printf "==========================================================================================\n"
end

document freertos_fault
  dump freertos exception
end
