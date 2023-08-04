#ifndef TESTDLG_H
#define TESTDLG_H

#include "listdialog.h"
#include <QObject>

class TestDlg : public ListDialog
{
    Q_OBJECT
public:
    TestDlg(const ListTypes::Data& data, QWidget *parent, Qt::WindowFlags f = Qt::WindowFlags());

protected:
    virtual bool validate() override;
};

#endif // TESTDLG_H
