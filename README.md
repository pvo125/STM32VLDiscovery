# STM32VLDiscovery
* Отладочная плата  STM32VLDiscovery контроллер заменен (pin-to-pin) STM32F100RB-->STM32F103RB.
* Подключен знакосинтезирующий ЖКИ типа MT-16S2D (китайский аналог 1602). По 4 битной шине.
* Таймер TIM3  в режиме Encoder mode 1 для управления меню путем вращения ручки энкодера.
* Настроен ADC1 на преобразование с вывода PA0 (ADC_IN0)  встроенный температурный датчик (ADC_IN16). Для обоих каналов 
установлено время преобразование 239,5 циклов. Частота тактирования модуля ADC1 установлена 12 MHz. Т о время преобразование по каждому каналу=239,5+12,5=252 цикла или 21мкс. ADC1 запускается с таймера TIM4 с периодом  1 секунда. Модуль DMA1 копирует результаты каждого преобразования в соответствующие ячейки памяти, которые отображаются на ЖКИ при выборе соотв. пункта меню.
* Настроен модуль для приема сообщений по CAN сети (CAN1). Частота 125 KHz Sample point=75%. Транссивер CAN шины
SN65HVD230.
* 13.03.17. Добавил взможность обновления прошивки по CAN сети в режиме приложения. Управляющий MCU присылает запрос на обновление в виде сообщения id=471 UPDATE_FIRMWARE_REQ сообщение содержит размер бинарника который сохраняется в size_firmware. Далее разблокируется флэш если надо очищаются необходимае сектора (с адреса NAMBER_UPD_PAGE) и отправляется подтверждение на выдачу данных
  * ID=(NETNAME_INDEX<<8)|0x72
  * CAN_Data_TX.Data[0]=NETNAME_INDEX
  * CAN_Data_TX.Data[1]='g'	GET_DATA.
Далее принимаются сообщения по 8 байт прошивки для обновления с индексом id=473 DOWNLOAD_FIRMWARE и записываются во вторую половину флэш. После принятия всех байт проверяется целостность записанной прошивки crc32_check() и выдаются сообщение содержащее CAN_Data_TX.Data[1]='c' если CRC_OK! или CAN_Data_TX.Data[1]='e' если CRC_ERROR! Выставляется флаг write_flashflag=1 если CRC_OK! В сеторе FLAG_STATUS_PAGE перебираются байты пока не будет найдено чистое поле 0xFFFF. В этом байте пишется флаг 0x00A7 для бутлодера, который перепишет обновление в рабочую часть флэш и сделает запуск приложения. После секндной паузы делается RESET для запуска бутлоадера.

* Для получения готового bin файла делается последовательность:

    * $K\ARM\ARMCC\BIN\fromelf.exe --bin -o 32VLDisc.bin !L получаем из .axf бинарник .bin
    * srec_cat 32VLDisc.bin -bin -exclude 0x1C 0x28 -length-l-e 0x1C 4 -generate 0x20 0x28 -repeat-string 32VLDisc.bin  -o 32VLDisc.bin -bin - исключаем из бинарника адреса с 0x1C до 0x28 по адресу 0x1c записываем 4 байта размер бинарника. По адресу 0x20 8 байт строки "32VLDisc"
    * srec_cat 32VLDisc.bin -bin -crc32-l-e -max-addr 32VLDisc.bin -bin -o 32VLDisc.bin - считаем crc32 для полученного бинарника и сохраняем его в конце файла 32VLDisc.bin.
