#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <memory>
#include <QtWidgets/QtWidgets>

#include <private/qcpdb_p.h>

using namespace std;

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

    shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogGeneralTab(shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogPageSetupTab(shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogOptionsTab(shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogJobsTab(shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    shared_ptr<CommonPrintDialogBackend> backend;
    
    explicit CommonPrintDialogMainLayout(shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent = nullptr);
    void connectSignalsAndSlots();
};

class QCommonPrintDialog : public QDialog
{
    Q_OBJECT
private:
    CommonPrintDialogMainLayout *mainLayout;
    shared_ptr<CommonPrintDialogBackend> backend;

public:
    explicit QCommonPrintDialog(QWidget *parent = nullptr);
    ~QCommonPrintDialog();
};

#endif // QCOMMONPRINTDIALOG_H
