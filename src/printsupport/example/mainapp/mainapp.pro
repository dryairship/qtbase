TEMPLATE = app
TARGET = mainapptest

QT = core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES +=  mainapptest.cpp

CONFIG += cpdbtest

CONFIG(cpdbtest){
    message("LOLOLOL")
}