
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
    clname = new QLineEdit;
    ipadr = new QLineEdit;
    contlist = new QListView;
    recipname =new QLineEdit;


    m_pTcpSocket = new QTcpSocket(this);




   connect(m_ptxtInput, SIGNAL(returnPressed()),
            this,        SLOT(SendToServer())
           );
    m_ptxtInfo->setReadOnly(true);

    QPushButton* pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(SendToServer()));

    QPushButton* connbutton = new QPushButton("&Connect");
    connect(connbutton, SIGNAL(clicked()), SLOT(conn()));

    //Layout setup
    QGridLayout* pvbxLayout = new QGridLayout;
    pvbxLayout->addWidget(m_ptxtInfo,2,0);
   // pvbxLayout->addWidget(contlist,2,1);
    pvbxLayout->addWidget(m_ptxtInput,3,0);
    pvbxLayout->addWidget(pcmd,3,1);
    pvbxLayout->addWidget(connbutton,1,1);
    pvbxLayout->addWidget(clname,0,0);
    pvbxLayout->addWidget(new QLabel("<-Enter a name"),0,1);
    pvbxLayout->addWidget(ipadr,1,0);
    pvbxLayout->addWidget(recipname,4,0);
    setLayout(pvbxLayout);
}




void MyClient::conn()
{
    m_pTcpSocket->connectToHost(ipadr->text(), 49212);
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
        QTime   time;
        QString str;
        Message *mess=0; //!
        in >> time >> mess; //Надо ли передавать time? Не логичнее ли писать time клиента?
        switch(int(*mess))
        {
        case CONNECTED:
            {
                str = "Connected!";
                Message* auth_req = new Message(AUTH_REQUEST, clname->text());
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
               str = mess->text;
               break;
           }
        case MESSAGE_TO_CLIENT:
           {
               str=mess->text;
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


    out << quint16(0) << QTime::currentTime() << *message;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
       //m_ptxtInput->setText("");



}

// ------------------------------------------------------------------
void MyClient::slotConnected()
{
    m_ptxtInfo->append("Connecting to server...");
}
