
小白git用户点击：http://www.jianshu.com/p/85cb197cb125  参考这个一起开发


代码是来自于csdn 雷霄骅，在他的代码基础上做一些修改等
如果有侵犯权利请与作者联系
# ffmpeg
学习ffmpeg 
ffmpe-3.2.4
SDL2-2.0.5
faac-1.28
lame-3.99.5
yasm-1.3.0
x264
code
	video_out/	  程序运行结束的结果文件
	video_src/	  视频的源文件
	my_decode/ 	  解码一个ts文件，变成yuv420p，uyuv422，rgb24文件（文件后缀都是yuv），或者提取一个ppm的图片
				播放方式：ffplay -i test.yuv -pix_fmt rgb24 -video_size 640x272
	my_sdl/
	my_sdl2/          将yuv文件中yuv420p的数据显示到屏幕上
	my_decoder_sdl/   将my_decoder和my_sdl合并使用
	my_totoal/       

	mod by tiantang46800
