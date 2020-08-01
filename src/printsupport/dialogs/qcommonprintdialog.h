#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

#include <memory>
#include <QtWidgets/QtWidgets>
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
    QComboBox *m_pagesComboBox;
    QSpinBox *m_copiesSpinBox;
    QCheckBox *m_collateCheckBox;
    QCheckBox *m_reverseCheckBox;

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
    QComboBox *m_scaleComboBox;
    QComboBox *m_paperSourceComboBox;
    QComboBox *m_pageRangeComboBox;
    QComboBox *m_outputBinComboBox;

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
    QComboBox *m_finishingsComboBox;
    QComboBox *m_ippAttributeFidelityComboBox;
    QComboBox *m_colorModeComboBox;
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

private Q_SLOTS:
    void extraOptionsComboBoxValueChanged(QString currentText);

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
    CommonPrintDialogExtraOptionsTab *m_extraOptionsTab;

    std::shared_ptr<CommonPrintDialogBackend> m_backend;

    void connectSignalsAndSlots();
    void connectComboBoxSignal(QComboBox* comboBox);
    void updateComboBox(QComboBox *comboBox, QMap<QString, QStringList> options, QSet<QString>* usedKeys);

private Q_SLOTS:
    void printerListChanged();
    void newPrinterSelected(int row);
    void remotePrintersCheckBoxStateChanged(int state);
    void copiesSpinBoxValueChanged(int value);
    void collateCheckBoxStateChanged(int state);
    void reverseCheckBoxStateChanged(int state);
    void comboBoxValueChanged(QString currentText);

public:
    explicit CommonPrintDialogMainLayout(QCommonPrintDialog *commonPrintDialog,
        std::shared_ptr<CommonPrintDialogBackend> backend, QWidget* parent = nullptr);

};

class QCommonPrintDialog : public QDialog
{
    Q_OBJECT
private:
    CommonPrintDialogMainLayout *m_mainLayout;
    std::shared_ptr<CommonPrintDialogBackend> m_backend;

public:
    explicit QCommonPrintDialog(QWidget *parent = nullptr);
};

QT_END_NAMESPACE

#endif // QCOMMONPRINTDIALOG_H
