#include "testdlg.h"
#include <QMessageBox>

TestDlg::TestDlg(const ListTypes::Data& data, QWidget *parent, Qt::WindowFlags f) : ListDialog(data, parent, f)
{
}

bool TestDlg::validate()
{
    auto item = getForm();

    // example of validation...

    if (item.description.empty())
    {
        QMessageBox::warning(this, "Validation checks", "Description cannot be empty.");
        this->m_pDescription->setFocus();
        return false;
    }

    return true;
}
