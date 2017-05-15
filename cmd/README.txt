3大类：播放-转码-录制
http://www.commandlinefu.com/
一、转码：
1、MKV文件,起始时间为第10s, 持续时间为10s。
ffmpeg -ss 00:00:10 -t 00:00:10 -i cuc_ieschool.mkv  cuc_ieschool_10.mkv 
2、源文件转化为 MP4文件,视频编码器为libx264,音频编码器为libmp3lame。
liu@liu-Lenovo-V3000:~/ffmpeg/tools/tool$ ffmpeg  -i cuc_ieschool.mkv -c:v h264 -c:a libmp3lame cuc_ieschool.mp4
3、 AVI文件,分辨率为640x360,帧率为25fps,视频码率为600kbps,音频码率为64kbps。
ffmpeg -i cuc_ieschool.mkv  -b:v 600k -b:a 64k -s 640x320 cuc_ieschool.avi
4、 FLV文件,视频编码器为libx264,音频编码器为libmp3lame,音频采样率为44100Hz。
liu@liu-Lenovo-V3000:~/ffmpeg/tools/tool$ ffmpeg -i cuc_ieschool.mkv -c:v h264 -c:a libmp3lame -ar 44100 cuc_ieschool.flv
5、 YUV420P格式像素数据,分辨率为640x360。
liu@liu-Lenovo-V3000:~/ffmpeg/tools/tool$ ffmpeg -i cuc_ieschool.mkv -s 640x320 cuc_ieschool.yuv
播放测试命令：ffplay -f rawvideo -video_size 640x320 cuc_ieschool.yuv
ffplay -i test.yuv -pix_fmt yuv420p -s 640x272
播放rgb24的原始数据：
ffplay -i test.yuv -pix_fmt rgb24 -video_size 640x272
播放uyvy422的原始数据：注意原始数据文件命名必须用yuv来命名
ffplay -i test.yuv -pix_fmt uyvy422 -video_size 640x272

6、H.264码流文件,分辨率为640x360。
liu@liu-Lenovo-V3000:~/ffmpeg/tools/tool$ ffmpeg -i cuc_ieschool.mkv -s 640x320 cuc_ieschool.h264
7、  WAVE格式音频采样数据。
liu@liu-Lenovo-V3000:~/ffmpeg/tools/tool$ ffmpeg -i cuc_ieschool.mkv  cuc_ieschool.wav
8、 MP3码流文件。
liu@liu-Lenovo-V3000:~/ffmpeg/tools/tool$ ffmpeg -i cuc_ieschool.mkv  cuc_ieschool.mp3
二、录取：
1、录取mic的声音：
ffmpeg -f alsa -i hw:1 alsaout.wav
2、录取电脑播放的声音：

3、录取屏幕
ffmpeg -f x11grab -framerate 25 -video_size cif -i :0.0 out.mpg
ffmpeg -video_size 1024x768 -framerate 25 -f x11grab -i :0.0+100,200 output.mp4
ffmpeg -video_size 1024x768 -framerate 25 -f x11grab -i :0.0+100,200 -f alsa -ac 2 -i hw:0 output.mkv（又屏幕又声音）
录制出来的效果就是屏幕就一个画面
以下命令有效：真个屏幕+mic+系统声音（感动了，qscale 0.01 是视频质量，数字越小质量越好）
ffmpeg -s 800x600 -r 25 -f x11grab -i :0.0 -f alsa -ac 2 -ar 44100 -i pulse -qscale 0.51 output.flv
References:
http://www.commandlinefu.com/commands/view/148/capture-video-of-a-linux-desktop
https://trac.ffmpeg.org/wiki/How%20to%20grab%20the%20desktop%20(screen)%20with%20FFmpeg
http://blog.csdn.net/vikingmei/article/details/8996034
也可以使用：
ffmpeg -f x11grab -r 25 -s 800x600 -i :0.0 /tmp/outputFile.mpg

4、远程录取屏幕，
ssh liu@192.168.0.186 "ffmpeg -f x11grab -r 5 -s 1280x720 -i :0 -f avi -" | ffplay - &>/dev/null


三、播放：
正常播放：
ffpaly  文件名
播放测试命令：ffplay -f rawvideo -video_size 640x320 cuc_ieschool.yuv
ffplay -i test.yuv -pix_fmt yuv420p -s 640x272
播放rgb24的原始数据：
ffplay -i test.yuv -pix_fmt rgb24 -video_size 640x272
播放uyvy422的原始数据：注意原始数据文件命名必须用yuv来命名
ffplay -i test.yuv -pix_fmt uyvy422 -video_size 640x272



