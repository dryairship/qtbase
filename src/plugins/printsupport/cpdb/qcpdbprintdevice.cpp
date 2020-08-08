#include "qcpdbprintdevice.h"

#include "qcpdbprintersupport_p.h"

QT_BEGIN_NAMESPACE

QCpdbPrintDevice::QCpdbPrintDevice(const QString &id)
    : QPlatformPrintDevice(id)
{
}

bool QCpdbPrintDevice::isValid() const
{
    return true;
}

QT_END_NAMESPACE
