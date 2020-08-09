#ifndef QCPDBPRINTENGINE_P_H
#define QCPDBPRINTENGINE_P_H

#include <memory>

#include "QtPrintSupport/qprintengine.h"

#include <QtCore/qstring.h>
#include <QtGui/qpaintengine.h>

#include <private/qpaintengine_p.h>
#include <private/qprintdevice_p.h>
#include <private/qprintengine_pdf_p.h>

#include "private/qcpdb_p.h"

QT_BEGIN_NAMESPACE

class QCpdbPrintEnginePrivate;

class QCpdbPrintEngine : public QPdfPrintEngine
{
    Q_DECLARE_PRIVATE(QCpdbPrintEngine)
public:
    QCpdbPrintEngine(QPrinter::PrinterMode m, const QString &deviceId);

    void setProperty(PrintEnginePropertyKey key, const QVariant &value) override;
};

class QCpdbPrintEnginePrivate : public QPdfPrintEnginePrivate
{
    Q_DECLARE_PUBLIC(QCpdbPrintEngine)

    std::shared_ptr<CommonPrintDialogBackend> m_backend;
public:
    QCpdbPrintEnginePrivate(QPrinter::PrinterMode m);

    bool openPrintDevice() override;
    void closePrintDevice() override;
};

QT_END_NAMESPACE

#endif // QCPDBPRINTENGINE_P_H
