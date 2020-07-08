#include <QtWidgets/QtWidgets>
#include <QUuid>
#include <QStringList>

#include <private/qcpdb_p.h>
#include "qcommonprintdialog.h"

QCommonPrintDialog::QCommonPrintDialog(QWidget *parent) :
    QDialog (parent)
{
    char* id = QUuid::createUuid().toString().remove('{').remove('}').toLatin1().data();
    backend = new CommonPrintDialogBackend(id);

    resize(360, 480);
    mainLayout = new CommonPrintDialogMainLayout(backend, parent);
    mainLayout->connectSignalsAndSlots();
    setLayout(mainLayout);
}

QCommonPrintDialog::~QCommonPrintDialog() {
}

CommonPrintDialogMainLayout::CommonPrintDialogMainLayout(
    CommonPrintDialogBackend *backend, QWidget* parent)
    : backend(backend)
{
    tabWidget = new QTabWidget;

    generalTab = new CommonPrintDialogGeneralTab(backend, parent);
    pageSetupTab = new CommonPrintDialogPageSetupTab(backend, parent);
    optionsTab = new CommonPrintDialogOptionsTab(backend, parent);
    jobsTab = new CommonPrintDialogJobsTab(backend, parent);

    tabWidget->addTab(generalTab, tr("General"));
    tabWidget->addTab(pageSetupTab, tr("Page Setup"));
    tabWidget->addTab(optionsTab, tr("Options"));
    tabWidget->addTab(jobsTab, tr("Jobs"));

    printButton = new QPushButton(tr("Print"));
    printButton->setDefault(true);
    cancelButton = new QPushButton(tr("Cancel"));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(printButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(tabWidget);
    controlsLayout->addItem(buttonLayout);

    addItem(controlsLayout);
}

void CommonPrintDialogMainLayout::connectSignalsAndSlots()
{
    QObject::connect(
        CpdbPrinterListMaintainer::getInstance(), SIGNAL(printerListChanged()),
        generalTab, SLOT(printerListChanged())
    );

    QObject::connect(
        generalTab->destinationComboBox, SIGNAL(currentIndexChanged(int)),
        generalTab, SLOT(newPrinterSelected(int))
    );
}

CommonPrintDialogGeneralTab::CommonPrintDialogGeneralTab(
    CommonPrintDialogBackend *backend, QWidget *parent)
    : QWidget(parent), backend(backend)
{
    destinationComboBox = new QComboBox;
    remotePrintersCheckBox = new QCheckBox;
    paperComboBox = new QComboBox;
    pagesComboBox = new QComboBox;
    copiesSpinBox = new QSpinBox;
    collateCheckBox = new QCheckBox;
    orientationComboBox = new QComboBox;
    colorModeComboBox = new QComboBox;

    pagesComboBox->addItem(tr("All"));

    QFormLayout *layout = new QFormLayout;

    layout->addRow(new QLabel(tr("Destination")), destinationComboBox);
    layout->addRow(new QLabel(tr("Remote Printers")), remotePrintersCheckBox);
    layout->addRow(new QLabel(tr("Paper")), paperComboBox);
    layout->addRow(new QLabel(tr("Pages")), pagesComboBox);
    layout->addRow(new QLabel(tr("Copies")), copiesSpinBox);
    layout->addRow(new QLabel(tr("Collate Pages")), collateCheckBox);
    layout->addRow(new QLabel(tr("Orientation")), orientationComboBox);
    layout->addRow(new QLabel(tr("Color Mode")), colorModeComboBox);

    copiesSpinBox->setValue(1);

    setLayout(layout);
}

void CommonPrintDialogGeneralTab::printerListChanged()
{
    qDebug("qCPD: Updating Printers list");

    const QStringList printers = backend->getAvailablePrinters();
    destinationComboBox->clear();
    destinationComboBox->addItems(printers);
}

void CommonPrintDialogGeneralTab::newPrinterSelected(int i)
{
    QString selectedPrinterName = destinationComboBox->currentText();
    if(!CpdbPrinterListMaintainer::printerList.contains(selectedPrinterName))
        return;

    QPair<QString, QString> selectedPrinter = CpdbPrinterListMaintainer::printerList[selectedPrinterName];
    qDebug("qCPD: New Printer Selected: %d, %s", i, selectedPrinterName.toLocal8Bit().data());
    QMap<QString, QStringList> options = backend->getOptionsForPrinter(selectedPrinter.first, selectedPrinter.second);
    for (auto it = options.begin(); it != options.end(); it++) {
        qDebug("Option %s: [%s]", it.key().toLocal8Bit().data(), it.value().join(", ").toLocal8Bit().data());
    }
    populatePaperSizeComboBox(options[QString::fromUtf8("media")]);
    populateComboBox(orientationComboBox, options[QString::fromUtf8("orientation-requested")]);
    populateComboBox(colorModeComboBox, options[QString::fromUtf8("print-color-mode")]);
}

void CommonPrintDialogGeneralTab::populatePaperSizeComboBox(QStringList sizes) {
    paperComboBox->clear();
    for(auto pwgSize : sizes){
        paperComboBox->addItem(CpdbUtils::convertPWGToReadablePaperSize(pwgSize));
    }
    paperComboBox->setEnabled(paperComboBox->count() != 0);
}

void CommonPrintDialogGeneralTab::populateComboBox(QComboBox *comboBox, QStringList values)
{
    comboBox->clear();
    comboBox->addItems(values);
    comboBox->setEnabled(comboBox->count() != 0);
}

CommonPrintDialogPageSetupTab::CommonPrintDialogPageSetupTab(
    CommonPrintDialogBackend *backend, QWidget *parent)
    : backend(backend)
{
    bothSidesComboBox = new QComboBox;
    pagesPerSideComboBox = new QComboBox;
    onlyPrintComboBox = new QComboBox;
    scaleSpinBox = new QSpinBox;
    scaleSpinBox->setRange(0, 200);
    scaleSpinBox->setValue(100);
    scaleSpinBox->setSuffix(tr("%"));
    paperSourceComboBox = new QComboBox;
    pageRangeComboBox = new QComboBox;

    QFormLayout *layout = new QFormLayout;

    layout->addRow(new QLabel(tr("Layout")));
    layout->addRow(new QLabel(tr("Print Both Sides")), bothSidesComboBox);
    layout->addRow(new QLabel(tr("Pages Per Side")), pagesPerSideComboBox);
    layout->addRow(new QLabel(tr("Only Print")), onlyPrintComboBox);
    layout->addRow(new QLabel(tr("Scale")), scaleSpinBox);
    layout->addRow(new QLabel(tr("")));
    layout->addRow(new QLabel(tr("Paper")));
    layout->addRow(new QLabel(tr("Paper Source")), paperSourceComboBox);

    setLayout(layout);

    (void)parent;
}

CommonPrintDialogOptionsTab::CommonPrintDialogOptionsTab(
    CommonPrintDialogBackend *backend, QWidget *parent)
    : backend(backend)
{
    marginTopValue = new QLineEdit;
    marginBottomValue = new QLineEdit;
    marginLeftValue = new QLineEdit;
    marginRightValue = new QLineEdit;
    resolutionComboBox = new QComboBox;
    qualityComboBox = new QComboBox;
    outputBinComboBox = new QComboBox;
    finishingsComboBox = new QComboBox;
    ippAttributeFidelityComboBox = new QComboBox;

    layout = new QFormLayout;

    layout->addRow((new QLabel(tr("Margin"))));
    layout->addRow(new QLabel(tr("Top")), marginTopValue);
    layout->addRow(new QLabel(tr("Bottom")), marginBottomValue);
    layout->addRow(new QLabel(tr("Left")), marginLeftValue);
    layout->addRow(new QLabel(tr("Right")), marginRightValue);
    layout->addRow(new QLabel(tr("")));
    layout->addRow(new QLabel(tr("Resolution")), resolutionComboBox);
    layout->addRow(new QLabel(tr("Quality")), qualityComboBox);
    layout->addRow(new QLabel(tr("Output Bin")), outputBinComboBox);
    layout->addRow(new QLabel(tr("Finishings")), finishingsComboBox);

    setLayout(layout);

    (void)parent;
}

CommonPrintDialogJobsTab::CommonPrintDialogJobsTab(
    CommonPrintDialogBackend *backend, QWidget *parent)
    : backend(backend)
{
    QWidget *jobsWidget = new QWidget;
    jobsLayout = new QGridLayout;

    jobsLayout->addWidget(new QLabel(tr("Printer")), 0, 0);
    jobsLayout->addWidget(new QLabel(tr("Location")), 0, 1);
    jobsLayout->addWidget(new QLabel(tr("Status")), 0, 2);

    jobsWidget->setLayout(jobsLayout);

    scrollArea = new QScrollArea;
    scrollArea->setMinimumHeight(240);
    scrollArea->setWidget(jobsWidget);
    scrollArea->setWidgetResizable(true);

    refreshButton = new QPushButton(tr("Refresh"));
    startJobComboBox = new QComboBox;
    saveJobButton = new QPushButton(tr("Save"));
    jobNameComboBox = new QComboBox;
    jobPriorityComboBox = new QComboBox;
    jobSheetsComboBox = new QComboBox;

    QFormLayout *layout = new QFormLayout;
    layout->addRow(scrollArea);
    layout->addRow(new QLabel(tr("Refresh")), refreshButton);
    layout->addRow(new QLabel(tr("Start Job")), startJobComboBox);
    layout->addRow(new QLabel(tr("Save Job")), saveJobButton);
    layout->addRow(new QLabel(tr("Job Name")), jobNameComboBox);
    layout->addRow(new QLabel(tr("Job Priority")), jobPriorityComboBox);
    layout->addRow(new QLabel(tr("Job Sheets")), jobSheetsComboBox);

    setLayout(layout);

    (void)parent;
}
