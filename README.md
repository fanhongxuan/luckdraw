# luckdraw
抽奖软件
# 编译方法：
1) 使用vc2010编译。需要事先编译好wxWidgets的库，并修改sln文件中对应的目录位置。

# 使用方法：
1）/media/default.jpg是默认的显示背景，直接替换成自己想要的背景图片，任意大小均可。

2）/media/title.txt中是抽奖的title,修改成需要的内容即可。

3）通过右键菜单配置中奖人员，和奖项设置。

4）通过右键菜单查看中奖纪录。

5）在奖项设置中，选择当前需要抽的奖项。可以随时添加奖项，并选中抽奖，程序任意时刻退出，重新进入，都会保持之前状态。

6）奖项设置中，配置不同的奖池，比如，三等奖，四等奖设置奖池值为0，一等奖，二等奖设置奖池值为1，那么，抽中了三等奖或者四等奖的人，还可以
接着抽一等奖或者二等奖。

7）奖项设置中，如果设置一个奖项为包含所有，则意味着不管之前的抽奖情况，所有人员均有资格参与本奖项的抽奖。

8）奖项设置中，可以设置奖项的总数量，如果一个奖项需要分批抽的话，设置每次抽奖的数量，每次抽奖数量不能超过52。

9）奖项设置中，如果选择分布显示结果，那么抽奖结果，就会分成几次，最终揭开大奖：）

10）人员名单导入，从execel中复制三列，分别为工号，姓名，部门，粘贴到txt文件中，然后通过人员设置中导入，或者直接替换media/employee.txt均可。

11） 抽奖结果中，每次抽中的多个人分为一组，可以按人弃奖或者按组弃奖。

12） 抽奖结果中，可以记录是否领奖，或者直接导入中奖记录到txt文件。



# 下载使用：
直接下载，解压后运行run.exe即可。有可能需要安装VC2010 Runtime.
