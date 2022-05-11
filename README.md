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

// kPanasonicAcSwingHAuto, kPanasonicAcSwingHFullLeft, kPanasonicAcSwingHFullRight, kPanasonicAcSwingHLeft, kPanasonicAcSwingHMiddle, kPanasonicAcSwingHRight


char vswing = kPanasonicAcSwingVLow;

char hswing = kPanasonicAcSwingHAuto;

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
    
  - id: "Балкон A/C: выключение кондиционера"
    alias: balconyac_off
    initial_state: false
    trigger:
      - platform: state
        entity_id: switch.away
        from: "off"
        to: "on"
      # - platform: time
      #   at: '07:10'
    condition:
      - condition: state
        entity_id: binary_sensor.bn_ac_control_general
        state: "on"
      ## Кондиционер включен
      - condition: state
        entity_id: binary_sensor.balconyac_work_sensor
        state: "on"
    action:
      - service: climate.turn_off
        target:
          entity_id: climate.balconyac

  - id: "Балкон A/C: переключение температуры и режима"
    alias: balconyac_night_mode
    initial_state: true
    trigger:
      - platform: time
        at: input_datetime.bn_night_mode_time_on
        id: "night"
      - platform: time
        at: input_datetime.bn_day_mode_time_on
        id: "day"
    condition:
      - condition: state
        entity_id: binary_sensor.bn_ac_control_general
        state: "on"
      ## Кондиционер включен
      - condition: state
        entity_id: binary_sensor.balconyac_work_sensor
        state: "on"
    action:
      - choose:
          - conditions:
              - condition: trigger
                id: "night"
            sequence:
              - service: climate.set_temperature
                data:
                  temperature: '{{ states("input_number.bn_night_temp") | float(0) | round(1,"half") }}'
                  hvac_mode: heat
                target:
                  entity_id: climate.balconyac
              - delay: 2
              - service: climate.set_preset_mode
                target:
                  entity_id: climate.balconyac
                data:
                  preset_mode: sleep
          - conditions:
              - condition: trigger
                id: "day"
            sequence:
              - service: climate.set_temperature
                data:
                  temperature: '{{ states("input_number.bn_day_temp") | float(0) | round(1,"half") }}'
                  hvac_mode: heat
                target:
                  entity_id: climate.balconyac
              - delay: 2
              - service: climate.set_preset_mode
                target:
                  entity_id: climate.balconyac
                data:
                  preset_mode: none    
```
