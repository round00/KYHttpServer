1、TCP阻塞IO如何判断数据是否已经接收完？ <br>
例如客户端发过来了2048B的数据，而代码里接收buffer大小是1024，那么循环读取两次后数据就
接收完了，但是我们怎么判断已经读完了，否则如果继续::read的话，就会一直阻塞下去。

2、用map存放HttpCallback（一个function类型），一开始写的是const HttpCallback&，但是
这样在之后从map获取HttpCallback的时候，它是已经失效的，当我从const HttpCallback&改成
HttpCallback就没有问题了，这个问题没弄明白。<br>

