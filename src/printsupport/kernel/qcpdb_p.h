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

struct _FrontendObj;
using FrontendObj = _FrontendObj;

struct _PrinterObj;
using PrinterObj = _PrinterObj;

struct _Option;
using Option = _Option;

class _Options;
using Options = _Options;

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

typedef QMap<QString, CpdbPrinter*> CpdbPrinterList;

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

class CpdbUtils {
public:
    static QStringList convertOptionToQStringList(Option* option);
    static QMap<QString, QStringList> convertOptionsToQMap(Options* options);
    static QString convertPWGToReadablePaperSize(QString paperSize);
    static QString convertReadablePaperSizeToPWG(QString paperSize);
    static QStringList convertPaperSizesToReadable(QStringList paperSizes);
};

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
    void setCollateEnabled(bool enabled);
    void setReversePageOrder(bool reverse);
    void setPaperSize(QString paperSize);
    void setOrientation(QString orientation);
    void setColorMode(QString colorMode);
    void setPrintBothSidesOption(QString colorMode);
    void setPagesPerSide(QString pagesPerSide);
    void setScale(QString scale);
    void setResolution(QString resolution);
    void setQuality(QString quality);
    void setOutputBin(QString outputBin);
    void setFinishings(QString finishings);
    void setExtraOption(QString optionName, QString optionValue);

private:
    FrontendObj *m_frontendObj;
    PrinterObj *m_printerObj;
    char* m_id;
};

#endif // QCPDB_P_H