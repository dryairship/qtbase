/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcpdb_p.h"

// To prevent compilation errors from glib-2.0/gio/gio.h
#undef signals

extern "C" {
    #include <cpdb-libs-frontend.h> 
}

/*!
    Creates a new CpdbPrinter object with the given details.

    \a id is the ID of the printer.

    \a backend is the backend (CUPS, GCP, FILE, etc.)

    \a name is the name of the printer.

    \a location is the location of the printer.

    \a state is the state of the printer.
*/
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

/*!
    Returns the instance of this singleton class. Creates a new instance
    if no instance has been created before.
*/
CpdbPrinterListMaintainer* CpdbPrinterListMaintainer::getInstance()
{
    // Create a new instanse if it does not exist
    if (!m_instance)
        m_instance = new CpdbPrinterListMaintainer;

    return m_instance;
}

/*!
    This function is called as the callback whenever the backend
    detects a new printer.
*/
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

/*!
    This function is called as the callback whenever the backend
    detects that an existing printer was removed.
*/
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

/*!
    Converts \a option from Option* type to a QStringList.
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

/*!
    Converts \a options from Options* type to a QMap which has
    option names as keys and the supported values for the options
    as the value corresponding to that key.
*/
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

/*!
    Converts \a paperSize from PWG codename to human readable name.
*/
QString CpdbUtils::convertPWGToReadablePaperSize(QString paperSize)
{
    return QString::fromUtf8(pwg_to_readable(paperSize.toLocal8Bit().constData()));
}

/*!
    Converts \a paperSizes from PWG codenames to human redable names.
*/
QStringList CpdbUtils::convertPaperSizesToReadable(QStringList paperSizes)
{
    QStringList readableList;
    for(QString paperSize : paperSizes)
        readableList << convertPWGToReadablePaperSize(paperSize);
    return readableList;
}

/*!
    Converts \a paperSize from human redable name to PWG codename.
*/
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

/*!
    Creates a new backend with the given \a id
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
    // Disconnect the frontend from the backend upon destruction.
    disconnect_from_dbus(m_frontendObj);
}

/*!
    Returns available printers detected by the backend.
    The printers may or may not be accepting jobs.
*/
CpdbPrinterList CommonPrintDialogBackend::getAvailablePrinters()
{
    return CpdbPrinterListMaintainer::printerList;
}

/*!
    Sets the printer with the given \a printerId  and \a backend as
    the currently selected printer. Doing this will enable the retrieval
    of the options supported by this printer from the backend.
*/
void CommonPrintDialogBackend::setCurrentPrinter(QString printerId, QString backend)
{
    qDebug("printerId: %s, backend: %s", printerId.toLocal8Bit().data(), backend.toLocal8Bit().data());
    m_printerObj = find_PrinterObj(m_frontendObj, printerId.toLocal8Bit().data(), backend.toLocal8Bit().data());
}

/*!
    Returns the options supported by the currently selected printer.
*/
QMap<QString, QStringList> CommonPrintDialogBackend::getOptionsForCurrentPrinter()
{
    return CpdbUtils::convertOptionsToQMap(get_all_options(m_printerObj));
}

/*!
    Sets the visibility of remote printers to \a visible
*/
void CommonPrintDialogBackend::setRemotePrintersVisible(bool visible)
{
    if(visible)
        unhide_remote_cups_printers(m_frontendObj);
    else
        hide_remote_cups_printers(m_frontendObj);
}

/*!
    Disables or enables collation of copies, depending on the value of
    \a enabled
*/
void CommonPrintDialogBackend::setCollateEnabled(bool enabled)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("multiple-document-handling"),
        enabled ? const_cast<char*>("separate-documents-collated-copies")
                : const_cast<char*>("separate-documents-uncollated-copies")
    );
}

/*!
    Sets if copies should be printed in reverse order, depending on the
    value of \a reverse
*/
void CommonPrintDialogBackend::setReversePageOrder(bool reverse)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("page-delivery"),
        reverse ? const_cast<char*>("reverse-order")
                : const_cast<char*>("same-order")
    );
}

/*!
    Sets the value of the option \a optionName to the value \a optionValue
*/
void CommonPrintDialogBackend::setSelectableOption(QString optionName, QString optionValue)
{
    add_setting_to_printer(
        m_printerObj,
        optionName.toLatin1().data(),
        optionValue.toLatin1().data()
    );
}

/*!
    Sets the page-range equal to \a range
*/
void CommonPrintDialogBackend::setPageRange(QVariant range)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("page-ranges"),
        range.isNull() ? NULL
                       : range.value<QString>().toLatin1().data()
    );
}

/*!
    Sets the number of copies equal to \a copies
*/
void CommonPrintDialogBackend::setNumCopies(QVariant copies)
{
    add_setting_to_printer(
        m_printerObj,
        const_cast<char*>("copies"),
        copies.value<QString>().toLatin1().data()
    );
}

/*!
    Prints the file at \a filePath

    Normally, this \a filePath is the path of a QTemporaryFile.
*/
void CommonPrintDialogBackend::printFile(QString filePath)
{
    qDebug("Printing file: %s", filePath.toLatin1().data());
    print_file(m_printerObj, filePath.toLatin1().data());
}
