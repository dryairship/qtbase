#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <memory>
#include <QtWidgets/QtWidgets>

#include <private/qcpdb_p.h>

class CommonPrintDialogGeneralTab : public QWidget
{
    Q_OBJECT

private:
    QComboBox *m_destinationComboBox;
    QCheckBox *m_remotePrintersCheckBox;
    QComboBox *m_paperSizeComboBox;
    QComboBox *m_pagesComboBox;
    QSpinBox *m_copiesSpinBox;
    QCheckBox *m_collateCheckBox;
    QComboBox *m_orientationComboBox;
    QComboBox *m_colorModeComboBox;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogGeneralTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogPageSetupTab : public QWidget
{
    Q_OBJECT

private:
    QComboBox *m_bothSidesComboBox;
    QComboBox *m_pagesPerSideComboBox;
    QSpinBox *m_scaleSpinBox;
    QComboBox *m_paperSourceComboBox;
    QComboBox *m_pageRangeComboBox;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogPageSetupTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogOptionsTab : public QWidget
{
    Q_OBJECT

private:
    QLineEdit *m_marginTopValue;
    QLineEdit *m_marginBottomValue;
    QLineEdit *m_marginLeftValue;
    QLineEdit *m_marginRightValue;
    QComboBox *m_resolutionComboBox;
    QComboBox *m_qualityComboBox;
    QComboBox *m_outputBinComboBox;
    QComboBox *m_finishingComboBox;
    QComboBox *m_ippAttributeFidelityComboBox;
    QFormLayout *m_layout;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogOptionsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogJobsTab : public QWidget
{
    Q_OBJECT

private:
    QPushButton *m_refreshButton;
    QComboBox *m_startJobComboBox;
    QPushButton *m_saveJobButton;
    QComboBox *m_jobPriorityComboBox;
    QComboBox *m_jobSheetsComboBox;
    QComboBox *m_jobNameComboBox;
    QGridLayout *m_jobsLayout;
    QScrollArea *m_scrollArea;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogJobsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogExtraOptionsTab : public QWidget
{
    Q_OBJECT

private:
    QFormLayout *m_layout;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogExtraOptionsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
    QComboBox *addNewComboBox(QString name);
    void deleteAllComboBoxes();

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogMainLayout : public QHBoxLayout
{
    Q_OBJECT

private:
    QPushButton *m_printButton;
    QPushButton *m_cancelButton;
    
    QTabWidget *m_tabWidget;
    CommonPrintDialogGeneralTab *m_generalTab;
    CommonPrintDialogPageSetupTab *m_pageSetupTab;
    CommonPrintDialogOptionsTab *m_optionsTab;
    CommonPrintDialogJobsTab *m_jobsTab;
    CommonPrintDialogExtraOptionsTab *m_extraOptionsTab;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    void connectSignalsAndSlots();
    void populateComboBox(QComboBox *comboBox, QStringList sizes);

private Q_SLOTS:
    void printerListChanged();
    void newPrinterSelected(int selectedIndex);
    void remotePrintersCheckBoxStateChanged(int state);
    void copiesSpinBoxValueChanged(int value);
    void collateCheckBoxStateChanged(int state);
    void paperSizeComboBoxValueChanged(QString currentText);
    void orientationComboBoxValueChanged(QString currentText);
    void colorModeComboBoxValueChanged(QString currentText);

public:
    explicit CommonPrintDialogMainLayout(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent = nullptr);

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
