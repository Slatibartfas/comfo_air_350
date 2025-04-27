
#include "comfoair_bridge.h"

namespace esphome {
namespace comfoair_bridge {

ComfoAirBridge *ComfoAirBridge::instance = nullptr;

void ComfoAirBridge::setup() {}

void ComfoAirBridge::update() {
  send_status_request();
}

void ComfoAirBridge::loop() {
  static uint8_t buffer[8];
  static int pos = 0;

  while (available()) {
    uint8_t byte = read();
    buffer[pos++] = byte;

    if (pos == 8) {
      pos = 0;
      if (validate_checksum(buffer)) {
        handle_response(buffer);
      }
    }
  }
}

void ComfoAirBridge::send_status_request() {
  uint8_t request[] = {0x07, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48};
  this->write_array(request, sizeof(request));
}

bool ComfoAirBridge::validate_checksum(uint8_t *data) {
  uint8_t sum = 0;
  for (int i = 0; i < 7; i++) {
    sum += data[i];
  }
  return (sum % 256) == data[7];
}

void ComfoAirBridge::handle_response(uint8_t *data) {
  if (data[1] == 0x41) {  // Statusantwort
    int supply_temp = data[2];
    int exhaust_temp = data[3];
    int fresh_temp = data[4];
    int return_temp = data[5];
    bool bypass = (data[6] & 0x01) > 0;
    bool filter = (data[6] & 0x02) == 0;

    if (supply_temp_sensor != nullptr)
      supply_temp_sensor->publish_state(supply_temp / 2.0f);
    if (exhaust_temp_sensor != nullptr)
      exhaust_temp_sensor->publish_state(exhaust_temp / 2.0f);
    if (fresh_temp_sensor != nullptr)
      fresh_temp_sensor->publish_state(fresh_temp / 2.0f);
    if (return_temp_sensor != nullptr)
      return_temp_sensor->publish_state(return_temp / 2.0f);
    if (bypass_active != nullptr)
      bypass_active->publish_state(bypass);
    if (filter_ok != nullptr)
      filter_ok->publish_state(filter);
  }
}

void ComfoAirBridge::set_bypass(bool active) {
  if (!instance) return;
  uint8_t command[] = {
    0x07, 0xB1, 
    static_cast<uint8_t>(active ? 0x01 : 0x00),
    0x00, 0x00, 0x00, 0x00, 0x00
  };
  command[7] = calculate_checksum(command);
  instance->write_array(command, sizeof(command));
}

void ComfoAirBridge::set_power(bool on) {
  if (!instance) return;
  uint8_t command[] = {
    0x07, 0xB2, 
    static_cast<uint8_t>(on ? 0x01 : 0x00),
    0x00, 0x00, 0x00, 0x00, 0x00
  };
  command[7] = calculate_checksum(command);
  instance->write_array(command, sizeof(command));
}

void ComfoAirBridge::set_fan_speed(float level) {
  if (!instance) return;
  uint8_t command[] = {
    0x07, 0xB3, 
    static_cast<uint8_t>(level),
    0x00, 0x00, 0x00, 0x00, 0x00
  };
  command[7] = calculate_checksum(command);
  instance->write_array(command, sizeof(command));
}

uint8_t ComfoAirBridge::calculate_checksum(uint8_t *data) {
  uint8_t sum = 0;
  for (int i = 0; i < 7; i++) {
    sum += data[i];
  }
  return sum % 256;
}

}  // namespace comfoair_bridge
}  // namespace esphome
