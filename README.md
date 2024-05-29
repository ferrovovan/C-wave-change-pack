# C Wav-files change-programs pack

![LOGO](https://github.com/ferrovovan/C-wave-change-pack/blob/main/Logo.png)

Programs to read, change wave files, on C language.  
  
Avaible programs:
- wav_info
- concat_wav
- mult_wav
- cut_wav
- fan_1_hour_wav
- two_channels_wav
- zero_wav (null_wav)
- 1_hour_wav
  
### Compiling
Move into **root** directory and use command `make <program_name>`, where *<program_name>* is string from *Avaible programs* list. It compiles program in **build** directory.   
For example command
```
make cut_wav
```
create executible file *cut_wav* in **build** directory (*cut_wav.exe* on windows).  

### How to use?
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
