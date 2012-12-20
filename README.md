Peep
====

一个在Linux下使用的简单的远程控制程序，使用UDP协议进行传输。

实现进程隐藏、开机启动、文件传输、执行Shell命令等功能。

`rootkit`目录下是一个简单的[Rootkit](https://github.com/ivyl/rootkit)(Linux)，为实现隐藏进程、文件和获取Root权限。默认隐藏程序自身，以防止Rootkit被检测。

* `3.2.0-34-generic.ko`  对应内核编译的文件，可直接加载(Ubuntu 12.04 x64);
* `2.6.32-38-generic.ko` 对应内核编译的文件，可直接加载(Ubuntu 10.04 x32);

`sh`目录下为程序调用的一些shell脚本，用于实现添加启动项等功能。

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