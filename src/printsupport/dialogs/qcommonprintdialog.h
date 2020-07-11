#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <memory>
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

    std::shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogGeneralTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    std::shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogPageSetupTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    std::shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogOptionsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
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

    std::shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogJobsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
};

class CommonPrintDialogExtraOptionsTab : public QWidget
{
    Q_OBJECT

public:
    QFormLayout *layout;

    std::shared_ptr<CommonPrintDialogBackend> backend;

    explicit CommonPrintDialogExtraOptionsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
    QComboBox *addNewComboBox(QString name);
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
    CommonPrintDialogExtraOptionsTab *extraOptionsTab;

    std::shared_ptr<CommonPrintDialogBackend> backend;
    
    explicit CommonPrintDialogMainLayout(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent = nullptr);
    void connectSignalsAndSlots();
    void populateComboBox(QComboBox *comboBox, QStringList sizes);

public Q_SLOTS:
    void printerListChanged();
    void newPrinterSelected(int selectedIndex);
};

class QCommonPrintDialog : public QDialog
{
    Q_OBJECT
private:
    CommonPrintDialogMainLayout *m_mainLayout;
    std::shared_ptr<CommonPrintDialogBackend> m_backend;

public:
    explicit QCommonPrintDialog(QWidget *parent = nullptr);
    ~QCommonPrintDialog();
};

#endif // QCOMMONPRINTDIALOG_H
