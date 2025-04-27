#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace comfoair_bridge {

class ComfoAirBridge : public PollingComponent, public uart::UARTDevice {
 public:
  // Sensoren
  sensor::Sensor *supply_temp_sensor{nullptr};
  sensor::Sensor *exhaust_temp_sensor{nullptr};
  sensor::Sensor *fresh_temp_sensor{nullptr};
  sensor::Sensor *return_temp_sensor{nullptr};

  binary_sensor::BinarySensor *bypass_active{nullptr};
  binary_sensor::BinarySensor *filter_ok{nullptr};

  static ComfoAirBridge *instance;

  ComfoAirBridge() : PollingComponent(30000) {
    instance = this;
  }

  void setup() override;
  void update() override;
  void loop() override;

  void send_status_request();
  bool validate_checksum(uint8_t *data);
  void handle_response(uint8_t *data);

  void set_bypass(bool active);
  void set_power(bool on);
  void set_fan_speed(float level);
  static uint8_t calculate_checksum(uint8_t *data);
};

}  // namespace comfoair_bridge
}  // namespace esphome
