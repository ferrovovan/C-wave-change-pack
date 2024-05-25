# C Wav-files change-programs pack

![LOGO](https://github.com/ferrovovan/C-wave-change-pack/blob/main/Logo.png)

Programs to read, change wave files, on C language.  
  
Avaible programs:
1. wav_info
2. concat_wav
3. boosted_wav
4. slowed_wav
5. mult_wav
6. cut_wav
7. fan_1_hour_wav
8. two_channels_wav
9. zero_wav (null_wav)
10. 1_hour_wav
  
### Compile
make <program_name>  
Examples:
```
make boosted_wav
make cut_wav
```

### Examples (exampleti gratia) Usage
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
