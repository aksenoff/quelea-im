#include "dbeditor.h"

DBEditor::DBEditor(Database* DB, QWidget* pwgt/*= 0*/)
    : QDialog(pwgt), db(DB)
{
    clientsTable = new QTableView;
    model = new QSqlTableModel;

    model->setTable("clients");
    model->select();
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    clientsTable->setModel(model);

    addClientButton = new QPushButton(tr("Add client"));
    delClientButton = new QPushButton(tr("Delete client"));

    connect(addClientButton,SIGNAL(clicked()),
            this, SLOT(createClientDialog()));
    connect(delClientButton,SIGNAL(clicked()),
            this, SLOT(deleteClientDialog()));
    connect(clientsTable, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(changePassword(QModelIndex)));

    QVBoxLayout* buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(addClientButton);
    buttonLayout->addWidget(delClientButton);
    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(clientsTable);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    resize(300,400);
}

// ----------------------------------------------------------------------

void DBEditor::createClientDialog()
{
    bool ok;
    QString username = QInputDialog::getText(this, tr("User name"),
                                             tr("User name:"), QLineEdit::Normal,
                                                 "", &ok);
    if (ok && !username.isEmpty()){
        QString password = QInputDialog::getText(this, tr("User password"),
                                                 tr("Password:"), QLineEdit::Password,"", &ok);
        if (ok && !password.isEmpty()){
            int result = db->addClient(username, password);
            if(result == 1)
                QMessageBox::information (this, "User added", "User added!");
            if(result == -1) {
                QMessageBox::warning(this, "Error", "Such name is already exist!");
                createClientDialog();
            }
            if(result == 0)
                QMessageBox::warning(this, "Error", "Such does not added!");
         }
    }
}
// ----------------------------------------------------------------------
void DBEditor::deleteClientDialog()
{
    //db->deleteClient(clientsTable->currentIndex().data().toString());
}
// ----------------------------------------------------------------------

void DBEditor::changePassword(QModelIndex index)
{
   if (index.column()==1) {
       bool ok;
       QString password1 = QInputDialog::getText(this, tr("New password"),
                                                 tr("Password:"), QLineEdit::Password,"", &ok);
       if (ok && !password1.isEmpty()) {
            QString password2 = QInputDialog::getText(this, tr("Repeat password"),
                                                 tr("Repeat password:"), QLineEdit::Password,"", &ok);
            if(ok && !password2.isEmpty() && (password1 == password2))
                     if(model->setData(index,db->hash(password2)))
                         QMessageBox::information (this, "Password changed", "Password changed!");
       }
   }

}
