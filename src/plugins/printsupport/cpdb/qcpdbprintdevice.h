#ifndef QCPDBPRINTDEVICE_H
#define QCPDBPRINTDEVICE_H

#include <qpa/qplatformprintdevice.h>

QT_BEGIN_NAMESPACE

class QCpdbPrintDevice : public QPlatformPrintDevice
{
public:
    explicit QCpdbPrintDevice(const QString &id);

    bool isValid() const override;
};

QT_END_NAMESPACE

#endif // QCPDBPRINTDEVICE_H
