# air_conditioning
ESP8266 Wemos D1 mini - Управление кондиционером Panasonic

# Настраиваем пин, на котором у нас IR LED
const uint16_t kIrLed = 4;  // Wemos D1 mini - D2 pin. (ESP8266 GPIO pin to use. Recommended: 0 (D3).)

# Минимальная температура, смотрим на пульте
const float const_min = 16;
# Максимальная температура, смотрим на пульте
const float const_max = 30;
# Температура по-умолчанию
const float const_target_temp = 25;

# Положение заслонки
// kPanasonicAcSwingVAuto, kPanasonicAcSwingVHighest, kPanasonicAcSwingVHigh, kPanasonicAcSwingVMiddle, kPanasonicAcSwingVLow, kPanasonicAcSwingVLowest

char swing = kPanasonicAcSwingVLow;

# Пример автоматизации Home Assistant
```
automation:
  - id: "Балкон A/C: включение кондиционера"
    alias: balconyac_on
   initial_state: true
    trigger:
      - platform: state
        entity_id: sensor.lr_remote_control_action
        to: 1_double
    condition:
      - condition: state
        entity_id: binary_sensor.bn_ac_control_general
        state: "on"
    action:
      - choose:
          - conditions:
              ## Кондиционер выключен
              - condition: state
                entity_id: binary_sensor.balconyac_work_sensor
                state: "off"
            sequence:
              - service: climate.set_temperature
                data:
                  temperature: 25
                  hvac_mode: heat
                target:
                  entity_id: climate.balconyac
              - delay: 2
              - service: climate.set_preset_mode
                target:
                  entity_id: climate.balconyac
                data:
                  preset_mode: none
        default:
          - service: climate.turn_off
            target:
              entity_id: climate.balconyac
    mode: queued
    max: 5
```
