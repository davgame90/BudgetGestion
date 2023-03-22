//
// Created by dmonnie5 on 22/03/23.
//
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    tabWidget = new QTabWidget(this);

    setupExpensesTab();
    tabWidget->addTab(tabExpenses, "Dépenses");

    setupStatisticsTab();
    tabWidget->addTab(tabStatistics, "Statistiques");

    setCentralWidget(tabWidget);
    setWindowTitle("Gestion de budget");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupExpensesTab()
{
    // Create the widgets for the Expenses tab
    tabExpenses = new QWidget();
    lblName = new QLabel("Nom:");
    lblPrice = new QLabel("Prix:");
    lblDate = new QLabel("Date:");
    lblCategory = new QLabel("Categorie:");
    txtName = new QLineEdit();
    spnPrice = new QSpinBox();
    cboCategory = new QComboBox();
    tblExpenses = new QTableWidget();
    btnAddExpense = new QPushButton("Ajouter");
    hbxExpenses = new QHBoxLayout();
    vbxExpenses = new QVBoxLayout();

    cboCategory->addItem("Nourriture");
    cboCategory->addItem("Maison");
    cboCategory->addItem("Transport");
    cboCategory->addItem("Loisir");

    tblExpenses->setColumnCount(4);
    QStringList headers;
    headers << "Nom" << "Prix" << "Date" << "Categorie";
    tblExpenses->setHorizontalHeaderLabels(headers);

    hbxExpenses->addWidget(lblName);
    hbxExpenses->addWidget(txtName);
    hbxExpenses->addWidget(lblPrice);
    hbxExpenses->addWidget(spnPrice);
    hbxExpenses->addWidget(lblCategory);
    hbxExpenses->addWidget(cboCategory);
    hbxExpenses->addWidget(btnAddExpense);
    vbxExpenses->addLayout(hbxExpenses);
    vbxExpenses->addWidget(tblExpenses);
    tabExpenses->setLayout(vbxExpenses);
}

void MainWindow::setupStatisticsTab()
{
    tabStatistics = new QWidget();
    vbxStatistics = new QVBoxLayout();
    QLabel *lblStatistics = new QLabel("Statistiques");
    vbxStatistics->addWidget(lblStatistics);
    tabStatistics->setLayout(vbxStatistics);
}
