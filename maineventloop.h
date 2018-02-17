#ifndef MAINEVENTLOOP_H
#define MAINEVENTLOOP_H

#include <QObject>
#include <QApplication>
#include <functional>
#include <boost/noncopyable.hpp>
#include <thread>
#include <memory>

class Task: public QObject
{
    Q_OBJECT
public:
    Task(std::function<void(void)> func):
        QObject(nullptr),
        _func(func)
    {

    }

    void call()
    {
        _func();
    }
private:
    std::function<void(void)> _func;
};

class MainEventLoop : public QObject, boost::noncopyable
{
    Q_OBJECT

public:
    static std::shared_ptr<MainEventLoop> instance(int argc, char* argv[])
    {
        std::shared_ptr<MainEventLoop> loop;
        std::atomic<bool> flag;
        flag.store(false);

        std::thread qt_thread([&loop, &flag, argc, argv]() mutable {
            QApplication* app = new QApplication(argc, argv);
            std::shared_ptr<MainEventLoop> loop_guard(new MainEventLoop());
            loop = loop_guard;
            loop_guard.reset();

            loop->__set_app(app);
            flag.store(true);
            app->exec();
        });

        while(!flag.load());
        loop->__set_main_thread(qt_thread);
        return loop;
    }

    void post_task(std::function<void(void)> task)
    {
        emit post_task_sginal(new Task(task));
    }


    void __set_main_thread(std::thread& thread)
    {
        _main_thread.swap(thread);
    }

    void __set_app(QApplication* app){
        _app = app;
    }

    void exit()
    {
        _app->exit();
    }

    virtual ~MainEventLoop()
    {
        _main_thread.join();
        //delete _app;
    }

    void keep(std::shared_ptr<QObject> obj)
    {
        _keeps.push_back(obj);
    }

private:
    explicit MainEventLoop(QObject *parent = nullptr);
    std::thread _main_thread;
    QApplication *_app;
    std::vector<std::shared_ptr<QObject>> _keeps;

signals:
    void post_task_sginal(Task* );


public slots:
    void on_post_task(Task* task){
        task->call();
        //delete task;
    }
};

#endif // MAINEVENTLOOP_H
