#ifndef QCPDB_P_H
#define QCPDB_P_H

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

class CpdbPrinterListMaintainer : public QObject
{
    Q_OBJECT

private:
    static CpdbPrinterListMaintainer *instance;
    CpdbPrinterListMaintainer();

public:
    static CpdbPrinterList printerList;

    static CpdbPrinterListMaintainer *getInstance();

public slots:
    static int addPrinter(PrinterObj *p);
    static int removePrinter(PrinterObj *p);

signals:
    void printerListChanged();
};

class CommonPrintDialogBackend
{
public:
    CommonPrintDialogBackend();
    CommonPrintDialogBackend(char*);
    ~CommonPrintDialogBackend();
    QStringList getAvailablePrinters();

private:
    FrontendObj *frontendObj;
    PrinterObj *printerObj;
    char* id;
};

#endif // QCPDB_P_H
