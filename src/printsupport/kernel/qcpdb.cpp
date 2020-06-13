#include "qcpdb_p.h"

// To prevent compilation errors from glib-2.0/gio/gio.h
#undef signals

extern "C" {
    #include <cpdb-libs-frontend.h> 
}

int CpdbCallbackFunctions::addPrinterCallback(PrinterObj *p)
{
    std::cout << "Adding printer : ";
    std::cout << p->name << ", ";
    std::cout << p->id << ", ";
    std::cout << p->backend_name << "\n";

    return 0;
}

int CpdbCallbackFunctions::removePrinterCallback(PrinterObj *p)
{
    std::cout << "Removing printer : ";
    std::cout << p->name << ", ";
    std::cout << p->id << ", ";
    std::cout << p->backend_name << "\n";

    return 0;
}

CommonPrintDialogBackend::CommonPrintDialogBackend(char* id) : id {id}
{
    event_callback addCallbackFn = reinterpret_cast<event_callback>(CpdbCallbackFunctions::addPrinterCallback);
    event_callback removeCallbackFn = reinterpret_cast<event_callback>(CpdbCallbackFunctions::removePrinterCallback);

    frontendObj = get_new_FrontendObj(id, addCallbackFn, removeCallbackFn);
    connect_to_dbus(frontendObj);
}

CommonPrintDialogBackend::~CommonPrintDialogBackend()
{
    disconnect_from_dbus(frontendObj);
}