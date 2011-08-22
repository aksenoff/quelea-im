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
            if (ok && !password.isEmpty())
                if(db->addClient(username, password))
                    QMessageBox::information (this, "User added", "User added!");
        }
}
