#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <memory>
#include <QtWidgets/QtWidgets>
#include <QtPrintSupport/QtPrintSupport>
#include <QtPrintSupport/private/qcpdb_p.h>
#include <QtPrintSupport/private/qtprintsupportglobal_p.h>

QT_REQUIRE_CONFIG(cpdb);

QT_BEGIN_NAMESPACE

class QCommonPrintDialog;
class CommonPrintDialogBackend;

class CommonPrintDialogGeneralTab : public QWidget
{
    Q_OBJECT

private:
    QTableWidget *m_destinationWidget;
    QCheckBox *m_remotePrintersCheckBox;
    QSpinBox *m_copiesSpinBox;
    QCheckBox *m_collateCheckBox;
    QCheckBox *m_reverseCheckBox;
    QRadioButton *m_rangeAllRadioButton;
    QRadioButton *m_rangeCurrentPageRadioButton;
    QRadioButton *m_rangeSelectionRadioButton;
    QRadioButton *m_rangeCustomRangeRadioButton;
    QLineEdit *m_customRangeLineEdit;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogGeneralTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogPageSetupTab : public QWidget
{
    Q_OBJECT

private:
    QComboBox *m_paperSizeComboBox;
    QComboBox *m_orientationComboBox;
    QComboBox *m_bothSidesComboBox;
    QComboBox *m_pagesPerSideComboBox;
    QComboBox *m_pageOrderingComboBox;
    QComboBox *m_scaleComboBox;
    QComboBox *m_paperSourceComboBox;
    QComboBox *m_pageRangeComboBox;
    QComboBox *m_outputBinComboBox;
    QComboBox *m_onlyPrintComboBox;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogPageSetupTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogOptionsTab : public QWidget
{
    Q_OBJECT

private:
    QDoubleSpinBox *m_marginTopValue;
    QDoubleSpinBox *m_marginBottomValue;
    QDoubleSpinBox *m_marginLeftValue;
    QDoubleSpinBox *m_marginRightValue;
    QComboBox *m_marginUnitComboBox;
    QComboBox *m_resolutionComboBox;
    QComboBox *m_qualityComboBox;
    QComboBox *m_finishingsComboBox;
    QComboBox *m_ippAttributeFidelityComboBox;
    QComboBox *m_colorModeComboBox;
    QFormLayout *m_layout;
    QFormLayout *m_extraOptionsLayout;
    QGroupBox *m_extraOptionsGroupBox;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogOptionsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);
    QComboBox *addNewComboBox(QString name);
    void deleteAllComboBoxes();

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogJobsTab : public QWidget
{
    Q_OBJECT

private:
    QComboBox *m_startJobAtComboBox;
    QComboBox *m_jobPriorityComboBox;
    QComboBox *m_jobSheetsComboBox;
    QLineEdit *m_jobNameLineEdit;
    QRadioButton *m_startJobNowRadioButton;
    QRadioButton *m_startJobAtRadioButton;
    QRadioButton *m_startJobOnHoldRadioButton;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    explicit CommonPrintDialogJobsTab(std::shared_ptr<CommonPrintDialogBackend> backend, QWidget *parent = nullptr);

    friend class CommonPrintDialogMainLayout;
};

class CommonPrintDialogMainLayout : public QHBoxLayout
{
    Q_OBJECT

private:
    QCommonPrintDialog *m_commonPrintDialog;
    QPushButton *m_printButton;
    QPushButton *m_cancelButton;
    
    QTabWidget *m_tabWidget;
    CommonPrintDialogGeneralTab *m_generalTab;
    CommonPrintDialogPageSetupTab *m_pageSetupTab;
    CommonPrintDialogOptionsTab *m_optionsTab;
    CommonPrintDialogJobsTab *m_jobsTab;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    void connectSignalsAndSlots();
    void connectComboBoxSignal(QComboBox* comboBox);
    void connectRangeRadioButtonSignal(QRadioButton* radioButton);
    void connectStartJobAtRadioButtonSignal(QRadioButton* radioButton);
    void updateComboBox(QComboBox *comboBox, QMap<QString, QStringList> options, QSet<QString>* usedKeys);

private Q_SLOTS:
    void printerListChanged();
    void newPrinterSelected(int row);
    void remotePrintersCheckBoxStateChanged(int state);
    void startJobAtRadioButtonChanged(bool checked);
    void comboBoxValueChanged(QString currentText);
    void paperSizeComboBoxValueChanged(QString currentText);
    void applySettingsAndAccept();

public:
    explicit CommonPrintDialogMainLayout(QCommonPrintDialog *commonPrintDialog,
        std::shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent = nullptr);

};

class QCommonPrintDialog : public QDialog
{
    Q_OBJECT
private:
    CommonPrintDialogMainLayout *m_mainLayout;
    QPrinter *m_printer;
    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    friend class CommonPrintDialogMainLayout;
public:
    explicit QCommonPrintDialog(QPrinter *printer = nullptr, QWidget *parent = nullptr);
};

QT_END_NAMESPACE

#endif // QCOMMONPRINTDIALOG_H
