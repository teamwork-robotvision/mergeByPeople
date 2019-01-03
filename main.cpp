#include "mainwindow.h"
#include <QApplication>
#include <thread>
sem_t semKey;

void runVideoCheck(MainWindow *p){
    int sign=0;
    while(p->getChoice()!=-2){
        sem_wait(&semKey);
        MainWindow::videoCheck(MainWindow::ptr);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    sem_init(&semKey,0,0);
    MainWindow w;
    MainWindow::ptr=&w;
    w.show();
    thread th(runVideoCheck,&w);
    th.detach();
    a.exec();
    w.setChoice(-2);
    sem_destroy(&semKey);
    return 0;
}
