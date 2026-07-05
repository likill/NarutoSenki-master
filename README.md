## 框架:

cocos2dx 2.2.2

下载链接：[cocos2d-x-2.2.2](http://cdn.cocos2d-x.org/cocos2d-x-2.2.2.zip)；[cocos2d-x下载官网](https://cocos2d-x.org/download)；[Cocos全版本下载](https://github.com/fusijie/Cocos-Resource "Cocos 资料大全（全版本）")，下载Cocos框架后解压，在cocos2d-x-2.2.2下新建projects文件夹，将NarutoSenki.rar解压后移动至/projects即可配置好框架。
## 环境:

vs2010

Farewell,Good Luck.


最简单运行方式：
NarutoSenki.exe路径在\cocos2d-x-2.2.2\projects\NarutoSenki-master\proj.win32\Debug.win32\NarutoSenki.exe

右键 NarutoSenki.exe，创建快捷方式
右键快捷方式 -> 属性
目标 保持：
D:\gampalace\narokuto\cocos2d-x-2.2.2\projects\NarutoSenki-master\proj.win32\Debug.win32\NarutoSenki.exe
起始位置 改成：
D:\gampalace\narokuto\cocos2d-x-2.2.2\projects\NarutoSenki-master\Resources
双击这个快捷方式运行

也可以用命令行测试：
cd /d D:\gampalace\narokuto\cocos2d-x-2.2.2\projects\NarutoSenki-master\Resources
..\proj.win32\Debug.win32\NarutoSenki.exe


W/A/S/D = 移动
J       = 普攻
K       = 技能1
L       = 技能2
U       = 技能3
I       = OUGIS1，要求等级 >= 2 且 CKR 足够
O       = OUGIS2，要求等级 >= 4 且 CKR2 足够

E       = 拉面回血，原 item1Button，金币/冷却不足不会生效
F       = 替身木，gear06，没购买不会生效
R       = gear00，隐身/加速类装备，没购买不会生效
T       = gear03，陷阱装备，没购买不会生效
G       = 打开装备界面，沿用原 onGear 逻辑
P       = 暂停