#include "esphome.h"
#include "IRremoteESP8266.h"
#include "IRsend.h"
#include "ir_Panasonic.h"


const uint16_t kIrLed = 4;  // Wemos D1 mini - D1 pin. (ESP8266 GPIO pin to use. Recommended: 0 (D3).)

const float const_min = 16;
const float const_max = 30;
const float const_target_temp = 25;

// Добавил положение заслонки
// kPanasonicAcSwingVAuto, kPanasonicAcSwingVHighest, kPanasonicAcSwingVHigh, kPanasonicAcSwingVMiddle, kPanasonicAcSwingVLow, kPanasonicAcSwingVLowest
char swing = kPanasonicAcSwingVLow;

IRPanasonicAc ac(kIrLed);

class PanasonicAC : public Component, public Climate {
  public:
//    sensor::Sensor *sensor_{nullptr};

//    void set_sensor(sensor::Sensor *sensor) { this->sensor_ = sensor; }

    void setup() override
    {
//      if (this->sensor_) {
//        this->sensor_->add_on_state_callback([this](float state) {
//          this->current_temperature = state;
//          this->publish_state();
//        });
//        this->current_temperature = this->sensor_->state;
//      } else {
//        this->current_temperature = NAN;
//      }

      auto restore = this->restore_state_();
      if (restore.has_value()) {
        restore->apply(this);
      } else {
        this->mode = climate::CLIMATE_MODE_OFF;
        this->target_temperature = roundf(clamp(this->current_temperature, const_min, const_max));
        this->fan_mode = climate::CLIMATE_FAN_AUTO;
        this->swing_mode = climate::CLIMATE_SWING_OFF;
        this->preset = climate::CLIMATE_PRESET_NONE;
      }

      if (isnan(this->target_temperature)) {
        this->target_temperature = const_target_temp;
      }

      ac.begin();
      ac.on();
      ac.setTemp(this->target_temperature);
      ac.setSwingVertical(swing);

      if (this->mode == CLIMATE_MODE_OFF) {
        ac.off();
      } else if (this->mode == CLIMATE_MODE_COOL) {
        ac.setMode(kPanasonicAcCool);
      } else if (this->mode == CLIMATE_MODE_DRY) {
        ac.setMode(kPanasonicAcDry);
      } else if (this->mode == CLIMATE_MODE_FAN_ONLY) {
        ac.setMode(kPanasonicAcFan);
      } else if (this->mode == CLIMATE_MODE_HEAT) {
        ac.setMode(kPanasonicAcHeat);
      } else if (this->mode == CLIMATE_MODE_AUTO) {
        ac.setMode(kPanasonicAcFanAuto);
      } 

      if (this->fan_mode == CLIMATE_FAN_AUTO) {
        ac.setFan(kPanasonicAcFanAuto);
      } else if (this->fan_mode == CLIMATE_FAN_LOW) {
        ac.setFan(kPanasonicAcFanLow);
      } else if (this->fan_mode == CLIMATE_FAN_MEDIUM) {
        ac.setFan(kPanasonicAcFanMed);
      } else if (this->fan_mode == CLIMATE_FAN_HIGH) {
        ac.setFan(kPanasonicAcFanHigh);
      }

      // Preset
      if (this->preset == CLIMATE_PRESET_NONE) {
          ac.setQuiet(false);
          ac.setPowerful(false);
      } else if (this->preset == CLIMATE_PRESET_SLEEP) {
          ac.setQuiet(true);
      } else if (this->preset == CLIMATE_PRESET_BOOST) {
          ac.setPowerful(true);
      }

      ac.send();

      ESP_LOGD("DEBUG", "Panasonic A/C remote is in the following state:");
      ESP_LOGD("DEBUG", "  %s\n", ac.toString().c_str());
    }

    climate::ClimateTraits traits() {
      auto traits = climate::ClimateTraits();
      traits.set_visual_min_temperature(const_min);
      traits.set_visual_max_temperature(const_max);
      traits.set_visual_temperature_step(0.5f);
      traits.set_supported_modes({
          climate::CLIMATE_MODE_OFF,
//          climate::CLIMATE_MODE_HEAT_COOL,
          climate::CLIMATE_MODE_COOL,
          climate::CLIMATE_MODE_HEAT,
          climate::CLIMATE_MODE_FAN_ONLY,
          climate::CLIMATE_MODE_DRY,
          climate::CLIMATE_MODE_AUTO,
      });
      traits.set_supported_fan_modes({
          climate::CLIMATE_FAN_AUTO,
          climate::CLIMATE_FAN_LOW,
          climate::CLIMATE_FAN_MEDIUM,
          climate::CLIMATE_FAN_HIGH,
      });
      traits.set_supported_swing_modes({
          climate::CLIMATE_SWING_OFF,
          climate::CLIMATE_SWING_VERTICAL,
      });
	  
	  // Preset
	  traits.set_supported_presets({
          climate::CLIMATE_PRESET_NONE,
          climate::CLIMATE_PRESET_SLEEP,
		  climate::CLIMATE_PRESET_BOOST
      });
	  	  
	  traits.set_supports_current_temperature(true);
      return traits;
    }

  void control(const ClimateCall &call) override {
    if (call.get_mode().has_value()) {
      // User requested mode change
      ClimateMode mode = *call.get_mode();
      // Send mode to hardware
      switch(mode) {
        case CLIMATE_MODE_HEAT:
          ac.on();
          ac.setMode(kPanasonicAcHeat);
          break;
        case CLIMATE_MODE_COOL:
          ac.on();
          ac.setMode(kPanasonicAcCool);
          break;
        case CLIMATE_MODE_AUTO:
          ac.on();
          ac.setMode(kPanasonicAcAuto);
          break;
        case CLIMATE_MODE_FAN_ONLY:  //fan only
          ac.on();
          //this->target_temperature = kPanasonicAcFanModeTemp;
          ac.setMode(kPanasonicAcFan);
          break;
        case CLIMATE_MODE_DRY:
          ac.on();
          ac.setMode(kPanasonicAcDry);
          break;
        case CLIMATE_MODE_OFF:
          ac.off();
          break;
      }
    // Publish updated state
    this->mode = mode;
//    this->publish_state();
    }

    if (call.get_target_temperature().has_value()) {
      float temp = *call.get_target_temperature();
      ac.setTemp(temp);
      this->target_temperature = temp;
//      this->publish_state();
    }

    if (call.get_fan_mode().has_value()) {
      ClimateFanMode fan_mode = *call.get_fan_mode();
      if (fan_mode == CLIMATE_FAN_AUTO) {
        ac.setFan(kPanasonicAcFanAuto);
      } else if (fan_mode == CLIMATE_FAN_LOW) {
        ac.setFan(kPanasonicAcFanLow);
      } else if (fan_mode == CLIMATE_FAN_MEDIUM) {
        ac.setFan(kPanasonicAcFanMed);
      } else if (fan_mode == CLIMATE_FAN_HIGH) {
        ac.setFan(kPanasonicAcFanHigh);
      }
      this->fan_mode = fan_mode;
    }

    // Preset
    if (call.get_preset().has_value()) {
      ClimatePreset preset = *call.get_preset();
      if (preset == CLIMATE_PRESET_NONE) {
        ac.setQuiet(false);
        ac.setPowerful(false);
      } else if (preset == CLIMATE_PRESET_SLEEP) {
        ac.setPowerful(true);   // Почему-то наоборот с пауэр работает
      } else if (preset == CLIMATE_PRESET_BOOST) {
        ac.setQuiet(true);      // Почему-то наоборот с пауэр работает
      }
      this->preset = preset;
    }


    this->publish_state();
    ac.send();

    ESP_LOGD("DEBUG", "Panasonic A/C remote is in the following state:");
    ESP_LOGD("DEBUG", "  %s\n", ac.toString().c_str());
  }
};
