Peep
====

一个在Linux下使用的简单的远程控制程序，使用UDP协议进行传输。

实现进程隐藏、开机启动、文件传输、执行Shell命令等功能。

`rootkit`目录下是一个简单的[Rootkit](https://github.com/ivyl/rootkit)(Linux)，为实现隐藏进程、文件和获取Root权限。默认隐藏程序自身，以防止Rootkit被检测。

* `3.2.0-34-generic.ko`  对应内核编译的文件，可直接加载(Ubuntu 12.04 x64);
* `2.6.32-38-generic.ko` 对应内核编译的文件，可直接加载(Ubuntu 10.04 x32);

功能介绍
-------

除去可以执行的Shell命令外，还内置了如下命令。

* `hide <PID>` : Hide process with given id.
* `tree <path>` : Show the nesting of sub-directories.
* `tran <RemoteFile> <NewFile>` : Transfer file from the remote machine.

开机启动通过修改`$HOME/.profile`实现; remote主动连接control。

License
-------

Peep is licensed under BSD and GPL.

参考
----

* <https://github.com/ivyl/rootkit>
* <http://nandal.in/2012/05/control-remote-machine-using-c-programs/>
* <http://nandal.in/2011/12/tree-sh-shell-script-to-list-all-subdirectories/>