#include <memory>
#include <QtWidgets/QtWidgets>
#include <QUuid>
#include <QStringList>

#include <private/qcpdb_p.h>
#include "qcommonprintdialog.h"

QT_BEGIN_NAMESPACE

QCommonPrintDialog::QCommonPrintDialog(QPrinter *printer, QWidget *parent)
    : QDialog (parent), m_printer(printer)
{
    auto id = QUuid::createUuid().toString().remove('{').remove('}').toLatin1();
    m_backend = std::make_shared<CommonPrintDialogBackend>(id.data());
    QVariant backendAsQVariant = QVariant::fromValue(m_backend);
    m_printer->printEngine()->setProperty(PPK_CommonPrintDialogBackend, backendAsQVariant);

    // Set the size of the print dialog
    resize(640, 480);

    // Set the layout of the dialog
    m_mainLayout = new CommonPrintDialogMainLayout(this, m_backend, parent);
    setLayout(m_mainLayout);
}

CommonPrintDialogMainLayout::CommonPrintDialogMainLayout(
    QCommonPrintDialog* commonPrintDialog, std::shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent)
    : m_commonPrintDialog(commonPrintDialog), m_backend(backend)
{
    // m_tabWidget contains all the tabs
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

    QPageLayout currentPageLayout = m_commonPrintDialog->m_printer->pageLayout();
    QPageLayout::Unit unit = currentPageLayout.units();
    QMarginsF margins = currentPageLayout.margins();
    m_optionsTab->m_marginTopValue->setValue(margins.top());
    m_optionsTab->m_marginBottomValue->setValue(margins.bottom());
    m_optionsTab->m_marginLeftValue->setValue(margins.left());
    m_optionsTab->m_marginRightValue->setValue(margins.right());
    int unitIndex = m_optionsTab->m_marginUnitComboBox->findData(unit);
    if(unitIndex != -1)
        m_optionsTab->m_marginUnitComboBox->setCurrentIndex(unitIndex);

    m_printButton = new QPushButton(tr("Print"));
    m_printButton->setDefault(true);
    m_cancelButton = new QPushButton(tr("Cancel"));

    // buttonLayout contains the two buttons (Print and Cancel)
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_printButton);
    buttonLayout->addWidget(m_cancelButton);

    // controlsLayout is the entire layout containing the buttons and the tab widget
    QVBoxLayout *controlsLayout = new QVBoxLayout;
    controlsLayout->addWidget(m_tabWidget);
    controlsLayout->addItem(buttonLayout);

    addItem(controlsLayout);

    // Connect signals for all the components to the respective slots
    connectSignalsAndSlots();
}

void CommonPrintDialogMainLayout::connectSignalsAndSlots()
{
    QObject::connect(
        m_printButton, SIGNAL(clicked()),
        this, SLOT(applySettingsAndAccept())
    );

    QObject::connect(
        m_cancelButton, SIGNAL(clicked()),
        m_commonPrintDialog, SLOT(reject())
    );

    // To update the displayed printers whenever the printer list changes at the backend
    QObject::connect(
        CpdbPrinterListMaintainer::getInstance(), SIGNAL(printerListChanged()),
        this, SLOT(printerListChanged())
    );

    // To load the options for the newly selected printer
    QObject::connect(
        m_generalTab->m_destinationWidget, SIGNAL(currentCellChanged(int, int, int, int)),
        this, SLOT(newPrinterSelected(int))
    );

    QObject::connect(
        m_generalTab->m_remotePrintersCheckBox, SIGNAL(stateChanged(int)),
        this, SLOT(remotePrintersCheckBoxStateChanged(int))
    );

    // Paper size combobox has a separate slot because we need to convert paper
    // sizes from human readable names to code names before setting the option
    QObject::connect(
        m_pageSetupTab->m_paperSizeComboBox, SIGNAL(currentTextChanged(QString)),
        this, SLOT(paperSizeComboBoxValueChanged(QString))
    );

    QObject::connect(
        m_generalTab->m_rangeCustomRangeRadioButton, SIGNAL(toggled(bool)),
        m_generalTab->m_customRangeLineEdit, SLOT(setEnabled(bool))
    );

    // Connect signals for the start-job time radio buttons
    connectStartJobAtRadioButtonSignal(m_jobsTab->m_startJobNowRadioButton);
    connectStartJobAtRadioButtonSignal(m_jobsTab->m_startJobAtRadioButton);
    connectStartJobAtRadioButtonSignal(m_jobsTab->m_startJobOnHoldRadioButton);

    // Connect signals for all the generic combo boxes
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

    // count variable is required to determine which row in the
    // destination table corresponds to which printer
    int count = 0;

    for(auto printer : printers) {
        // Insert the items at row index `count`
        m_generalTab->m_destinationWidget->setItem(count, 0, new QTableWidgetItem(printer->name));
        m_generalTab->m_destinationWidget->setItem(count, 1, new QTableWidgetItem(printer->location));
        m_generalTab->m_destinationWidget->setItem(count, 2, new QTableWidgetItem(printer->state));

        // Columns 3 and 4 are hidden in the destination table
        m_generalTab->m_destinationWidget->setItem(count, 3, new QTableWidgetItem(printer->id));
        m_generalTab->m_destinationWidget->setItem(count, 4, new QTableWidgetItem(printer->backend));
        count ++;
    }

    // Whenever the printers list is updated, the topmost printer is selected
    // This is to ensure that one printer is always selected
    m_generalTab->m_destinationWidget->setCurrentCell(0, 0);
    newPrinterSelected(0);
}

void CommonPrintDialogMainLayout::newPrinterSelected(int row)
{
    // Extract data from the hidden columns 3 (id) and 4 (backend)
    QString id = m_generalTab->m_destinationWidget->item(row, 3)->text();
    QString backend = m_generalTab->m_destinationWidget->item(row, 4)->text();

    qDebug("qCPD: New Printer Selected: %s, %s", id.toLocal8Bit().data(), backend.toLocal8Bit().data());

    // Tell the backend about the newly selected printer, so it may load the options for this printer
    m_backend->setCurrentPrinter(id, backend);

    // Retrieve the options for the newly selected printer
    QMap<QString, QStringList> options = m_backend->getOptionsForCurrentPrinter();
    for (auto it = options.begin(); it != options.end(); it++) {
        qDebug("Option %s: [%s]", it.key().toLocal8Bit().data(), it.value().join(tr(", ")).toLocal8Bit().data());
    }

    // usedKeys is a set that contains the names of those options that have been handled by
    // the print dialog. Any options not handled by the dialog will have to be displayed
    // in the extra-options tab.
    QSet<QString> usedKeys;

    // Add those options into the usedKeys set that are not combo boxes.
    // The options that will be combo-boxes will be added into the usedKeys
    // set in the updateComboBox method.
    // IMPORTANT: These settings must be applied when the dialog box is accepted,
    // by calling the appropriate backend functions in applySettingsAndAccept()
    usedKeys.insert(QString::fromUtf8("copies")); // will be an integer in a spin box
    usedKeys.insert(QString::fromUtf8("multiple-document-handling")); // will be a check box
    usedKeys.insert(QString::fromUtf8("page-delivery")); // will be a check box
    usedKeys.insert(QString::fromUtf8("page-ranges")); // will be a radio button group
    usedKeys.insert(QString::fromUtf8("job-name")); // will be a line edit box

    // Convert paper sizes from code names to human readable names
    options[QString::fromUtf8("media")] = CpdbUtils::convertPaperSizesToReadable(options[QString::fromUtf8("media")]);

    // Remove these two options from the list because they will be displayed as radio buttons
    options[QString::fromUtf8("job-hold-until")].removeOne(QString::fromUtf8("no-hold"));
    options[QString::fromUtf8("job-hold-until")].removeOne(QString::fromUtf8("indefinite"));

    // Update the combo boxes with the options for the newly selected printer, and add
    // the option names to the usedKeys set.
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

    // Clear the extraOptions tab
    m_extraOptionsTab->deleteAllComboBoxes();

    // extraCount maintains the number of items in the extraOptions tab.
    // If this number is 0, we will disable the extraOptions tab.
    int extraCount = 0;

    for (QString optionKey : options.keys()) {
        // SKip those options that have been handled by the dialog
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

    // Disable or enable the extraOptions tab based on extraCount value
    int extraOptionsTabIndex = m_tabWidget->indexOf(m_extraOptionsTab);
    bool enableExtraOptionsTab = extraCount>0;
    m_tabWidget->setTabEnabled(extraOptionsTabIndex, enableExtraOptionsTab);

    // HACK: Updating the values in the m_startJobAtComboBox automatically enables it,
    // HACK: even if the start job option is set to "Now" or "On Hold". So we manually
    // HACK: disable this combo box.
    if(!m_jobsTab->m_startJobAtRadioButton->isChecked())
        m_jobsTab->m_startJobAtComboBox->setEnabled(false);
}

void CommonPrintDialogMainLayout::comboBoxValueChanged(QString currentText)
{
    QString optionName = qvariant_cast<QString>(sender()->property("cpdbOptionName"));
    qDebug("qCPD | optionChanged > %s : %s", optionName.toLatin1().data(), currentText.toLatin1().data());
    m_backend->setSelectableOption(optionName, currentText);
}

void CommonPrintDialogMainLayout::paperSizeComboBoxValueChanged(QString currentText)
{
    if(currentText.isEmpty())
        return;

    QString optionName = qvariant_cast<QString>(sender()->property("cpdbOptionName"));
    qDebug("qCPD | optionChanged > %s : %s", optionName.toLatin1().data(), currentText.toLatin1().data());
    QString pwgSize = CpdbUtils::convertReadablePaperSizeToPWG(currentText);

    static const QString smallX = QString::fromUtf8("x");
    static const QString underscore = QString::fromUtf8("_");
    static const QString mmUnit = QString::fromUtf8("mm");
    static const QString inUnit = QString::fromUtf8("in");

    // pwgSize is of the format `<name>_<width>x<height><unit>`
    // paperSizeDimensions will be of the format `<width>x<height><unit>`
    QString paperSizeDimensions = pwgSize.split(underscore).last();

    // Extract the unit from the dimensions. It is always stored in the last 2 characters.
    QString unitString = paperSizeDimensions.right(2);
    paperSizeDimensions.remove(unitString); // Remove the unit from the dimensions string.

    QPageSize::Unit unit;
    if(unitString == mmUnit)
        unit = QPageSize::Millimeter;
    else if(unitString == inUnit)
        unit = QPageSize::Inch;
    else
        return; // Unhandled page size

    // Now paperSizeDimensions will be of the format `<width>x<height>`
    QStringList dimensionsNumbers = paperSizeDimensions.split(smallX);
    if(dimensionsNumbers.size() != 2)
        return; // Invalid dimension format
    qreal width = dimensionsNumbers[0].toDouble();
    qreal height = dimensionsNumbers[1].toDouble();

    m_commonPrintDialog->m_printer->setPageSize(QPageSize(
        QSizeF(width, height), unit, pwgSize, QPageSize::ExactMatch));

    m_backend->setSelectableOption(optionName, currentText);
}

void CommonPrintDialogMainLayout::remotePrintersCheckBoxStateChanged(int state)
{
    qDebug("qCPD: remotePrintersStateChanged: %d", state);
    m_backend->setRemotePrintersVisible(state == Qt::Checked);
}

void CommonPrintDialogMainLayout::startJobAtRadioButtonChanged(bool checked)
{
    // Since this slot is called upon every toggle of a radio button, we want
    // to filter the events to only those when a radio button is selected.
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

void CommonPrintDialogMainLayout::updateComboBox(QComboBox *comboBox, QMap<QString, QStringList> options, QSet<QString>* usedKeys)
{
    QString optionName = qvariant_cast<QString>(comboBox->property("cpdbOptionName"));
    comboBox->clear();
    if(options.contains(optionName))
        comboBox->addItems(options[optionName]);
    // Enable this combobox only if it has some selectable values
    comboBox->setEnabled(comboBox->count() != 0);
    usedKeys->insert(optionName);
}

void CommonPrintDialogMainLayout::applySettingsAndAccept()
{
    // Add those settings to printers which are not combo boxes
    m_backend->setNumCopies(m_generalTab->m_copiesSpinBox->value());
    m_backend->setCollateEnabled(m_generalTab->m_collateCheckBox->isChecked());
    m_backend->setReversePageOrder(m_generalTab->m_reverseCheckBox->isChecked());
    m_backend->setSelectableOption(QString::fromUtf8("job-name"), m_jobsTab->m_jobNameLineEdit->text());
    if(m_generalTab->m_rangeAllRadioButton->isChecked())
        m_backend->setPageRange(QString::fromUtf8("1-9999"));
    else if(m_generalTab->m_rangeCustomRangeRadioButton->isChecked())
        m_backend->setPageRange(m_generalTab->m_customRangeLineEdit->text());

    // Add margins settings to PDF printer
    QMarginsF margins = QMarginsF(
        m_optionsTab->m_marginLeftValue->value(),
        m_optionsTab->m_marginTopValue->value(),
        m_optionsTab->m_marginRightValue->value(),
        m_optionsTab->m_marginBottomValue->value()
    );
    QVariant marginsUnitVariant = m_optionsTab->m_marginUnitComboBox->currentData();
    QPageLayout::Unit marginsUnit = marginsUnitVariant.value<QPageLayout::Unit>();
    m_commonPrintDialog->m_printer->setPageMargins(margins, marginsUnit);

    m_commonPrintDialog->accept();
}

CommonPrintDialogGeneralTab::CommonPrintDialogGeneralTab(
    std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent)
    : QWidget(parent), m_backend(backend)
{
    // Initially, the table has 0 rows and 5 columns
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

    // Set the columns 0 and 1 to have 1:1 ratio of width
    bottomLayout->setStretch(0,1);
    bottomLayout->setStretch(1,1);

    layout->addRow(printerGroupBox);
    layout->addRow(bottomLayout);

    m_copiesSpinBox->setRange(1, 9999); // TODO: change 9999 to a dynamically determined value if possible
    m_copiesSpinBox->setValue(1);
    m_rangeCurrentPageRadioButton->setEnabled(false);
    m_rangeSelectionRadioButton->setEnabled(false);
    m_customRangeLineEdit->setEnabled(false);
    m_rangeAllRadioButton->setChecked(true);

    QStringList destinationWidgetHeaders = {tr("Printer"), tr("Location"), tr("State")};
    m_destinationWidget->setHorizontalHeaderLabels(destinationWidgetHeaders);
    m_destinationWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_destinationWidget->horizontalHeader()->setHighlightSections(false);
    m_destinationWidget->verticalHeader()->setHighlightSections(false);
    // Hide the columns 3 (printer id) and 4 (printer backend)
    m_destinationWidget->setColumnHidden(3, true);
    m_destinationWidget->setColumnHidden(4, true);

    // Allow selection of a single entire row instead of cells
    m_destinationWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_destinationWidget->setSelectionMode(QAbstractItemView::SingleSelection);

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
    m_marginTopValue = new QDoubleSpinBox;
    m_marginBottomValue = new QDoubleSpinBox;
    m_marginLeftValue = new QDoubleSpinBox;
    m_marginRightValue = new QDoubleSpinBox;
    m_marginUnitComboBox = new QComboBox;
    m_resolutionComboBox = new QComboBox;
    m_qualityComboBox = new QComboBox;
    m_colorModeComboBox = new QComboBox;
    m_finishingsComboBox = new QComboBox;
    m_ippAttributeFidelityComboBox = new QComboBox;

    m_layout = new QFormLayout;

    QGroupBox *marginsGroupBox = new QGroupBox(tr("Margins"));
    QGridLayout *marginsGroupBoxLayout = new QGridLayout;
    marginsGroupBoxLayout->addWidget(new QLabel(tr("Units")), 0, 1, Qt::AlignRight);
    marginsGroupBoxLayout->addWidget(m_marginUnitComboBox, 0, 2);
    marginsGroupBoxLayout->addWidget(new QLabel(tr("Top")), 1, 0, Qt::AlignRight);
    marginsGroupBoxLayout->addWidget(m_marginTopValue, 1, 1);
    marginsGroupBoxLayout->addWidget(new QLabel(tr("Bottom")), 1, 2, Qt::AlignRight);
    marginsGroupBoxLayout->addWidget(m_marginBottomValue, 1, 3);
    marginsGroupBoxLayout->addWidget(new QLabel(tr("Left")), 2, 0, Qt::AlignRight);
    marginsGroupBoxLayout->addWidget(m_marginLeftValue, 2, 1);
    marginsGroupBoxLayout->addWidget(new QLabel(tr("Right")), 2, 2, Qt::AlignRight);
    marginsGroupBoxLayout->addWidget(m_marginRightValue, 2, 3);
    marginsGroupBox->setLayout(marginsGroupBoxLayout);

    m_layout->addRow(marginsGroupBox);
    m_layout->addRow(new QLabel(tr("Resolution")), m_resolutionComboBox);
    m_layout->addRow(new QLabel(tr("Quality")), m_qualityComboBox);
    m_layout->addRow(new QLabel(tr("Color Mode")), m_colorModeComboBox);
    m_layout->addRow(new QLabel(tr("Finishings")), m_finishingsComboBox);

    setLayout(m_layout);

    m_marginUnitComboBox->addItem(QString::fromUtf8("Millimeter"), QPageLayout::Millimeter);
    m_marginUnitComboBox->addItem(QString::fromUtf8("Point"), QPageLayout::Point);
    m_marginUnitComboBox->addItem(QString::fromUtf8("Inch"), QPageLayout::Inch);
    m_marginUnitComboBox->addItem(QString::fromUtf8("Pica"), QPageLayout::Pica);
    m_marginUnitComboBox->addItem(QString::fromUtf8("Didot"), QPageLayout::Didot);
    m_marginUnitComboBox->addItem(QString::fromUtf8("Cicero"), QPageLayout::Cicero);

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

    // Set the default job name to contain the current time
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
