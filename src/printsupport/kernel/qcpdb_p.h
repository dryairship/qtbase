#ifndef QCPDB_P_H
#define QCPDB_P_H

#include <QtGui/qevent.h>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QStringList>


// Check if cpdb-libs is installed on the system
#if __has_include(<cpdb-libs-frontend.h>)
  #define QCPDB_USING_CPDB 1
#else
  #define QCPDB_USING_CPDB 0
#endif

// Forward declaration of classes in cpdb-libs-frontend.h
struct _FrontendObj;
using FrontendObj = _FrontendObj;
struct _PrinterObj;
using PrinterObj = _PrinterObj;
struct _Option;
using Option = _Option;
class _Options;
using Options = _Options;

// A class to store details of a printer
class CpdbPrinter : public QObject
{
    Q_OBJECT
public:
    QString id;
    QString backend;
    QString name;
    QString location;
    QString state;

    CpdbPrinter(QString id, QString backend, QString name, QString location, QString state);
};

// CpdbPrinterList is a map from the id of a printer to the
// CpdbPrinter (which contains more details about the printer).
typedef QMap<QString, CpdbPrinter*> CpdbPrinterList;

// This is a singleton class that maintains the list of printers
// discovered by the backend. This class is needed because the
// callback functions required by the cpd backend need to be static.
// Whenever a new printer is added or removed, the printerListChanged()
// signal is emitted.
class CpdbPrinterListMaintainer : public QObject
{
    Q_OBJECT

private:
    static CpdbPrinterListMaintainer *m_instance;
    CpdbPrinterListMaintainer();

public:
    static CpdbPrinterList printerList;

    static CpdbPrinterListMaintainer *getInstance();

public Q_SLOTS:
    static int addPrinter(PrinterObj *p);
    static int removePrinter(PrinterObj *p);

Q_SIGNALS:
    void printerListChanged();
};

// A utility class to provide functions for commonly used conversions.
class CpdbUtils {
public:
    static QStringList convertOptionToQStringList(Option* option);
    static QMap<QString, QStringList> convertOptionsToQMap(Options* options);
    static QString convertPWGToReadablePaperSize(QString paperSize);
    static QString convertReadablePaperSizeToPWG(QString paperSize);
    static QStringList convertPaperSizesToReadable(QStringList paperSizes);
};

// This class communicates with the cpdb backends, fetches the options,
// and issues the print command.
class CommonPrintDialogBackend
{
public:
    CommonPrintDialogBackend();
    CommonPrintDialogBackend(char* id);
    ~CommonPrintDialogBackend();
    CpdbPrinterList getAvailablePrinters();
    void setCurrentPrinter(QString printerId, QString backendName);
    QMap<QString, QStringList> getOptionsForCurrentPrinter();
    void setRemotePrintersVisible(bool visible);
    void setPageRange(QVariant range);
    void setCollateEnabled(bool enabled);
    void setReversePageOrder(bool reverse);
    void setSelectableOption(QString optionName, QString optionValue);

private:
    FrontendObj *m_frontendObj;
    PrinterObj *m_printerObj;
    char* m_id;
};

#endif // QCPDB_P_H
