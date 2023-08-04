#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include "testdlg.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    auto data = ListTypes::Data { "Animals",
        {
            { "This is an attribute" },
            { "...and this is another" },
            { "Something about this" },
            { "Additional data" },
            { "You get the picture by now" }
        },
        {
            { "Dog", { 1, 1, 1, 1 } },
            { "Cat", { 2, 2, 2, 2 } },
            { "Cow", { 3, 3, 3, 3 } },
            { "Pig", { 100, 200, 300, 400 } }
        }
    };

    m_pTest = new TestDlg(data, this);
    m_pTest->connect(m_pTest, &TestDlg::accepted, this, &MainWindow::testAccepted);
    m_pTest->open();
}

void MainWindow::testAccepted()
{
    auto data = m_pTest->getData();

    // whatever
    QString msg = "Test dialog returned.\n\nThe following items are in the data structure:\n\n\n";

    for (const auto& item: data.items)
    {
        msg += QString("\n%1").arg(item.description.c_str());
        if (item.deleted) msg += " (deleted)";
    }

    QMessageBox::information(this, "", msg);
}

