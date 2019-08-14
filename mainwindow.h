/*
 * Copyright (c) 2013 Georg Lippitsch
 *
 * This file is part of Aussackler.
 *
 * Aussackler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aussackler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aussackler.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSortFilterProxyModel>
#include "ui_mainwindow.h"
#include "ui_account.h"
#include "ui_document.h"
#include "ui_category.h"
#include "ui_doclist.h"
#include "ui_settings.h"
#include "ui_vatcategory.h"
#include "entrywindow.h"
#include "transaction.h"
#include "entrymodel.h"
#include "assetsmodel.h"
#include "docmodel.h"
#include "calc.h"
#include "vat.h"

class ASMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ASMainWindow(ASTransactionList * transactions,
                 QWidget * parent = 0);

signals:
    void signalLoad(const QString fileName);
    void signalSave(const QString fileName);

private slots:
    void on_actionNew_triggered();
    void on_actionLoad_triggered();
    void on_actionSave_triggered();
    void on_actionSaveas_triggered();
    void on_actionQuit_triggered();

    void on_actionAddEntries_triggered();
    void on_actionNewEntry_triggered();
    void on_actionCorrectEntry_triggered();
    void on_actionDeleteEntry_triggered();
    void on_actionNewDocument_triggered();
    void on_actionCorrectDocument_triggered();
    void on_actionNewAccount_triggered();
    void on_actionNewCategory_triggered();
    void on_actionNewVatCategory_triggered();
    void on_actionSettings_triggered();

    void on_tabWidget_currentChanged(int index);

    void createAccount();
    void createDocument();
    void createCategory();
    void createVatCategory();

private:
    QString m_currentFileName;
    Ui::MainWindow ui;
    Ui::AccountWindow accountUi;
    Ui::DocumentWindow editDocUi;
    Ui::CategoryWindow categoryUi;
    Ui::VatCategoryWindow vatCategoryUi;
    Ui::DocumentsWindow docUi;
    Ui::SettingsWindow settingsUi;
    ASEntryWindow * entryWindow;
    QDialog * accountDialog;
    QDialog * editDocDialog;
    QDialog * categoryDialog;
    QDialog * vatCategoryDialog;
    QDialog * docDialog;
    QDialog * settingsDialog;
    ASTransactionList * m_transactions;
    EntryModel * m_entryModel;
    QSortFilterProxyModel * m_proxyModel;
    DocModel * m_docModel;
    AssetsModel * m_assetsModel;
    ASCalc * m_calc;
    ASVat * m_vat;
    ASDocument * m_docOverride;
    QString m_fileFilter;
    bool m_populate;

    void createEntryWindow();

    void connectModels();
    void disconnectModels();
};

#endif
