#include "qcpdb_p.h"
#include <QtCore/qdebug.h>

// To prevent compilation errors from glib-2.0/gio/gio.h
#undef signals

extern "C" {
    #include <cpdb-libs-frontend.h> 
}

CpdbPrinter::CpdbPrinter(QString id, QString backend, QString name, QString location, QString state)
    : id(id), backend(backend), name(name), location(location), state(state)
{
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*

    CpdbPrinterListMaintainer

    A singleton class to maintain a list of printers.

*/

// Define the static instances
CpdbPrinterListMaintainer* CpdbPrinterListMaintainer::m_instance = nullptr;
CpdbPrinterList CpdbPrinterListMaintainer::printerList = *(new CpdbPrinterList);

CpdbPrinterListMaintainer::CpdbPrinterListMaintainer()
{
    m_instance = nullptr;
    printerList = *(new CpdbPrinterList);
}

CpdbPrinterListMaintainer* CpdbPrinterListMaintainer::getInstance()
{
    // Create a new instanse if it does not exist
    if (!m_instance)
        m_instance = new CpdbPrinterListMaintainer;

    return m_instance;
}

int CpdbPrinterListMaintainer::addPrinter(PrinterObj *p)
{
    qDebug("Adding printer: name=%s, id=%s, backend=%s", p->name, p->id, p->backend_name);

    // Add a new printer to the printerList map
    printerList[QString::fromUtf8(p->id)] = new CpdbPrinter(
        QString::fromUtf8(p->id),
        QString::fromUtf8(p->backend_name),
        QString::fromUtf8(p->name),
        QString::fromUtf8(p->location),
        QString::fromUtf8(p->state)
    );

    emit (getInstance()->printerListChanged());

    return 0;
}

int CpdbPrinterListMaintainer::removePrinter(PrinterObj *p)
{
    qDebug("Removing printer: name=%s, id=%s, backend=%s", p->name, p->id, p->backend_name);

    // Delete the removed printer from the printerList map
    printerList.remove(QString::fromUtf8(p->id));

    emit (getInstance()->printerListChanged());

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*

    CpdbUtils

    A class that contains some commonly used functions.

*/


QStringList CpdbUtils::convertOptionToQStringList(Option* option)
{
    QStringList qsl;

    // Return an empty list if the option is a null pointer
    if(option == nullptr)
        return qsl;

    for(int i = 0; i < option->num_supported; i++) {
        // This check prevents the default value from being inserted twice into the list.
        if(strcmp(option->supported_values[i], option->default_value) != 0)
            qsl.append(QString::fromUtf8(option->supported_values[i]));
    }

    // Only add default_value if it is set.
    // The default value (if set) becomes the first item in the list.
    // This check prevents empty or "NA" strings from going into the combo boxes.
    if(strcmp(option->default_value, "") != 0 && strcmp(option->default_value, "NA") != 0 )
        qsl.prepend(QString::fromUtf8(option->default_value));

    return qsl;
}

QMap<QString, QStringList> CpdbUtils::convertOptionsToQMap(Options* options)
{
    QMap<QString, QStringList> qmap;

    GHashTableIter it;
    g_hash_table_iter_init(&it, options->table);
    gpointer _key, _value;

    while (g_hash_table_iter_next(&it, &_key, &_value)) {
        QString key = QString::fromUtf8(static_cast<char *>(_key));
        QStringList value = CpdbUtils::convertOptionToQStringList(static_cast<Option *>(_value));
        qmap[key] = value;
    }
    return qmap;
}

QString CpdbUtils::convertPWGToReadablePaperSize(QString paperSize)
{
    return QString::fromUtf8(pwg_to_readable(paperSize.toLocal8Bit().constData()));
}

QStringList CpdbUtils::convertPaperSizesToReadable(QStringList paperSizes)
{
    QStringList readableList;
    for(QString paperSize : paperSizes)
        readableList << convertPWGToReadablePaperSize(paperSize);
    return readableList;
}

QString CpdbUtils::convertReadablePaperSizeToPWG(QString paperSize)
{
    return QString::fromUtf8(readable_to_pwg(paperSize.toLocal8Bit().constData()));
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*

    CommonPrintDialogBackend

    The main backend class.

*/

CommonPrintDialogBackend::CommonPrintDialogBackend(char* id) : m_id {id}
{
    event_callback addCallbackFn = reinterpret_cast<event_callback>(CpdbPrinterListMaintainer::addPrinter);
    event_callback removeCallbackFn = reinterpret_cast<event_callback>(CpdbPrinterListMaintainer::removePrinter);

    m_frontendObj = get_new_FrontendObj(m_id, addCallbackFn, removeCallbackFn);
    connect_to_dbus(m_frontendObj);
}

CommonPrintDialogBackend::CommonPrintDialogBackend()
{
}

CommonPrintDialogBackend::~CommonPrintDialogBackend()
{
    disconnect_from_dbus(m_frontendObj);
}

CpdbPrinterList CommonPrintDialogBackend::getAvailablePrinters()
{
    return CpdbPrinterListMaintainer::printerList;
}

void CommonPrintDialogBackend::setCurrentPrinter(QString printerId, QString backend)
{
    qDebug("printerId: %s, backend: %s", printerId.toLocal8Bit().data(), backend.toLocal8Bit().data());
    m_printerObj = find_PrinterObj(m_frontendObj, printerId.toLocal8Bit().data(), backend.toLocal8Bit().data());
}

QMap<QString, QStringList> CommonPrintDialogBackend::getOptionsForCurrentPrinter()
{
    return CpdbUtils::convertOptionsToQMap(get_all_options(m_printerObj));
}

void CommonPrintDialogBackend::setRemotePrintersVisible(bool visible)
{
    if(visible)
        unhide_remote_cups_printers(m_frontendObj);
    else
        hide_remote_cups_printers(m_frontendObj);
}

void CommonPrintDialogBackend::setCollateEnabled(bool enabled)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("multiple-document-handling"),
        enabled ? const_cast<char*>("separate-documents-collated-copies")
                : const_cast<char*>("separate-documents-uncollated-copies")
    );
}

void CommonPrintDialogBackend::setReversePageOrder(bool reverse)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("page-delivery"),
        reverse ? const_cast<char*>("reverse-order")
                : const_cast<char*>("same-order")
    );
}

void CommonPrintDialogBackend::setSelectableOption(QString optionName, QString optionValue)
{
    add_setting_to_printer(
        m_printerObj,
        optionName.toLatin1().data(),
        optionValue.toLatin1().data()
    );
}

void CommonPrintDialogBackend::setPageRange(QVariant range)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("page-ranges"),
        range.isNull() ? NULL
                       : range.value<QString>().toLatin1().data()
    );
}

void CommonPrintDialogBackend::setNumCopies(QVariant copies)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("copies"),
        copies.value<QString>().toLatin1().data()
    );
}

void CommonPrintDialogBackend::printFile(QString filePath)
{
    qDebug("Printing file: %s", filePath.toLatin1().data());
    print_file(m_printerObj, filePath.toLatin1().data());
}
