#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QDateTime>

#include "lradc.h"
#include <stdio.h>	/* using printf()        */
#include <stdlib.h>      /* using sleep()         */
#include <fcntl.h>       /* using file operation  */
#include <sys/ioctl.h>   /* using ioctl()         */
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <termio.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <pthread.h>


#define CMD_VOLTAGE IMX28_ADC_CH0
#define CMD_TEMPTURE IMX28_ADC_CH1

int fd;


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    ui->setupUi(this);


    fd_export = open(LED_EXPORT_PATH,O_WRONLY);
    write(fd_export,GPIO_3_26,strlen(GPIO_3_26));
    write(fd_export,GPIO_3_22,strlen(GPIO_3_22));
    write(fd_export,GPIO_3_20,strlen(GPIO_3_20));
    write(fd_export,GPIO_2_07,strlen(GPIO_2_07));

    LED_1_fd_dir = open(LED_1_DIRECT_PATH,O_RDWR);
    write(LED_1_fd_dir,OUT,strlen(OUT));
    LED_1_fd_dev = open(LED_1_DEV_PATH,O_RDWR);

    LED_2_fd_dir = open(LED_2_DIRECT_PATH,O_RDWR);
    write(LED_2_fd_dir,OUT,strlen(OUT));
    LED_2_fd_dev = open(LED_2_DEV_PATH,O_RDWR);

    LED_3_fd_dir = open(LED_3_DIRECT_PATH,O_RDWR);
    write(LED_3_fd_dir,OUT,strlen(OUT));
    LED_3_fd_dev = open(LED_3_DEV_PATH,O_RDWR);

    LED_4_fd_dir = open(LED_4_DIRECT_PATH,O_RDWR);
    write(LED_4_fd_dir,OUT,strlen(OUT));
    LED_4_fd_dev = open(LED_4_DEV_PATH,O_RDWR);

    fd=open("/dev/magic-adc",O_RDONLY);

    fd_beep=open(BEEP_DEV_PAT,O_RDWR);



    QTimer *timer =new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpDate()));
    timer->start(1000);

    pid_t pid;

    pipe(pipes);	//创建管道

    pid = fork();	//创建进程

        switch(pid){
            case -1:
                perror("\nfork\n");

        case 0:
            char c[]="0";
            char c1[]="1";
                int i=0;
                char tmp[1024];
                int len;
                struct termios opt,oldtio;

                int iFd=open("/dev/ttySP0",O_RDWR|O_NOCTTY);
                if(fd<0){
                    perror("/dev/ttySP0 fail 1\n");
                }

                tcgetattr(iFd, &opt);

                //cfsetispeed(&opt, B57600);
                //cfsetospeed(&opt, B57600);

                 cfsetispeed(&opt, B115200);
                 cfsetospeed(&opt, B115200);


                /*
                 * raw mode
                 */
                opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
                opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
                opt.c_oflag   &=   ~(OPOST);
                opt.c_cflag   &=   ~(CSIZE   |   PARENB);
                opt.c_cflag   |=   CS8;

                /*
                 * 'DATA_LEN' bytes can be read by serial
                 */
                opt.c_cc[VMIN]   =   255;
                opt.c_cc[VTIME]  =   150;

                if (tcsetattr(iFd,   TCSANOW,   &opt)<0) {
                   perror("/dev/ttySP0 fail 2\n");
                }
                for(i = 0; i < 16; i++)
                    tmp[i] = i%0xFF;

                write(iFd, tmp, 16);

                while(1){
                    len = read(iFd, tmp, 0x01);
                    for(i = 0; i < len; i++)
                        printf(" %d", tmp[i]);
                    printf("\n");
                    if(tmp[0]==48){
                        write( pipes[1], c, 1);	//写入管道
                    }
                    else
                        write( pipes[1], c1, 1);	//写入管道
                };

        }

}

Widget::~Widget()
{
    delete ui;
}
void Widget::timerUpDate(){

    int V_O_2,V_CO_2,test;
    float O_2,CO_2;
    QString str_R,str_T;
    char c;
    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    ui->timerlabel->setText(str);//在标签上显示时间



    ioctl(fd,CMD_VOLTAGE,&V_O_2);
    ioctl(fd,CMD_TEMPTURE,&V_CO_2);

    test=ui->spinBox_5->value();

    str_R.sprintf("%d",V_O_2);
    str_T.sprintf("%d",test);



    ui->label_8->setText(str_R);
    ui->label_9->setText(str_T);

    O_2 = V_O_2/4096.0;
    CO_2 = V_CO_2/4096.0;
    V_O_2 = (int)(O_2*100);
    V_CO_2 = (int)(CO_2*100);
    ui->verticalSlider_O_2->setValue(V_O_2);
    ui->verticalSlider_CO_2->setValue(V_CO_2);
    if(V_O_2<20){
        write(fd_beep,"1",1);
    }
    else{
        write(fd_beep,"0",1);
    }

    if(read(pipes[0],&c,1)>0){

        if(c==48){
            ui->label_24->setText("DOWN0");
        }
       else {
            ui->label_24->setText("UP1");
        }

    }

}

void Widget::on_checkBox_clicked(bool checked)
{
    if(checked){
       write(LED_1_fd_dev,LOW_LEVEL,strlen(LOW_LEVEL));
    }
    else{
       write(LED_1_fd_dev,HIGH_LEVEL,strlen(HIGH_LEVEL));
    }
}

void Widget::on_checkBox_2_clicked(bool checked)
{
    if(checked){
       write(LED_2_fd_dev,LOW_LEVEL,strlen(LOW_LEVEL));
    }
    else{
       write(LED_2_fd_dev,HIGH_LEVEL,strlen(HIGH_LEVEL));
    }
}

void Widget::on_checkBox_3_clicked(bool checked)
{
    if(checked){
       write(LED_3_fd_dev,LOW_LEVEL,strlen(LOW_LEVEL));
    }
    else{
       write(LED_3_fd_dev,HIGH_LEVEL,strlen(HIGH_LEVEL));
    }
}

void Widget::on_checkBox_4_clicked(bool checked)
{
    if(checked){
       write(LED_4_fd_dev,LOW_LEVEL,strlen(LOW_LEVEL));
    }
    else{
       write(LED_4_fd_dev,HIGH_LEVEL,strlen(HIGH_LEVEL));
    }
}

