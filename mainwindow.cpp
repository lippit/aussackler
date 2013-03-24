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

#include <QMessageBox>
#include <QFileDialog>
#include <QDomDocument>
#include "mainwindow.h"
#include "entry.h"
#include "invest.h"

ASMainWindow::ASMainWindow(ASTransactionList * transactions,
                           QWidget * parent) :
    QMainWindow(parent),
    entryWindow(NULL),
    accountDialog(NULL),
    categoryDialog(NULL),
    m_transactions(transactions),
    m_entryModel(NULL),
    m_docModel(NULL),
    m_populate(true)
{
    ui.setupUi(this);

    docDialog = new QDialog(this);
    docUi.setupUi(docDialog);
    docUi.tableView->setModel(m_docModel);

    m_fileFilter = tr(
        "Aussackler XML Dateien (*.ass);;"
        "XML Dateien (*.xml);;"
        "Alle Dateien (*)"
        );

    on_actionNew_activated();
}

void ASMainWindow::connectModels()
{
    m_entryModel->transactionsChanged();
    m_docModel->transactionsChanged();

    connect(m_transactions, SIGNAL(newTransaction()),
            m_entryModel, SLOT(transactionsChanged()));
    connect(m_transactions, SIGNAL(newTransaction()),
            m_docModel, SLOT(transactionsChanged()));
}

void ASMainWindow::disconnectModels()
{
    if (m_entryModel)
    {
        disconnect(m_transactions, SIGNAL(newTransaction()),
                   m_entryModel, SLOT(transactionsChanged()));
    }
    if (m_docModel)
    {
        disconnect(m_transactions, SIGNAL(newTransaction()),
                   m_docModel, SLOT(transactionsChanged()));
    }
}

void ASMainWindow::on_actionNew_activated()
{
    disconnectModels();

    for (int i = m_transactions->size() - 1; i >= 0; --i)
    {
        delete m_transactions->at(i);
    }
    m_transactions->clear();

    // Model setup

    ui.mainTable->setModel(NULL);
    ui.earningsTable->setModel(NULL);
    ui.investTable->setModel(NULL);

    delete m_entryModel;
    delete m_docModel;
    //delete m_earningsModel;
    //delete m_investModel;

    m_entryModel = new EntryModel(m_transactions);
    m_docModel = new DocModel(m_transactions);

    ui.mainTable->setModel(m_entryModel);
    docUi.tableView->setModel(m_docModel);

    connectModels();

    m_populate = true;
}

void ASMainWindow::createEntryWindow()
{
    if (!entryWindow)
    {
        entryWindow = new ASEntryWindow(m_transactions, &docUi,
                                        docDialog, this);
    }
    if (m_populate)
    {
        entryWindow->populateLists();
        m_populate = false;
    }
}

void ASMainWindow::on_actionAddEntries_activated()
{
    createEntryWindow();

    while(entryWindow->exec());
}

void ASMainWindow::on_actionNewEntry_activated()
{
    createEntryWindow();

    entryWindow->exec();
}

void ASMainWindow::on_actionNewAccount_activated()
{
    if (!accountDialog)
    {
        accountDialog = new QDialog(this);
        accountUi.setupUi(accountDialog);
        connect(accountDialog, SIGNAL(accepted()),
                this, SLOT(createAccount()));
    }
    accountUi.replaces->clear();
    accountUi.replaces->addItem("-", 0);
    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASAccount * a = dynamic_cast<ASAccount*>(*it);
        if (a)
        {
            accountUi.replaces->addItem(a->getDescription(),
                                        QVariant::fromValue((void*)a));
        }
    }
    accountDialog->show();
}

void ASMainWindow::on_actionNewCategory_activated()
{
    if (!categoryDialog)
    {
        categoryDialog = new QDialog(this);
        categoryUi.setupUi(categoryDialog);
        categoryUi.replaces->addItem("-", 0);
        connect(categoryDialog, SIGNAL(accepted()),
                this, SLOT(createCategory()));
    }
    categoryUi.replaces->clear();
    categoryUi.replaces->addItem("-", 0);
    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASCategory * c = dynamic_cast<ASCategory*>(*it);
        if (c)
        {
            categoryUi.replaces->addItem(c->getDescription(),
                                         QVariant::fromValue((void*)c));
        }
    }
    categoryDialog->show();
}

void ASMainWindow::on_actionSave_activated()
{
    if (m_currentFileName.isEmpty())
    {
        on_actionSaveas_activated();
        return;
    }

    QDomDocument doc("aussackler");
    QDomElement root = doc.createElement("aussackler");
    doc.appendChild(root);
    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for(; it != m_transactions->constEnd(); ++it)
    {
        QDomElement de = doc.createElement("transaction");
        (*it)->writeToXml(&doc, &de);
        root.appendChild(de);
    }

    QFile file(m_currentFileName);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(doc.toByteArray(4));
    file.close();
}

void ASMainWindow::on_actionSaveas_activated()
{
    m_currentFileName= QFileDialog::getSaveFileName(this,
                                                    tr("Speichern unter"),
                                                    QString(), m_fileFilter);

    if (!m_currentFileName.isEmpty())
    {
        on_actionSave_activated();
    }
}

void ASMainWindow::on_actionLoad_activated()
{
    m_currentFileName= QFileDialog::getOpenFileName(this,
                                                    tr("Datei laden"),
                                                    QString(), m_fileFilter);

    if (m_currentFileName.isEmpty())
        return;

    QDomDocument doc("aussackler");
    QFile file(m_currentFileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    on_actionNew_activated();

    QDomElement docElem = doc.documentElement();

    disconnectModels();

    QDomNode n = docElem.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull() && e.tagName() == "transaction")
        {
            if (!e.hasAttribute("type"))
                continue;
            ASTransaction::TransactionType type =
                (ASTransaction::TransactionType)
                ASTransaction::TransactionTypeStrings.
                indexOf(e.attribute("type"));
            ASTransaction * a = NULL;
            switch(type)
            {
            case ASTransaction::TRANSACTION_TYPE_BASE:
                a = new ASTransaction(m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_CATEGORY:
                a = new ASCategory(m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_ACCOUNT:
                a = new ASAccount(m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_ACCOUNTENTRY:
                a = new ASAccountEntry(m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_INVESTMENT:
                a = new ASInvestEntry(m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_DOCUMENT:
                a = new ASDocument(m_transactions);
                break;
            }
            if (a)
            {
                a->readFromXml(&e);
                a->commit();
            }
        }
        n = n.nextSibling();
    }

    connectModels();

    m_populate = true;
}

void ASMainWindow::on_actionQuit_activated()
{
    close();
}

void ASMainWindow::createAccount()
{
    QString desc = accountUi.description->text();

    if (desc.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Fehler"),
                              tr("Beschreibung erforderlich!"));
        return;
    }

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASAccount * a = dynamic_cast<ASAccount*>(*it);
        if (a && a->getDescription() == desc)
        {
            QMessageBox::critical(this,
                                  tr("Fehler"),
                                  tr("Beschreibung bereits vorhanden!"));
            return;
        }
    }

    ASAccount * a = new ASAccount(m_transactions,
                                  (ASAccount*)accountUi.replaces->itemData(
                                      accountUi.replaces->currentIndex()).
                                  value<void *>());
    a->setDescription(desc);
    a->commit();

    m_populate = true;
}

void ASMainWindow::createCategory()
{
    QString desc = categoryUi.description->text();

    if (desc.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Fehler"),
                              tr("Beschreibung erforderlich!"));
        return;
    }

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASCategory * c = dynamic_cast<ASCategory*>(*it);
        if (c && c->getDescription() == desc)
        {
            QMessageBox::critical(this,
                                  tr("Fehler"),
                                  tr("Beschreibung bereits vorhanden!"));
            return;
        }
    }

    ASCategory * c = new ASCategory(m_transactions,
                                    (ASCategory*)categoryUi.replaces->itemData(
                                        categoryUi.replaces->currentIndex()).
                                    value<void *>());
    c->setDescription(desc);
    c->commit();

    m_populate = true;
}
