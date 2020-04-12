## Умный сервис прогноза погоды (средний уровень сложности)

### Проектирование сервиса

Сервис написан на языке C++ с использованием стандарта C++20, cmake в качестве системы сборки и boost для http-запросов к API погоды. Для реализации работы чат-бота в Telegram использована библиотека tgbot-cpp (см. далее).

Интерфейс сервиса представляет собой чат-бот в Telegram, в котором можно выбрать один из двух языкой (русский либо английский) и город, для которого необходимо узнать погоду. В качестве ответа бот предоставляет текстовое сообщение, в котором, в зависимости от того, какие данные удалось получить с помощью OpenWeatherMap API, указаны данные о погоде в заданном городе. Данные с OpenWeatherMap не всегда полные, поэтому иногда часть параметров может отсутствовать в выводе. В наиболее полном варианте бот покажет общие данные о погоде (например, пасмурно, солнечно, дождь), температуру воздуха (или ее диапазон), то, как она ощущается, атмосферное давление, относительную влажность, облачность, направление и скорость ветра. Пример такого ответа:

```
Weather (Moscow): broken clouds;
Temperature: from 0°C to 4°C (average 2.58°C), feels like -2.55°C;
Atmospheric pressure: 762 mm Hg;
Humidity: 40%;
Cloudiness: 69%;
Wind: 3 m/s, E.
```

### Работа сервиса

Видео с демонстрацией работы можно найти по ссылке https://yadi.sk/i/KtrQatT1Er0Q1Q.

### Процесс работы программы

Через интерфейс мессенджера Telegram от пользователя, впервые начавшего общение с ботом, запрашивается его язык и город. Эти данные сохраняются в оперативной памяти для быстрого доступа. Хранить их на диске практически не имеет смысла, т.к. даже в случае перезагрузки сервера и потери данных не случится ничего страшного: большой важности эта информация о пользователях не представляет.

В качестве базы данных городов взята база OpenWeatherMap. Для нескольких крупных городов (Москва, Санкт-Петербург, Цюрих и др.) были добавлены названия на русском языке.

После ввода указанных данных, пользователь может при помощи нажатия на кнопку "Узнать погоду!" получить информацию о погоде в выбранном городе. Когда пользователь нажимает на кнопку, формируется запрос к OpenWeatherMap API, данные из json-ответа анализируются и подставляются в текстовые поля. Доступны опции смены языка и города.

### Как запустить и собрать

Предварительно нужно переименовать файл settings_sample.json в settings.json и вставить туда корректные токены. Кроме того, нужно поставить библиотеку boost и компилятор clang++ (на моей машине gcc отказывается компилировать designated-инициализаторы из C++20, clang справляется отлично). Библиотека tgbot-cpp тянется вместе с репозиторием как подмодуль, отдельно устанавливать ее не нужно.

```
$ git clone --recurse-submodules https://github.com/kovdan01/weather-bot.git
$ mkdir build
$ cd build
$ cmake -DCMAKE_CXX_COMPILER=clang++ ../weather-bot/
$ cmake --build .
```

Запускать необходимо исполняемый файл weather-smart-bot:

```
./weather-smart-bot
```

Если сервер находится в России, то нужно делать это через torsocks из-за блокировки API Telegram:

```
torsocks ./weather-smart-bot
```
