#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
/*IO 3.26 122 ;3.22 118;3.20 116;2.7 71; /*/
#define LED_EXPORT_PATH "/sys/class/gpio/export"

#define LED_1_DEV_PATH    "/sys/class/gpio/gpio122/value"
#define LED_1_DIRECT_PATH "/sys/class/gpio/gpio122/direction"

#define LED_2_DEV_PATH    "/sys/class/gpio/gpio118/value"
#define LED_2_DIRECT_PATH "/sys/class/gpio/gpio118/direction"

#define LED_3_DEV_PATH    "/sys/class/gpio/gpio116/value"
#define LED_3_DIRECT_PATH "/sys/class/gpio/gpio116/direction"

#define LED_4_DEV_PATH    "/sys/class/gpio/gpio71/value"
#define LED_4_DIRECT_PATH "/sys/class/gpio/gpio71/direction"

#define BEEP_DEV_PAT    "/sys/class/leds/beep/brightness"

#define OUT               "out"
#define IN                "in"
#define GPIO_3_26         "122"
#define GPIO_3_22         "118"
#define GPIO_3_20         "116"
#define GPIO_2_07         "71"
#define HIGH_LEVEL        "1"
#define LOW_LEVEL         "0"


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int fd_export;
    int LED_1_fd_dir,LED_1_fd_dev;
    int LED_2_fd_dir,LED_2_fd_dev;
    int LED_3_fd_dir,LED_3_fd_dev;
    int LED_4_fd_dir,LED_4_fd_dev;
    int fd_beep;

    int pipes[2];

    int fd_uart;
private:
    Ui::Widget *ui;
private slots:
    void timerUpDate();
    void on_checkBox_clicked(bool checked);
    void on_checkBox_2_clicked(bool checked);
    void on_checkBox_3_clicked(bool checked);
    void on_checkBox_4_clicked(bool checked);
};

#endif // WIDGET_H
