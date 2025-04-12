# 学习笔记
## 2.UE5设置
1. 设置源码IDE
2. 禁止热重载，自动compile c++ class
3. 禁止live coding
4. 设置默认和起始地图
5. 确保rider集成插件打开
   
## 3.git 版本控制
1. 创建.gitignore文件，不会上传添加在列的文件

## 4.创建AuraCharacterBase基类
1.在ue5中下载用于调试的工具
![](https://tuchuanglpa.oss-cn-beijing.aliyuncs.com/tuchuanglpa/20250412191135571.png)
2. 点击rider中的run按钮在非debug模式下运行，debug按钮在debug模式下运行
## 5.Player and Enemy Charecters
1. 将 Abstract 添加到 UCLASS 宏中用于标记该类为 抽象基类，禁止直接实例化;

>核心作用:
1.禁止创建实例：标记为 Abstract 的类无法在 UE 编辑器中被直接拖放到场景或蓝图继承，只能作为其他类的父类使用。
2.强制派生：确保开发者必须通过子类实现具体功能，符合面向对象设计中的抽象类原则。
