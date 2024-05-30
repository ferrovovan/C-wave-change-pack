# C Wav-files change-programs pack

![LOGO](https://github.com/ferrovovan/C-wave-change-pack/blob/main/Logo.png)

Использование данного программного обеспечения предполагает от пользователя  знания:  
1. Что такое директория.
2. Как открыть терминал в нужной директории.
3. Как использовать исполняемый файл в терминале.
4. Базового представления природы волновово файла.
  
Программы для чтения и изменения wav-файлов.  
Обрабатывают только PCM-совместимые форматы (смотри ![WAVE Specifications/rfc2361.txt](https://github.com/ferrovovan/C-wave-change-pack/blob/update_readme/WAVE%20Specifications/rfc2361.txt)  300 строку).  
  
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
Перейдите в **корневую** директорию и используйте команду шаблоном `make <program_name>`, где *<program_name>* - название программы из списка *Доступные программы*. Компилирует программу в директорию **build**.   
Пример команды:
```
make cut_wav
```
Это создаст исполняемый файл *cut_wav* в директории **build** (*cut_wav.exe* в Windows).  

### Как использовать?
1. Скомпилируйте выбранную программу.
2. Переместите её в свою рабочую директорию (где лежат 'рабочие'-wave-файлы).
3. Откройте там терминал (консоль).
4. Напишите команду по шаблону:  
Для Windows: `<program_name>.exe [параметры]`  
Для Linux: `./<program_name> [параметры]`  
Если параметры не заданы, выводится справка по использованию.  
  
Параметры программ:
- wav_info  
<входной_файл> - путь до входного файла.  
Пример:  
```
./wav_info test.wav
```
Печатает информацию об заголовке test.wav  
  

- concat_wav  
 <входной_файл1> <входной_файл2> <выходной_файл> - пути до файлов.  
Пример:  
```
./wav_info test.wav
```
Печатает информацию об заголовке test.wav  

- cut_wav
Пример:  
Cut fragment (start 3, end 6 in seconds)
```
./cut_wav -i in.wav --start 3 --end 6 -o out.wav
```

Information about wav-file

Create tune (there A tune, 1 second duration)
```
./form_pitch A.wav 440.00 0.5 1
```
Two channels wav
```
./two_channels_wav.out --input in.wav -o two_channels.wav
```
