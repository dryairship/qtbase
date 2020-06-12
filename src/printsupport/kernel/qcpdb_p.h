#ifndef QCPDB_P_H
#define QCPDB_P_H

#include <iostream>

#if __has_include(<cpdb-libs-frontend.h>)
  #define QCPDB_USING_CPDB 1
#else
  #define QCPDB_USING_CPDB 0
#endif

void initCPDB();

#endif
