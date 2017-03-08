#include "j1587lib.h"

void GetErrorText(J1587Status err, char* buf, int bufsiz);
void CheckAndPrintError(J1587Status stat, char * str);
void PrintHwType(int t, int chan);
