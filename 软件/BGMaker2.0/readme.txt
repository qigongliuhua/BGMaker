BMS：存放源BMS文件，可以从bestdori上下载
MusicScore：目标位置

config.txt：配置文件，可以修改轨道和音符的映射
sourcename.txt：希望转换的BMS文件名，前提是BMS文件夹下有

=================================================

详解：

   节拍      		音符（变速）  	音轨（BPS）
   20.000,  		1.000,       	     	6.000,
   21.000,  		1.000,        	2.000,
   21.500,  		1.000,        	2.000,




=====================================================
配置文件详解：


/*根据音乐名称映射按键*/
#note
#bd.wav			%01$		//白键映射成01		
#flick.wav			%02$		//粉键映射成02
#skill.wav			%01$		//……
#slide_a.wav		%21%22$	
#slide_end_a.wav		%23$		//右绿结尾映射成23
#slide_end_flick_a.wav	%24$
#slide_b.wav		%11%12$		
#slide_end_b.wav		%13$
#slide_end_flick_b.wav	%14$
#fever_note.wav		%01$
#fever_note_flick.wav	%02$
#fever_slide_a.wav		%21%22$
#fever_slide_end_a.wav	%23$
#fever_slide_end_flick_a.wav	%24$
#fever_slide_b.wav		%11%12$ 
#fever_slide_end_b.wav	%13$
#fever_slide_end_flick_b.wav	%14$
#nodeend




/*轨道映射*/
#track
#****6	%01$	
#****1	%02$
#****2	%03$
#****3	%04$
#****4	%05$
#****5	%06$
#****8	%07$
#trackend


/*路径*/
#sourcepath
#DESTINATION PATH	%..\BGMaker\BMS\$		
#sourcepathend
#destinationpath
#DESTINATION PATH	%..\BGMaker\MusicScore\$		
#destinationpathend



/*好久前搞得了，我也忘了干啥的了== */
#index
#001		%Yes! BanG_Dream!$
#002		%STAR BEAT!〜ホシノコドウ〜$
#003		%ぽっぴん'しゃっふる$
#004		%ティアドロップス$
#005		%$
#006		%$

