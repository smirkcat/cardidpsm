## 匹配身份证号码和银行卡号

- 支持15位和18位身份证号
- 支持全球几乎所有市面银行卡号
- [conf](conf)文件夹分别放有身份证号码和银行卡号的识别策略文件
- 目前是以vs2017生成的[psm.sln](psm.sln)解决方案，如果以其方式或者其他系统，请自己生成相关方案
- 如果有更好的建议欢迎提交issue或者pull-requests

本程序是用于识别文本里面的身份证号码和银行卡号码

原来程序为纯c程序，改造为c++程序

## nodejs C++ Addons 调用方式 (nodejs v8.0+ 已增加 C/C++ Addons - N-API)

安装环境参考我的另一项目[easyprnodejs][1]或者[node-gyp的github主页][2]

执行下面脚本
```shell
$ node-gyp configure && node-gyp build
$ node test.js
```

[1]: https://github.com/smirkcat/EasyPR-native/tree/master/easyprnodejs
[2]: https://github.com/nodejs/node-gyp