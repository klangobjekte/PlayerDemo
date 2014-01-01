#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <map>
#include "psndevents.h"

QString
time2str(double t, int e)
{
  char const * sign = "";
  if (t < 0) {
    sign = "-";
    t = -t;
  }
  double shift = 1.0;
  double roundup = 0.5;
  for (int i=0; i<e; ++i) {
    shift *= 10.0;
    roundup /= 10.0;
  }
  t += roundup;
  int sec = (int)t;
  int msec = (int)((t-(double)sec)*shift);
  int min = sec / 60;
  int hour = min / 60;
  min %= 60;
  sec %= 60;
  char s[128];
  if (hour != 0)
    sprintf(s,"%s%d:%02d:%02d.",sign,hour,min,sec);
  else if (min != 0)
    sprintf(s,"%s%d:%02d.",sign,min,sec);
  else
    sprintf(s,"%s%d.",sign,sec);
  char f[16];
  sprintf(f,"%%0%dd",e);
  sprintf(&s[strlen(s)],f,msec);
  return QString(s);
}


PlayerWidget::PlayerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerWidget),
    player(new PSndPlayer()),
    waveformCursorProxy(new WaveformCursorProxy(this)),
    waveformSelectionProxy(new WaveformSelectionProxy(this)),
    paused(false),
    hasRuler(false),
    speed(1)
{
    ui->setupUi(this);
    outDevices = player->audioOutPut()->getOutputDevices();
    map<int,string>::const_iterator iter;
    for(iter = outDevices.begin();iter!=outDevices.end();++iter){
      QString dev = QString::fromStdString(iter->second);
      //QString dev = QString::fromStdWString(iter->second);
      ui->devicesComboBox->addItem(dev.toUtf8());
    }
    player->getPlayerTicker()->registerReceiver(this);
    player->getPlayerTicker()->registerReceiver(waveformCursorProxy);
    ui->label->setText("0.0000");
    //ui->label->setBackgroundRole(QPalette::Dark);
    //ui->label->setForegroundRole(QPalette::HighlightedText);


    ui->seekSlider->setRange(0, 10*1000);
    ruler = new WaveformRuler(true, this);
    ui->rulerLayout_->addWidget(ruler);
    //ui->gridLayout->addWidget(ruler,0,1);

    ui->cursorGroupBox->setId(ui->cursorArrowButton,0);
    ui->cursorGroupBox->setId(ui->cursorIbeamButton,1);
    ui->cursorGroupBox->setId(ui->cursorOpenHandButton,2);

    waveformScrolBar = new WaveformScrollBar(this);
    ui->scrollBarLayout->addWidget(waveformScrolBar);
    ui->hZoomSlider->setRange(1,9999);
    connect(waveformSelectionProxy, SIGNAL(waveformSelectionChanged(double,double,Waveform*)),
        this, SLOT(changeSelection(double,double,Waveform*)));

    ui->vZoomSlider->setValue(50);
    ui->vZoomSlider->setRange(1,100);
}

PlayerWidget::~PlayerWidget()
{
    player->getPlayerTicker()->unregisterReceiver(this);
    delete player;
    delete ui;
}

void PlayerWidget::on_browsePushButton_clicked(){
    //QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

    QString home;
#ifdef WIN32
    home = "C:/Qt_Projekte/testfolder/_no_prob/_Time";
#else
   home =  "/Users/Admin/Documents/python/_testfiles/testfolder/_no_prob/_Time";
#endif
    QStringList files = QFileDialog::getOpenFileNames(
        this,"Open Audio File",
        home,
         "All (**)"
            );
    //if (sname.length() > 0)
    //  ui->lineEdit->setText(sname.toUtf8());

    foreach(QString name, files){
    QString utfstring = name.toUtf8();
    QTableWidgetItem *pathItem = new QTableWidgetItem(utfstring);

    int currentRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(currentRow);
    ui->tableWidget->setItem(currentRow,0,pathItem);
    ui->tableWidget->resizeColumnsToContents();
    //if (ui->tableWidget->columnWidth(0) > 300)
    //    ui->tableWidget->setColumnWidth(0, 300);
    }

}

void PlayerWidget::on_addAndPlayPushButton_clicked(){
    qDebug() << "on_addAndPlayPushButton_clicked!";
    QFileInfo finfo(ui->lineEdit->text());
    if (!finfo.exists()) {
        QString msg("Can't find file: '%1'");
        QMessageBox::critical(this,"Error",msg.arg(finfo.filePath()));
        return;
    }
    player->pause();
    #ifdef WIN32
        player->addFile(finfo.filePath().toStdWString());
    #else
        player->addAndPlayFile(finfo.filePath().toStdString().c_str());
    #endif
    on_loadWavePushButton_clicked();
}

void PlayerWidget::on_addPushButton_clicked(){
    qDebug() << "on_addPushButton_clicked!";
    QFileInfo finfo(ui->lineEdit->text());
    if (!finfo.exists()) {
        QString msg("Can't find file: '%1'");
        QMessageBox::critical(this,"Error",msg.arg(finfo.filePath()));
        return;
    }
    player->pause();
    #ifdef WIN32
        player->addFile(finfo.filePath().toStdWString());
    #else
        player->addFile(finfo.filePath().toStdString().c_str());
    #endif

}

void PlayerWidget::on_removePushButton_clicked(){

}

void PlayerWidget::on_reverseButton_clicked(bool checked){
    qDebug() << "on_reverseButton_clicked: " << checked;
    player->reverse(checked);
}

void PlayerWidget::on_playPushButton_clicked(){
    speed = 1;
    player->audioOutPut()->setPlaydirection(FORWARD);
    paused = false;
    ui->pausePushButton->setText("||");
    player->play(0,5);
}

void PlayerWidget::on_revPlayPushButton_clicked(){
    player->audioOutPut()->setPlaydirection(REVERSE);

}

void PlayerWidget::on_fastPlayPushButton_clicked(){
    speed+=0.1;
    qDebug() << "on_fastPlayPushButton_clicked" << speed;
    //player->audioOutPut()->setPlaydirection(FORWARD);
    player->audioOutPut()->setPlaySpeed(speed);
}

void PlayerWidget::on_fastRevPushButton_clicked(){
    qDebug() << "on_fastRevPushButton_clicked";
    //player->audioOutPut()->setPlaydirection(REVERSE);
    speed-=0.1;
    player->audioOutPut()->setPlaySpeed(speed);
}

void PlayerWidget::on_repeatPushButton_clicked(){
    paused = false;
    ui->pausePushButton->setText("||");
    //player->repeat(waveformSelection->getBeginSeconds(),
      //	 waveformSelection->getWidthSeconds());
}

void PlayerWidget::on_stopPushButton_clicked(){
    qDebug() << "onstopPushButtonClicked";
    speed = 1;
    player->stop();
}

void PlayerWidget::on_pausePushButton_clicked(){

}

void PlayerWidget::on_devicesComboBox_currentIndexChanged(QString id){
    int _id =0;

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
    player->audioOutPut()->setOutputDevice(_id);
}

void PlayerWidget::customEvent(QEvent *e){
    if (e->type() == (QEvent::Type)PlayerPosition){
      ui->label->setText(time2str(((PlayerPositionEvent*)e)->time(),4));
      ui->seekSlider->setSliderPosition(((PlayerPositionEvent*)e)->time()*1000);

    }
}



int PlayerWidget::getVolume(){
    return 0;
}

void PlayerWidget::on_loadWavePushButton_clicked(){

    qDebug() << "on_loadWavePushButton_clicked";

    currentSources = player->getCurrentSources();

    map<psnd_string,MediaSource*>::iterator sourceIter;
    psnd_string filename;
    //map<psnd_string,WaveformMap >::iterator witer;
    int gridCurRow = 0;
    Waveform* waveform =0;
    MediaSource *mediaSource = 0;
    unregisterWaveform();
#ifdef _WIN32
        filename = ui->lineEdit->text().toStdWString();
#else
       filename = ui->lineEdit->text().toStdString();
#endif

    //for (sourceIter= currentSources.begin();sourceIter != currentSources.end();sourceIter++){
    if(currentSources.find(filename) != currentSources.end()){

            //filename = sourceIter->first;
            mediaSource = currentSources[filename];
            currentFile = filename;
            double len = mediaSource->duration();
            qDebug() << "len: " << len;

            ui->seekSlider->setRange(0, len*1000);

            if(waveforms.find(filename) == waveforms.end()){

                waveFormBuffer = new WaveFormBuffer(this,
                                                filename.c_str(),
                                                mediaSource->outData()->outSampleRate,
                                                mediaSource->outData()->channels,
                                                mediaSource->outData()->num_frames,
                                                len);


                //qDebug() << "calling loadBuffer 1 --------------------------------------";
                //waveFormBuffer->loadBuffer(mediaSource->outData()->data,
                //                       0,
                //                       len);
                for (int ch=0; ch<mediaSource->outData()->channels; ++ch)
                {
                    //WaveformVRuler *verticalRuler = new WaveformVRuler(this);
                    waveform = new Waveform  (waveFormBuffer, ch, 0.0, len, this); //4ter Parameter ist Duration
                    //waveform->setObjectName(tr("waveform%1").arg(_sndfiles.size()));
                    //waveform->setWaveformCursorShape(_waveformCursorShape);
                    //ui->gridLayout->addWidget(verticalRuler, gridCurRow, 0); // Vertical Ruler (WaveformRuler)

                    waveformSelectionProxy->registerWaveform(waveform);

                    waveformCursorProxy->registerWaveform(waveform);

                    //waveformScrolBar->registerWaveform(waveform);

                    connect(waveformSelectionProxy,SIGNAL(waveformSelectionChanged(double,double,Waveform*)),
                            waveform,SLOT(setSelectionParameter(double,double,Waveform*)));
                    connect(ui->cursorGroupBox,SIGNAL(buttonClicked(int)),
                            waveform,SLOT(setWaveformCursorShape(int)));
                    waveforms[filename][ch] = waveform;


                }// for channels

            }//! existing waveform?

            waveformMap = waveforms[filename];
            WaveformMap::iterator channeliter;
            bool runOnce = true;
            for(channeliter = waveformMap.begin();channeliter != waveformMap.end();channeliter++){
                waveform = channeliter->second;
                if(runOnce){
                    waveform->getWaveFormBuffer()->noCopyBuffer(mediaSource->outData()->data,0,len);
                    runOnce = false;
                }
                ui->gridLayout->addWidget(waveform, gridCurRow, 0); // Waveform
                //verticalRuler->connectToWaveform(waveform);
                //verticalRuler->show();

                waveform->show();
                if (hasRuler == false) {
                    ruler->connectToWaveform(waveform);

                    ruler->show();
                    //waveform->setHasRuler(true);
                    hasRuler = true;
                }
                //connect(waveform, SIGNAL(waveformMouseMoved(Waveform*,double)),
                //    this, SLOT(setTime(Waveform*,double)));
                //connect(waveform, SIGNAL(waveformMouseMoved(double)),
                //    this, SLOT(seek(int)));

                //! nach Click in Waveform wird ab Klick weitergespielt
                //connect(waveform, SIGNAL(waveformMouseReleased(Waveform*,double)),
                //    this, SLOT(onRelChangePlayPosition()));
                //connect(this, SIGNAL(setVertZoom(double)),
                //        waveform, SLOT(setAmplitudeRatio(double)));
                //connect(this,SIGNAL(waveformCursorChanged(int)),
                //        waveform,SLOT(setWaveformCursorShape(int)));


                //QColor waveformColor(QColor(42, 38, 38));
                //QColor backgroundColor(QColor(85, 85, 127));
                //soundfile->setColor(waveformColor,backgroundColor);
                waveform->display(0,len,true);
                ++gridCurRow;

            }//! iterchannels

        }// if filename

      //player->resume();

       //QSlider *hzoomSlider = new QSlider(this);
       //hzoomSlider->setOrientation(Qt::Horizontal);
       //hzoomSlider->setFixedWidth(300);
       //hzoomSlider->setMinimum(-10);
       //hzoomSlider->setMaximum(10);
       //grid->addWidget(hzoomSlider, 4,0);
    //}//! for sources

}

//! fuer Mainwindow
void PlayerWidget::on_cursorGroupBox_buttonClicked(int id){
    qDebug() << "on_cursorGroupBox_buttonClicked: " << id;
    emit publishWaveformCursorChanged(id);
}

void PlayerWidget::changeSelection(double beg, double dur, Waveform*)
{
    qDebug() << "changeSelection beg : " << beg;
    startTime = beg;
    selectionduration = dur;
    endTime = beg+dur;
    //player->setPosition();
    player->setPosition(beg*1000);
    ui->label->setText(time2str((double)beg,4));
    //PLAYER_DEBUG << "double beg: " << beg;
    //selectionBeginTimeLabel->setTime(beg);
    //selectionEndTimeLabel->setTime(beg+dur);
    //selectionDurationTimeLabel->setTime(dur);
    //createSndFile->setTime(beg,dur);
}



void PlayerWidget::on_tableWidget_cellPressed(int row, int column)
{
    qDebug() << "on_tableWidget_cellPressed: " << row << column;
    ui->lineEdit->setText(ui->tableWidget->item(row,column)->text());
    player->stop();
    on_addAndPlayPushButton_clicked();
    //on_addPushButton_clicked();
    //on_loadWavePushButton_clicked();


}

void PlayerWidget::unregisterWaveform(){
    hasRuler = false;
    int count = ui->gridLayout->rowCount();
    for(int i=0;i<count;i++){
        if(!(ui->gridLayout->itemAtPosition(i,0)==0))
            ui->gridLayout->removeItem(ui->gridLayout->itemAtPosition(i,0));
    }

    map<psnd_string,WaveformMap >::iterator witer;
    witer  = waveforms.find(currentFile);
    if(witer!= waveforms.cend()){

        WaveformMap wMap = witer->second;

        WaveformMap::iterator channeliter;
        for(channeliter = wMap.begin();channeliter != wMap.end();channeliter++){
            Waveform *waveform = channeliter->second;
            //waveformCursorProxy->unregisterWaveform(waveform);
            //waveformSelectionProxy->unregisterWaveform(waveform);
            //ruler->disconnectWaveform();
            waveform->hide();
        }

    }
}

void PlayerWidget::on_hZoomSlider_sliderMoved(int value){
    qDebug() << "on_hZoomSlider_sliderMoved: " << value;
    map<psnd_string,WaveformMap >::iterator witer;
    witer  = waveforms.find(currentFile);
    WaveformMap wMap = witer->second;
    WaveformMap::iterator channeliter;
    for(channeliter = wMap.begin();channeliter != wMap.end();channeliter++){
        Waveform *waveform = channeliter->second;
        //waveformCursorProxy->unregisterWaveform(waveform);
        //waveformSelectionProxy->unregisterWaveform(waveform);
        //ruler->disconnectWaveform();
        double len = waveform->getWaveFormBuffer()->getLengthSeconds();
        waveform->display(0,len/value,true);
    }
}

void PlayerWidget::on_seekSlider_sliderMoved(int value){
    qDebug() << "on_seekSlider_sliderMoved: " << value;
    player->setPosition(value);
    ui->label->setText(time2str((double)value/1000,4));
}

void PlayerWidget::on_volumeSlider_valueChanged(int value){
    qDebug() << "on_volumeSlider_valueChanged" << value;
    player->setVolume(value);
}

void PlayerWidget::on_vZoomSlider_sliderMoved(int value){
    double vratio = value/50.0;
    qDebug() << "on_vZoomSlider_sliderMoved: " << vratio;
    //vratio *= 1.5;
    //vratio /= 1.5;
    //emit setVertZoom(vratio);

    map<psnd_string,WaveformMap >::iterator witer;
    witer  = waveforms.find(currentFile);
    WaveformMap wMap = witer->second;
    WaveformMap::iterator channeliter;
    for(channeliter = wMap.begin();channeliter != wMap.end();channeliter++){
        Waveform *waveform = channeliter->second;
        //waveformCursorProxy->unregisterWaveform(waveform);
        //waveformSelectionProxy->unregisterWaveform(waveform);
        //ruler->disconnectWaveform();
        double len = waveform->getWaveFormBuffer()->getLengthSeconds();
        //waveform->display(0,len/value,true);
        waveform->setAmplitudeRatio(vratio);
    }
}
