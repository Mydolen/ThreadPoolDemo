#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThreadPool>
#include <QRunnable>
#include <QElapsedTimer>
#include <QTimer>

struct MyStruct{
    int x;
    QString str;
};
Q_DECLARE_METATYPE(MyStruct)

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MyThreadPool;         //线程池
class MyChildThread;        //继承于QThread
class MyChildQObjectThread; //继承于QObject

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_start_clicked();

    void timi_send_to_threadPool();

    void on_btn_stop_clicked();

    void on_sb_timer_valueChanged(int arg1);

    void slot_receive_result(MyStruct temp);

    void on_cb_auto_delete_toggled(bool checked);

    void on_spinBox_valueChanged(int arg1);

    /**
     * @brief slot_receive_perform
     * @brief 继承于QObject子线程类的结果接收函数
     */
    void slot_receive_perform();

private:
    QTimer *m_timer;
    QElapsedTimer *m_QTimer_timer;
    QElapsedTimer *m_QThreadPool_timer;
    Ui::MainWindow *ui;
    quint32 timer_count = 1;
    MyStruct mystruct;
    MyThreadPool *p_my_threadpool;
    bool  m_auto_delete_runnable;
    int timer_max_count = 1000;
    QThread WorkThread;
    MyChildQObjectThread *pMyChildQObjectThread;
    MyChildThread *pMyChildThread;
};

class MyThreadPool : public QObject, public QRunnable
{
    Q_OBJECT
public:
    MyThreadPool(MyStruct temp);
    ~MyThreadPool();
protected:
    void run() override;
private:
    MyStruct m_struct;
    QElapsedTimer   test_timer;
signals:
    void sig_output(MyStruct result);
};

class MyChildThread : public QThread
{
    Q_OBJECT
public:
    MyChildThread();
    ~MyChildThread();
protected:
    void run() override;
signals:
};

class MyChildQObjectThread : public QObject
{
    Q_OBJECT
public:
    MyChildQObjectThread();
    ~MyChildQObjectThread();
public slots:
    void slot_perform();
signals:
    void sig_begin_perform();
    void sig_finish_perform();
};

#endif // MAINWINDOW_H
