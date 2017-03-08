/*
**                   Copyright 2010 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
** Description:
**
** Dumps exception information in the same format as the VM itself
** uses when there is no user specified exception handler.
**
** The somewhat unpleasant look of the code is due to the fact that any
** use of local variables (and constant strings as well as references and
** array slices currently all count as such) uses up stack space.
**
** See the t User's Guide for a nicer looking (but with higher stack usage)
** version of this code.
**
*/

on exception {
  static char general_str[47] =
    "Execution error %d at %d after %d cycles.\n";
  printf(general_str, this.error, this.pc, this.cycle);
  if (this.line) {
    static char line_str[37] = "Execution stopped after/at line %d.\n";
    printf(line_str, this.line);
  } else {
    static char noline_str[45] = "Line number information excluded from code.\n";
    printf(noline_str);
  }

  static char stack_str[14] = "Stack: (%d):\n";
  printf(stack_str, this.stack.count);

  static char addr_str[7] = "%04x: ";
  static char int_str[6]  = "%08x ";
  static char line_str[8] = "%s%s%s\n";
  static char buf[6 + 4 * 9 + 1];
  static char int_buf[10];
  static char space[16 * 3 + 1];
  static char ascii[16 + 1];

  static int sp, i, j, fp, frame_size;
  fp = this.locals;
  j = 0;
  for(sp = this.stack.count - 1; sp >= 0; sp--) {
    frame_size = this.stack[fp - 1];
    if (sp >= fp + frame_size) {
      static char stacked_str[16] = "%4d: 0x%08x %d\n";
      printf(stacked_str, sp, this.stack[sp], this.stack[sp]);
    } else {
      if (frame_size) {
        static char locals[27]  = "Locals (%d @ 0x%08x):\n";
        printf(locals, frame_size * 4, this.stack_base + fp * 4);

        for(i = 0; i < frame_size; i += 4) {
          sprintf(buf, addr_str, i);
          for(j = 0; (j < 4) && (i + j < frame_size); j++) {
            sprintf(int_buf, int_str, this.stack[fp + i + j]);
            strcat(buf, int_buf);
          }
          space = ' ';
          space[(4 - j) * 9] = '\0';
          ascii = '.';
          for(j = 0; (j < 16) && (i + j / 4 < frame_size); j++) {
            static byte b;
            b = this.stack[fp + i + j / 4] >> (8 * (j % 4));
            if ((b >= 32) && (b < 127)) {
              ascii[j] = b;
            }
          }
          ascii[j] = '\0';
          printf(line_str, buf, space, ascii);
        }
      }
      sp -= frame_size;
      sp--;
      fp  = this.stack[sp--];
      if (sp >= 0) {
        static char ret_str[20] = "Return address: %d\n";
        printf(ret_str, this.stack[sp]);
      }
    }
  }

  // This code is a copy of that just above, since we cannot (currently) pass
  // the data to a function if we are to keep the local variables intact.
  static char globals[15] = "Globals (%d):\n";
  printf(globals, this.globals.count * 4);
  for(i = 0; i < this.globals.count; i += 4) {
    sprintf(buf, addr_str, i * 4);
    for(j = 0; (j < 4) && (i + j < this.globals.count); j++) {
      sprintf(int_buf, int_str, this.globals[i + j]);
      strcat(buf, int_buf);
    }
    space = ' ';
    space[(4 - j) * 9] = '\0';
    ascii = '.';
    for(j = 0; (j < 16) && (i + j / 4 < this.globals.count); j++) {
      static byte b;
      b = this.globals[i + j / 4] >> (8 * (j % 4));
      if ((b >= 32) && (b < 127)) {
        ascii[j] = b;
      }
    }
    ascii[j] = '\0';
    printf(line_str, buf, space, ascii);
  }
}
