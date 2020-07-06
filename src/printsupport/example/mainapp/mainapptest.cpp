#include <QApplication>
#include <QDialog>
#include <QString>
#include <QPrinter>
#include <QPrintDialog>
#include <QAbstractPrintDialog>
#include <QTextDocument>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    QPrinter printer;

    QString html = QString::fromUtf8("<h1>Meow</h1><br><h2>Hello</h2><br><h3>Hola</h3>");

    QPrintDialog dialog(&printer, NULL);
    dialog.setWindowTitle("Print Document");
    if (dialog.exec() == QDialog::Accepted) {
        QTextDocument textDocument;
        textDocument.setHtml(html);
        textDocument.print(&printer);
        cout << "File Printed" << endl;
    } else {
        cout << "File NOT Printed" << endl;
    }
    a.exit();
    return 0;
}