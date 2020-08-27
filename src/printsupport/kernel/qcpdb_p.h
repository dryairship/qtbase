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

#ifndef QCPDB_P_H
#define QCPDB_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtGui/qevent.h>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QStringList>
#include <memory>

#define PPK_CommonPrintDialogBackend QPrintEngine::PrintEnginePropertyKey(0xfd00)

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
    void setNumCopies(QVariant copies);
    void setPageRange(QVariant range);
    void setCollateEnabled(bool enabled);
    void setReversePageOrder(bool reverse);
    void setSelectableOption(QString optionName, QString optionValue);
    void printFile(QString filePath);

private:
    FrontendObj *m_frontendObj;
    PrinterObj *m_printerObj;
    char* m_id;
};

Q_DECLARE_METATYPE(std::shared_ptr<CommonPrintDialogBackend>)

#endif // QCPDB_P_H
