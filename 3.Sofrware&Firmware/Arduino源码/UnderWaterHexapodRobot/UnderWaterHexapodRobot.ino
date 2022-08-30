/*******************************************************
   主板：Baize_ServoDriver_esp32
   功能：UnderWaterHexapodRobot水下六足机器人Arduino程序
   引脚：SDA:21   SCL:22
   对于ARDUINO UNO，SCL:A5，SDA:A4
   Designer: Allen
   E-mail:953598974@qq.com
   Date:2022-08-22
*******************************************************/
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define led 0
#define MAX_SRV_CLIENTS 3   //最大同时联接数，即你想要接入的设备数量，8266tcpserver只能接入五个

const char *ssid = "Baize"; 
const char *password = "baizerobot"; 

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();               //驱动1~16或(0~15)号舵机
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);          //驱动17~32或(16~31)号舵机

WiFiServer server(8266);//你要的端口号，随意修改，范围0-65535
WiFiClient serverClients[MAX_SRV_CLIENTS];

//#define SERVOMIN  102               //0.5/20 * 4096 = 102
//#define SERVOMID  307               //1.5/20 * 4096 = 307
//#define SERVOMAX  512               //2.5/20 * 4096 = 512
//实际测试
#define SERVOMIN  102               
#define SERVOMID  327               
#define SERVOMAX  552

//pwm.setPWM(i, 0, pulselen);第一个参数是通道数;第二个是高电平起始点，也就是从0开始;第三个参数是高电平终止点。

char cmd = 'e';//a:forward;   b:backward;   c:left;   d:right;   e:stop;

int rec[18] = {327,333,327,338,333,322,312,327,294,319,329,337,309,316,327,314,307,317};
int direct[18] = {-1,-1,-1,
-1,-1,-1,
-1,1,1,
-1,1,1,
-1,-1,-1,
-1,1,1
};

float forward[40][18] = {
16.11,-45.44,-44.11,-12.33,-44.23,-39.34,-16.30,-45.17,-48.35,16.11,-44.95,-43.75,-12.33,-44.68,-39.67,-16.30,-44.64,-47.97,
15.94,-46.87,-45.17,-12.21,-44.25,-39.42,-16.10,-46.72,-49.45,15.94,-44.96,-43.78,-12.21,-46.03,-40.72,-16.10,-44.65,-47.95,
15.49,-49.16,-46.88,-11.90,-44.30,-39.63,-15.56,-49.21,-51.17,15.49,-44.96,-43.85,-11.90,-48.20,-42.44,-15.56,-44.66,-47.90,
14.64,-52.16,-49.12,-11.31,-44.38,-40.00,-14.59,-52.48,-53.36,14.64,-44.97,-43.98,-11.31,-51.06,-44.78,-14.59,-44.69,-47.79,
13.32,-55.68,-51.72,-10.39,-44.50,-40.56,-13.09,-56.30,-55.79,13.32,-44.98,-44.16,-10.39,-54.44,-47.61,-13.09,-44.73,-47.62,
11.49,-59.42,-54.47,-9.08,-44.64,-41.30,-11.09,-60.33,-58.20,11.49,-44.99,-44.38,-9.08,-58.11,-50.75,-11.09,-44.79,-47.36,
9.16,-63.06,-57.11,-7.37,-44.78,-42.18,-8.64,-64.12,-60.28,9.16,-45.00,-44.61,-7.37,-61.79,-53.98,-8.64,-44.85,-46.98,
6.39,-66.16,-59.33,-5.25,-44.90,-43.13,-5.87,-67.13,-61.67,6.39,-45.00,-44.81,-5.25,-65.12,-57.03,-5.87,-44.92,-46.46,
3.29,-68.27,-60.83,-2.77,-44.97,-44.09,-2.93,-68.86,-62.08,3.29,-45.00,-44.95,-2.77,-67.67,-59.60,-2.93,-44.98,-45.80,
-0.00,-69.02,-61.36,0.00,-45.00,-45.00,-0.00,-69.02,-61.36,-0.00,-45.00,-45.00,0.00,-69.02,-61.36,-0.00,-45.00,-45.00,
-3.29,-68.27,-60.83,2.93,-44.98,-45.80,2.77,-67.67,-59.60,-3.29,-45.00,-44.95,2.93,-68.86,-62.08,2.77,-44.97,-44.09,
-6.39,-66.16,-59.33,5.87,-44.92,-46.46,5.25,-65.12,-57.03,-6.39,-45.00,-44.81,5.87,-67.13,-61.67,5.25,-44.90,-43.13,
-9.16,-63.06,-57.11,8.64,-44.85,-46.98,7.37,-61.79,-53.98,-9.16,-45.00,-44.61,8.64,-64.12,-60.28,7.37,-44.78,-42.18,
-11.49,-59.42,-54.47,11.09,-44.79,-47.36,9.08,-58.12,-50.75,-11.49,-44.99,-44.38,11.09,-60.33,-58.20,9.08,-44.64,-41.30,
-13.32,-55.68,-51.72,13.09,-44.73,-47.62,10.39,-54.44,-47.61,-13.32,-44.98,-44.16,13.09,-56.30,-55.79,10.39,-44.50,-40.56,
-14.64,-52.16,-49.12,14.59,-44.69,-47.79,11.31,-51.06,-44.78,-14.64,-44.97,-43.98,14.59,-52.48,-53.36,11.31,-44.38,-40.00,
-15.49,-49.16,-46.88,15.56,-44.66,-47.90,11.90,-48.20,-42.44,-15.49,-44.96,-43.85,15.56,-49.21,-51.17,11.90,-44.30,-39.63,
-15.94,-46.87,-45.17,16.10,-44.65,-47.95,12.21,-46.03,-40.72,-15.94,-44.96,-43.78,16.10,-46.72,-49.45,12.21,-44.25,-39.42,
-16.11,-45.44,-44.11,16.30,-44.64,-47.97,12.33,-44.69,-39.67,-16.11,-44.95,-43.75,16.30,-45.17,-48.35,12.33,-44.23,-39.34,
-16.14,-44.95,-43.75,16.33,-44.64,-47.97,12.35,-44.23,-39.33,-16.14,-44.95,-43.75,16.33,-44.64,-47.97,12.35,-44.23,-39.33,
-16.11,-44.95,-43.75,16.30,-45.17,-48.35,12.33,-44.23,-39.34,-16.11,-45.44,-44.11,16.30,-44.64,-47.97,12.33,-44.68,-39.67,
-15.94,-44.96,-43.78,16.10,-46.72,-49.45,12.21,-44.25,-39.42,-15.94,-46.87,-45.17,16.10,-44.65,-47.95,12.21,-46.03,-40.72,
-15.49,-44.96,-43.85,15.56,-49.21,-51.17,11.90,-44.30,-39.63,-15.49,-49.16,-46.88,15.56,-44.66,-47.90,11.90,-48.20,-42.44,
-14.64,-44.97,-43.98,14.59,-52.48,-53.36,11.31,-44.38,-40.00,-14.64,-52.16,-49.12,14.59,-44.69,-47.79,11.31,-51.06,-44.78,
-13.32,-44.98,-44.16,13.09,-56.30,-55.79,10.39,-44.50,-40.56,-13.32,-55.68,-51.72,13.09,-44.73,-47.62,10.39,-54.44,-47.61,
-11.49,-44.99,-44.38,11.09,-60.33,-58.20,9.08,-44.64,-41.30,-11.49,-59.42,-54.47,11.09,-44.79,-47.36,9.08,-58.11,-50.75,
-9.16,-45.00,-44.61,8.64,-64.12,-60.28,7.37,-44.78,-42.18,-9.16,-63.06,-57.11,8.64,-44.85,-46.98,7.37,-61.79,-53.98,
-6.39,-45.00,-44.81,5.87,-67.13,-61.67,5.25,-44.90,-43.13,-6.39,-66.16,-59.33,5.87,-44.92,-46.46,5.25,-65.12,-57.03,
-3.29,-45.00,-44.95,2.93,-68.86,-62.08,2.77,-44.97,-44.09,-3.29,-68.27,-60.83,2.93,-44.98,-45.80,2.77,-67.67,-59.60,
0.00,-45.00,-45.00,0.00,-69.02,-61.36,-0.00,-45.00,-45.00,-0.00,-69.02,-61.36,0.00,-45.00,-45.00,-0.00,-69.02,-61.36,
3.29,-45.00,-44.95,-2.77,-67.67,-59.60,-2.93,-44.98,-45.80,3.29,-68.27,-60.83,-2.77,-44.97,-44.09,-2.93,-68.86,-62.08,
6.39,-45.00,-44.81,-5.25,-65.12,-57.03,-5.87,-44.92,-46.46,6.39,-66.16,-59.33,-5.25,-44.90,-43.13,-5.87,-67.13,-61.67,
9.16,-45.00,-44.61,-7.37,-61.79,-53.98,-8.64,-44.85,-46.98,9.16,-63.06,-57.11,-7.37,-44.78,-42.18,-8.64,-64.12,-60.28,
11.49,-44.99,-44.38,-9.08,-58.11,-50.75,-11.09,-44.79,-47.36,11.49,-59.42,-54.47,-9.08,-44.64,-41.30,-11.09,-60.33,-58.20,
13.32,-44.98,-44.16,-10.39,-54.44,-47.61,-13.09,-44.73,-47.62,13.32,-55.68,-51.72,-10.39,-44.50,-40.56,-13.09,-56.30,-55.79,
14.64,-44.97,-43.98,-11.31,-51.06,-44.78,-14.59,-44.69,-47.79,14.64,-52.16,-49.12,-11.31,-44.38,-40.00,-14.59,-52.48,-53.36,
15.49,-44.96,-43.85,-11.90,-48.20,-42.44,-15.56,-44.66,-47.90,15.49,-49.16,-46.88,-11.90,-44.30,-39.63,-15.56,-49.21,-51.17,
15.94,-44.96,-43.78,-12.21,-46.03,-40.72,-16.10,-44.65,-47.95,15.94,-46.87,-45.17,-12.21,-44.25,-39.42,-16.10,-46.72,-49.45,
16.11,-44.95,-43.75,-12.33,-44.68,-39.67,-16.30,-44.64,-47.97,16.11,-45.44,-44.11,-12.33,-44.23,-39.34,-16.30,-45.17,-48.35,
16.14,-44.95,-43.75,-12.35,-44.23,-39.33,-16.33,-44.64,-47.97,16.14,-44.95,-43.75,-12.35,-44.23,-39.33,-16.33,-44.64,-47.97
};
//波动步态前进
float forwardF[120][18] = {
26.16,-45.12,-41.70,-12.55,-44.20,-39.19,-16.70,-44.63,-48.00,5.61,-45.00,-44.86,-4.65,-44.92,-43.38,-29.45,-44.45,-48.58,
25.90,-46.50,-42.78,-12.51,-44.20,-39.22,-16.77,-44.63,-48.01,5.55,-45.00,-44.86,-4.70,-44.92,-43.36,-29.38,-44.45,-48.58,
25.22,-48.74,-44.56,-12.41,-44.22,-39.29,-16.95,-44.63,-48.03,5.38,-45.00,-44.87,-4.83,-44.92,-43.30,-29.19,-44.45,-48.58,
23.94,-51.69,-46.98,-12.21,-44.25,-39.42,-17.29,-44.62,-48.06,5.08,-45.00,-44.88,-5.07,-44.91,-43.21,-28.83,-44.45,-48.58,
21.93,-55.18,-49.88,-11.91,-44.30,-39.62,-17.82,-44.61,-48.10,4.60,-45.00,-44.90,-5.44,-44.89,-43.05,-28.28,-44.45,-48.57,
19.07,-58.95,-53.03,-11.49,-44.36,-39.89,-18.54,-44.59,-48.16,3.96,-45.00,-44.93,-5.93,-44.87,-42.84,-27.53,-44.46,-48.56,
15.34,-62.69,-56.15,-10.95,-44.43,-40.22,-19.45,-44.57,-48.22,3.14,-45.00,-44.96,-6.55,-44.83,-42.56,-26.59,-44.46,-48.54,
10.78,-65.94,-58.84,-10.32,-44.51,-40.61,-20.52,-44.55,-48.29,2.18,-45.00,-44.98,-7.26,-44.79,-42.23,-25.49,-44.47,-48.52,
5.57,-68.21,-60.70,-9.59,-44.59,-41.02,-21.72,-44.53,-48.36,1.12,-45.00,-44.99,-8.03,-44.73,-41.85,-24.27,-44.49,-48.48,
-0.00,-69.02,-61.36,-8.82,-44.67,-41.44,-22.99,-44.51,-48.43,-0.00,-45.00,-45.00,-8.82,-44.67,-41.44,-22.99,-44.51,-48.43,
-5.57,-68.21,-60.70,-8.03,-44.73,-41.85,-24.27,-44.49,-48.48,-1.12,-45.00,-44.99,-9.59,-44.59,-41.02,-21.72,-44.53,-48.36,
-10.78,-65.94,-58.84,-7.26,-44.79,-42.23,-25.49,-44.47,-48.52,-2.18,-45.00,-44.98,-10.32,-44.51,-40.61,-20.52,-44.55,-48.29,
-15.34,-62.69,-56.15,-6.55,-44.83,-42.56,-26.59,-44.46,-48.54,-3.14,-45.00,-44.96,-10.95,-44.43,-40.22,-19.45,-44.57,-48.22,
-19.07,-58.96,-53.03,-5.93,-44.87,-42.84,-27.53,-44.46,-48.56,-3.96,-45.00,-44.93,-11.49,-44.36,-39.89,-18.54,-44.59,-48.16,
-21.93,-55.18,-49.88,-5.44,-44.89,-43.05,-28.28,-44.45,-48.57,-4.60,-45.00,-44.91,-11.91,-44.30,-39.62,-17.82,-44.61,-48.10,
-23.94,-51.69,-46.98,-5.07,-44.91,-43.21,-28.83,-44.45,-48.58,-5.08,-45.00,-44.88,-12.21,-44.25,-39.42,-17.29,-44.62,-48.06,
-25.22,-48.74,-44.56,-4.83,-44.92,-43.31,-29.19,-44.45,-48.58,-5.38,-45.00,-44.87,-12.41,-44.22,-39.29,-16.95,-44.63,-48.03,
-25.90,-46.50,-42.78,-4.70,-44.92,-43.36,-29.38,-44.45,-48.58,-5.55,-45.00,-44.86,-12.51,-44.20,-39.22,-16.77,-44.63,-48.01,
-26.16,-45.12,-41.70,-4.65,-44.92,-43.38,-29.46,-44.45,-48.58,-5.61,-45.00,-44.86,-12.55,-44.20,-39.19,-16.70,-44.63,-48.00,
-26.19,-44.65,-41.34,-4.64,-44.92,-43.38,-29.47,-44.45,-48.58,-5.62,-45.00,-44.86,-12.56,-44.20,-39.19,-16.69,-44.63,-48.00,
-26.19,-44.65,-41.35,-4.64,-44.92,-43.38,-29.41,-44.98,-48.97,-5.63,-45.00,-44.86,-12.56,-44.19,-39.18,-16.68,-44.63,-48.00,
-26.14,-44.65,-41.36,-4.59,-44.93,-43.40,-29.05,-46.55,-50.10,-5.69,-45.00,-44.85,-12.60,-44.19,-39.16,-16.61,-44.64,-48.00,
-26.00,-44.66,-41.41,-4.46,-44.93,-43.46,-28.08,-49.06,-51.89,-5.86,-45.00,-44.84,-12.71,-44.17,-39.09,-16.43,-44.64,-47.98,
-25.75,-44.67,-41.49,-4.21,-44.94,-43.55,-26.31,-52.39,-54.20,-6.16,-45.00,-44.83,-12.90,-44.14,-38.95,-16.09,-44.65,-47.95,
-25.36,-44.69,-41.61,-3.83,-44.95,-43.70,-23.58,-56.31,-56.79,-6.63,-45.00,-44.80,-13.19,-44.09,-38.75,-15.58,-44.66,-47.90,
-24.83,-44.72,-41.77,-3.31,-44.96,-43.90,-19.90,-60.49,-59.37,-7.27,-45.00,-44.76,-13.59,-44.01,-38.46,-14.87,-44.68,-47.82,
-24.15,-44.75,-41.97,-2.65,-44.98,-44.14,-15.39,-64.45,-61.53,-8.08,-45.00,-44.70,-14.08,-43.92,-38.09,-14.00,-44.71,-47.73,
-23.35,-44.78,-42.19,-1.85,-44.99,-44.41,-10.33,-67.56,-62.82,-9.02,-45.00,-44.63,-14.65,-43.79,-37.65,-12.98,-44.73,-47.61,
-22.44,-44.82,-42.43,-0.96,-45.00,-44.70,-5.08,-69.20,-62.82,-10.05,-44.99,-44.53,-15.26,-43.65,-37.16,-11.86,-44.77,-47.46,
-21.48,-44.85,-42.67,0.00,-45.00,-45.00,-0.00,-69.02,-61.36,-11.13,-44.99,-44.42,-15.90,-43.50,-36.62,-10.70,-44.80,-47.30,
-20.51,-44.87,-42.90,0.98,-45.00,-45.28,4.61,-67.19,-58.62,-12.20,-44.98,-44.30,-16.51,-43.33,-36.08,-9.56,-44.83,-47.13,
-19.57,-44.90,-43.10,1.93,-44.99,-45.54,8.56,-64.14,-54.98,-13.22,-44.98,-44.18,-17.09,-43.16,-35.55,-8.49,-44.86,-46.95,
-18.71,-44.91,-43.28,2.80,-44.98,-45.77,11.80,-60.41,-50.90,-14.12,-44.97,-44.05,-17.60,-43.00,-35.06,-7.54,-44.88,-46.78,
-17.97,-44.93,-43.42,3.55,-44.97,-45.95,14.32,-56.46,-46.78,-14.89,-44.97,-43.94,-18.03,-42.86,-34.64,-6.73,-44.90,-46.63,
-17.38,-44.94,-43.53,4.16,-44.96,-46.09,16.18,-52.63,-42.95,-15.50,-44.96,-43.85,-18.36,-42.75,-34.30,-6.10,-44.92,-46.51,
-16.95,-44.94,-43.61,4.60,-44.95,-46.19,17.47,-49.21,-39.64,-15.94,-44.96,-43.78,-18.60,-42.66,-34.05,-5.64,-44.93,-46.41,
-16.66,-44.95,-43.66,4.89,-44.94,-46.26,18.27,-46.37,-37.01,-16.22,-44.95,-43.73,-18.76,-42.60,-33.89,-5.35,-44.94,-46.35,
-16.51,-44.95,-43.69,5.05,-44.94,-46.29,18.69,-44.27,-35.16,-16.38,-44.95,-43.71,-18.84,-42.57,-33.80,-5.19,-44.94,-46.32,
-16.45,-44.95,-43.70,5.11,-44.94,-46.30,18.85,-42.98,-34.09,-16.44,-44.95,-43.70,-18.87,-42.56,-33.77,-5.13,-44.94,-46.31,
-16.44,-44.95,-43.70,5.12,-44.94,-46.30,18.88,-42.56,-33.76,-16.44,-44.95,-43.70,-18.88,-42.56,-33.76,-5.12,-44.94,-46.30,
-16.44,-44.95,-43.70,5.13,-44.94,-46.31,18.87,-42.56,-33.77,-16.45,-44.95,-43.70,-18.85,-42.98,-34.09,-5.11,-44.94,-46.30,
-16.38,-44.95,-43.71,5.19,-44.94,-46.32,18.84,-42.57,-33.80,-16.51,-44.95,-43.69,-18.69,-44.27,-35.16,-5.05,-44.94,-46.29,
-16.22,-44.95,-43.73,5.35,-44.94,-46.35,18.76,-42.60,-33.89,-16.66,-44.95,-43.66,-18.27,-46.37,-37.01,-4.89,-44.94,-46.25,
-15.94,-44.96,-43.78,5.64,-44.93,-46.41,18.60,-42.66,-34.05,-16.95,-44.94,-43.61,-17.47,-49.21,-39.64,-4.60,-44.95,-46.19,
-15.50,-44.96,-43.85,6.10,-44.92,-46.51,18.36,-42.75,-34.30,-17.38,-44.94,-43.53,-16.18,-52.63,-42.95,-4.16,-44.96,-46.09,
-14.89,-44.97,-43.94,6.73,-44.90,-46.63,18.03,-42.86,-34.64,-17.97,-44.93,-43.42,-14.32,-56.46,-46.78,-3.55,-44.97,-45.95,
-14.12,-44.97,-44.05,7.54,-44.88,-46.78,17.60,-43.00,-35.06,-18.71,-44.91,-43.28,-11.80,-60.41,-50.90,-2.80,-44.98,-45.77,
-13.22,-44.98,-44.18,8.49,-44.86,-46.95,17.09,-43.16,-35.55,-19.57,-44.90,-43.10,-8.56,-64.14,-54.98,-1.93,-44.99,-45.54,
-12.20,-44.98,-44.30,9.56,-44.83,-47.13,16.51,-43.33,-36.08,-20.51,-44.87,-42.90,-4.61,-67.19,-58.62,-0.98,-45.00,-45.28,
-11.13,-44.99,-44.42,10.70,-44.80,-47.30,15.90,-43.50,-36.62,-21.48,-44.85,-42.67,0.00,-69.02,-61.36,-0.00,-45.00,-45.00,
-10.05,-44.99,-44.53,11.86,-44.77,-47.46,15.26,-43.65,-37.16,-22.44,-44.82,-42.43,5.08,-69.19,-62.82,0.96,-45.00,-44.70,
-9.02,-45.00,-44.63,12.98,-44.73,-47.61,14.65,-43.79,-37.65,-23.35,-44.78,-42.19,10.33,-67.56,-62.82,1.85,-44.99,-44.41,
-8.08,-45.00,-44.70,14.00,-44.71,-47.73,14.08,-43.92,-38.09,-24.15,-44.75,-41.97,15.39,-64.45,-61.53,2.65,-44.98,-44.14,
-7.27,-45.00,-44.76,14.87,-44.68,-47.82,13.59,-44.01,-38.46,-24.83,-44.72,-41.77,19.90,-60.49,-59.37,3.31,-44.96,-43.90,
-6.63,-45.00,-44.80,15.58,-44.66,-47.90,13.19,-44.09,-38.75,-25.36,-44.69,-41.61,23.58,-56.31,-56.79,3.83,-44.95,-43.70,
-6.16,-45.00,-44.83,16.09,-44.65,-47.95,12.90,-44.14,-38.95,-25.75,-44.67,-41.49,26.31,-52.39,-54.20,4.21,-44.94,-43.55,
-5.86,-45.00,-44.84,16.43,-44.64,-47.98,12.71,-44.17,-39.09,-26.00,-44.66,-41.41,28.08,-49.06,-51.89,4.46,-44.93,-43.46,
-5.69,-45.00,-44.85,16.61,-44.64,-48.00,12.60,-44.19,-39.16,-26.14,-44.65,-41.36,29.05,-46.55,-50.10,4.59,-44.93,-43.40,
-5.63,-45.00,-44.86,16.68,-44.63,-48.00,12.56,-44.19,-39.18,-26.19,-44.65,-41.35,29.41,-44.98,-48.97,4.64,-44.92,-43.38,
-5.62,-45.00,-44.86,16.69,-44.63,-48.00,12.56,-44.20,-39.19,-26.19,-44.65,-41.34,29.47,-44.45,-48.58,4.64,-44.92,-43.38,
-5.61,-45.00,-44.86,16.70,-44.63,-48.00,12.55,-44.20,-39.19,-26.16,-45.12,-41.70,29.46,-44.45,-48.58,4.65,-44.92,-43.38,
-5.55,-45.00,-44.86,16.77,-44.63,-48.01,12.51,-44.20,-39.22,-25.90,-46.50,-42.78,29.38,-44.45,-48.58,4.70,-44.92,-43.36,
-5.38,-45.00,-44.87,16.95,-44.63,-48.03,12.41,-44.22,-39.29,-25.22,-48.74,-44.56,29.19,-44.45,-48.58,4.83,-44.92,-43.30,
-5.08,-45.00,-44.88,17.29,-44.62,-48.06,12.21,-44.25,-39.42,-23.94,-51.69,-46.98,28.83,-44.45,-48.58,5.07,-44.91,-43.21,
-4.60,-45.00,-44.90,17.82,-44.61,-48.10,11.91,-44.30,-39.62,-21.93,-55.18,-49.88,28.28,-44.46,-48.57,5.44,-44.89,-43.05,
-3.96,-45.00,-44.93,18.54,-44.59,-48.16,11.49,-44.36,-39.89,-19.07,-58.95,-53.03,27.53,-44.46,-48.56,5.93,-44.87,-42.84,
-3.14,-45.00,-44.96,19.45,-44.57,-48.22,10.95,-44.43,-40.22,-15.34,-62.69,-56.15,26.59,-44.46,-48.54,6.55,-44.83,-42.56,
-2.18,-45.00,-44.98,20.52,-44.55,-48.29,10.32,-44.51,-40.61,-10.78,-65.94,-58.84,25.49,-44.47,-48.52,7.26,-44.79,-42.23,
-1.12,-45.00,-44.99,21.72,-44.53,-48.36,9.59,-44.59,-41.02,-5.57,-68.21,-60.70,24.27,-44.49,-48.48,8.03,-44.73,-41.85,
0.00,-45.00,-45.00,22.99,-44.51,-48.43,8.82,-44.67,-41.44,0.00,-69.02,-61.36,22.99,-44.51,-48.43,8.82,-44.67,-41.44,
1.12,-45.00,-44.99,24.27,-44.49,-48.48,8.03,-44.73,-41.85,5.57,-68.21,-60.70,21.72,-44.53,-48.36,9.59,-44.59,-41.02,
2.18,-45.00,-44.98,25.49,-44.47,-48.52,7.26,-44.79,-42.23,10.78,-65.94,-58.84,20.52,-44.55,-48.29,10.32,-44.51,-40.61,
3.14,-45.00,-44.96,26.59,-44.46,-48.54,6.55,-44.83,-42.56,15.34,-62.69,-56.15,19.45,-44.57,-48.22,10.95,-44.43,-40.22,
3.96,-45.00,-44.93,27.53,-44.46,-48.56,5.93,-44.87,-42.84,19.07,-58.96,-53.03,18.54,-44.59,-48.16,11.49,-44.36,-39.89,
4.60,-45.00,-44.90,28.28,-44.45,-48.57,5.44,-44.89,-43.05,21.93,-55.18,-49.88,17.82,-44.61,-48.10,11.91,-44.30,-39.62,
5.08,-45.00,-44.88,28.83,-44.45,-48.58,5.07,-44.91,-43.21,23.94,-51.69,-46.98,17.29,-44.62,-48.06,12.21,-44.25,-39.42,
5.38,-45.00,-44.87,29.19,-44.45,-48.58,4.83,-44.92,-43.31,25.22,-48.74,-44.56,16.95,-44.63,-48.03,12.41,-44.22,-39.29,
5.55,-45.00,-44.86,29.38,-44.45,-48.58,4.70,-44.92,-43.36,25.90,-46.50,-42.78,16.77,-44.63,-48.01,12.51,-44.20,-39.22,
5.61,-45.00,-44.86,29.46,-44.45,-48.58,4.65,-44.92,-43.38,26.16,-45.12,-41.70,16.70,-44.63,-48.00,12.55,-44.20,-39.19,
5.62,-45.00,-44.86,29.47,-44.45,-48.58,4.64,-44.92,-43.38,26.19,-44.65,-41.34,16.69,-44.63,-48.00,12.56,-44.20,-39.19,
5.63,-45.00,-44.86,29.41,-44.98,-48.97,4.64,-44.92,-43.38,26.19,-44.65,-41.35,16.68,-44.63,-48.00,12.56,-44.19,-39.18,
5.69,-45.00,-44.85,29.05,-46.55,-50.10,4.59,-44.93,-43.40,26.14,-44.65,-41.36,16.61,-44.64,-48.00,12.60,-44.19,-39.16,
5.86,-45.00,-44.84,28.08,-49.06,-51.89,4.46,-44.93,-43.46,26.00,-44.66,-41.41,16.43,-44.64,-47.98,12.71,-44.17,-39.09,
6.16,-45.00,-44.83,26.31,-52.39,-54.20,4.21,-44.94,-43.55,25.75,-44.67,-41.49,16.09,-44.65,-47.95,12.90,-44.14,-38.95,
6.63,-45.00,-44.80,23.58,-56.31,-56.79,3.83,-44.95,-43.70,25.36,-44.69,-41.61,15.58,-44.66,-47.90,13.19,-44.09,-38.75,
7.27,-45.00,-44.76,19.90,-60.49,-59.37,3.31,-44.96,-43.90,24.83,-44.72,-41.77,14.87,-44.68,-47.82,13.59,-44.01,-38.46,
8.08,-45.00,-44.70,15.39,-64.45,-61.53,2.65,-44.98,-44.14,24.15,-44.75,-41.97,14.00,-44.71,-47.73,14.08,-43.92,-38.09,
9.02,-45.00,-44.63,10.33,-67.56,-62.82,1.85,-44.99,-44.41,23.35,-44.78,-42.19,12.98,-44.73,-47.61,14.65,-43.79,-37.65,
10.05,-44.99,-44.53,5.08,-69.19,-62.82,0.96,-45.00,-44.70,22.44,-44.82,-42.43,11.86,-44.77,-47.46,15.26,-43.65,-37.16,
11.13,-44.99,-44.42,0.00,-69.02,-61.36,-0.00,-45.00,-45.00,21.48,-44.85,-42.67,10.70,-44.80,-47.30,15.90,-43.50,-36.62,
12.20,-44.98,-44.30,-4.61,-67.19,-58.62,-0.98,-45.00,-45.28,20.51,-44.87,-42.90,9.56,-44.83,-47.13,16.51,-43.33,-36.08,
13.22,-44.98,-44.18,-8.56,-64.14,-54.98,-1.93,-44.99,-45.54,19.57,-44.90,-43.10,8.49,-44.86,-46.95,17.09,-43.16,-35.55,
14.12,-44.97,-44.05,-11.80,-60.41,-50.90,-2.80,-44.98,-45.77,18.71,-44.91,-43.28,7.54,-44.88,-46.78,17.60,-43.00,-35.06,
14.89,-44.97,-43.94,-14.32,-56.46,-46.78,-3.55,-44.97,-45.95,17.97,-44.93,-43.42,6.73,-44.90,-46.63,18.03,-42.86,-34.64,
15.50,-44.96,-43.85,-16.18,-52.63,-42.95,-4.16,-44.96,-46.09,17.38,-44.94,-43.53,6.10,-44.92,-46.51,18.36,-42.75,-34.30,
15.94,-44.96,-43.78,-17.47,-49.20,-39.64,-4.60,-44.95,-46.19,16.95,-44.94,-43.61,5.64,-44.93,-46.41,18.60,-42.66,-34.05,
16.22,-44.95,-43.73,-18.27,-46.37,-37.01,-4.89,-44.94,-46.26,16.66,-44.95,-43.66,5.35,-44.94,-46.35,18.76,-42.60,-33.89,
16.38,-44.95,-43.71,-18.69,-44.27,-35.16,-5.05,-44.94,-46.29,16.51,-44.95,-43.69,5.19,-44.94,-46.32,18.84,-42.57,-33.80,
16.44,-44.95,-43.70,-18.85,-42.98,-34.09,-5.11,-44.94,-46.30,16.45,-44.95,-43.70,5.13,-44.94,-46.31,18.87,-42.56,-33.77,
16.44,-44.95,-43.70,-18.88,-42.56,-33.76,-5.12,-44.94,-46.30,16.44,-44.95,-43.70,5.12,-44.94,-46.30,18.88,-42.56,-33.76,
16.45,-44.95,-43.70,-18.87,-42.56,-33.77,-5.13,-44.94,-46.31,16.44,-44.95,-43.70,5.11,-44.94,-46.30,18.85,-42.98,-34.09,
16.51,-44.95,-43.69,-18.84,-42.57,-33.80,-5.19,-44.94,-46.32,16.38,-44.95,-43.71,5.05,-44.94,-46.29,18.69,-44.27,-35.16,
16.66,-44.95,-43.66,-18.76,-42.60,-33.89,-5.35,-44.94,-46.35,16.22,-44.95,-43.73,4.89,-44.94,-46.25,18.27,-46.37,-37.01,
16.95,-44.94,-43.61,-18.60,-42.66,-34.05,-5.64,-44.93,-46.41,15.94,-44.96,-43.78,4.60,-44.95,-46.19,17.47,-49.21,-39.64,
17.38,-44.94,-43.53,-18.36,-42.75,-34.30,-6.10,-44.92,-46.51,15.50,-44.96,-43.85,4.16,-44.96,-46.09,16.18,-52.63,-42.95,
17.97,-44.93,-43.42,-18.03,-42.86,-34.64,-6.73,-44.90,-46.63,14.89,-44.97,-43.94,3.55,-44.97,-45.95,14.32,-56.46,-46.78,
18.71,-44.91,-43.28,-17.60,-43.00,-35.06,-7.54,-44.88,-46.78,14.12,-44.97,-44.05,2.80,-44.98,-45.77,11.80,-60.41,-50.90,
19.57,-44.90,-43.10,-17.09,-43.16,-35.55,-8.49,-44.86,-46.95,13.22,-44.98,-44.18,1.93,-44.99,-45.54,8.56,-64.14,-54.98,
20.51,-44.87,-42.90,-16.51,-43.33,-36.08,-9.56,-44.83,-47.13,12.20,-44.98,-44.30,0.98,-45.00,-45.28,4.61,-67.19,-58.62,
21.48,-44.85,-42.67,-15.90,-43.50,-36.62,-10.70,-44.80,-47.30,11.13,-44.99,-44.42,0.00,-45.00,-45.00,-0.00,-69.02,-61.36,
22.44,-44.82,-42.43,-15.26,-43.65,-37.16,-11.86,-44.77,-47.46,10.05,-44.99,-44.53,-0.96,-45.00,-44.70,-5.08,-69.19,-62.82,
23.35,-44.78,-42.19,-14.65,-43.79,-37.65,-12.98,-44.73,-47.61,9.02,-45.00,-44.63,-1.85,-44.99,-44.41,-10.33,-67.56,-62.82,
24.15,-44.75,-41.97,-14.08,-43.92,-38.09,-14.00,-44.71,-47.73,8.08,-45.00,-44.70,-2.65,-44.98,-44.14,-15.39,-64.45,-61.53,
24.83,-44.72,-41.77,-13.59,-44.01,-38.46,-14.87,-44.68,-47.82,7.27,-45.00,-44.76,-3.31,-44.96,-43.90,-19.90,-60.49,-59.37,
25.36,-44.69,-41.61,-13.19,-44.09,-38.75,-15.58,-44.66,-47.90,6.63,-45.00,-44.80,-3.83,-44.95,-43.70,-23.58,-56.31,-56.79,
25.75,-44.67,-41.49,-12.90,-44.14,-38.95,-16.09,-44.65,-47.95,6.16,-45.00,-44.83,-4.21,-44.94,-43.55,-26.31,-52.39,-54.20,
26.00,-44.66,-41.41,-12.71,-44.17,-39.09,-16.43,-44.64,-47.98,5.86,-45.00,-44.84,-4.46,-44.93,-43.46,-28.08,-49.06,-51.89,
26.14,-44.65,-41.36,-12.60,-44.19,-39.16,-16.61,-44.64,-48.00,5.69,-45.00,-44.85,-4.59,-44.93,-43.40,-29.05,-46.55,-50.10,
26.19,-44.65,-41.35,-12.56,-44.19,-39.18,-16.68,-44.63,-48.00,5.63,-45.00,-44.86,-4.64,-44.92,-43.38,-29.41,-44.98,-48.97,
26.19,-44.65,-41.34,-12.56,-44.20,-39.19,-16.69,-44.63,-48.00,5.62,-45.00,-44.86,-4.64,-44.92,-43.38,-29.47,-44.45,-48.58
};

//波动步态右移
float forwardFH[120][18] = {
-0.00,-26.47,-14.15,-8.30,-28.84,-35.49,8.32,-28.84,-35.49,0.00,-29.83,-27.29,-2.39,-29.87,-32.10,16.31,-26.95,-37.80,
-0.00,-27.58,-15.21,-8.27,-28.85,-35.47,8.35,-28.83,-35.51,0.00,-29.83,-27.32,-2.42,-29.87,-32.13,16.26,-26.96,-37.79,
-0.00,-29.42,-17.09,-8.16,-28.87,-35.43,8.46,-28.81,-35.55,0.00,-29.84,-27.40,-2.49,-29.86,-32.18,16.12,-26.99,-37.76,
-0.00,-31.89,-19.79,-7.98,-28.91,-35.35,8.64,-28.77,-35.63,0.00,-29.86,-27.56,-2.64,-29.84,-32.29,15.87,-27.05,-37.71,
-0.00,-34.84,-23.23,-7.70,-28.97,-35.23,8.94,-28.70,-35.74,0.00,-29.89,-27.80,-2.86,-29.82,-32.45,15.49,-27.14,-37.63,
-0.00,-38.09,-27.24,-7.31,-29.06,-35.05,9.35,-28.61,-35.90,0.00,-29.92,-28.12,-3.16,-29.78,-32.67,14.97,-27.27,-37.52,
-0.00,-41.35,-31.59,-6.84,-29.15,-34.83,9.88,-28.49,-36.09,0.00,-29.95,-28.52,-3.55,-29.73,-32.94,14.33,-27.42,-37.38,
-0.00,-44.32,-36.01,-6.31,-29.26,-34.57,10.51,-28.34,-36.31,0.00,-29.98,-28.99,-4.02,-29.66,-33.25,13.60,-27.60,-37.20,
-0.00,-46.64,-40.14,-5.72,-29.37,-34.26,11.23,-28.17,-36.54,0.00,-30.00,-29.49,-4.55,-29.58,-33.59,12.81,-27.79,-37.00,
-0.00,-47.96,-43.66,-5.13,-29.48,-33.93,12.01,-27.98,-36.78,0.00,-30.01,-30.01,-5.13,-29.48,-33.93,12.01,-27.98,-36.78,
-0.00,-47.96,-46.22,-4.55,-29.58,-33.59,12.81,-27.79,-37.00,-0.00,-30.00,-30.51,-5.72,-29.37,-34.26,11.23,-28.17,-36.54,
0.00,-46.52,-47.62,-4.02,-29.66,-33.25,13.60,-27.60,-37.20,-0.00,-29.98,-30.98,-6.31,-29.26,-34.57,10.51,-28.34,-36.31,
0.00,-43.79,-47.82,-3.55,-29.73,-32.94,14.33,-27.42,-37.38,-0.00,-29.95,-31.40,-6.84,-29.15,-34.83,9.88,-28.49,-36.09,
0.00,-40.16,-47.01,-3.16,-29.78,-32.67,14.97,-27.27,-37.52,-0.00,-29.92,-31.74,-7.31,-29.06,-35.05,9.35,-28.61,-35.90,
0.00,-36.20,-45.50,-2.86,-29.82,-32.45,15.49,-27.14,-37.63,-0.00,-29.88,-32.01,-7.70,-28.97,-35.23,8.94,-28.70,-35.74,
0.00,-32.43,-43.66,-2.64,-29.84,-32.29,15.87,-27.05,-37.71,-0.00,-29.86,-32.21,-7.98,-28.91,-35.35,8.64,-28.77,-35.63,
0.00,-29.25,-41.83,-2.49,-29.86,-32.18,16.12,-26.99,-37.76,-0.00,-29.84,-32.34,-8.16,-28.87,-35.43,8.45,-28.81,-35.55,
0.00,-26.90,-40.31,-2.42,-29.87,-32.13,16.26,-26.96,-37.79,-0.00,-29.82,-32.40,-8.27,-28.85,-35.47,8.35,-28.83,-35.51,
0.00,-25.50,-39.30,-2.39,-29.87,-32.10,16.31,-26.95,-37.80,-0.00,-29.82,-32.43,-8.30,-28.84,-35.49,8.32,-28.84,-35.49,
0.00,-25.04,-38.94,-2.39,-29.87,-32.10,16.32,-26.94,-37.80,-0.00,-29.82,-32.43,-8.31,-28.84,-35.49,8.31,-28.84,-35.49,
0.00,-25.05,-38.94,-2.38,-29.87,-32.10,16.28,-27.39,-38.15,-0.00,-29.82,-32.44,-8.32,-28.84,-35.49,8.30,-28.84,-35.49,
0.00,-25.06,-38.93,-2.35,-29.87,-32.07,16.02,-28.73,-39.15,-0.00,-29.82,-32.46,-8.35,-28.83,-35.51,8.27,-28.85,-35.47,
0.00,-25.13,-38.90,-2.28,-29.88,-32.02,15.35,-30.92,-40.65,-0.00,-29.80,-32.53,-8.46,-28.81,-35.55,8.16,-28.87,-35.43,
0.00,-25.25,-38.85,-2.14,-29.89,-31.91,14.14,-33.84,-42.45,-0.00,-29.78,-32.65,-8.64,-28.77,-35.63,7.98,-28.91,-35.35,
0.00,-25.42,-38.77,-1.93,-29.91,-31.74,12.37,-37.24,-44.29,-0.00,-29.75,-32.84,-8.94,-28.70,-35.74,7.70,-28.97,-35.23,
0.00,-25.65,-38.66,-1.64,-29.94,-31.50,10.14,-40.80,-45.86,-0.00,-29.70,-33.10,-9.35,-28.61,-35.90,7.31,-29.06,-35.05,
0.00,-25.93,-38.50,-1.29,-29.96,-31.20,7.59,-44.06,-46.82,-0.00,-29.63,-33.42,-9.88,-28.49,-36.09,6.84,-29.15,-34.83,
0.00,-26.24,-38.32,-0.88,-29.98,-30.84,4.94,-46.56,-46.87,-0.00,-29.53,-33.78,-10.51,-28.34,-36.31,6.31,-29.26,-34.57,
0.00,-26.58,-38.09,-0.45,-29.99,-30.44,2.36,-47.91,-45.82,-0.00,-29.40,-34.17,-11.23,-28.17,-36.54,5.72,-29.37,-34.26,
0.00,-26.91,-37.85,-0.00,-30.00,-30.00,0.00,-47.96,-43.66,-0.00,-29.27,-34.57,-12.01,-27.98,-36.78,5.13,-29.48,-33.93,
0.00,-27.22,-37.58,0.44,-29.99,-29.55,-2.07,-46.76,-40.55,-0.00,-29.11,-34.95,-12.81,-27.79,-37.00,4.55,-29.58,-33.59,
0.00,-27.50,-37.32,0.84,-29.98,-29.12,-3.81,-44.58,-36.79,-0.00,-28.94,-35.31,-13.60,-27.60,-37.20,4.02,-29.66,-33.25,
0.00,-27.74,-37.07,1.20,-29.96,-28.71,-5.21,-41.75,-32.70,-0.00,-28.78,-35.62,-14.33,-27.42,-37.38,3.55,-29.73,-32.94,
0.00,-27.93,-36.85,1.49,-29.94,-28.37,-6.30,-38.60,-28.61,-0.00,-28.63,-35.87,-14.97,-27.27,-37.52,3.16,-29.78,-32.67,
0.00,-28.08,-36.67,1.73,-29.91,-28.09,-7.11,-35.45,-24.80,-0.00,-28.51,-36.07,-15.49,-27.14,-37.63,2.86,-29.82,-32.45,
0.00,-28.18,-36.53,1.89,-29.89,-27.88,-7.66,-32.54,-21.51,-0.00,-28.41,-36.21,-15.87,-27.05,-37.71,2.64,-29.84,-32.29,
0.00,-28.25,-36.44,2.00,-29.88,-27.74,-8.01,-30.10,-18.89,-0.00,-28.35,-36.30,-16.12,-26.99,-37.76,2.49,-29.86,-32.18,
0.00,-28.28,-36.39,2.06,-29.87,-27.67,-8.19,-28.28,-17.06,-0.00,-28.31,-36.35,-16.26,-26.96,-37.79,2.42,-29.87,-32.13,
0.00,-28.29,-36.37,2.08,-29.87,-27.64,-8.26,-27.17,-16.01,-0.00,-28.30,-36.37,-16.31,-26.95,-37.80,2.39,-29.87,-32.10,
0.00,-28.30,-36.37,2.08,-29.87,-27.64,-8.27,-26.80,-15.68,-0.00,-28.30,-36.37,-16.32,-26.94,-37.80,2.39,-29.87,-32.10,
0.00,-28.30,-36.37,2.09,-29.87,-27.63,-8.27,-26.80,-15.69,-0.00,-28.29,-36.37,-16.28,-27.39,-38.15,2.38,-29.87,-32.10,
0.00,-28.31,-36.35,2.11,-29.87,-27.61,-8.25,-26.82,-15.73,-0.00,-28.28,-36.39,-16.02,-28.73,-39.15,2.35,-29.87,-32.07,
0.00,-28.35,-36.30,2.17,-29.86,-27.53,-8.22,-26.85,-15.83,-0.00,-28.25,-36.44,-15.35,-30.92,-40.65,2.28,-29.88,-32.02,
0.00,-28.41,-36.21,2.27,-29.84,-27.39,-8.15,-26.92,-16.02,-0.00,-28.18,-36.53,-14.14,-33.84,-42.45,2.14,-29.89,-31.91,
0.00,-28.50,-36.07,2.44,-29.82,-27.18,-8.05,-27.02,-16.31,-0.00,-28.08,-36.67,-12.37,-37.24,-44.29,1.93,-29.91,-31.74,
0.00,-28.63,-35.87,2.65,-29.78,-26.89,-7.90,-27.16,-16.71,-0.00,-27.93,-36.85,-10.14,-40.80,-45.86,1.64,-29.94,-31.50,
0.00,-28.78,-35.62,2.92,-29.73,-26.51,-7.72,-27.33,-17.20,-0.00,-27.74,-37.07,-7.59,-44.06,-46.82,1.29,-29.96,-31.20,
0.00,-28.94,-35.31,3.24,-29.66,-26.06,-7.50,-27.52,-17.77,-0.00,-27.50,-37.32,-4.94,-46.56,-46.87,0.88,-29.98,-30.84,
0.00,-29.11,-34.95,3.57,-29.58,-25.56,-7.25,-27.73,-18.39,-0.00,-27.22,-37.58,-2.36,-47.91,-45.82,0.45,-29.99,-30.44,
0.00,-29.27,-34.57,3.92,-29.48,-25.02,-6.98,-27.94,-19.04,-0.00,-26.91,-37.85,-0.00,-47.96,-43.66,0.00,-30.00,-30.00,
0.00,-29.41,-34.17,4.26,-29.37,-24.47,-6.71,-28.13,-19.68,-0.00,-26.58,-38.09,2.07,-46.76,-40.55,-0.44,-29.99,-29.55,
0.00,-29.53,-33.78,4.57,-29.26,-23.94,-6.44,-28.31,-20.27,-0.00,-26.24,-38.32,3.81,-44.58,-36.79,-0.84,-29.98,-29.12,
0.00,-29.63,-33.42,4.85,-29.15,-23.45,-6.20,-28.47,-20.80,-0.00,-25.93,-38.51,5.21,-41.75,-32.70,-1.20,-29.96,-28.71,
0.00,-29.70,-33.10,5.08,-29.05,-23.03,-5.99,-28.59,-21.25,-0.00,-25.65,-38.66,6.30,-38.60,-28.61,-1.49,-29.94,-28.37,
0.00,-29.75,-32.84,5.26,-28.97,-22.69,-5.81,-28.69,-21.60,-0.00,-25.42,-38.77,7.11,-35.45,-24.80,-1.73,-29.91,-28.09,
0.00,-29.78,-32.65,5.39,-28.91,-22.44,-5.69,-28.76,-21.86,-0.00,-25.25,-38.85,7.66,-32.54,-21.51,-1.89,-29.89,-27.88,
0.00,-29.80,-32.53,5.47,-28.86,-22.28,-5.60,-28.80,-22.02,-0.00,-25.13,-38.90,8.01,-30.10,-18.89,-2.00,-29.88,-27.74,
0.00,-29.82,-32.46,5.52,-28.84,-22.19,-5.56,-28.82,-22.11,-0.00,-25.07,-38.93,8.19,-28.28,-17.06,-2.06,-29.87,-27.67,
0.00,-29.82,-32.44,5.54,-28.83,-22.16,-5.54,-28.83,-22.15,-0.00,-25.05,-38.94,8.26,-27.17,-16.01,-2.08,-29.87,-27.64,
0.00,-29.82,-32.43,5.54,-28.83,-22.15,-5.54,-28.83,-22.15,-0.00,-25.05,-38.94,8.27,-26.80,-15.68,-2.08,-29.87,-27.64,
0.00,-29.82,-32.43,5.54,-28.83,-22.15,-5.54,-28.83,-22.16,-0.00,-25.50,-39.30,8.27,-26.80,-15.69,-2.09,-29.87,-27.63,
0.00,-29.82,-32.40,5.56,-28.82,-22.11,-5.52,-28.84,-22.19,-0.00,-26.90,-40.31,8.26,-26.82,-15.73,-2.11,-29.87,-27.61,
0.00,-29.84,-32.34,5.60,-28.80,-22.02,-5.47,-28.86,-22.28,-0.00,-29.25,-41.83,8.22,-26.85,-15.83,-2.17,-29.86,-27.53,
0.00,-29.85,-32.21,5.69,-28.75,-21.86,-5.39,-28.91,-22.44,-0.00,-32.43,-43.66,8.15,-26.92,-16.02,-2.27,-29.84,-27.39,
0.00,-29.88,-32.01,5.81,-28.69,-21.60,-5.26,-28.97,-22.69,-0.00,-36.20,-45.50,8.05,-27.02,-16.31,-2.44,-29.82,-27.18,
0.00,-29.91,-31.74,5.99,-28.59,-21.25,-5.08,-29.05,-23.03,-0.00,-40.16,-47.01,7.90,-27.16,-16.71,-2.65,-29.78,-26.89,
0.00,-29.95,-31.40,6.20,-28.47,-20.80,-4.85,-29.15,-23.45,-0.00,-43.79,-47.82,7.72,-27.33,-17.20,-2.92,-29.73,-26.51,
0.00,-29.98,-30.98,6.44,-28.31,-20.27,-4.57,-29.26,-23.94,-0.00,-46.52,-47.62,7.50,-27.52,-17.77,-3.24,-29.66,-26.06,
0.00,-30.00,-30.51,6.71,-28.13,-19.68,-4.26,-29.37,-24.47,-0.00,-47.96,-46.22,7.25,-27.73,-18.39,-3.57,-29.58,-25.56,
-0.00,-30.01,-30.01,6.98,-27.94,-19.04,-3.92,-29.48,-25.02,-0.00,-47.96,-43.66,6.98,-27.94,-19.04,-3.92,-29.48,-25.02,
-0.00,-30.00,-29.49,7.25,-27.73,-18.39,-3.57,-29.58,-25.56,-0.00,-46.64,-40.14,6.71,-28.13,-19.68,-4.26,-29.37,-24.47,
-0.00,-29.98,-28.99,7.50,-27.52,-17.77,-3.24,-29.66,-26.06,0.00,-44.32,-36.01,6.44,-28.31,-20.27,-4.57,-29.26,-23.94,
-0.00,-29.95,-28.52,7.72,-27.33,-17.20,-2.92,-29.73,-26.51,0.00,-41.35,-31.59,6.20,-28.47,-20.80,-4.85,-29.15,-23.45,
-0.00,-29.92,-28.12,7.90,-27.16,-16.71,-2.65,-29.78,-26.89,0.00,-38.09,-27.24,5.99,-28.59,-21.25,-5.08,-29.05,-23.03,
-0.00,-29.89,-27.80,8.05,-27.02,-16.31,-2.44,-29.82,-27.18,0.00,-34.84,-23.23,5.81,-28.69,-21.60,-5.26,-28.97,-22.69,
-0.00,-29.86,-27.56,8.15,-26.92,-16.02,-2.27,-29.84,-27.39,0.00,-31.89,-19.79,5.69,-28.76,-21.86,-5.39,-28.91,-22.44,
-0.00,-29.84,-27.40,8.22,-26.85,-15.83,-2.17,-29.86,-27.53,0.00,-29.42,-17.09,5.60,-28.80,-22.02,-5.47,-28.86,-22.28,
-0.00,-29.83,-27.32,8.25,-26.82,-15.73,-2.11,-29.87,-27.61,0.00,-27.58,-15.21,5.56,-28.82,-22.11,-5.52,-28.84,-22.19,
-0.00,-29.83,-27.29,8.27,-26.80,-15.69,-2.09,-29.87,-27.63,0.00,-26.47,-14.15,5.54,-28.83,-22.15,-5.54,-28.83,-22.16,
-0.00,-29.83,-27.28,8.27,-26.80,-15.68,-2.08,-29.87,-27.64,0.00,-26.11,-13.82,5.54,-28.83,-22.15,-5.54,-28.83,-22.15,
-0.00,-29.83,-27.28,8.26,-27.17,-16.01,-2.08,-29.87,-27.64,-0.00,-26.11,-13.83,5.54,-28.83,-22.16,-5.54,-28.83,-22.15,
-0.00,-29.82,-27.25,8.19,-28.28,-17.06,-2.06,-29.87,-27.67,0.00,-26.13,-13.87,5.52,-28.84,-22.19,-5.56,-28.82,-22.11,
-0.00,-29.81,-27.16,8.01,-30.10,-18.89,-2.00,-29.88,-27.74,0.00,-26.17,-13.99,5.47,-28.86,-22.28,-5.60,-28.80,-22.02,
-0.00,-29.80,-27.00,7.66,-32.54,-21.51,-1.89,-29.89,-27.88,0.00,-26.25,-14.20,5.39,-28.91,-22.44,-5.69,-28.75,-21.86,
-0.00,-29.76,-26.76,7.11,-35.45,-24.80,-1.73,-29.91,-28.09,0.00,-26.37,-14.52,5.26,-28.97,-22.69,-5.81,-28.69,-21.60,
-0.00,-29.72,-26.43,6.30,-38.60,-28.61,-1.49,-29.94,-28.37,0.00,-26.54,-14.96,5.08,-29.05,-23.03,-5.99,-28.59,-21.25,
-0.00,-29.65,-26.00,5.21,-41.75,-32.70,-1.20,-29.96,-28.71,0.00,-26.74,-15.51,4.85,-29.15,-23.45,-6.20,-28.47,-20.80,
-0.00,-29.57,-25.49,3.81,-44.58,-36.79,-0.84,-29.98,-29.12,0.00,-26.97,-16.15,4.57,-29.26,-23.94,-6.44,-28.31,-20.27,
-0.00,-29.46,-24.92,2.07,-46.76,-40.55,-0.44,-29.99,-29.55,0.00,-27.21,-16.85,4.26,-29.37,-24.47,-6.71,-28.13,-19.68,
-0.00,-29.34,-24.30,-0.00,-47.96,-43.66,0.00,-30.00,-30.00,0.00,-27.46,-17.57,3.92,-29.48,-25.02,-6.98,-27.94,-19.04,
-0.00,-29.20,-23.68,-2.36,-47.91,-45.82,0.45,-29.99,-30.44,0.00,-27.70,-18.28,3.57,-29.58,-25.56,-7.25,-27.73,-18.39,
-0.00,-29.06,-23.08,-4.94,-46.56,-46.87,0.88,-29.98,-30.84,0.00,-27.91,-18.95,3.24,-29.66,-26.06,-7.50,-27.52,-17.77,
-0.00,-28.93,-22.52,-7.59,-44.06,-46.82,1.29,-29.96,-31.20,0.00,-28.10,-19.54,2.92,-29.73,-26.51,-7.72,-27.33,-17.20,
-0.00,-28.81,-22.05,-10.14,-40.80,-45.86,1.64,-29.94,-31.51,0.00,-28.25,-20.05,2.65,-29.78,-26.89,-7.90,-27.16,-16.71,
-0.00,-28.70,-21.66,-12.37,-37.24,-44.29,1.93,-29.91,-31.74,0.00,-28.36,-20.44,2.44,-29.82,-27.18,-8.05,-27.02,-16.31,
-0.00,-28.63,-21.38,-14.14,-33.84,-42.45,2.14,-29.89,-31.91,0.00,-28.44,-20.73,2.27,-29.84,-27.39,-8.15,-26.92,-16.02,
-0.00,-28.58,-21.20,-15.35,-30.92,-40.65,2.28,-29.88,-32.02,0.00,-28.50,-20.91,2.17,-29.86,-27.53,-8.22,-26.85,-15.83,
-0.00,-28.55,-21.10,-16.02,-28.73,-39.15,2.35,-29.87,-32.07,0.00,-28.52,-21.01,2.11,-29.87,-27.61,-8.26,-26.82,-15.73,
-0.00,-28.54,-21.06,-16.28,-27.39,-38.15,2.38,-29.87,-32.10,0.00,-28.53,-21.05,2.09,-29.87,-27.63,-8.27,-26.80,-15.69,
-0.00,-28.54,-21.06,-16.32,-26.94,-37.80,2.39,-29.87,-32.10,0.00,-28.54,-21.06,2.08,-29.87,-27.64,-8.27,-26.80,-15.68,
-0.00,-28.53,-21.05,-16.31,-26.95,-37.80,2.39,-29.87,-32.10,0.00,-28.54,-21.06,2.08,-29.87,-27.64,-8.26,-27.17,-16.01,
-0.00,-28.52,-21.01,-16.26,-26.96,-37.79,2.42,-29.87,-32.13,0.00,-28.55,-21.10,2.06,-29.87,-27.67,-8.19,-28.28,-17.06,
-0.00,-28.50,-20.91,-16.12,-26.99,-37.76,2.49,-29.86,-32.18,0.00,-28.58,-21.20,2.00,-29.88,-27.74,-8.01,-30.10,-18.89,
-0.00,-28.44,-20.73,-15.87,-27.05,-37.71,2.64,-29.84,-32.29,0.00,-28.63,-21.38,1.89,-29.89,-27.88,-7.66,-32.54,-21.51,
-0.00,-28.36,-20.44,-15.49,-27.14,-37.63,2.86,-29.82,-32.45,0.00,-28.70,-21.66,1.73,-29.91,-28.09,-7.11,-35.45,-24.80,
-0.00,-28.25,-20.04,-14.97,-27.27,-37.52,3.16,-29.78,-32.67,0.00,-28.81,-22.05,1.49,-29.94,-28.37,-6.30,-38.60,-28.61,
-0.00,-28.09,-19.54,-14.33,-27.42,-37.38,3.55,-29.73,-32.94,0.00,-28.93,-22.52,1.20,-29.96,-28.71,-5.21,-41.75,-32.70,
-0.00,-27.91,-18.95,-13.60,-27.60,-37.20,4.02,-29.66,-33.25,0.00,-29.06,-23.08,0.84,-29.98,-29.12,-3.81,-44.58,-36.79,
-0.00,-27.70,-18.28,-12.81,-27.79,-37.00,4.55,-29.58,-33.59,0.00,-29.20,-23.68,0.44,-29.99,-29.55,-2.07,-46.76,-40.55,
-0.00,-27.46,-17.57,-12.01,-27.98,-36.78,5.13,-29.48,-33.93,0.00,-29.34,-24.30,-0.00,-30.00,-30.00,0.00,-47.96,-43.66,
-0.00,-27.21,-16.85,-11.23,-28.17,-36.54,5.72,-29.37,-34.26,0.00,-29.46,-24.92,-0.45,-29.99,-30.44,2.36,-47.91,-45.82,
-0.00,-26.97,-16.15,-10.51,-28.34,-36.31,6.31,-29.26,-34.57,0.00,-29.57,-25.49,-0.88,-29.98,-30.84,4.94,-46.56,-46.87,
-0.00,-26.74,-15.51,-9.88,-28.49,-36.09,6.84,-29.15,-34.83,0.00,-29.65,-26.00,-1.29,-29.96,-31.20,7.59,-44.06,-46.82,
-0.00,-26.54,-14.96,-9.35,-28.61,-35.90,7.31,-29.06,-35.05,0.00,-29.72,-26.43,-1.64,-29.94,-31.50,10.14,-40.80,-45.86,
-0.00,-26.37,-14.52,-8.94,-28.70,-35.74,7.70,-28.97,-35.23,0.00,-29.76,-26.76,-1.93,-29.91,-31.74,12.37,-37.24,-44.29,
-0.00,-26.25,-14.20,-8.64,-28.77,-35.63,7.98,-28.91,-35.35,0.00,-29.80,-27.01,-2.14,-29.89,-31.91,14.14,-33.84,-42.45,
-0.00,-26.17,-13.99,-8.46,-28.81,-35.55,8.16,-28.87,-35.43,0.00,-29.82,-27.16,-2.28,-29.88,-32.02,15.35,-30.92,-40.65,
-0.00,-26.13,-13.87,-8.35,-28.83,-35.51,8.27,-28.85,-35.47,0.00,-29.82,-27.25,-2.35,-29.87,-32.07,16.02,-28.73,-39.15,
-0.00,-26.11,-13.83,-8.31,-28.84,-35.49,8.30,-28.84,-35.49,0.00,-29.83,-27.28,-2.38,-29.87,-32.10,16.28,-27.39,-38.15,
-0.00,-26.11,-13.82,-8.31,-28.84,-35.49,8.31,-28.84,-35.49,0.00,-29.83,-27.28,-2.38,-29.87,-32.10,16.32,-26.94,-37.80
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  Serial.println("UnderWaterHexapodRobot program!");
  
  pwm.begin();
  pwm1.begin();
  pwm.setPWMFreq(50);  // Analog servos run at ~50 Hz updates
  pwm1.setPWMFreq(50);  // Analog servos run at ~50 Hz updates

    for(int i=0;i<16;i++)
    {
      pwm.setPWM(i, 0, rec[i]);
      
    }
      pwm1.setPWM(0, 0, rec[16]);
      pwm1.setPWM(1, 0, rec[17]);
  delay(1000);

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
        delay(500);
  }
  server.begin();
  server.setNoDelay(true);  //加上后才正常些

}

void loop() {

  blink();
  
  uint8_t i;
    if (server.hasClient())
    {
        for (i = 0; i < MAX_SRV_CLIENTS; i++)
        {
            if (!serverClients[i] || !serverClients[i].connected())
            {
                if (serverClients[i]) serverClients[i].stop();//未联接,就释放
                serverClients[i] = server.available();//分配新的
                continue;
            }
 
        }
        WiFiClient serverClient = server.available();
        serverClient.stop();
    }
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
    {
        if (serverClients[i] && serverClients[i].connected())
        {
            digitalWrite(led, 0);//有链接存在,就一直长亮
 
            if (serverClients[i].available())
            {
                while (serverClients[i].available()) 
                cmd = serverClients[i].read();
                delay(1);
                Serial.write(serverClients[i].read());
            }
        }
    }

  
//    for(int j=0;j<120;j++)
//    {
//        for(int i=0;i<16;i++)
//        {
//          pwm.setPWM(i, 0, map(forwardF[j][i]*direct[i],-90,90,-225,225)+rec[i]);
//          
//        }
//        pwm1.setPWM(16-16, 0, map(forwardF[j][16]*direct[16],-90,90,-225,225)+rec[16]);
//        pwm1.setPWM(17-16, 0, map(forwardF[j][17]*direct[17],-90,90,-225,225)+rec[17]);
//        delay(10);
//    }

    if(cmd == 'a')//前进
    {
      for(int j=0;j<120;j++)
      {
          for(int i=0;i<16;i++)
          {
            pwm.setPWM(i, 0, map(forwardF[j][i]*direct[i],-90,90,-225,225)+rec[i]);
            
          }
          pwm1.setPWM(16-16, 0, map(forwardF[j][16]*direct[16],-90,90,-225,225)+rec[16]);
          pwm1.setPWM(17-16, 0, map(forwardF[j][17]*direct[17],-90,90,-225,225)+rec[17]);
          delay(10);
      }
    }
    else if(cmd == 'b')//后退
    {
      for(int j=0;j<120;j++)
      {
          for(int i=0;i<16;i++)
          {
            pwm.setPWM(i, 0, map(forwardF[119-j][i]*direct[i],-90,90,-225,225)+rec[i]);
            
          }
          pwm1.setPWM(16-16, 0, map(forwardF[119-j][16]*direct[16],-90,90,-225,225)+rec[16]);
          pwm1.setPWM(17-16, 0, map(forwardF[119-j][17]*direct[17],-90,90,-225,225)+rec[17]);
          delay(10);
      }      
      
    }
    else if(cmd == 'c')
    {
      for(int j=0;j<120;j++)
      {
          for(int i=0;i<6;i++)
          {
            pwm.setPWM(i, 0, map(forwardF[j][i]*direct[i],-90,90,-225,225)+rec[i]);
            
          }
          for(int i=6;i<15;i++)
          {
            pwm.setPWM(i, 0, map(forwardF[119-j][i]*direct[i],-90,90,-225,225)+rec[i]);
            
          }
          pwm.setPWM(15, 0, map(forwardF[j][15]*direct[15],-90,90,-225,225)+rec[15]);
          pwm1.setPWM(16-16, 0, map(forwardF[j][16]*direct[16],-90,90,-225,225)+rec[16]);
          pwm1.setPWM(17-16, 0, map(forwardF[j][17]*direct[17],-90,90,-225,225)+rec[17]);
          delay(10);
      }
    }
    else if(cmd == 'd')
    {
      for(int j=0;j<120;j++)
      {
          for(int i=0;i<6;i++)
          {
            pwm.setPWM(i, 0, map(forwardF[119-j][i]*direct[i],-90,90,-225,225)+rec[i]);
            
          }
          for(int i=6;i<15;i++)
          {
            pwm.setPWM(i, 0, map(forwardF[j][i]*direct[i],-90,90,-225,225)+rec[i]);
            
          }
          pwm.setPWM(15, 0, map(forwardF[119-j][15]*direct[15],-90,90,-225,225)+rec[15]);
          pwm1.setPWM(16-16, 0, map(forwardF[119-j][16]*direct[16],-90,90,-225,225)+rec[16]);
          pwm1.setPWM(17-16, 0, map(forwardF[119-j][17]*direct[17],-90,90,-225,225)+rec[17]);
          delay(10);
      }
    }
    else
    {
      delay(100);
    }
}

void blink()
{
    static long previousMillis = 0;
    static int currstate = 0;
 
    if (millis() - previousMillis > 200)  //200ms
    {
        previousMillis = millis();
        currstate = 1 - currstate;
        digitalWrite(led, currstate);
    }
}
