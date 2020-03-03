1、用map存放HttpCallback（一个function类型），一开始写的是const HttpCallback&，但是
这样在之后从map获取HttpCallback的时候，它是已经失效的，当我从const HttpCallback&改成
HttpCallback就没有问题了，这个问题没弄明白。

