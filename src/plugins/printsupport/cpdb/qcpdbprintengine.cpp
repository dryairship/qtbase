
#include "qcpdbprintengine_p.h"

#include <qpa/qplatformprintplugin.h>
#include <qpa/qplatformprintersupport.h>

#include <qiodevice.h>
#include <qfile.h>
#include <qdebug.h>

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


bool QCpdbPrintEnginePrivate::openPrintDevice()
{
    if (outDevice)
        return false;

    QString cpdbTempFile = QString::fromLocal8Bit("/tmp/qtcpdb.pdf");
    outDevice = new QFile(cpdbTempFile);
    static_cast<QFile *>(outDevice)->open(QIODevice::WriteOnly);

    return true;
}

void QCpdbPrintEnginePrivate::closePrintDevice()
{
    QPdfPrintEnginePrivate::closePrintDevice();
    qDebug("Printer closing. PDF would have been written. TODO: Send it to backend.");
}

QT_END_NAMESPACE
