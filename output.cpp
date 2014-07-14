#include <cstring>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "output.h"
#include "QDebug"

Output* Output::output;
MainWindow* Output::mainWindow;

Output* Output::Inst()
{
    if(!output)
        output = new Output();

    return output;
}


Output::Output()
{

}

void Output::kprintf(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);

    char buffer[1024];
    vsprintf(buffer, msg, args);

    QString string(buffer);


    mainWindow->write_output(string);

    va_end(args);
}

void Output::progressBar(unsigned long long current, unsigned long long maximum)
{
    int progress = (current * 100)/maximum;
    mainWindow->advanceProgess(progress);
}

void Output::setMainWindow(MainWindow *mainwindow)
{
    Output::mainWindow = mainwindow;
}
