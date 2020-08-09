
#ifndef QCPDBPRINTERSUPPORT_H
#define QCPDBPRINTERSUPPORT_H

#include <qpa/qplatformprintersupport.h>

#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

class QCpdbPrinterSupport : public QPlatformPrinterSupport
{
public:
    QPrintEngine *createNativePrintEngine(QPrinter::PrinterMode printerMode, const QString &deviceId = QString()) override;
    QPaintEngine *createPaintEngine(QPrintEngine *printEngine, QPrinter::PrinterMode) override;

    QPrintDevice createPrintDevice(const QString &id) override;
    QStringList availablePrintDeviceIds() const override;
    QString defaultPrintDeviceId() const override;

    static QString staticDefaultPrintDeviceId();
};

QT_END_NAMESPACE

#endif // QCPDBPRINTERSUPPORT_H
