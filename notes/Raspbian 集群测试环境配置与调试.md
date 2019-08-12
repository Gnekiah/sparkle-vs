---
title: Raspbian 集群测试环境配置与调试
created: '2019-07-08T06:31:22.079Z'
modified: '2019-08-12T02:43:43.322Z'
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
树莓派采用 ARM 架构，尽管 Raspbian 基于 Debian 且使用了相同的 apt 包管理，但是所有的软件包都需要用 armhf 重新编译。这里没有从 Raspbian 官方源中找到能用的 Ceph。以下采用手动编译。
### CEPH 集群部署
- 需要的包安装
```
root@pi# apt-get install vim screen htop iotop btrfs-tools lsb-release gdisk
```
- 修改源文件 /etc/apt/sources.list
```
deb http://mirrordirector.raspbian.org/raspbian/ testing main contrib non-free rpi
#deb-src http://archive.raspbian.org/raspbian/ testing main contrib non-free rpi
```
- 更新仓库列表，并配置 ceph 源与官方密钥
```
root@pi# apt-get update
root@pi# wget --no-check-certificate -q -O- 'https://ceph.com/git/?p=ceph.git&a=blob_plain&f=keys/release.asc' | apt-key add -
root@pi# echo deb http://ceph.com/debian-firefly/ wheezy main | sudo tee /etc/apt/sources.list.d/ceph.list

root@pi# apt-get update
root@pi# apt-get install ceph-deploy ceph ceph-common
```
- 添加 /etc/hosts 主机名
```
127.0.0.1       raspberrypi
172.28.34.1     pi1
172.28.34.2     pi2
172.28.34.3     pi3
172.28.34.4     pi4
172.28.34.5     pi5
```
- 生成唯一 uuid，创建 /etc/ceph/ceph.conf 并添加配置内容
```
fsid=c4489f2e-4c0c-4a75-ba2c-df8b7073a26b
mon initial members = pi1
mon host = 172.28.34.1

auth_cluster_required = cephx
auth_service_required = cephx
auth_client_required = cephx

osd_crush_chooseleaf_type = 1
osd_pool_default_szie = 1
osd_journal_size = 100
```
- 为此集群创建密钥环、并生成监视器密钥
```
croot@pi# eph-authtool --create-keyring /etc/ceph/ceph.mon.keyring --gen-key -n mon. --cap mon 'allow *'
```
- 生成管理员密钥环，生成 client.admin 用户并加入密钥环
```
root@pi# ceph-authtool --create-keyring /etc/ceph/ceph.client.admin.keyring --gen-key -n client.admin --set-uid=0 --cap mon 'allow *' --cap osd 'allow *' --cap mds 'allow' --cap mgr 'allow *'
```
- 把 client.admin 密钥加入 ceph.mon.keyring
```
root@pi# ceph-authtool /etc/ceph/ceph.mon.keyring --import-keyring /etc/ceph/ceph.client.admin.keyring
```
- 用规划好的主机名、对应 IP 地址、和 FSID 生成一个监视器图
```
root@pi# monmaptool --create --add pi1 172.28.34.1 --fsid c4489f2e-4c0c-4a75-ba2c-df8b7073a26b /ceph-data/monmap
```
- 在监视器主机上分别创建数据目录
```
root@pi# mkdir /var/lib/ceph/mon/ceph-pi1
root@pi# chown -R ceph.ceph /var/lib/ceph/mon/ceph-pi1/
```
- 用监视器图和密钥环组装守护进程所需的初始数据
```
root@pi# ceph-mon --mkfs -i pi1 --keyring /etc/ceph/ceph.mon.keyring
```
- 启动监视器
```
root@pi# systemctl restart ceph-mon@pi1
```

### 创建 OSD
- ~~创建块设备~~
~~`root@pi# dd if=/dev/zero of=/cephimage bs=4M count=1024`~~
```
root@pi# ceph osd create
root@pi# mkdir /var/lib/ceph/osd/ceph-0
root@pi# ceph-osd -i 0 --mkfs --mkkey
root@pi# ceph auth add osd.0 osd 'allow *' mon 'allow rwx' -i /var/lib/ceph/osd/ceph-0/keyring
root@pi# ceph osd crush add osd.0 0.2 host=pi1
root@pi# systemctl enable ceph-osd@0
root@pi# chown -R ceph:ceph /var/lib/ceph/osd/ceph-0/
root@pi# systemctl start ceph-osd@0
```

### 创建 MGR
```
root@pi# mkdir /var/lib/ceph/mgr/ceph-pi1
root@pi# ceph auth get-or-create mgr.pi1 mon 'allow *' osd 'allow *' mds 'allow *' -o /var/lib/ceph/mgr/ceph-pi1/keyring
root@pi# chown ceph:ceph -R /var/lib/ceph/mgr/ceph-pi1/
root@pi# systemctl enable ceph-mgr@pi1
root@pi# systemctl start ceph-mgr@pi1
```

### 创建 MDS
```
root@pi# mkdir -p /var/lib/ceph/mds/ceph-pi1
root@pi# ceph auth get-or-create mds.pi1 mds 'allow *' osd 'allow rwx' mon 'allow profile mds' -o /var/lib/ceph/mds/ceph-pi1/keyring
root@pi# systemctl enable ceph-mds@pi1
root@pi# systemctl start ceph-mds@pi1
```

### 创建 pool
```
root@pi# ceph osd pool create fsdata 128 128
root@pi# ceph osd pool create fsmetadata 128 128
```

### 创建 ceph 文件系统
```
root@pi# ceph fs new cephfs fsmetadata fsdata
root@pi# ceph mds stat
```

### 将 ceph fs 挂载到 fuse
```
root@pi# mkdir /cephinterface
root@pi# ceph-fuse -m 172.28.34.1:6821 /cephinterface
```

### 疑难杂症
- Reduced data availability: 234 pgs inactive
```
root@pi# ceph osd pool set ceph_benchmark min_size 1
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
- 方便起见，通过 git 同步各个节点的配置文件 /root/hadoop-3.1.2/etc/hadoop/*

### 运行测试
- 格式化并启动
```
root@pi# /root/hadoop-3.1.2/bin/hdfs namenode -format
root@pi# /root/hadoop-3.1.2/sbin/start-dfs.sh
```


## Sparkle
