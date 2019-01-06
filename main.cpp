#include <QApplication>
#include <thread>
#include "function.h"

sem_t semKey;

void runVideoCheck(MainWindow *p){
    while(p->getChoice()!=-2){
        sem_wait(&semKey);
        videoCheck(MainWindow::ptr);
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
