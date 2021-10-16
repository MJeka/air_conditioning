# air_conditioning
ESP8266 Wemos D1 mini - Управление кондиционером Panasonic

# Настраиваем пин, на котором у нас IR LED
const uint16_t kIrLed = 4;  // Wemos D1 mini - D1 pin. (ESP8266 GPIO pin to use. Recommended: 0 (D3).)

# Минимальная температура, смотрим на пульте
const float const_min = 16;
# Максимальная температура, смотрим на пульте
const float const_max = 30;
# Температура по-умолчанию
const float const_target_temp = 25;

# Положение заслонки
// kPanasonicAcSwingVAuto, kPanasonicAcSwingVHighest, kPanasonicAcSwingVHigh, kPanasonicAcSwingVMiddle, kPanasonicAcSwingVLow, kPanasonicAcSwingVLowest

char swing = kPanasonicAcSwingVLow;
