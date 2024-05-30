# C Wav-files change-programs pack

![LOGO](https://github.com/ferrovovan/C-wave-change-pack/blob/main/Logo.png)

Программы для чтения и изменения wav-файлов.  
Обрабатывают только PCM-совместимые форматы (смотри C-wave-change-pack/WAVE Specifications/rfc2361.txt  300 строку).  
  
Доступные программы:
- `wav_info`
- `concat_wav`
- `mult_wav`
- `cut_wav`
- `two_channels_wav`
- `zero_wav (null_wav)`
- `1_hour_wav`
- `fan_1_hour_wav`

#### Описание программ:
- wav_info  
Выводит информацию об wav-файле.

- concat_wav  
Конкатенирует 2 схожих по формату wav-файла.

- mult_wav  
Конкатенирует wav-файл с самим собой.

- cut_wav  
Вырезает промежуток заданной длины.

- two_channels_wav  
Конвертирует моно стерео.

- zero_wav (null_wav)
Создаёт пустой wav-файл заданной длительностью.

- 1_hour_wav  
Растягивает аудио до заданной длительности, путём копирования выбранного фрагмента.  

- fan_1_hour_wav  
Тоже что и 1_hour_wav, но с неправильной расшифровкой байтов, приводящей к странным результатам.



### Сборка (компиляция)
Перейдите в **корневую** директорию[^1] и используйте команду шаблоном `make <program_name>`, где *<program_name>* - название программы из списка ***Доступные программы***. Компилирует программу в директорию **build**.   
Пример команды:
```
make cut_wav
```
Это создаст исполняемый файл *cut_wav* в директории **build** (*cut_wav.exe* в Windows).  

[^1]: папка.

### Как использовать?
1. Скомпилируйте выбранную программу.
2. Переместите в свою рабочую директорию (где лежат 'рабочие'-wave-файлы).
3. Откройте там терминал (консоль).
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
