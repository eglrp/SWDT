#include "MyWindow.h"
#include "MyMainWindow.h"
#include <QApplication>
#include <QFont>
#include <QPushButton>


namespace {
namespace local {

int very_simple_example(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setFixedSize(400, 300);
    window.setWindowTitle("Qt4 Application #1");

    QPushButton btnQuit("Quit", &window);
    btnQuit.setFont(QFont("Times", 18, QFont::Bold));
    btnQuit.setGeometry(10, 10, 80, 30);
    btnQuit.resize(160, 30);

    QObject::connect(&btnQuit, SIGNAL(clicked()), &app, SLOT(quit()));

    window.show();

    return app.exec();
}

int simple_example_1(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MyWindow window;
    //QObject::connect(&window, SIGNAL(areaChanged()), &window, SLOT(handAreaChanged()));
    QObject::connect(&window, SIGNAL(applicationQuitSignaled()), &app, SLOT(quit()));

    window.show();

    window.setArea(100);

    return app.exec();
}

int simple_example_2(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MyMainWindow window;
    QObject::connect(&window, SIGNAL(applicationQuitSignaled()), &app, SLOT(quit()));

    window.show();

    return app.exec();
}

}  // namespace local
}  // unnamed namespace

// Meta Object Compiler (moc).
//  moc object_name.h -o moc_object_name.cpp
// Resource Compiler (rcc).
//	(optional) rcc -binary project_name.qrc -o project_name.rcc
//	rcc project_name.qrc -name project_name -o rcc_project_name.cpp

int qt4_main(int argc, char* argv[])
{
    //const int retval = local::very_simple_example(argc, argv);
    //const int retval = local::simple_example_1(argc, argv);
    const int retval = local::simple_example_2(argc, argv);
    
    return retval;
}
