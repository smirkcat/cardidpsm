## 匹配身份证号码和银行卡号

- 支持15位和18位银行卡号
- 支持全球几乎所有市面银行卡号
- [conf](conf)文件夹分别放有身份证号码和银行卡号的识别策略文件
- 目前是以vs2017生成的解决方案，如果以其方式或者其他系统，请自己生成相关方案


本程序是用于识别文本里面的身份证号码和银行卡号码

原来程序为纯c程序，改造为c++程序

## nodejs C++ Addons (nodejs v8 已增加 C/C++ Addons - N-API)

安装环境参考我的另一项目[easyprnodejs][1]

执行下面脚本
```shell
$ node-gyp configure && node-gyp build
$ node test.js
```

[1]: https://github.com/smirkcat/EasyPR-native/tree/master/easyprnodejs