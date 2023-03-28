//
// Created by dmonnie5 on 22/03/23.
//
#include <QDate>
#include "mainwindow.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>

using namespace QtCharts;

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
    setMinimumSize(800, 600);

    connect(tabWidget, &QTabWidget::currentChanged, [this](int index) {
        if (index == 1) {
            updatePieChart();
        }
    });
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

    cboCategory->addItem("Nouveau...");
    cboCategory->addItem("Nourriture");
    cboCategory->addItem("Maison");
    cboCategory->addItem("Transport");
    cboCategory->addItem("Loisir");
    cboCategory->addItem("Autre");

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

    // Connect the add button to a slot that will check if the selected category is "Nouveau..."
// and show a dialog to add a new category
    connect(btnAddExpense, &QPushButton::clicked, [this]() {
        if (cboCategory->currentText() == "Nouveau...") {
            bool ok;
            QString newCategory = QInputDialog::getText(this, tr("Ajouter une catégorie"),
                                                        tr("Nom de la nouvelle catégorie à ajouter:"), QLineEdit::Normal,
                                                        "", &ok);
            if (ok && !newCategory.isEmpty()) {
                cboCategory->addItem(newCategory);
                cboCategory->setCurrentText(newCategory);
            }
        } else {
            onAddExpenseClicked();
        }
    });

}

//void MainWindow::setupStatisticsTab()
//{
//    tabStatistics = new QWidget();
//    vbxStatistics = new QVBoxLayout();
//    QLabel *lblStatistics = new QLabel("Statistiques");
//    vbxStatistics->addWidget(lblStatistics);

//    // Add a table to display the statistics
//    QTableWidget *tblStatistics = new QTableWidget();
//    tblStatistics->setColumnCount(2);
//    QStringList headers;
//    headers << "Categorie" << "Total";
//    tblStatistics->setHorizontalHeaderLabels(headers);
//    vbxStatistics->addWidget(tblStatistics);

//    // Calculate the total expenses for each category and add them to the table
//    QMap<QString, int> totals;
//    for (int i = 0; i < tblExpenses->rowCount(); i++) {
//        QString category = tblExpenses->item(i, 3)->text();
//        int price = tblExpenses->item(i, 1)->text().toInt();
//        totals[category] += price;
//    }
//    int row = 0;
//    for (auto it = totals.begin(); it != totals.end(); ++it) {
//        if (it.value() > 0) {
//            tblStatistics->insertRow(row);
//            tblStatistics->setItem(row, 0, new QTableWidgetItem(it.key()));
//            tblStatistics->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
//            row++;
//        }
//    }

//    tabStatistics->setLayout(vbxStatistics);
//}

void MainWindow::setupStatisticsTab()
{
    tabStatistics = new QWidget();
    vbxStatistics = new QVBoxLayout();

    // Créer le graphique à secteurs
    QPieSeries *series = new QPieSeries();
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Répartition des dépenses par catégorie"));
    chart->legend()->setVisible(true);

    // Créer la vue du graphique et l'ajouter au layout
//    QChartView *chartView = new QChartView(chart);
//    chartView->setRenderHint(QPainter::Antialiasing);
//    vbxStatistics->addWidget(chartView);
//    tabStatistics->setLayout(vbxStatistics);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    vbxStatistics->addWidget(chartView);

    tblStatistics = new QTableWidget();
    tblStatistics->setColumnCount(2);
    QStringList headers;
    headers << "Categorie" << "Total";
    tblStatistics->setHorizontalHeaderLabels(headers);
    vbxStatistics->addWidget(tblStatistics);

    tabStatistics->setLayout(vbxStatistics);
}

void MainWindow::updatePieChart()
{
    QChartView *chartView = qobject_cast<QChartView*>(tabStatistics->layout()->itemAt(0)->widget());
    QChart *chart = chartView->chart();
    QPieSeries *series = qobject_cast<QPieSeries*>(chart->series().at(0));
    series->clear();

    QMap<QString, int> totals;

    // Calculez les totaux pour chaque catégorie
    for (int i = 0; i < tblExpenses->rowCount(); i++) {
        QString category = tblExpenses->item(i, 3)->text();
        int price = tblExpenses->item(i, 1)->text().toInt();
        totals[category] += price;
    }

    // Ajoutez les totaux au graphique à secteurs
    for (auto it = totals.begin(); it != totals.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice *slice = new QPieSlice(it.key(), it.value());
            series->append(slice);
        }
    }
}



void MainWindow::updateStatisticsTab() {
    QMap<QString, int> totals;

    // Iterate over each expense and update the totals map
    for (int i = 0; i < tblExpenses->rowCount(); i++) {
        QString category = tblExpenses->item(i, 3)->text();
        int price = tblExpenses->item(i, 1)->text().toInt();
        totals[category] += price;
    }

    // Clear the statistics table
    QTableWidget *tblStatistics = qobject_cast<QTableWidget*>(tabStatistics->layout()->itemAt(1)->widget());
    tblStatistics->setRowCount(0);

    // Add each category total to the statistics table
    int row = 0;
    for (auto it = totals.begin(); it != totals.end(); ++it) {
        if (it.value() > 0) {
            tblStatistics->insertRow(row);
            tblStatistics->setItem(row, 0, new QTableWidgetItem(it.key()));
            tblStatistics->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
            row++;
        }
    }
}

//void MainWindow::onAddExpenseClicked()
//{
//    // Get the input values
//    QString name = txtName->text();
//    int price = spnPrice->value();
//    QString category = cboCategory->currentText();
//    QDate date = QDate::currentDate();

//    // Create a new row and add it to the table
//    int row = tblExpenses->rowCount();
//    tblExpenses->insertRow(row);
//    tblExpenses->setItem(row, 0, new QTableWidgetItem(name));
//    tblExpenses->setItem(row, 1, new QTableWidgetItem(QString::number(price)));
//    tblExpenses->setItem(row, 2, new QTableWidgetItem(date.toString("dd/MM/yyyy")));
//    tblExpenses->setItem(row, 3, new QTableWidgetItem(category));

//    updateStatisticsTab();
//}

void MainWindow::onAddExpenseClicked()
{
    // Get the input values
    QString name = txtName->text();
    int price = spnPrice->value();
    QString category = cboCategory->currentText();
    QDate date = QDate::currentDate();

    // Create a new row and add it to the table
    int row = tblExpenses->rowCount();
    tblExpenses->insertRow(row);
    tblExpenses->setItem(row, 0, new QTableWidgetItem(name));
    tblExpenses->setItem(row, 1, new QTableWidgetItem(QString::number(price)));
    tblExpenses->setItem(row, 2, new QTableWidgetItem(date.toString("dd/MM/yyyy")));
    tblExpenses->setItem(row, 3, new QTableWidgetItem(category));

    updateStatisticsTab();
    updatePieChart();
}

