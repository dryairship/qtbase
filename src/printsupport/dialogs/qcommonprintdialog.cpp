#include <memory>
#include <QtWidgets/QtWidgets>
#include <QUuid>
#include <QStringList>

#include <private/qcpdb_p.h>
#include "qcommonprintdialog.h"
QT_BEGIN_NAMESPACE

QCommonPrintDialog::QCommonPrintDialog(QWidget *parent)
    : QDialog (parent)
{
    auto id = QUuid::createUuid().toString().remove('{').remove('}').toLatin1();
    m_backend = std::make_shared<CommonPrintDialogBackend>(id.data());

    resize(640, 480);
    m_mainLayout = new CommonPrintDialogMainLayout(this, m_backend, parent);
    setLayout(m_mainLayout);
}

CommonPrintDialogMainLayout::CommonPrintDialogMainLayout(
    QCommonPrintDialog* commonPrintDialog, std::shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent)
    : m_commonPrintDialog(commonPrintDialog), m_backend(backend)
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
    m_tabWidget->addTab(m_jobsTab, tr("Job"));
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
        m_printButton, SIGNAL(clicked()),
        m_commonPrintDialog, SLOT(accept())
    );

    QObject::connect(
        m_cancelButton, SIGNAL(clicked()),
        m_commonPrintDialog, SLOT(reject())
    );

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
        m_pageSetupTab->m_paperSizeComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(paperSizeComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_generalTab->m_customRangeLineEdit, SIGNAL(textChanged(QString)),
        this, SLOT(customRangeLineEditTextChanged(QString))
    );

    QObject::connect(
        m_jobsTab->m_jobNameLineEdit, SIGNAL(textChanged(QString)),
        this, SLOT(lineEditTextChanged(QString))
    );

    connectRangeRadioButtonSignal(m_generalTab->m_rangeAllRadioButton);
    connectRangeRadioButtonSignal(m_generalTab->m_rangeCurrentPageRadioButton);
    connectRangeRadioButtonSignal(m_generalTab->m_rangeSelectionRadioButton);
    connectRangeRadioButtonSignal(m_generalTab->m_rangeCustomRangeRadioButton);

    connectStartJobAtRadioButtonSignal(m_jobsTab->m_startJobNowRadioButton);
    connectStartJobAtRadioButtonSignal(m_jobsTab->m_startJobAtRadioButton);
    connectStartJobAtRadioButtonSignal(m_jobsTab->m_startJobOnHoldRadioButton);

    connectComboBoxSignal(m_pageSetupTab->m_orientationComboBox);
    connectComboBoxSignal(m_optionsTab->m_colorModeComboBox);
    connectComboBoxSignal(m_pageSetupTab->m_bothSidesComboBox);
    connectComboBoxSignal(m_pageSetupTab->m_pagesPerSideComboBox);
    connectComboBoxSignal(m_pageSetupTab->m_pageOrderingComboBox);
    connectComboBoxSignal(m_pageSetupTab->m_scaleComboBox);
    connectComboBoxSignal(m_pageSetupTab->m_onlyPrintComboBox);
    connectComboBoxSignal(m_pageSetupTab->m_outputBinComboBox);
    connectComboBoxSignal(m_optionsTab->m_resolutionComboBox);
    connectComboBoxSignal(m_optionsTab->m_qualityComboBox);
    connectComboBoxSignal(m_jobsTab->m_startJobAtComboBox);
    connectComboBoxSignal(m_jobsTab->m_jobPriorityComboBox);
    connectComboBoxSignal(m_jobsTab->m_jobSheetsComboBox);
    connectComboBoxSignal(m_optionsTab->m_finishingsComboBox);
}

void CommonPrintDialogMainLayout::connectComboBoxSignal(QComboBox* comboBox)
{
    QObject::connect(
        comboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(comboBoxValueChanged(QString))
    );
}

void CommonPrintDialogMainLayout::connectRangeRadioButtonSignal(QRadioButton* radioButton)
{
    QObject::connect(
        radioButton, SIGNAL(toggled(bool)),
        this, SLOT(rangeRadioButtonChanged(bool))
    );
}

void CommonPrintDialogMainLayout::connectStartJobAtRadioButtonSignal(QRadioButton* radioButton)
{
    QObject::connect(
        radioButton, SIGNAL(toggled(bool)),
        this, SLOT(startJobAtRadioButtonChanged(bool))
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
    usedKeys.insert(QString::fromUtf8("copies")); // will be an integer in a spin box
    usedKeys.insert(QString::fromUtf8("multiple-document-handling")); // will be a check box
    usedKeys.insert(QString::fromUtf8("page-delivery")); // will be a check box
    usedKeys.insert(QString::fromUtf8("page-ranges")); // will be a radio button group
    usedKeys.insert(QString::fromUtf8("job-name")); // will be a line edit box

    options[QString::fromUtf8("media")] = CpdbUtils::convertPaperSizesToReadable(options[QString::fromUtf8("media")]);
    options[QString::fromUtf8("job-hold-until")].removeOne(QString::fromUtf8("no-hold"));
    options[QString::fromUtf8("job-hold-until")].removeOne(QString::fromUtf8("indefinite"));

    updateComboBox(m_pageSetupTab->m_bothSidesComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_pagesPerSideComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_pageOrderingComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_onlyPrintComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_scaleComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_paperSizeComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_orientationComboBox, options, &usedKeys);
    updateComboBox(m_pageSetupTab->m_outputBinComboBox, options, &usedKeys);
    updateComboBox(m_optionsTab->m_resolutionComboBox, options, &usedKeys);
    updateComboBox(m_optionsTab->m_qualityComboBox, options, &usedKeys);
    updateComboBox(m_optionsTab->m_colorModeComboBox, options, &usedKeys);
    updateComboBox(m_optionsTab->m_finishingsComboBox, options, &usedKeys);
    updateComboBox(m_jobsTab->m_startJobAtComboBox, options, &usedKeys);
    updateComboBox(m_jobsTab->m_jobPriorityComboBox, options, &usedKeys);
    updateComboBox(m_jobsTab->m_jobSheetsComboBox, options, &usedKeys);

    m_extraOptionsTab->deleteAllComboBoxes();

    int extraCount = 0;
    for (QString optionKey : options.keys()) {
        if(usedKeys.contains(optionKey))
            continue;
        QComboBox *newComboBox = m_extraOptionsTab->addNewComboBox(optionKey);
        QObject::connect(
            newComboBox, SIGNAL(currentTextChanged(QString)),
            this, SLOT(comboBoxValueChanged(QString))
        );
        updateComboBox(newComboBox, options, &usedKeys);
        extraCount++;
    }

    int extraOptionsTabIndex = m_tabWidget->indexOf(m_extraOptionsTab);
    bool enableExtraOptionsTab = extraCount>0;
    m_tabWidget->setTabEnabled(extraOptionsTabIndex, enableExtraOptionsTab);

    if(!m_jobsTab->m_startJobAtRadioButton->isChecked())
        m_jobsTab->m_startJobAtComboBox->setEnabled(false);
}

void CommonPrintDialogMainLayout::comboBoxValueChanged(QString currentText)
{
    QString optionName = qvariant_cast<QString>(sender()->property("cpdbOptionName"));
    qDebug("qCPD | optionChanged > %s : %s", optionName.toLatin1().data(), currentText.toLatin1().data());
    m_backend->setSelectableOption(optionName, currentText);
}

void CommonPrintDialogMainLayout::lineEditTextChanged(QString currentText)
{
    QString optionName = qvariant_cast<QString>(sender()->property("cpdbOptionName"));
    qDebug("qCPD | optionChanged > %s : %s", optionName.toLatin1().data(), currentText.toLatin1().data());
    m_backend->setSelectableOption(optionName, currentText);
}

void CommonPrintDialogMainLayout::paperSizeComboBoxValueChanged(QString currentText)
{
    QString optionName = qvariant_cast<QString>(sender()->property("cpdbOptionName"));
    qDebug("qCPD | optionChanged > %s : %s", optionName.toLatin1().data(), currentText.toLatin1().data());
    QString pwgSize = CpdbUtils::convertReadablePaperSizeToPWG(currentText);
    m_backend->setSelectableOption(optionName, currentText);
}

void CommonPrintDialogMainLayout::remotePrintersCheckBoxStateChanged(int state)
{
    qDebug("qCPD: remotePrintersStateChanged: %d", state);
    m_backend->setRemotePrintersVisible(state == Qt::Checked);
}

void CommonPrintDialogMainLayout::rangeRadioButtonChanged(bool checked)
{
    if(!checked)
        return;

    QRadioButton *radioButton = static_cast<QRadioButton *>(sender());
    QString radioButtonText = radioButton->text();

    if(radioButtonText == tr("All")) {
        qDebug("qCPD: rangeRadioButton: All");
        m_backend->setPageRange(QString());
        m_generalTab->m_customRangeLineEdit->setEnabled(false);
    } else if(radioButtonText == tr("Pages: ")) {
        QString range = m_generalTab->m_customRangeLineEdit->text();
        qDebug("qCPD: rangeRadioButton: Pages: %s", range.toLatin1().data());
        m_backend->setPageRange(range);
        m_generalTab->m_customRangeLineEdit->setEnabled(true);
    }
}

void CommonPrintDialogMainLayout::startJobAtRadioButtonChanged(bool checked)
{
    if(!checked)
        return;

    QRadioButton *radioButton = static_cast<QRadioButton *>(sender());
    QString radioButtonText = radioButton->text();
    QString optionName = qvariant_cast<QString>(m_jobsTab->m_startJobAtComboBox->property("cpdbOptionName"));

    if(radioButtonText == tr("Now")) {
        qDebug("qCPD | startJobAt: Now");
        m_backend->setSelectableOption(optionName, QString::fromUtf8("no-hold"));
        m_jobsTab->m_startJobAtComboBox->setEnabled(false);
    } else if(radioButtonText == tr("On Hold")) {
        qDebug("qCPD | startJobAt: Oh Hold");
        m_backend->setSelectableOption(optionName, QString::fromUtf8("indefinite"));
        m_jobsTab->m_startJobAtComboBox->setEnabled(false);
    } else if(radioButtonText == tr("At: ")) {
        QString startJobTime = m_jobsTab->m_startJobAtComboBox->currentText();
        qDebug("qCPD | startJobAt: At %s", startJobTime.toLatin1().data());
        m_backend->setSelectableOption(optionName, startJobTime);
        m_jobsTab->m_startJobAtComboBox->setEnabled(true);
    }
}

void CommonPrintDialogMainLayout::customRangeLineEditTextChanged(QString currentText)
{
    qDebug("qCPD: rangeRadioButton: Pages: %s", currentText.toLatin1().data());
    m_backend->setPageRange(currentText);
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

void CommonPrintDialogMainLayout::updateComboBox(QComboBox *comboBox, QMap<QString, QStringList> options, QSet<QString>* usedKeys)
{
    QString optionName = qvariant_cast<QString>(comboBox->property("cpdbOptionName"));
    comboBox->clear();
    if(options.contains(optionName))
        comboBox->addItems(options[optionName]);
    comboBox->setEnabled(comboBox->count() != 0);
    usedKeys->insert(optionName);
}

CommonPrintDialogGeneralTab::CommonPrintDialogGeneralTab(
    std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : QWidget(parent), m_backend(backend)
{
    m_destinationWidget = new QTableWidget(0, 5, this);
    m_remotePrintersCheckBox = new QCheckBox;
    m_copiesSpinBox = new QSpinBox;
    m_collateCheckBox = new QCheckBox;
    m_reverseCheckBox = new QCheckBox;
    m_rangeAllRadioButton = new QRadioButton(tr("All"));
    m_rangeCurrentPageRadioButton = new QRadioButton(tr("Current Page"));
    m_rangeSelectionRadioButton = new QRadioButton(tr("Selection"));
    m_rangeCustomRangeRadioButton = new QRadioButton(tr("Pages: "));
    m_customRangeLineEdit = new QLineEdit;

    QFormLayout *layout = new QFormLayout;

    QGroupBox *printerGroupBox = new QGroupBox(tr("Printer"));
    QFormLayout *printerGroupBoxLayout = new QFormLayout;
    printerGroupBoxLayout->addRow(m_destinationWidget);
    printerGroupBoxLayout->addRow(new QLabel(tr("Remote Printers")), m_remotePrintersCheckBox);
    printerGroupBox->setLayout(printerGroupBoxLayout);

    QGroupBox *copiesGroupBox = new QGroupBox(tr("Copies"));
    QFormLayout *copiesGroupBoxLayout = new QFormLayout;
    copiesGroupBoxLayout->addRow(new QLabel(tr("Copies")), m_copiesSpinBox);
    copiesGroupBoxLayout->addRow(new QLabel(tr("Collate Pages")), m_collateCheckBox);
    copiesGroupBoxLayout->addRow(new QLabel(tr("Reverse")), m_reverseCheckBox);
    copiesGroupBox->setLayout(copiesGroupBoxLayout);

    QGroupBox *rangeGroupBox = new QGroupBox(tr("Range"));
    QFormLayout *rangeGroupBoxLayout = new QFormLayout;
    rangeGroupBoxLayout->addRow(m_rangeAllRadioButton);
    rangeGroupBoxLayout->addRow(m_rangeCurrentPageRadioButton);
    rangeGroupBoxLayout->addRow(m_rangeSelectionRadioButton);
    rangeGroupBoxLayout->addRow(m_rangeCustomRangeRadioButton, m_customRangeLineEdit);
    rangeGroupBox->setLayout(rangeGroupBoxLayout);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(rangeGroupBox);
    bottomLayout->addWidget(copiesGroupBox);
    bottomLayout->setStretch(0,1);
    bottomLayout->setStretch(1,1);

    layout->addRow(printerGroupBox);
    layout->addRow(bottomLayout);

    m_copiesSpinBox->setRange(1, 9999); // TODO: change 9999 to a dynamically determined value if possible
    m_copiesSpinBox->setValue(1);
    m_collateCheckBox->setEnabled(false);
    m_rangeCurrentPageRadioButton->setEnabled(false);
    m_rangeSelectionRadioButton->setEnabled(false);
    m_customRangeLineEdit->setEnabled(false);
    m_rangeAllRadioButton->setChecked(true);

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
    std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_bothSidesComboBox = new QComboBox;
    m_pagesPerSideComboBox = new QComboBox;
    m_pageOrderingComboBox = new QComboBox;
    m_scaleComboBox = new QComboBox;
    m_paperSourceComboBox = new QComboBox;
    m_pageRangeComboBox = new QComboBox;
    m_paperSizeComboBox = new QComboBox;
    m_orientationComboBox = new QComboBox;
    m_outputBinComboBox = new QComboBox;
    m_onlyPrintComboBox = new QComboBox;

    QGridLayout *layout = new QGridLayout;

    QGroupBox *layoutGroupBox = new QGroupBox(tr("Layout"));
    QFormLayout *layoutGroupBoxLayout = new QFormLayout;
    layoutGroupBoxLayout->addRow(new QLabel(tr("Print Both Sides")), m_bothSidesComboBox);
    layoutGroupBoxLayout->addRow(new QLabel(tr("Pages Per Side")), m_pagesPerSideComboBox);
    layoutGroupBoxLayout->addRow(new QLabel(tr("Page Ordering")), m_pageOrderingComboBox);
    layoutGroupBoxLayout->addRow(new QLabel(tr("Only Print Pages")), m_onlyPrintComboBox);
    layoutGroupBoxLayout->addRow(new QLabel(tr("Scale")), m_scaleComboBox);
    layoutGroupBox->setLayout(layoutGroupBoxLayout);

    QGroupBox *paperGroupBox = new QGroupBox(tr("Paper"));
    QFormLayout *paperGroupBoxLayout = new QFormLayout;
    paperGroupBoxLayout->addRow(new QLabel(tr("Paper Source")), m_paperSourceComboBox);
    paperGroupBoxLayout->addRow(new QLabel(tr("Output Bin")), m_outputBinComboBox);
    paperGroupBoxLayout->addRow(new QLabel(tr("Paper Size")), m_paperSizeComboBox);
    paperGroupBoxLayout->addRow(new QLabel(tr("Orientation")), m_orientationComboBox);
    paperGroupBox->setLayout(paperGroupBoxLayout);

    layout->addWidget(layoutGroupBox, 0, 0);
    layout->addWidget(paperGroupBox, 0, 1);
    setLayout(layout);

    m_bothSidesComboBox->setProperty("cpdbOptionName", QString::fromUtf8("sides"));
    m_pagesPerSideComboBox->setProperty("cpdbOptionName", QString::fromUtf8("number-up"));
    m_pageOrderingComboBox->setProperty("cpdbOptionName", QString::fromUtf8("number-up-layout"));
    m_scaleComboBox->setProperty("cpdbOptionName", QString::fromUtf8("print-scaling"));
    m_paperSizeComboBox->setProperty("cpdbOptionName", QString::fromUtf8("media"));
    m_orientationComboBox->setProperty("cpdbOptionName", QString::fromUtf8("orientation-requested"));
    m_outputBinComboBox->setProperty("cpdbOptionName", QString::fromUtf8("output-bin"));
    m_onlyPrintComboBox->setProperty("cpdbOptionName", QString::fromUtf8("page-set"));

    (void)parent;
}

CommonPrintDialogOptionsTab::CommonPrintDialogOptionsTab(
    std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_marginTopValue = new QLineEdit;
    m_marginBottomValue = new QLineEdit;
    m_marginLeftValue = new QLineEdit;
    m_marginRightValue = new QLineEdit;
    m_resolutionComboBox = new QComboBox;
    m_qualityComboBox = new QComboBox;
    m_colorModeComboBox = new QComboBox;
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
    m_layout->addRow(new QLabel(tr("Color Mode")), m_colorModeComboBox);
    m_layout->addRow(new QLabel(tr("Finishings")), m_finishingsComboBox);

    setLayout(m_layout);

    m_resolutionComboBox->setProperty("cpdbOptionName", QString::fromUtf8("printer-resolution"));
    m_qualityComboBox->setProperty("cpdbOptionName", QString::fromUtf8("print-quality"));
    m_colorModeComboBox->setProperty("cpdbOptionName", QString::fromUtf8("print-color-mode"));
    m_finishingsComboBox->setProperty("cpdbOptionName", QString::fromUtf8("finishings"));

    (void)parent;
}

CommonPrintDialogJobsTab::CommonPrintDialogJobsTab(
    std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_startJobAtComboBox = new QComboBox;
    m_jobNameLineEdit = new QLineEdit;
    m_jobPriorityComboBox = new QComboBox;
    m_jobSheetsComboBox = new QComboBox;
    m_startJobAtRadioButton = new QRadioButton(tr("At: "));
    m_startJobNowRadioButton = new QRadioButton(tr("Now"));
    m_startJobOnHoldRadioButton = new QRadioButton(tr("On Hold"));

    QGroupBox *startJobGroupBox = new QGroupBox(tr("Start Job"));
    QFormLayout *startJobGroupBoxLayout = new QFormLayout;
    startJobGroupBoxLayout->addRow(m_startJobNowRadioButton);
    startJobGroupBoxLayout->addRow(m_startJobAtRadioButton, m_startJobAtComboBox);
    startJobGroupBoxLayout->addRow(m_startJobOnHoldRadioButton);
    startJobGroupBox->setLayout(startJobGroupBoxLayout);

    QFormLayout *layout = new QFormLayout;
    layout->addRow(new QLabel(tr("Job Name")), m_jobNameLineEdit);
    layout->addRow(new QLabel(tr("Job Priority")), m_jobPriorityComboBox);
    layout->addRow(new QLabel(tr("Job Sheets")), m_jobSheetsComboBox);
    layout->addRow(startJobGroupBox);

    setLayout(layout);

    m_startJobNowRadioButton->setChecked(true);
    m_startJobAtComboBox->setEnabled(false);

    m_startJobAtComboBox->setProperty("cpdbOptionName", QString::fromUtf8("job-hold-until"));
    m_jobNameLineEdit->setProperty("cpdbOptionName", QString::fromUtf8("job-name"));
    m_jobPriorityComboBox->setProperty("cpdbOptionName", QString::fromUtf8("job-priority"));
    m_jobSheetsComboBox->setProperty("cpdbOptionName", QString::fromUtf8("job-sheets"));

    QString defaultJobName = QDateTime::currentDateTime()
        .toString(QString::fromUtf8("Job @ h:m:s AP on ddd, MMMM d, yyyy"));
    m_jobNameLineEdit->setText(defaultJobName);

    (void)parent;
}

CommonPrintDialogExtraOptionsTab::CommonPrintDialogExtraOptionsTab(
    std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : m_backend(backend)
{
    m_layout = new QFormLayout;
    setLayout(m_layout);

    (void)parent;
}

QComboBox *CommonPrintDialogExtraOptionsTab::addNewComboBox(QString name)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setProperty("cpdbOptionName", name);
    m_layout->addRow(new QLabel(name), comboBox);
    return comboBox;
}

void CommonPrintDialogExtraOptionsTab::deleteAllComboBoxes()
{
    int rowCount = m_layout->rowCount();
    for(int i=rowCount-1; i>=0; i--)
        m_layout->removeRow(i);
}

QT_END_NAMESPACE
