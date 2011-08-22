#include <QtGui>
#include "settingsDialog.h"
#include "database.h"

SettingsDialog::SettingsDialog(Database* DB, QWidget* pwgt/*= 0*/)
    : QDialog(pwgt), db(DB)
{
    tabWidget = new QTabWidget;
    QWidget* authWidget = new QWidget;
    dbGroupBox = new QGroupBox(tr("Use database"),authWidget);
    dbGroupBox->setCheckable(true);
    dbGroupBox->resize(411,150);

    dbTooltip = new QLabel(tr("Load DB file or create new one"));
    dbState = new QLabel(tr("File is not selected"));
    dbPathEdit = new QLineEdit;
    dbPathEdit->setReadOnly(true);
    dbLoadButton = new QPushButton(tr("Load")+"...");
    dbCreateButton = new QPushButton(tr("Create")+"...");
    dbEditButton = new QPushButton(tr("Edit")+"...");

    dbLoadButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    dbCreateButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    dbEditButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    connect(dbLoadButton, SIGNAL(clicked()),
            this, SLOT(loadDB()));
    connect(dbCreateButton, SIGNAL(clicked()),
            this, SLOT(createDB()));
    connect(dbEditButton, SIGNAL(clicked()),
            this, SLOT(openDBEditor()));

    QHBoxLayout* dbButtonLayout = new QHBoxLayout;
    dbButtonLayout->addSpacing(200);
    dbButtonLayout->addWidget(dbLoadButton);
    dbButtonLayout->addWidget(dbCreateButton);
    dbButtonLayout->addWidget(dbEditButton);

    QVBoxLayout* dbLayout = new QVBoxLayout(dbGroupBox);
    dbLayout->addWidget(dbTooltip);
    dbLayout->addWidget(dbPathEdit);
    dbLayout->addWidget(dbState);
    dbLayout->addWidget(dbTooltip);
    dbLayout->addWidget(dbTooltip);
    dbLayout->addLayout(dbButtonLayout);

    ldapGroupBox = new QGroupBox;

    tabWidget->addTab(authWidget,tr("Authorization"));

    okButton = new QPushButton(tr("&Ok"));
    cancelButton = new QPushButton(tr("&Cancel"));

    okButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    cancelButton->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    // Layout setup
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addWidget(tabWidget);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    resize(451,338);
    setWindowTitle(tr("Settings")+" - Quelea");
}

// ----------------------------------------------------------------------

void SettingsDialog::loadDB()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open database"),
                                                    QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                    tr("Quelea Database Files (*.qdb)"));
    if (!fileName.isEmpty())
        if (!fileName.isEmpty())
            switch (db->openDB(fileName)) {
                case DB_OPEN_OK :
                dbState->setText("<FONT COLOR=GREEN>"+tr("Database opened")+"</FONT>");
                dbPathEdit->setText(fileName);
                break;

                case DB_OPEN_ERROR :
                dbState->setText("<FONT COLOR=RED>"+tr("Database open error: file error")+"</FONT>");
                break;

                case DB_VERSION_ERROR :
                dbState->setText("<FONT COLOR=RED>"+tr("Database open error: uncompatible DB version")+"</FONT>");
                break;
        }


}

// ----------------------------------------------------------------------

void SettingsDialog::createDB()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Create new database"),
                                                    QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                                    tr("Quelea Database Files (*.qdb)"));
    if(db->createDB(fileName)) {
        dbState->setText("<FONT COLOR=GREEN>"+tr("Database created and opened")+"</FONT>");
        dbPathEdit->setText(fileName);
    }
    else
        dbState->setText("<FONT COLOR=RED>"+tr("Database creation error")+"</FONT>");
}

// ----------------------------------------------------------------------
void SettingsDialog::openDBEditor()
{
    dbEdit = new DBEditor(db);
    dbEdit->exec();
    delete dbEdit;

}
// ----------------------------------------------------------------------
bool SettingsDialog::useDB() const
{
    return dbGroupBox->isChecked();
}
// ----------------------------------------------------------------------
