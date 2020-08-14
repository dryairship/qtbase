/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCOMMONPRINTDIALOG_H
#define QCOMMONPRINTDIALOG_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// to version without notice, or even be removed.
//
// We mean it.
//
//

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
