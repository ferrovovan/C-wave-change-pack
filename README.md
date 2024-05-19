# C Wav-files change-programs pack

![LOGO](https://github.com/ferrovovan/C-wave-change-pack/blob/main/Logo.png)

Programs to read, change wave files, on C language.  
  
  
### Compile
make <program_name>  
Example:
```
make boosted_wav
```

### Examples (exampleti gratia) Usage
Cut fragment (start 3, end 6 in seconds)
```
./cut_wav.out -i in.wav -st 3 -end 6 -o out.wav
```
Information about wav-file
```
./wav_info.out -i test.wav
```
Create tune (there A tune, 1 second duration)
```
./form_pitch A.wav 440.00 0.5 1
```
