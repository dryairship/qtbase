#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <QtWidgets/QtWidgets>

class CommonPrintDialogGeneralTab : public QWidget
{
    Q_OBJECT

public:
    QComboBox *destinationComboBox;
    QCheckBox *remotePrintersCheckBox;
    QComboBox *paperComboBox;
    QComboBox *pagesComboBox;
    QSpinBox *copiesSpinBox;
    QCheckBox *collateCheckBox;
    QComboBox *orientationComboBox;
    QComboBox *colorModeComboBox;

    explicit CommonPrintDialogGeneralTab(QWidget *parent = nullptr);
};

class CommonPrintDialogPageSetupTab : public QWidget
{
    Q_OBJECT

public:
    QComboBox *bothSidesComboBox;
    QComboBox *pagesPerSideComboBox;
    QComboBox *onlyPrintComboBox;
    QSpinBox *scaleSpinBox;
    QComboBox *paperSourceComboBox;
    QComboBox *pageRangeComboBox;

    explicit CommonPrintDialogPageSetupTab(QWidget *parent = nullptr);
};

class CommonPrintDialogOptionsTab : public QWidget
{
    Q_OBJECT

public:
    QLineEdit *marginTopValue;
    QLineEdit *marginBottomValue;
    QLineEdit *marginLeftValue;
    QLineEdit *marginRightValue;
    QComboBox *resolutionComboBox;
    QComboBox *qualityComboBox;
    QComboBox *outputBinComboBox;
    QComboBox *finishingsComboBox;
    QComboBox *ippAttributeFidelityComboBox;
    QFormLayout *layout;

    explicit CommonPrintDialogOptionsTab(QWidget *parent = nullptr);
};

class CommonPrintDialogJobsTab : public QWidget
{
    Q_OBJECT

public:
    QPushButton *refreshButton;
    QComboBox *startJobComboBox;
    QPushButton *saveJobButton;
    QComboBox *jobPriorityComboBox;
    QComboBox *jobSheetsComboBox;
    QComboBox *jobNameComboBox;
    QGridLayout *jobsLayout;
    QScrollArea *scrollArea;

    explicit CommonPrintDialogJobsTab(QWidget *parent = nullptr);
};

class CommonPrintDialogMainLayout : public QHBoxLayout
{
    Q_OBJECT

public:
    QPushButton *printButton;
    QPushButton *cancelButton;
    
    QTabWidget *tabWidget;
    CommonPrintDialogGeneralTab *generalTab;
    CommonPrintDialogPageSetupTab *pageSetupTab;
    CommonPrintDialogOptionsTab *optionsTab;
    CommonPrintDialogJobsTab *jobsTab;
    
    explicit CommonPrintDialogMainLayout(QWidget* parent = nullptr);
};

class QCommonPrintDialog : public QDialog
{
    Q_OBJECT
private:
    CommonPrintDialogMainLayout *mainLayout;

public:
    explicit QCommonPrintDialog(QWidget *parent = nullptr);
    ~QCommonPrintDialog();
};

#endif // QCOMMONPRINTDIALOG_H
