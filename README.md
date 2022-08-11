# UnderWaterHexapodRobot


一款水下机器人

直接把整个项目clone到本地文件夹。

然后把CRAB_0805这个功能包拷到自己的工作空间里面，catkin_make编译一下。

然后就可以运行了。

先

roslaunch CRAB_0805 display.launch 

会自动打开RVIZ可视化软件，然后手动添加好robot的model和tf树。就能看到机器人了，这个时候的机器人是杂乱的。

因为关节之间的关系还没有发布，我们是通过程序来发布的。当然你可以在display.launch里面添加hello.txt里面的代码使得机器人可以出现gui来手动调节关节变量（我们这里通过程序）。

接着运行程序

rosrun CRAB_0805 CRAB_0805

然后在RVIZ里面选则base_link作为参考的link。就可以看到RVIZ里面的机器人动了起来。


在solidworks里建立完坐标系后的样子：
![solidworks图片1](https://github.com/Allen953/UnderWaterHexapodRobot/blob/main/7.Photos%20%26%20Videos/hexzpodRobot_photo1.png)

在RVIZ可视化的样子：
![RVIZ图片2](https://github.com/Allen953/UnderWaterHexapodRobot/blob/main/7.Photos%20%26%20Videos/QQ%E5%9B%BE%E7%89%8720220811134717.png)



