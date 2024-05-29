# C Wav-files change-programs pack

![LOGO](https://github.com/ferrovovan/C-wave-change-pack/blob/main/Logo.png)

Programs to read, change wave files, on C language.  
  
Avaible programs:
- `wav_info`
- `concat_wav`
- `mult_wav`
- `cut_wav`
- `fan_1_hour_wav`
- `two_channels_wav`
- `zero_wav (null_wav)`
- `1_hour_wav`

#### Programs description:
###### wav_info  
Print information about wave file
###### concat_wav  
Concatenate 2 wav-files
###### mult_wav
Умножает амплитуды двух wav-файлов.
###### cut_wav
Обрезает wav-файл до заданной длины.
###### fan_1_hour_wav
Создаёт wav-файл продолжительностью в один час.
###### two_channels_wav
Конвертирует моно wav-файл в стерео.
###### zero_wav
Create empty wave-file with input paramets.
###### 1_hour_wav
Extend wave-file to expected duration.  



### Compiling
Move into **root** directory and use command `make <program_name>`, where *<program_name>* is string from ***Avaible programs*** list. It compiles program in **build** directory.   
For example command
```
make cut_wav
```
create executible file *cut_wav* in **build** directory (*cut_wav.exe* on windows).  

### How to use?
1. Compile selected program.
2. Bring it in your working directory (where store 'work'-wave-files).
3. Open there terminal (console).
4. Write command like in examples:  

Cut fragment (start 3, end 6 in seconds)
```
./cut_wav.out -i in.wav --start 3 --end 6 -o out.wav
```

Information about wav-file
```
./wav_info.out test.wav
```
Create tune (there A tune, 1 second duration)
```
./form_pitch A.wav 440.00 0.5 1
```
Two channels wav
```
./two_channels_wav.out --input in.wav -o two_channels.wav
```
