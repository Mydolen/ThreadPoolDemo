#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_timer = new QTimer(this);
    m_QTimer_timer = new QElapsedTimer();
    m_QThreadPool_timer = new QElapsedTimer();
    mystruct.x = 1;
    ui->btn_stop->setEnabled(false);
    timer_count = ui->sb_timer->value();
    m_auto_delete_runnable = true;

    qDebug()<<"MainWindow 所在线程："<<QThread::currentThreadId();
#if 0
    pMyChildThread = new MyChildThread();
    connect(pMyChildThread,&QThread::finished,pMyChildThread,&QObject::deleteLater);
    pMyChildThread->start();
#endif

#if 0
    pMyChildQObjectThread = new MyChildQObjectThread();
    pMyChildQObjectThread->moveToThread(&WorkThread);
    connect(&WorkThread,&QThread::finished,pMyChildQObjectThread,&QObject::deleteLater);
    connect(pMyChildQObjectThread,&MyChildQObjectThread::sig_finish_perform,this,&MainWindow::slot_receive_perform);
    WorkThread.start();
    emit pMyChildQObjectThread->sig_begin_perform();

#endif
}

MainWindow::~MainWindow()
{
    if(pMyChildThread)
    {
        pMyChildThread->requestInterruption();
        pMyChildThread->quit();
        pMyChildThread->wait();
    }
    delete ui;
}

void MainWindow::on_btn_start_clicked()
{
    if(m_timer->isActive())
        m_timer->stop();
    m_QTimer_timer->restart();
    m_QThreadPool_timer->restart();
    m_timer->setInterval(timer_count);
    connect(m_timer,&QTimer::timeout,this,&MainWindow::timi_send_to_threadPool);
    m_timer->start();
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(true);
}

void MainWindow::timi_send_to_threadPool()
{
    ui->textEdit->append(QString("定时器%1次结束计时").arg(mystruct.x));
    p_my_threadpool = new MyThreadPool(mystruct);
    connect(p_my_threadpool,&MyThreadPool::sig_output,this,&MainWindow::slot_receive_result);
    p_my_threadpool->setAutoDelete(m_auto_delete_runnable);
    QThreadPool::globalInstance()->start(p_my_threadpool);
    if(mystruct.x >= timer_max_count)
    {
        on_btn_stop_clicked();
        ui->textEdit->append(QString("定时器计时耗费时间：%1").arg(m_QTimer_timer->elapsed()));
    }
    mystruct.x++;
}


void MainWindow::on_btn_stop_clicked()
{
    if(m_timer->isActive())
        m_timer->stop();
    ui->btn_start->setEnabled(true);
    ui->btn_stop->setEnabled(false);
    mystruct.x = 0;
}

void MainWindow::on_sb_timer_valueChanged(int arg1)
{
    timer_count = arg1;
}

void MainWindow::slot_receive_result(MyStruct temp)
{
    ui->textEdit->append(QString("接收到处理结果：%1\n").arg(temp.x).append(temp.str));
    if(temp.x == timer_max_count)
    {
        QString str = "线程池";
        str.append(m_auto_delete_runnable ? "自动删除启用" : "自动删除禁用");
        str.append(tr("耗费时间：%1").arg(m_QThreadPool_timer->elapsed()));
        ui->textEdit->append(str);
    }
}

MyThreadPool::MyThreadPool(MyStruct temp) : m_struct(temp)
{
    qDebug()<<tr("MyThreadPool第%1次的构造函数所在线程：").arg(m_struct.x)<<QThread::currentThreadId();
}

MyThreadPool::~MyThreadPool()
{
    qDebug()<<tr("MyThreadPool第%1次的析构函数所在线程：").arg(m_struct.x)<<QThread::currentThreadId();
}

void MyThreadPool::run()
{
    test_timer.restart();
    while(test_timer.elapsed() < 100)
        ;
    int id = (int)(QThread::currentThreadId());
    m_struct.str.append(tr("第%1次线程ID:0x").arg(m_struct.x));
    m_struct.str.append(QString::number(id,16));
    emit sig_output(m_struct);
}


void MainWindow::on_cb_auto_delete_toggled(bool checked)
{
    m_auto_delete_runnable = checked;
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    timer_max_count = arg1;
}

void MainWindow::slot_receive_perform()
{
    WorkThread.exit();
}


MyChildThread::MyChildThread()
{
    qDebug()<<"MyChildThread的构造函数所在线程："<<QThread::currentThreadId();
}

MyChildThread::~MyChildThread()
{
    qDebug()<<"MyChildThread的析构函数所在线程："<<QThread::currentThreadId();
}

void MyChildThread::run()
{
    qDebug()<<"MyChildThread的执行函数所在线程："<<QThread::currentThreadId();
}

MyChildQObjectThread::MyChildQObjectThread()
{
    qDebug()<<"MyChildQObjectThread的构造函数所在线程："<<QThread::currentThreadId();
    connect(this,&MyChildQObjectThread::sig_begin_perform,this,&MyChildQObjectThread::slot_perform);
}

MyChildQObjectThread::~MyChildQObjectThread()
{
    qDebug()<<"MyChildQObjectThread的析构函数所在线程："<<QThread::currentThreadId();
}

void MyChildQObjectThread::slot_perform()
{
    qDebug()<<"MyChildQObjectThread的执行函数所在线程："<<QThread::currentThreadId();
    emit sig_finish_perform();
}
