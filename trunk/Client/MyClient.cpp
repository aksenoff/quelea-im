
#include <QtNetwork>
#include <QtGui>
#include "MyClient.h"
#include "../codes.h"

// ----------------------------------------------------------------------
MyClient::MyClient(
                  QWidget*       pwgt /*=0*/
                  ) : QWidget(pwgt)
                    , m_nNextBlockSize(0)


{
    m_ptxtInfo  = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    clname = new QComboBox;
    contlist = new QListWidget;
    recipname = new QLineEdit;



    m_pTcpSocket = new QTcpSocket(this);

    clname->setEditable(true);



   connect(m_ptxtInput, SIGNAL(returnPressed()),
            this,        SLOT(SendToServer())
           );
    m_ptxtInfo->setReadOnly(true);

    pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(sendmess()));
    pcmd->setEnabled(false);

    connbutton = new QPushButton("&Connect");
    connect(connbutton, SIGNAL(clicked()), SLOT(opendial()));
    connbutton->setEnabled(false);

    sendtochat = new QPushButton("&Send to chat");
    connect(sendtochat, SIGNAL(clicked()), SLOT(sendchat()));
    sendtochat->setEnabled(false);

    connect(clname, SIGNAL(editTextChanged(QString)),
            this, SLOT(enableConnButton()));


    //Layout setup
    QGridLayout* pvbxLayout = new QGridLayout;
    pvbxLayout->addWidget(new QLabel("Your name is:"),0,0);
    pvbxLayout->addWidget(clname,0,1);
    pvbxLayout->addWidget(m_ptxtInfo,2,0,1,2);
    pvbxLayout->addWidget(m_ptxtInput,3,0,1,0);
    pvbxLayout->addWidget(connbutton,0,2);
    pvbxLayout->addWidget(sendtochat,4,2);
    pvbxLayout->addWidget(pcmd,3,2);
    pvbxLayout->addWidget(contlist,2,2);
    setLayout(pvbxLayout);


}




void MyClient::conn(QString ipadr)
{
    m_pTcpSocket->connectToHost(ipadr, 49212);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );






}
// ----------------------------------------------------------------------
void MyClient::slotReadyRead()
{
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_4_5);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }
        QTime time = QTime::currentTime();
        QString str;
        Message *mess=0; //!
        in >> mess;
        switch(int(*mess))
        {
        case CONNECTED:
            {
                str = "Connected!";
                Message* auth_req = new Message(AUTH_REQUEST, clname->currentText());
                SendToServer(auth_req);
				delete auth_req;
                break;
            }

        case AUTH_RESPONSE:
           {
               str = "Authorized!";
               Message* contacts_req = new Message(CONTACTS_REQUEST);
               SendToServer(contacts_req);
			   delete contacts_req;
               break;
           }
        case CONTACTS_RESPONSE:
           {
               str = "Contacts received!";
               QStringList clist = mess->text.split(";");
               contlist->clear();
               contlist->addItem(">All users");
               contlist->addItems(clist);
               contlist->setCurrentRow(0);
               pcmd->setEnabled(true);
               sendtochat->setEnabled(true);
               break;
           }
        case MESSAGE_TO_CLIENT:
           {
               QStringList clist = mess->text.split(";");
               str=clist[0]+clist[2]+": "+clist[1];
               break;
           }

            
        }
        m_ptxtInfo->append(time.toString() + " " + str);

        delete mess;

        m_nNextBlockSize = 0;
    }
}

// ----------------------------------------------------------------------
void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError = 
        "Error: " + (err == QAbstractSocket::HostNotFoundError ? 
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ? 
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ? 
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    m_ptxtInfo->append(strError);
}

// ----------------------------------------------------------------------
void MyClient::SendToServer(Message* message)
{


    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);


    out << quint16(0)<< *message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);




}

// ------------------------------------------------------------------
void MyClient::slotConnected()
{
    m_ptxtInfo->append("Connecting to server...");
}

void MyClient::sendmess()
{
    QString str=contlist->currentItem()->text()+";"+m_ptxtInput->text();
    Message* newmess = new Message(MESSAGE_TO_SERVER,str);
    SendToServer(newmess);
    if (contlist->currentItem()->text()!=">All users")
    m_ptxtInfo->append(QTime::currentTime().toString()+" "+clname->currentText()+": "+m_ptxtInput->text());
    m_ptxtInput->setText("");
}

void MyClient::sendchat()
{
    QString str=contlist->currentItem()->text()+";"+m_ptxtInput->text();
    Message* newmess = new Message( MESSAGE_TO_CHAT,str);
    SendToServer(newmess);
    m_ptxtInput->setText("");


}

void MyClient::opendial()
{
    QStringList items;


        bool ok;
        QString ipadress = QInputDialog::getItem(this, tr("Select server"),
                                                 tr("Select server:"), items, 0, true, &ok);
        if (ok && !ipadress.isEmpty())
                 conn(ipadress);

}

void MyClient::enableConnButton()

{

    connbutton->setEnabled(!clname->currentText().isEmpty());
}


