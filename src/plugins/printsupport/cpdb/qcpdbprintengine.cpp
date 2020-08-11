#include "qcpdbprintengine_p.h"

#include <memory>

#include <qpa/qplatformprintplugin.h>
#include <qpa/qplatformprintersupport.h>

#include <qiodevice.h>
#include <qfile.h>
#include <qdebug.h>

#include "private/qcpdb_p.h"

QT_BEGIN_NAMESPACE

QCpdbPrintEngine::QCpdbPrintEngine(QPrinter::PrinterMode m, const QString &deviceId)
    : QPdfPrintEngine(*new QCpdbPrintEnginePrivate(m))
{
    Q_UNUSED(deviceId)
}

QCpdbPrintEnginePrivate::QCpdbPrintEnginePrivate(QPrinter::PrinterMode m)
    : QPdfPrintEnginePrivate(m)
{
}

void QCpdbPrintEngine::setProperty(PrintEnginePropertyKey key, const QVariant &value)
{
    Q_D(QCpdbPrintEngine);

    switch (int(key)) {
    case PPK_CommonPrintDialogBackend:
        d->m_backend = value.value<std::shared_ptr<CommonPrintDialogBackend>>();
        break;
    default:
        QPdfPrintEngine::setProperty(key, value);
        break;
    }
}

bool QCpdbPrintEnginePrivate::openPrintDevice()
{
    if (outDevice)
        return false;

    if(!cpdbTempFile.open())
        return false;

    outDevice = new QFile(cpdbTempFile.fileName());
    static_cast<QFile *>(outDevice)->open(QIODevice::WriteOnly);

    return true;
}

void QCpdbPrintEnginePrivate::closePrintDevice()
{
    QPdfPrintEnginePrivate::closePrintDevice();
    m_backend->printFile(cpdbTempFile.fileName());
    qDebug("File Printed.");
}

QT_END_NAMESPACE
