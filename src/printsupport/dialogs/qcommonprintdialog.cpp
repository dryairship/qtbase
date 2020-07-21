#include <memory>
#include <QtWidgets/QtWidgets>
#include <QUuid>
#include <QStringList>

#include <private/qcpdb_p.h>
#include "qcommonprintdialog.h"

using namespace std;

QCommonPrintDialog::QCommonPrintDialog(QWidget *parent) :
    QDialog (parent)
{
    char* id = QUuid::createUuid().toString().remove('{').remove('}').toLatin1().data();
    m_backend = make_shared<CommonPrintDialogBackend>(id);

    resize(500, 480);
    m_mainLayout = new CommonPrintDialogMainLayout(m_backend, parent);
    setLayout(m_mainLayout);
}

QCommonPrintDialog::~QCommonPrintDialog() {
}

CommonPrintDialogMainLayout::CommonPrintDialogMainLayout(
    shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent)
    : m_backend(backend)
{
    m_tabWidget = new QTabWidget;

    m_generalTab = new CommonPrintDialogGeneralTab(backend, parent);
    m_pageSetupTab = new CommonPrintDialogPageSetupTab(backend, parent);
    m_optionsTab = new CommonPrintDialogOptionsTab(backend, parent);
    m_jobsTab = new CommonPrintDialogJobsTab(backend, parent);
    m_extraOptionsTab = new CommonPrintDialogExtraOptionsTab(backend, parent);

    m_tabWidget->addTab(m_generalTab, tr("General"));
    m_tabWidget->addTab(m_pageSetupTab, tr("Page Setup"));
    m_tabWidget->addTab(m_optionsTab, tr("Options"));
    m_tabWidget->addTab(m_jobsTab, tr("Jobs"));
    m_tabWidget->addTab(m_extraOptionsTab, tr("Extra Options"));

    m_printButton = new QPushButton(tr("Print"));
    m_printButton->setDefault(true);
    m_cancelButton = new QPushButton(tr("Cancel"));
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_printButton);
    buttonLayout->addWidget(m_cancelButton);

    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(m_tabWidget);
    controlsLayout->addItem(buttonLayout);

    addItem(controlsLayout);

    connectSignalsAndSlots();
}

void CommonPrintDialogMainLayout::connectSignalsAndSlots()
{
    QObject::connect(
        CpdbPrinterListMaintainer::getInstance(), SIGNAL(printerListChanged()),
        this, SLOT(printerListChanged())
    );

    QObject::connect(
        m_generalTab->m_destinationWidget, SIGNAL(currentCellChanged(int, int, int, int)),
        this, SLOT(newPrinterSelected(int))
    );

    QObject::connect(
        m_generalTab->m_remotePrintersCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(remotePrintersCheckBoxStateChanged(int))
    );

    QObject::connect(
        m_generalTab->m_copiesSpinBox, SIGNAL(valueChanged(int)),
        this, SLOT(copiesSpinBoxValueChanged(int))
    );

    QObject::connect(
        m_generalTab->m_collateCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(collateCheckBoxStateChanged(int))
    );

    QObject::connect(
        m_generalTab->m_reverseCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(reverseCheckBoxStateChanged(int))
    );

    QObject::connect(
        m_generalTab->m_paperSizeComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(paperSizeComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_generalTab->m_orientationComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(orientationComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_generalTab->m_colorModeComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(colorModeComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_pageSetupTab->m_bothSidesComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(bothSidesComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_pageSetupTab->m_pagesPerSideComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(pagesPerSideComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_optionsTab->m_resolutionComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(resolutionComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_optionsTab->m_qualityComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(qualityComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_optionsTab->m_outputBinComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(outputBinComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_optionsTab->m_finishingsComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(finishingsComboBoxValueChanged(QString))
    );
}

void CommonPrintDialogMainLayout::printerListChanged()
{
    qDebug("qCPD: Updating Printers list");

    CpdbPrinterList printers = m_backend->getAvailablePrinters();
    m_generalTab->m_destinationWidget->setRowCount(printers.size());
    int count = 0;
    for(auto printer : printers) {
        m_generalTab->m_destinationWidget->setItem(count, 0, new QTableWidgetItem(printer->name));
        m_generalTab->m_destinationWidget->setItem(count, 1, new QTableWidgetItem(printer->location));
        m_generalTab->m_destinationWidget->setItem(count, 2, new QTableWidgetItem(printer->state));
        m_generalTab->m_destinationWidget->setItem(count, 3, new QTableWidgetItem(printer->id));
        m_generalTab->m_destinationWidget->setItem(count, 4, new QTableWidgetItem(printer->backend));
        count ++;
    }
    m_generalTab->m_destinationWidget->setCurrentCell(0, 0);
    newPrinterSelected(0);
}

void CommonPrintDialogMainLayout::newPrinterSelected(int row)
{
    QString id = m_generalTab->m_destinationWidget->item(row, 3)->text();
    QString backend = m_generalTab->m_destinationWidget->item(row, 4)->text();
    qDebug("qCPD: New Printer Selected: %s, %s", id.toLocal8Bit().data(), backend.toLocal8Bit().data());
    m_backend->setCurrentPrinter(id, backend);
    QMap<QString, QStringList> options = m_backend->getOptionsForCurrentPrinter();
    for (auto it = options.begin(); it != options.end(); it++) {
        qDebug("Option %s: [%s]", it.key().toLocal8Bit().data(), it.value().join(tr(", ")).toLocal8Bit().data());
    }

    QSet<QString> usedKeys;
    usedKeys.insert(tr("copies")); // will be an integer in a spin box
    usedKeys.insert(tr("multiple-document-handling")); // will be a check box
    usedKeys.insert(tr("page-delivery")); // will be a check box

    populateComboBox(m_generalTab->m_paperSizeComboBox, CpdbUtils::convertPaperSizesToReadable(options[tr("media")]));
    usedKeys.insert(tr("media"));
    populateComboBox(m_generalTab->m_orientationComboBox, options[tr("orientation-requested")]);
    usedKeys.insert(tr("orientation-requested"));
    populateComboBox(m_generalTab->m_colorModeComboBox, options[tr("print-color-mode")]);
    usedKeys.insert(tr("print-color-mode"));

    populateComboBox(m_pageSetupTab->m_bothSidesComboBox, options[tr("sides")]);
    usedKeys.insert(tr("sides"));
    populateComboBox(m_pageSetupTab->m_pagesPerSideComboBox, options[tr("number-up")]);
    usedKeys.insert(tr("number-up"));

    populateComboBox(m_optionsTab->m_resolutionComboBox, options[tr("printer-resolution")]);
    usedKeys.insert(tr("printer-resolution"));
    populateComboBox(m_optionsTab->m_qualityComboBox, options[tr("print-quality")]);
    usedKeys.insert(tr("print-quality"));
    populateComboBox(m_optionsTab->m_outputBinComboBox, options[tr("output-bin")]);
    usedKeys.insert(tr("output-bin"));
    populateComboBox(m_optionsTab->m_finishingsComboBox, options[tr("finishings")]);
    usedKeys.insert(tr("finishings"));

    populateComboBox(m_jobsTab->m_startJobComboBox, options[tr("job-hold-until")]);
    usedKeys.insert(tr("job-hold-until"));
    populateComboBox(m_jobsTab->m_jobNameComboBox, options[tr("job-name")]);
    usedKeys.insert(tr("job-name"));
    populateComboBox(m_jobsTab->m_jobPriorityComboBox, options[tr("job-priority")]);
    usedKeys.insert(tr("job-priority"));
    populateComboBox(m_jobsTab->m_jobSheetsComboBox, options[tr("job-sheets")]);
    usedKeys.insert(tr("job-sheets"));

    m_extraOptionsTab->deleteAllComboBoxes();
    for (auto it = options.begin(); it != options.end(); it++) {
        if(usedKeys.contains(it.key()))
            continue;
        QComboBox *newComboBox = m_extraOptionsTab->addNewComboBox(it.key());
        populateComboBox(newComboBox, it.value());
    }
}

void CommonPrintDialogMainLayout::remotePrintersCheckBoxStateChanged(int state)
{
    qDebug("qCPD: remotePrintersStateChanged: %d", state);
    m_backend->setRemotePrintersVisible(state == Qt::Checked);
}

void CommonPrintDialogMainLayout::copiesSpinBoxValueChanged(int value)
{
    qDebug("qCPD: copiesValueChanged: %d", value);
    m_generalTab->m_collateCheckBox->setEnabled(value != 1);
}

void CommonPrintDialogMainLayout::collateCheckBoxStateChanged(int state)
{
    qDebug("qCPD: collateStateChanged: %d", state);
    m_backend->setCollateEnabled(state == Qt::Checked);
}

void CommonPrintDialogMainLayout::reverseCheckBoxStateChanged(int state)
{
    qDebug("qCPD: reverseStateChanged: %d", state);
    m_backend->setReversePageOrder(state == Qt::Checked);
}

void CommonPrintDialogMainLayout::paperSizeComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: paperSizeChanged: %s", currentText.toLatin1().data());
    m_backend->setPaperSize(CpdbUtils::convertReadablePaperSizeToPWG(currentText));
}

void CommonPrintDialogMainLayout::orientationComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: orientationChanged: %s", currentText.toLatin1().data());
    m_backend->setOrientation(currentText);
}

void CommonPrintDialogMainLayout::colorModeComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: colorModeChanged: %s", currentText.toLatin1().data());
    m_backend->setColorMode(currentText);
}

void CommonPrintDialogMainLayout::bothSidesComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: bothSidesOptionChanged: %s", currentText.toLatin1().data());
    m_backend->setPrintBothSidesOption(currentText);
}

void CommonPrintDialogMainLayout::pagesPerSideComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: pagesPerSideChanged: %s", currentText.toLatin1().data());
    m_backend->setPagesPerSide(currentText);
}

void CommonPrintDialogMainLayout::resolutionComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: resolutionChanged: %s", currentText.toLatin1().data());
    m_backend->setResolution(currentText);
}

void CommonPrintDialogMainLayout::qualityComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: qualityChanged: %s", currentText.toLatin1().data());
    m_backend->setQuality(currentText);
}

void CommonPrintDialogMainLayout::outputBinComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: outputBinChanged: %s", currentText.toLatin1().data());
    m_backend->setOutputBin(currentText);
}

void CommonPrintDialogMainLayout::finishingsComboBoxValueChanged(QString currentText)
{
    qDebug("qCPD: finishingsChanged: %s", currentText.toLatin1().data());
    m_backend->setFinishings(currentText);
}

void CommonPrintDialogMainLayout::populateComboBox(QComboBox *comboBox, QStringList values)
{
    comboBox->clear();
    comboBox->addItems(values);
    comboBox->setEnabled(comboBox->count() != 0);
}

CommonPrintDialogGeneralTab::CommonPrintDialogGeneralTab(
    shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : QWidget(parent), m_backend(backend)
{
    m_destinationWidget = new QTableWidget(0, 5, this);
    m_remotePrintersCheckBox = new QCheckBox;
    m_paperSizeComboBox = new QComboBox;
    m_pagesComboBox = new QComboBox;
    m_copiesSpinBox = new QSpinBox;
    m_collateCheckBox = new QCheckBox;
    m_reverseCheckBox = new QCheckBox;
    m_orientationComboBox = new QComboBox;
    m_colorModeComboBox = new QComboBox;

    m_pagesComboBox->addItem(tr("All"));

    QFormLayout *layout = new QFormLayout;

    QGroupBox *copiesGroupBox = new QGroupBox(tr("Copies"));
    QFormLayout *copiesGroupBoxLayout = new QFormLayout;
    copiesGroupBoxLayout->addRow(new QLabel(tr("Copies")), m_copiesSpinBox);
    copiesGroupBoxLayout->addRow(new QLabel(tr("Collate Pages")), m_collateCheckBox);
    copiesGroupBoxLayout->addRow(new QLabel(tr("Reverse")), m_reverseCheckBox);
    copiesGroupBox->setLayout(copiesGroupBoxLayout);


    layout->addRow(m_destinationWidget);
    layout->addRow(new QLabel(tr("Remote Printers")), m_remotePrintersCheckBox);
    layout->addRow(new QLabel(tr("Paper Size")), m_paperSizeComboBox);
    layout->addRow(new QLabel(tr("Pages")), m_pagesComboBox);
    layout->addRow(new QLabel(tr("Orientation")), m_orientationComboBox);
    layout->addRow(new QLabel(tr("Color Mode")), m_colorModeComboBox);
    layout->addRow(copiesGroupBox);

    m_copiesSpinBox->setRange(1, 9999); // TODO: change 9999 to a dynamically determined value if possible
    m_copiesSpinBox->setValue(1);
    m_collateCheckBox->setEnabled(false);

    QStringList destinationWidgetHeaders = {tr("Printer"), tr("Location"), tr("State")};
    m_destinationWidget->setHorizontalHeaderLabels(destinationWidgetHeaders);
    m_destinationWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_destinationWidget->horizontalHeader()->setHighlightSections(false);
    m_destinationWidget->verticalHeader()->setHighlightSections(false);
    m_destinationWidget->setColumnHidden(3, true);
    m_destinationWidget->setColumnHidden(4, true);
    m_destinationWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    setLayout(layout);
}

CommonPrintDialogPageSetupTab::CommonPrintDialogPageSetupTab(
    shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_bothSidesComboBox = new QComboBox;
    m_pagesPerSideComboBox = new QComboBox;
    m_scaleSpinBox = new QSpinBox;
    m_scaleSpinBox->setRange(0, 200);
    m_scaleSpinBox->setValue(100);
    m_scaleSpinBox->setSuffix(tr("%"));
    m_paperSourceComboBox = new QComboBox;
    m_pageRangeComboBox = new QComboBox;

    QFormLayout *layout = new QFormLayout;

    QGroupBox *layoutGroupBox = new QGroupBox(tr("Layout"));
    QFormLayout *layoutGroupBoxLayout = new QFormLayout;
    layoutGroupBoxLayout->addRow(new QLabel(tr("Print Both Sides")), m_bothSidesComboBox);
    layoutGroupBoxLayout->addRow(new QLabel(tr("Pages Per Side")), m_pagesPerSideComboBox);
    layoutGroupBoxLayout->addRow(new QLabel(tr("Scale")), m_scaleSpinBox);
    layoutGroupBox->setLayout(layoutGroupBoxLayout);

    QGroupBox *paperGroupBox = new QGroupBox(tr("Paper"));
    QFormLayout *paperGroupBoxLayout = new QFormLayout;
    paperGroupBoxLayout->addRow(new QLabel(tr("Paper Source")), m_paperSourceComboBox);
    paperGroupBox->setLayout(paperGroupBoxLayout);

    layout->addRow(layoutGroupBox);
    layout->addRow(paperGroupBox);
    setLayout(layout);

    (void)parent;
}

CommonPrintDialogOptionsTab::CommonPrintDialogOptionsTab(
    shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_marginTopValue = new QLineEdit;
    m_marginBottomValue = new QLineEdit;
    m_marginLeftValue = new QLineEdit;
    m_marginRightValue = new QLineEdit;
    m_resolutionComboBox = new QComboBox;
    m_qualityComboBox = new QComboBox;
    m_outputBinComboBox = new QComboBox;
    m_finishingsComboBox = new QComboBox;
    m_ippAttributeFidelityComboBox = new QComboBox;

    m_layout = new QFormLayout;

    m_layout->addRow((new QLabel(tr("Margin"))));
    m_layout->addRow(new QLabel(tr("Top")), m_marginTopValue);
    m_layout->addRow(new QLabel(tr("Bottom")), m_marginBottomValue);
    m_layout->addRow(new QLabel(tr("Left")), m_marginLeftValue);
    m_layout->addRow(new QLabel(tr("Right")), m_marginRightValue);
    m_layout->addRow(new QLabel(tr("")));
    m_layout->addRow(new QLabel(tr("Resolution")), m_resolutionComboBox);
    m_layout->addRow(new QLabel(tr("Quality")), m_qualityComboBox);
    m_layout->addRow(new QLabel(tr("Output Bin")), m_outputBinComboBox);
    m_layout->addRow(new QLabel(tr("Finishings")), m_finishingsComboBox);

    setLayout(m_layout);

    (void)parent;
}

CommonPrintDialogJobsTab::CommonPrintDialogJobsTab(
    shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    QWidget *jobsWidget = new QWidget;
    m_jobsLayout = new QGridLayout;

    m_jobsLayout->addWidget(new QLabel(tr("Printer")), 0, 0);
    m_jobsLayout->addWidget(new QLabel(tr("Location")), 0, 1);
    m_jobsLayout->addWidget(new QLabel(tr("Status")), 0, 2);

    jobsWidget->setLayout(m_jobsLayout);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setMinimumHeight(240);
    m_scrollArea->setWidget(jobsWidget);
    m_scrollArea->setWidgetResizable(true);

    m_refreshButton = new QPushButton(tr("Refresh"));
    m_startJobComboBox = new QComboBox;
    m_saveJobButton = new QPushButton(tr("Save"));
    m_jobNameComboBox = new QComboBox;
    m_jobPriorityComboBox = new QComboBox;
    m_jobSheetsComboBox = new QComboBox;

    QFormLayout *layout = new QFormLayout;
    layout->addRow(m_scrollArea);
    layout->addRow(new QLabel(tr("Refresh")), m_refreshButton);
    layout->addRow(new QLabel(tr("Start Job")), m_startJobComboBox);
    layout->addRow(new QLabel(tr("Save Job")), m_saveJobButton);
    layout->addRow(new QLabel(tr("Job Name")), m_jobNameComboBox);
    layout->addRow(new QLabel(tr("Job Priority")), m_jobPriorityComboBox);
    layout->addRow(new QLabel(tr("Job Sheets")), m_jobSheetsComboBox);

    setLayout(layout);

    (void)parent;
}

CommonPrintDialogExtraOptionsTab::CommonPrintDialogExtraOptionsTab(
    shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_layout = new QFormLayout;
    setLayout(m_layout);

    (void)parent;
}

QComboBox *CommonPrintDialogExtraOptionsTab::addNewComboBox(QString name)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setProperty("optionName", name);
    m_layout->addRow(new QLabel(name), comboBox);

    QObject::connect(
        comboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(extraOptionsComboBoxValueChanged(QString))
    );

    return comboBox;
}

void CommonPrintDialogExtraOptionsTab::deleteAllComboBoxes()
{
    int rowCount = m_layout->rowCount();
    for(int i=rowCount-1; i>=0; i--)
        m_layout->removeRow(i);
}

void CommonPrintDialogExtraOptionsTab::extraOptionsComboBoxValueChanged(QString currentText)
{
    QString optionName = qvariant_cast<QString>(sender()->property("optionName"));
    qDebug("qCPD: extraOptionChanged: %s : %s", optionName.toLatin1().data(), currentText.toLatin1().data());
    m_backend->setExtraOption(optionName, currentText);
}
