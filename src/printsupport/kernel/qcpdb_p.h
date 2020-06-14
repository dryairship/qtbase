#ifndef QCPDB_P_H
#define QCPDB_P_H

#include <iostream>
#include <QList>

// Check if cpdb-libs is installed on the system
#if __has_include(<cpdb-libs-frontend.h>)
  #define QCPDB_USING_CPDB 1
#else
  #define QCPDB_USING_CPDB 0
#endif

struct _FrontendObj;
using FrontendObj = _FrontendObj;

struct _PrinterObj;
using PrinterObj = _PrinterObj;

typedef QMap<QPair<char*, char*>, char*> CpdbPrinterList;

class CpdbPrinterListMaintainer
{
private:
    static CpdbPrinterListMaintainer *instance;
    static CpdbPrinterList printerList;
    CpdbPrinterListMaintainer();

public:
    static CpdbPrinterListMaintainer *getInstance();
    static int addPrinter(PrinterObj *p);
    static int removePrinter(PrinterObj *p);
};

class CommonPrintDialogBackend
{
public:
    CommonPrintDialogBackend(char* );
    ~CommonPrintDialogBackend();

private:
    FrontendObj *frontendObj;
    PrinterObj *printerObj;
    char* id;

    CommonPrintDialogBackend();
};

#endif // QCPDB_P_H
