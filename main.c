#include <reg51.h>
#include <111602.h>
#include <stdio.h>
#include <stdlib.h>

sbit k0 = P1 ^ 0; //add
sbit k1 = P1 ^ 1; //sub
sbit k2 = P1 ^ 2; //change
sbit k3 = P1 ^ 3; //enter
sbit k4 = P1 ^ 4; //restart

#define uchar unsigned char
#define uint unsigned int

void init();//初始化,通电,初始化111602.h,设置
void set_random_num();//设置随机数,真随机需要time.h,这里用rand()函数,属于假随机
void re_delay(int n);//重写了111602.h中的delay函数,用来计时5s,uchar最大是244,将uchar改写成了int,
void my_delay();//消抖函数
void show_top_string(char my_string[]);//显示顶部字符串,第一行,0x00
void show_bottom_string(char my_string[]);//显示底部字符串,第二行,0x40
void show_value();//显示用户输入的当前值
void show_win();//显示胜利的页面,专门写一个函数,因为要显示胜利的所尝试的次数
void set_ten();//调数函数,这是调十位数的
void set_unit();//调数函数,这是调个位数的
void compare();//比较函数,用来比较用户输入的值和随机数的大小
void scan_key();//扫描按键函数
void index_page();//首页函数,显示欢迎页面
void reset_game();//重置游戏函数,用来重置游戏,重新开始

int random_num = 0;
/*猜数字小游戏,电脑随机产生一个数字,玩家输入,如果大了,就输出大了,如果小了,就输出小了,然后继续猜,直到猜对为止*/

const uint len_line = 16;//一行的最大字符数
const uchar my_nums[] = "0123456789";//用来显示数字的字符数组
const uchar win_string[] = "guess count:";//胜利页面的基础字符串
uint i;//循环用,每次使用前会重新赋值
uint guess_num = 0;//用户输入的数字
uint key = 0;// 0:锁定输入 1:修改十位 2:修改个位
uint enter_flag = 0;//是否按下了enter键,第一次按下enter键:开始游戏,第二次按下enter键:调用比较函数,检查输入是否和随机数相等
uint guess_count = 0;//尝试的次数

void init() {//must, only run once
    EA = 1;
    ET0 = 1;
    TMOD = 0X11;
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
    TR0 = 1;
    /*---------------------------*/
    LcdInitiate();//init 111602.h
    WriteAddress(0x00);
    delay(5);//must, only run once
}

void set_random_num() {
    //random get a num from 0 to 99
    random_num = rand() % 100 + 1;
}

void re_delay(int n)
{
    i=0;
    for(i;i<n;i++)
        delay1ms();
}

void my_delay() {
    //delay
    uint j = 0;
    for (j; j < 26000; j++);
}

void show_top_string(char my_string[]) {
    //print specify string as top line
    WriteAddress(0x00);

    i = 0;
    for (i; i < len_line; i++) {
        WriteData(my_string[i]);
    }
}

void show_bottom_string(char my_string[]) {
    //print specify string as bottom line
    WriteAddress(0x40);
    i = 0;
    for (i; i < len_line; i++) {
        WriteData(my_string[i]);
    }
}

void show_value() {
    WriteAddress(0x40);
    WriteData('u');
    WriteData(' ');
    WriteData('i');
    WriteData('n');
    WriteData('p');
    WriteData('u');
    WriteData('t');
    WriteData(':');
    WriteData(my_nums[guess_num / 100]);
    WriteData(my_nums[guess_num % 100 / 10]);
    WriteData(my_nums[guess_num % 10]);

//    //debug,show random num
//    WriteData(my_nums[random_num/100]);
//    WriteData(my_nums[random_num%100 / 10]);
//    WriteData(my_nums[random_num%10]);
}

void show_win() {
    show_top_string("    YOU WIN!    ");
    WriteAddress(0x40);
    i = 0;
    for (i; i < 13; i++) {
        WriteData(win_string[i]);
    }

    WriteData(my_nums[guess_count / 100]);
    WriteData(my_nums[guess_count % 100 / 10]);
    WriteData(my_nums[guess_count % 10]);

    re_delay(5000);//delay 5s
}

void set_ten() {
    //set ten
    if (k0 == 0) {
        my_delay();
        if (k0 == 0) { guess_num = guess_num + 10; }
    }

    if (k1 == 0) {
        my_delay();
        if (k1 == 0) {
            if (guess_num < 10) { guess_num = 99; }
            else { guess_num = guess_num - 10; }
        }
    }
}

void set_unit() {
    //set unit
    if (k0 == 0) {
        my_delay();
        if (k0 == 0) { guess_num++; }
    }
    if (k1 == 0) {
        my_delay();
        if (k1 == 0) {
            if (guess_num == 0) { guess_num = 99; }
            else { guess_num--; }
        }
    }
}

void compare() {
    //compare user guess num and random num
//    show_bottom_string();
    if (guess_num == random_num) {
        enter_flag = 0;//reset enter flag
        key = 0;//reset key,lock input
        show_win();
        reset_game();
        index_page();//show index page
    } else if (guess_num > random_num) {
        show_top_string("    TOO BIG!    ");
        show_bottom_string("                ");
        enter_flag = 1;
    } else if (guess_num < random_num) {
        show_top_string("   TOO SMALL!   ");
        show_bottom_string("                ");
        enter_flag = 1;
    }
}

void scan_key() {
    //hook key press event
    if (k3 == 0) {//enter
        my_delay();
        if (k3 == 0) {
            enter_flag++;
            if (enter_flag == 1) {//first press enter,start game
                show_top_string("INP U GUESS NUM:");
                show_bottom_string("                ");
                show_value();
                key = 1;//press enter,unlock input
            }
        }
    }

    if (enter_flag == 2) {//second press enter,compare user input num and random num
        guess_count++;compare();
    }

    /***********-------------------------***********/
    if (key != 0) {
        if (k2 == 0) {//change set
            my_delay();
            if (k2 == 0) {
                key++;
                if (key > 2) { key = 1; }
            }
        }
        if (key == 1) {//first set unit
            set_unit();
            show_value();
        } else if (key == 2) {//second set ten
            set_ten();
            show_value();
        }
    }

    /***********-------------------------***********/
    if (k4 == 0) {//restart
        my_delay();
        if (k4 == 0) {
            reset_game();
            index_page();//show index page
        }
    }
}

void index_page() {
    show_top_string(" GUESS NUM GAME ");
    show_bottom_string(" scope : [1,100] ");
}

void reset_game() {
    //reset game
    guess_num = 0;//reset value
    guess_count = 0;//reset count
    enter_flag = 0;//reset enter flag
    key = 0;//reset key,lock input
    set_random_num();//set random num
}

void main() {
    set_random_num();
    init();
    index_page();
    while (1) {
        scan_key();
    }
}


void timer() interrupt 1 {
    if (guess_num > 100) {
        guess_num = 0;
    }

    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;
}


