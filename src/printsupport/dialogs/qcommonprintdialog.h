#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <QtWidgets/QtWidgets>

#include <private/qcpdb_p.h>

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

    CommonPrintDialogBackend *backend;

    explicit CommonPrintDialogGeneralTab(CommonPrintDialogBackend *backend, QWidget *parent = nullptr);
    void populatePaperSizeComboBox(QStringList sizes);
    void populateComboBox(QComboBox *comboBox, QStringList sizes);

public Q_SLOTS:
    void printerListChanged();
    void newPrinterSelected(int selectedIndex);
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

    CommonPrintDialogBackend *backend;

    explicit CommonPrintDialogPageSetupTab(CommonPrintDialogBackend *backend, QWidget *parent = nullptr);
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

    CommonPrintDialogBackend *backend;

    explicit CommonPrintDialogOptionsTab(CommonPrintDialogBackend *backend, QWidget *parent = nullptr);
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

    CommonPrintDialogBackend *backend;

    explicit CommonPrintDialogJobsTab(CommonPrintDialogBackend *backend, QWidget *parent = nullptr);
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

    CommonPrintDialogBackend *backend;
    
    explicit CommonPrintDialogMainLayout(CommonPrintDialogBackend *backend, QWidget* parent = nullptr);
    void connectSignalsAndSlots();
};

class QCommonPrintDialog : public QDialog
{
    Q_OBJECT
private:
    CommonPrintDialogMainLayout *mainLayout;
    CommonPrintDialogBackend *backend;

public:
    explicit QCommonPrintDialog(QWidget *parent = nullptr);
    ~QCommonPrintDialog();
};

#endif // QCOMMONPRINTDIALOG_H
