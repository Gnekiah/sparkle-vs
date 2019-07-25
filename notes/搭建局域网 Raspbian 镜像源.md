---
title: 搭建局域网 Raspbian 镜像源
created: '2019-07-08T07:02:49.228Z'
modified: '2019-07-08T07:12:52.701Z'
---

# 搭建局域网 Raspbian 镜像源

Raspbian 官方提供了镜像源搭建的脚本 [raspbmirror.py.](https://raw.githubusercontent.com/plugwash/raspbian-tools/master/raspbmirror.py)
```
root@pi# mkdir /mirrors
root@pi# cd /mirrors
root@pi# git clone git@github.com:plugwash/raspbian-tools.git
root@pi# mkdir raspbian
root@pi# mkdir tmp
root@pi# cd raspbian
root@pi# ../raspbian-tools/raspbmirror.py --tmpdir ../tmp
```
