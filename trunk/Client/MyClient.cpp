
#include <QtNetwork>
#include <QtGui>
#include "MyClient.h"

// ----------------------------------------------------------------------
MyClient::MyClient(
                  QWidget*       pwgt /*=0*/
                  ) : QWidget(pwgt)
                    , m_nNextBlockSize(0)


{
    m_ptxtInfo  = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    clname = new QLineEdit;
    ipadr= new QLineEdit;


    m_pTcpSocket = new QTcpSocket(this);




   connect(m_ptxtInput, SIGNAL(returnPressed()),
            this,        SLOT(slotSendToServer())
           );
    m_ptxtInfo->setReadOnly(true);

    QPushButton* pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
    QPushButton* connbutton = new QPushButton("&Connect");
    connect(connbutton, SIGNAL(clicked()), SLOT(conn()));

    //Layout setup
    QGridLayout* pvbxLayout = new QGridLayout;
    pvbxLayout->addWidget(new QLabel("<-Enter a name"),3,1);
    pvbxLayout->addWidget(m_ptxtInfo,0,0);
    pvbxLayout->addWidget(m_ptxtInput,1,0);
    pvbxLayout->addWidget(pcmd,1,1);
    pvbxLayout->addWidget(connbutton,2,1);
    pvbxLayout->addWidget(clname,3,0);
    pvbxLayout->addWidget(ipadr,2,0);
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
        Message *Mess = new Message;
        in >> time >> Mess->code >> Mess->text;
        if (Mess->code==1)
         m_ptxtInfo->append(time.toString() + " " + "Connected");


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
void MyClient::slotSendToServer()
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_5);
    out << quint16(0) << QTime::currentTime()<< m_ptxtInput->text();

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->setText("");
}

// ------------------------------------------------------------------
void MyClient::slotConnected()
{
    m_ptxtInfo->append("Connecting to server...");
}
