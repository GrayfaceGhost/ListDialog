#include "listdialog.h"
#include <QBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDebug>
#include <QMessageBox>
#include <algorithm>

using namespace ListTypes;

ListDialog::ListDialog(const Data& data, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    m_Data(data), // copy
    m_initialised(false),
    m_formDirty(false),
    m_listChanged(false),
    m_adding(false)
{
    QMetaObject::invokeMethod(this, &ListDialog::init, Qt::QueuedConnection);
}

void ListDialog::add_item()
{
    handleDirtyForm(m_pList->count() - 1);

    m_adding = true;

    clearForm();
    enableForm(true);

    m_pList->addItem("");
    m_pList->setCurrentRow(m_pList->count() - 1);
    m_pDescription->setFocus();
}

void ListDialog::delete_item()
{
    auto row = m_pList->currentRow();
    if (row != -1)
    {
        const auto item = m_itemAtRow[row];
        m_Data.items[item].deleted = true;
        m_pList->takeItem(row);

        auto& v(m_itemAtRow);
        v.erase(std::remove_if(begin(v), end(v), [=](int i) { return i == item; }), end(v));

        m_pList->setCurrentRow(std::min(row, m_pList->count() - 1));

        m_listChanged = true;
    }
}

void ListDialog::apply()
{
    if (!validate()) return;

    auto new_item = getForm();

    if (m_adding)
    {
        applyNew(new_item);
    }
    else
    {
        applyEdit(new_item, m_pList->currentRow());
    }
}

void ListDialog::reset()
{
    if (m_adding)
    {
        clearForm();
    }
    else
    {
        fillForm();
    }
    m_formDirty = false;
    enableFormButtons();
}

void ListDialog::populateList()
{
    m_pList->clear();
    m_itemAtRow.clear();

    for (auto i = 0; i < m_Data.items.size(); ++i)
    {
        const auto& item = m_Data.items[i];
        if (!item.deleted)
        {
            m_pList->addItem(item.description.c_str());
            m_itemAtRow.push_back(i);
        }
    }

    if (m_Data.items.size() > 0)
    {
        m_pList->setCurrentRow(0);
    }
    else
    {
        listRowChanged(-1);
    }
}

void ListDialog::clearForm()
{
    m_pDescription->setText("");
    for (auto& ed: m_vEdits) { ed->setText(""); }
}

void ListDialog::fillForm()
{
    auto row = m_pList->currentRow();
    if (row == -1)
    {
        clearForm();
    }
    else
    {
        const auto& item = m_Data.items[m_itemAtRow[row]];

        m_pDescription->setText(item.description.c_str());

        for (auto i = 0; i < item.values.size(); ++i)
        {
            if (i >= m_vEdits.size()) break;
            m_vEdits[i]->setText(QString("%1").arg(item.values[i]));
        }
    }
}

void ListDialog::enableForm(bool enable)
{
    m_pDescription->setEnabled(enable);
    m_pDescLabel->setEnabled(enable);

    for (auto& ed: m_vEdits) { ed->setEnabled(enable); }
    for (auto& la: m_vLabels) { la->setEnabled(enable); }

    enableFormButtons();
}

void ListDialog::enableFormButtons()
{
    m_pApply->setEnabled(m_formDirty);
    m_pReset->setEnabled(m_formDirty);
}

void ListDialog::applyNew(const Item &item)
{
    m_pList->item(m_pList->count() - 1)->setText(item.description.c_str());
    m_Data.items.push_back(item);
    m_itemAtRow.push_back(m_Data.items.size() - 1);

    m_adding = false;

    m_listChanged = true;
    m_formDirty = false;
    enableFormButtons();
}

void ListDialog::applyEdit(const Item &item, int row)
{
    m_pList->item(row)->setText(item.description.c_str());
    m_Data.items[m_itemAtRow[row]] = item;

    m_listChanged = true;
    m_formDirty = false;
    enableFormButtons();
}

Item ListDialog::getForm()
{
    Item item(m_pDescription->text().toStdString());

    for (const auto& edit: m_vEdits)
    {
        item.values.push_back(edit->text().toDouble());
    }

    return item;
}

void ListDialog::init()
{
    if (m_initialised) return;
    m_initialised = true;

    auto mainLayout = new QGridLayout();
    mainLayout->setSpacing(16);

    mainLayout->addWidget(titleElement(), 0, 0);
    mainLayout->addLayout(topLeftLayout(), 1, 0);
    mainLayout->addLayout(topRightLayout(), 1, 1);
    mainLayout->addWidget(line(QFrame::HLine), 2, 0, 1, 2);
    mainLayout->addWidget(dialogButtonBox(), 3, 0, 1, 2);

    setLayout(mainLayout);

    setWindowTitle(m_Data.title.c_str());
    populateList();
}

QFrame* ListDialog::line(QFrame::Shape shape)
{
    auto line = new QFrame();

    line->setFrameShape(shape);
    line->setFrameShadow(QFrame::Sunken);

    return line;
}

QListWidget* ListDialog::listElement()
{
    m_pList = new QListWidget(this);

    m_pList->setMinimumHeight(200);
    connect(m_pList, &QListWidget::currentRowChanged, this, &ListDialog::listRowChanged);
    connect(m_pList, &QListWidget::currentItemChanged, this, &ListDialog::listItemChanged);

    return m_pList;
}

QLabel* ListDialog::titleElement()
{
    auto title = new QLabel(("<h1>" + m_Data.title + "</h1>").c_str());
    title->setTextFormat(Qt::RichText);

    return title;
}

QDialogButtonBox* ListDialog::dialogButtonBox()
{
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ListDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    return buttonBox;
}

QGridLayout* ListDialog::topLeftLayout()
{
    // item list with add and delete buttons below it, and a separator line on the right

    auto top_left = new QGridLayout();

    top_left->addWidget(listElement(), 0, 0);
    top_left->addLayout(listButtonsLayout(), 1, 0);
    top_left->addWidget(line(QFrame::VLine), 0, 2, 2, 1);

    return top_left;
}

QHBoxLayout* ListDialog::listButtonsLayout()
{
    auto hb1 = new QHBoxLayout();

    hb1->addWidget(m_pAdd = new QPushButton(tr("Add"), this));
    hb1->addWidget(m_pDelete = new QPushButton(tr("Delete"), this));
    hb1->addStretch(1);

    connect(m_pAdd, &QPushButton::clicked, this, &ListDialog::add_item);
    connect(m_pDelete, &QPushButton::clicked, this, &ListDialog::delete_item);

    return hb1;
}

QVBoxLayout* ListDialog::topRightLayout()
{
    // the form, a description field, then rows of label: edit, with Apply and Reset buttons

    auto top_right = new QVBoxLayout();

    top_right->addLayout(descriptionLayout());
    top_right->addLayout(formLayout());
    top_right->addLayout(formButtonsLayout());

    return top_right;
}

QHBoxLayout* ListDialog::formButtonsLayout()
{
    auto bl2 = new QHBoxLayout();

    m_pApply = new QPushButton(tr("Apply"));
    m_pReset = new QPushButton(tr("Reset"));
    connect(m_pApply, &QPushButton::clicked, this, &ListDialog::apply);
    connect(m_pReset, &QPushButton::clicked, this, &ListDialog::reset);

    bl2->addStretch(1);
    bl2->addWidget(m_pApply);
    bl2->addWidget(m_pReset);

    return bl2;
}

QVBoxLayout* ListDialog::descriptionLayout()
{
    auto desc = new QVBoxLayout();

    m_pDescription = new QLineEdit(this);
    connect(m_pDescription, &QLineEdit::textEdited, this, &ListDialog::formDataChanged);

    m_pDescLabel = new QLabel(tr("Description:"));
    m_pDescLabel->setBuddy(m_pDescription);
    desc->addWidget(m_pDescLabel);
    desc->addWidget(m_pDescription);
    desc->setSpacing(4);

    return desc;
}

QFormLayout* ListDialog::formLayout()
{
    auto form = new QFormLayout();
    form->setSpacing(12);

    for (const auto& field : m_Data.fields)
    {
        auto tx = new QLabel((field.name + ": ").c_str(), this);
        auto li = new QLineEdit(this);
        tx->setBuddy(li);
        li->setMinimumSize(100,0);

        form->addRow(tx, li);
        m_vLabels.push_back(tx);
        m_vEdits.push_back(li);

        connect(li, &QLineEdit::textEdited, this, &ListDialog::formDataChanged);
    }

    return form;
}

void ListDialog::listRowChanged(int currentRow)
{
    m_pDelete->setEnabled(currentRow >= 0 && !m_adding);

    if (m_adding) return;

    if (currentRow > -1)
    {
        enableForm(true);
        fillForm();
    }
    else
    {
        clearForm();
        enableForm(false);
    }
}

void ListDialog::formDataChanged(const QString& text)
{
    m_formDirty = true;
    enableFormButtons();
}

void ListDialog::accept()
{
    handleDirtyForm(m_pList->count() - 1);

    QDialog::accept();
}

void ListDialog::listItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    handleDirtyForm(m_pList->row(previous));
}

void ListDialog::handleDirtyForm(int row)
{
    if (m_formDirty)
    {
        if (m_adding && m_pList->currentRow() != m_pList->count() - 1 && formEmpty())
        {
            cancelAddAttempt();

            m_pList->setCurrentRow(row);

            return;
        }

        if (mustApplyChanges())
        {
            if (!validate()) return;

            if (m_adding)
            {
                applyNew(getForm());
            }
            else
            {
                applyEdit(getForm(), row);
            }
        }
        else
        {
            m_formDirty = false;
        }
    }
}

bool ListDialog::mustApplyChanges()
{
    return QMessageBox::question(this,
                                 m_Data.title.c_str(),
                                 tr("Keep changes in form?"),
                                 QMessageBox::Yes | QMessageBox::No) ==  QMessageBox::Yes;
}

bool ListDialog::formEmpty()
{
    auto item = getForm();

    if (!item.description.empty()) return false;

    for (const auto value: item.values)
    {
        if (value != 0.0) return false; // float comparison, ugly
    }

    return true;
}

void ListDialog::cancelAddAttempt()
{
    m_adding = false;

    m_pList->takeItem(m_pList->count() - 1);

    m_formDirty = false;
}
