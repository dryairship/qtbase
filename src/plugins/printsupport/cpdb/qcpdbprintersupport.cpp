#include "qcpdbprintersupport_p.h"

#include "qcpdbprintengine_p.h"
#include "qcpdbprintdevice.h"
#include <private/qprinterinfo_p.h>
#include <private/qprintdevice_p.h>

#include <QtPrintSupport/QPrinterInfo>

QT_BEGIN_NAMESPACE

QPrintEngine *QCpdbPrinterSupport::createNativePrintEngine(QPrinter::PrinterMode printerMode, const QString &deviceId)
{
    qDebug("Creating new printengine with device id: %s", deviceId.toLatin1().data());
    return new QCpdbPrintEngine(printerMode, (deviceId.isEmpty() ? defaultPrintDeviceId() : deviceId));
}

QPaintEngine *QCpdbPrinterSupport::createPaintEngine(QPrintEngine *engine, QPrinter::PrinterMode printerMode)
{
    Q_UNUSED(printerMode)
    qDebug("Casting engine to cpdb engine");
    return static_cast<QCpdbPrintEngine *>(engine);
}

QPrintDevice QCpdbPrinterSupport::createPrintDevice(const QString &id)
{
    qDebug("Creating Cpdb print device with id: %s", id.toLatin1().data());
    return QPlatformPrinterSupport::createPrintDevice(new QCpdbPrintDevice(id));
}

QStringList QCpdbPrinterSupport::availablePrintDeviceIds() const
{
    qDebug("Returning available printers list");
    static const QStringList deviceIds = {"CommonPrintDialogBackend"};
    return deviceIds;
}

QString QCpdbPrinterSupport::defaultPrintDeviceId() const
{
    return staticDefaultPrintDeviceId();
}

QString QCpdbPrinterSupport::staticDefaultPrintDeviceId()
{
    static const QString defaultPrintDeviceId = QString::fromUtf8("CommonPrintDialogBackend");
    return defaultPrintDeviceId;
}

QT_END_NAMESPACE
