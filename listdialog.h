#ifndef LISTDIALOG_H
#define LISTDIALOG_H

#include <QDialog>
#include <QObject>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <vector>

namespace ListTypes
{
    struct Item
    {
    public:
        Item(std::string description, std::vector<double> values = {}) :
            description(description),
            deleted(false),
            values(values)
        {}

        bool deleted;
        std::string description;
        std::vector<double> values;
    };

    struct Field
    {
        std::string name;
    };

    struct Data
    {
    public:
        Data(std::string title, std::vector<Field> fields, std::vector<Item> items = {}) :
            title(title),
            fields(fields),
            items(items)
        {}

        std::string title;
        std::vector<Field> fields;
        std::vector<Item> items;
    };
}

class ListDialog : public QDialog
{
    Q_OBJECT

public:
    ListDialog(const ListTypes::Data& data, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~ListDialog() {}

    ListTypes::Data getData() const { return m_Data; } // copy

public slots:
    void accept() override;

protected slots:
    void add_item();
    void delete_item();
    void apply();
    void reset();
    void init();
    void listRowChanged(int currentRow);
    void formDataChanged(const QString& text);
    void listItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

protected:
    QListWidget* m_pList;
    QPushButton* m_pAdd;
    QPushButton* m_pDelete;
    QPushButton* m_pApply;
    QPushButton* m_pReset;
    QLineEdit* m_pDescription;
    QLabel* m_pDescLabel;
    std::vector<QLabel*> m_vLabels;
    std::vector<QLineEdit*> m_vEdits;

    ListTypes::Data m_Data;
    std::vector<int> m_itemAtRow;
    bool m_initialised;
    bool m_formDirty;
    bool m_listChanged;
    bool m_adding;

    void populateList();
    void clearForm();
    void fillForm();
    void enableForm(bool enable);
    void enableFormButtons();
    void applyNew(const ListTypes::Item& item);
    void applyEdit(const ListTypes::Item& item, int row);
    ListTypes::Item getForm();
    void handleDirtyForm(int row);
    bool formEmpty();
    void cancelAddAttempt();
    bool mustApplyChanges();

    // override this to.. erm... validate the form data

    virtual bool validate() { return true; }

    // layouts and form element generators

    QGridLayout *topLeftLayout();
    QFormLayout *formLayout();
    QVBoxLayout *topRightLayout();
    QDialogButtonBox *dialogButtonBox();
    QVBoxLayout *descriptionLayout();
    QHBoxLayout *listButtonsLayout();
    QHBoxLayout *formButtonsLayout();
    QFrame *line(QFrame::Shape shape);
    QLabel *titleElement();
    QListWidget *listElement();
};

#endif // LISTDIALOG_H
