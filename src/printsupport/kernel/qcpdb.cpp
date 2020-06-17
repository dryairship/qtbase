#include "qcpdb_p.h"

// To prevent compilation errors from glib-2.0/gio/gio.h
#undef signals

extern "C" {
    #include <cpdb-libs-frontend.h> 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*

    CpdbPrinterListMaintainer

    A singleton class to maintain a list of printers.

*/

// Define the static instances
CpdbPrinterListMaintainer* CpdbPrinterListMaintainer::instance = nullptr;
CpdbPrinterList CpdbPrinterListMaintainer::printerList = *(new CpdbPrinterList);

CpdbPrinterListMaintainer::CpdbPrinterListMaintainer()
{
    instance = nullptr;
    printerList = *(new CpdbPrinterList);
}

CpdbPrinterListMaintainer* CpdbPrinterListMaintainer::getInstance()
{
    if (!instance) instance = new CpdbPrinterListMaintainer;
    return instance;
}

int CpdbPrinterListMaintainer::addPrinter(PrinterObj *p)
{
    qDebug("Adding printer: name=%s, id=%s, backend=%s", p->name, p->id, p->backend_name);

    printerList[qMakePair(p->backend_name, p->id)] = p->name;
    emit (getInstance()->printerListChanged());

    return 0;
}

int CpdbPrinterListMaintainer::removePrinter(PrinterObj *p)
{
    qDebug("Removing printer: name=%s, id=%s, backend=%s", p->name, p->id, p->backend_name);

    auto pair = qMakePair(p->backend_name, p->id);
    for (auto it = printerList.begin(); it != printerList.end(); it++) {
        if(it.key() == pair) {
            printerList.erase(it);
            break;
        }
    }
    emit (getInstance()->printerListChanged());

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*

    CommonPrintDialogBackend

    The main backend class.

*/

CommonPrintDialogBackend::CommonPrintDialogBackend(char* id) : id {id}
{
    event_callback addCallbackFn = reinterpret_cast<event_callback>(CpdbPrinterListMaintainer::addPrinter);
    event_callback removeCallbackFn = reinterpret_cast<event_callback>(CpdbPrinterListMaintainer::removePrinter);

    frontendObj = get_new_FrontendObj(id, addCallbackFn, removeCallbackFn);
    connect_to_dbus(frontendObj);
}

CommonPrintDialogBackend::CommonPrintDialogBackend()
{
}

CommonPrintDialogBackend::~CommonPrintDialogBackend()
{
    disconnect_from_dbus(frontendObj);
}

QStringList CommonPrintDialogBackend::getAvailablePrinters()
{
    QStringList printers;
    for(auto it = CpdbPrinterListMaintainer::printerList.begin();
        it != CpdbPrinterListMaintainer::printerList.end(); it++)
    {
        printers << QString::fromUtf8(it.value());
    }
    return printers;
}
