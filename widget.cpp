#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <map>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    paused(false)
{
    ui->setupUi(this);
#ifndef __DONT_USE_PLAYER
    player = new PSndPlayer();
    player->initPlayer();
    player->enableTicker();
    outDevices = player->getOutputDevices();
    map<int,string>::const_iterator iter;
    for(iter = outDevices.begin();iter!=outDevices.end();++iter){
      QString dev = QString::fromStdString(iter->second);
      //QString dev = QString::fromStdWString(iter->second);
      ui->devicesComboBox->addItem(dev.toUtf8());
    }

#endif
}

Widget::~Widget()
{
#ifndef __DONT_USE_PLAYER
  player->terminatePlayer();
  delete player;
#endif
    delete ui;
}

void Widget::on_browsePushButton_clicked(){
    QString home;
#ifdef WIN32
    home = "C:/Qt_Projekte/testfolder/_no_prob/_Time";
#else
   home =  "/Users/Admin/Documents/python/_testfiles/testfolder/_no_prob/_Time";
#endif

    QString sname = QFileDialog::getOpenFileName(
        this,"Open Audio File",
        home,
         "All (*.*)"
            );
    if (sname.length() > 0)
      ui->lineEdit->setText(sname.toUtf8());
}

void Widget::on_addPushButton_clicked(){
    qDebug() << "addPushButtonClicked!";
    QFileInfo finfo(ui->lineEdit->text());
    #ifndef __DONT_USE_PLAYER
    if(ui->lineEdit->text().isEmpty()){
        //fileEntry->setText("/Users/Admin/Documents/python/_testfiles/testfolder/_no_prob/_Time/Test_Time_St_16mono44.wav");
        //ui->lineEdit->setText("/Users/Admin/Documents/python/_testfiles/testfolder/_no_prob/_Time/Test_Time_St_16_00.wav");
    }
    if (!finfo.exists()) {
        QString msg("Can't find file: '%1'");
        QMessageBox::critical(this,"Error",msg.arg(finfo.filePath()));
        return;
    }
    player->pause();
    double len = 10;
    #ifdef WIN32
    //player->addFile(finfo.filePath().toStdWString().c_str());
     player->addFile(finfo.filePath().toStdWString());
    //player->addFile(finfo.filePath().toStdString().c_str());
    //player->addFile()

    //player->addFile(finfo.filePath().toStdString());
   // player->addFile(ui->lineEdit->text());
    #else
   //player->addFile(finfo.filePath().toStdString().c_str());
    #endif
    #endif
}

void Widget::on_removePushButton_clicked(){

}
void Widget::on_playPushButton_clicked(){
    paused = false;
    ui->pausePushButton->setText("||");
  #ifndef __DONT_USE_PLAYER
    player->play(0,5);
  #endif
}

void Widget::on_repeatPushButton_clicked(){
    paused = false;
    ui->pausePushButton->setText("||");
    //player->repeat(selection->getBeginSeconds(),
      //	 selection->getWidthSeconds());
}

void Widget::on_stopPushButton_clicked(){

}

void Widget::on_pausePushButton_clicked(){

}

void Widget::on_devicesComboBox_IndexChanged(QString id){
    int _id =0;
    #ifndef __DONT_USE_PLAYER
//#define _NEWCPP
#ifdef _NEWCPP
    //! c++11

    for(auto iter: outDevices){
        if(iter.second == id.toStdString().c_str()){
        _id =  iter.first;
        }
    }

#else
    map<int,string>::iterator iter;
    for(iter = outDevices.begin();iter != outDevices.end();++iter) {

        //QString dev = QString::fromStdString(iter->second);
        // devicesComboBox->addItem(dev);
        if(iter->second == id.toStdString().c_str()){
        _id =  iter->first;
        }
    }
#endif
    player->setOutputDevice(_id);
    #endif
}
