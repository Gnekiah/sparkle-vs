---
title: Raspbian 集群测试环境配置与调试
created: '2019-07-08T06:31:22.079Z'
modified: '2019-07-24T09:58:21.193Z'
---

# Raspbian 集群测试环境配置与调试

## 系统安装 & 环境设置
### 系统安装
- Raspberry PI 3B+
- Raspbian Stretch Lite
- SD Card Formatter
- Win32 Disk Imager
1. 读卡器连接电脑，使用 SD Card Formatter 格式化闪存卡
2. 通过 Win32 Disk Imager 将 .img 镜像写入闪存卡中
3. 闪存卡插入树莓派，连接 HDMI 显示器和键盘

### 设置键盘布局
```
root@pi# raspi-config
```
> -> Localisation Options 
> -> Change Keyboard Layout 
> -> General 104-key PC
> -> Other
> -> English(US)
> -> English(US)
> -> 剩下的选默认值

### 连接网络
IP 网段：172.28.34.1/22 - 172.28.34.20/22
```
root@pi# iwlist wlan0 scan | grep SSID     # search for access point

root@pi# echo 'network={'     >> /etc/wpa_supplicant/wpa_supplicant.conf
root@pi# echo 'ssid="YUKI_Y"' >> /etc/wpa_supplicant/wpa_supplicant.conf
root@pi# echo 'key_mgmt=NONE' >> /etc/wpa_supplicant/wpa_supplicant.conf
root@pi# echo '}'             >> /etc/wpa_supplicant/wpa_supplicant.conf

root@pi# echo 'interface wlan0'                  >> /etc/dhcpcd.conf
root@pi# echo 'static ip_address=172.28.34.3/22' >> /etc/dhcpcd.conf
root@pi# echo 'static routers=172.28.32.1'       >> /etc/dhcpcd.conf
```

### 配置 SSHD
```
root@pi# echo 'ClientAliveInterval 20' >> /etc/ssh/sshd_config
root@pi# echo 'ClientAliveCountMax 20' >> /etc/ssh/sshd_config
root@pi# echo 'PermitRootLogin yes' >> /etc/ssh/sshd_config
root@pi# systemctl enable ssh
root@pi# systemctl start ssh
```

### 切换源并更新
```
root@pi# apt update
root@pi# apt upgrade
```

### 设置临时代理以连接互联网
```
root@pi# apt install privoxy
```

### 其他必要配置
```
root@pi# apt install git
root@pi# ssh-keygen -t dsa
root@pi# ssh-keygen -t rsa
root@pi# cat ~/.ssh/id_rsa >> ~/.ssh/authorized_keys
root@pi# scp root@172.28.34.x:/root/.ssh/id_rsa.pub ~/tmp/ && cat ~/tmp/id_rsa.pub >> ~/.ssh/authorized_keys
```

### 网络限流

## Ceph
树莓派采用 ARM 架构，尽管 Raspbian 基于 Debian 且使用了相同的 apt 包管理，但是所有的软件包都需要用 armhf 重新编译。这里没有从 Raspbian 官方源中找到 Ceph
```
root@pi# apt install ceph

```

## HDFS
### 安装及配置
```
root@pi# apt install openjdk-9-jdk
root@pi# wget https://www-eu.apache.org/dist/hadoop/common/hadoop-3.1.2/hadoop-3.1.2-src.tar.gz
root@pi# wget https://www-eu.apache.org/dist/hadoop/common/hadoop-3.1.2/hadoop-3.1.2.tar.gz
root@pi# tar -zxvf hadoop-3.1.2.tar.gz
```

- 修改 hadoop-3.1.2/etc/hadoop/hadoop-env.sh
```
root@pi# echo 'export HADOOP_HOME=/root/hadoop-3.1.2' >> /root/hadoop-3.1.2/etc/hadoop/hadoop-env.sh
root@pi# echo 'export JAVA_HOME=/usr/lib/jvm/java-9-openjdk-armhf' >> /root/hadoop-3.1.2/etc/hadoop/hadoop-env.sh
```
- 修改 hadoop-3.1.2/etc/hadoop/core-site.xml，添加配置内容
```
<configuration>
    <property>
        <name>fs.defaultFS</name>
        <value>hdfs://172.28.34.1:9000</value>
    </property>
</configuration>

root@pi# echo '<configuration> <property>' >> /root/hadoop-3.1.2/etc/hadoop/core-site.xml
root@pi# echo '<name>fs.defaultFS</name>' >> /root/hadoop-3.1.2/etc/hadoop/core-site.xml
root@pi# echo '<value>hdfs://172.28.34.1:9000</value>' >> /root/hadoop-3.1.2/etc/hadoop/core-site.xml
root@pi# echo '</property> </configuration>' >> /root/hadoop-3.1.2/etc/hadoop/core-site.xml
```
- 修改 hadoop-3.1.2/etc/hadoop/hdfs-site.xml，添加配置内容
```
<configuration>
    <property>
        <name>dfs.namenode.name.dir</name>
        <value>/hdfs-data/dfs</value>
    </property>
    <property>
        <name>dfs.datanode.data.dir</name>
        <value>/hdfs-data/data</value>
    </property>
    <property>
        <name>dfs.replication</name>
        <value>4</value>
    </property>
</configuration>

root@pi# echo '<configuration><property>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '<name>dfs.namenode.name.dir</name>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '<value>/hdfs-data/dfs</value>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '</property><property>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '<name>dfs.datanode.data.dir</name>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '<value>/hdfs-data/data</value>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '</property><property>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '<name>dfs.replication</name>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '<value>4</value>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
root@pi# echo '</property></configuration>' >> /root/hadoop-3.1.2/etc/hadoop/hdfs-site.xml
```
- 修改 hadoop-3.1.2/etc/hadoop/mapred-site.xml，添加配置内容
```
<configuration>
    <property>
        <name>mapreduce.framework.name</name>
        <value>yarn</value>
    </property>
</configuration>

root@pi# echo '<configuration><property>' >> /root/hadoop-3.1.2/etc/hadoop/mapred-site.xml
root@pi# echo '<name>mapreduce.framework.name</name>' >> /root/hadoop-3.1.2/etc/hadoop/mapred-site.xml
root@pi# echo '<value>yarn</value>' >> /root/hadoop-3.1.2/etc/hadoop/mapred-site.xml
root@pi# echo '</property></configuration>' >> /root/hadoop-3.1.2/etc/hadoop/mapred-site.xml
```
- 修改 hadoop-3.1.2/etc/hadoop/yarn-site.xml，添加配置内容
```
<configuration>
    <property>
        <name>yarn.nodemanager.aux-services</name>
        <value>mapreduce_shuffle</value>
    </property>
</configuration>

root@pi# echo '<configuration><property>' >> /root/hadoop-3.1.2/etc/hadoop/yarn-site.xml
root@pi# echo '<name>yarn.nodemanager.aux-services</name>' >> /root/hadoop-3.1.2/etc/hadoop/yarn-site.xml
root@pi# echo '<value>mapreduce_shuffle</value>' >> /root/hadoop-3.1.2/etc/hadoop/yarn-site.xml
root@pi# echo '</property></configuration>' >> /root/hadoop-3.1.2/etc/hadoop/yarn-site.xml
```
- 向 workers 中添加 datanode 的 IP 地址
```
root@pi# echo '172.28.34.2' >> /root/hadoop-3.1.2/etc/hadoop/workers
root@pi# echo '172.28.34.3' >> /root/hadoop-3.1.2/etc/hadoop/workers
root@pi# echo '172.28.34.4' >> /root/hadoop-3.1.2/etc/hadoop/workers
root@pi# echo '172.28.34.5' >> /root/hadoop-3.1.2/etc/hadoop/workers
```
- 将配置好的 hadoop 拷贝到其他节点
```
root@pi# scp -r /root/hadoop-3.1.2 root@172.28.34.2:/root/
root@pi# scp -r /root/hadoop-3.1.2 root@172.28.34.3:/root/
root@pi# scp -r /root/hadoop-3.1.2 root@172.28.34.4:/root/
root@pi# scp -r /root/hadoop-3.1.2 root@172.28.34.5:/root/
```

### 运行测试
- 格式化并启动
```
root@pi# /root/hadoop-3.1.2/bin/hdfs namenode -format
root@pi# /root/hadoop-3.1.2/sbin/start-all.sh
```


## Sparkle
