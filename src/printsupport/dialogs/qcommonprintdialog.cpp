#include <QtWidgets/QtWidgets>

#include "qcommonprintdialog.h"

QCommonPrintDialog::QCommonPrintDialog(QWidget *parent) :
    QDialog (parent)
{
    resize(360, 480);
    mainLayout = new CommonPrintDialogMainLayout(parent);
    setLayout(mainLayout);
}

QCommonPrintDialog::~QCommonPrintDialog() {
}

CommonPrintDialogMainLayout::CommonPrintDialogMainLayout(QWidget* parent)
{
    tabWidget = new QTabWidget;

    generalTab = new CommonPrintDialogGeneralTab(parent);
    pageSetupTab = new CommonPrintDialogPageSetupTab(parent);
    optionsTab = new CommonPrintDialogOptionsTab(parent);
    jobsTab = new CommonPrintDialogJobsTab(parent);

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


CommonPrintDialogGeneralTab::CommonPrintDialogGeneralTab(QWidget *parent)
    : QWidget(parent)
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

    setLayout(layout);
}

CommonPrintDialogPageSetupTab::CommonPrintDialogPageSetupTab(QWidget *parent)
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

CommonPrintDialogOptionsTab::CommonPrintDialogOptionsTab(QWidget *parent)
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

CommonPrintDialogJobsTab::CommonPrintDialogJobsTab(QWidget *parent)
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
