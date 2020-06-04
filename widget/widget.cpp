#include "widget.h"
#include "ui_widget.h"
#include "screen/screen.h"
#include "systemTray/systemTray.h"
#include "lowLevelKeyBoardHook/LowLevelKeyBoardHook.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>
#include <QTime>
#include <QtWidgets/QSystemTrayIcon>
#include <functional>


Widget::Widget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::Widget) {
    ui->setupUi(this);
    beginPos = this->pos();
    leftPress = false;
    this->setProperty("CanMove", true);
    this->setWindowFlags(Qt::FramelessWindowHint |
                         Qt::WindowSystemMenuHint |
                         Qt::WindowMinMaxButtonsHint |
                         Qt::WindowStaysOnTopHint);
    LowLevelKeyboardHook *globalKBHook = new LowLevelKeyboardHook();
    globalKBHook->setKeyboardCall(std::bind(&Widget::onBtnRectScreenClicked, this));
    systemTrayInit(this);
}

Widget::~Widget() {
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)//鼠标左键按下
    {
        leftPress = true;
        beginPos = e->pos();//鼠标相对窗体的位置
    }
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if (leftPress) {//当前鼠标相对窗体的位置-刚按下左键时的相对位置=鼠标移动的大小
        move(e->pos() - beginPos + this->pos());
        //           鼠标移动的大小+窗体原来的位置=窗体移动后的位置
    }

}

void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    leftPress = false;
}

void Widget::enterEvent(QEvent *e)
{
    if (this->pos().y() <= 0)
    {
        move(pos().x(), 0);
    }

}

void Widget::leaveEvent(QEvent *e)
{
    if (this->pos().y() <= 0)
    {
        move(pos().x(), -29);
    }
}

void Widget::onBtnShowClicked() {
    this->show();
}

void Widget::onBtnHideClicked() {
    this->hide();
}

void Widget::onBtnFullScreenClicked()
{
    //获取全屏截图
    QPixmap qPixmap = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
    QString fileName = QFileDialog::getSaveFileName(this, "文件另存为", "", tr("Config Files (*.bmp)"));
    if (fileName.length() > 0 && qPixmap.save(fileName, "bmp")) {
        QMessageBox::information(this, "提示", "保存成功!", QMessageBox::Ok);
    }

}

void Widget::onBtnRectScreenClicked()
{
    if (windowState() != Qt::WindowMinimized) {
        setWindowState(Qt::WindowMinimized);
    }
    QTime _Timer = QTime::currentTime().addMSecs(250);
    while (QTime::currentTime() < _Timer) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }

    Screen *m = new Screen();
    m->fullScreen = QApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId());
    m->showFullScreen();
}

void Widget::onBtnMinClicked()
{
    if (windowState() != Qt::WindowMinimized) {
        setWindowState(Qt::WindowMinimized);
    }
}

void Widget::onSystemTrayClicked(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this->show();
            break;
    }
}

void Widget::onBtnCloseClicked()
{
    this->close();
}