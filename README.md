Peep
====

一个在Linux下使用的简单的远程控制程序，使用UDP协议进行传输。

实现进程隐藏、开机启动、文件传输、执行Shell命令等功能。

`sh`目录下为程序调用的一些shell脚本，用于实现添加启动项等功能。

* `init.sh` 初始化Remote程序，添加启动项;
* `start.sh` 开机启动的脚本，设置环境变量、启动Remote程序;
* `tree.sh` 一个列出文件夹所有子目录的脚本;
* ...

License
-------

Peep is licensed under BSD and GPL.

参考
----

* <https://github.com/ivyl/rootkit>
* <http://nandal.in/2012/05/control-remote-machine-using-c-programs/>
* <http://nandal.in/2011/12/tree-sh-shell-script-to-list-all-subdirectories/>