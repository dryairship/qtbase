#include "qcpdbprintersupport_p.h"

#include <qpa/qplatformprintplugin.h>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QCpdbPrinterSupportPlugin : public QPlatformPrinterSupportPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformPrinterSupportFactoryInterface_iid FILE "cpdb.json")

public:
    QStringList keys() const;
    QPlatformPrinterSupport *create(const QString &) override;
};

QStringList QCpdbPrinterSupportPlugin::keys() const
{
    return QStringList(QStringLiteral("cpdbprintersupport"));
}

QPlatformPrinterSupport *QCpdbPrinterSupportPlugin::create(const QString &key)
{
    if (key.compare(key, QLatin1String("cpdbprintersupport"), Qt::CaseInsensitive) == 0)
        return new QCpdbPrinterSupport;
    return 0;
}

QT_END_NAMESPACE

#include "main.moc"
