#include QMK_KEYBOARD_H
#include "tmk_core/common/eeprom.h"

#define EEPROM_ENABLED_ENCODER_MODES (VIA_EEPROM_CUSTOM_CONFIG_ADDR)
#define EEPROM_CUSTOM_ENCODER (VIA_EEPROM_CUSTOM_CONFIG_ADDR+1)

enum encoder_modes {
  ENC_MODE_VOLUME,
  ENC_MODE_MEDIA,
  ENC_MODE_SCROLL,
  ENC_MODE_BRIGHTNESS,
  ENC_MODE_BACKLIGHT,
  ENC_MODE_CUSTOM0,
  ENC_MODE_CUSTOM1,
  ENC_MODE_CUSTOM2,
  _NUM_ENCODER_MODES,
};

enum custom_encoder_behavior {
    ENC_CUSTOM_CW = 0,
    ENC_CUSTOM_CCW,
    ENC_CUSTOM_PRESS
};

uint8_t encoder_value = 32;
uint8_t encoder_mode = ENC_MODE_VOLUME;
uint8_t enabled_encoder_modes = 0x1F;

uint16_t retrieve_custom_encoder_config(uint8_t encoder_idx, uint8_t behavior) {
#ifdef DYNAMIC_KEYMAP_ENABLE
    void* addr = (void*)(EEPROM_CUSTOM_ENCODER + (encoder_idx * 6) + (behavior * 2));
    uint16_t keycode = eeprom_read_byte(addr) << 8;
    keycode |= eeprom_read_byte(addr + 1);
    return keycode;
#else
    return 0;
#endif
}

void set_custom_encoder_config(uint8_t encoder_idx, uint8_t behavior, uint16_t new_code) {
#ifdef DYNAMIC_KEYMAP_ENABLE
    void* addr = (void*)(EEPROM_CUSTOM_ENCODER + (encoder_idx * 6) + (behavior * 2));
    eeprom_update_byte(addr, (uint8_t)(new_code >> 8));
    eeprom_update_byte(addr + 1, (uint8_t)(new_code & 0xFF));
#endif
}

void pre_encoder_mode_change(void) {
	dprintf("Changing encoder mode.\n", encoder_mode);
}

void post_encoder_mode_change(void) {
	dprintf("Encoder mode: %u\n", encoder_mode);
}

//???
void change_encoder_mode(bool clockwise) {
  pre_encoder_mode_change();
  if(enabled_encoder_modes == 0){
    enabled_encoder_modes = 0x1F;
  }
  do {
    if(!clockwise){
      if (encoder_mode == 0){
        encoder_mode = _NUM_ENCODER_MODES - 1;
      } else{
        encoder_mode = encoder_mode - 1;
      }
    } else {
        encoder_mode = (encoder_mode + 1) % _NUM_ENCODER_MODES;
    }
  } while(((1 << encoder_mode) & enabled_encoder_modes) == 0);
  post_encoder_mode_change();
}

uint16_t handle_encoder_cw(void) {
	dprintf("Encoder mode: %u\n", encoder_mode);
  uint16_t mapped_code = 0;
  switch(encoder_mode){
    default:
    	break;
    case ENC_MODE_VOLUME:
      mapped_code = KC_VOLU;
      break;
    case ENC_MODE_MEDIA:
      mapped_code = KC_MEDIA_NEXT_TRACK;
      break;
    case ENC_MODE_SCROLL:
      mapped_code = KC_WH_D;
      break;
    case ENC_MODE_BACKLIGHT:
      mapped_code = RGB_VAI;
      break;
#ifdef DYNAMIC_KEYMAP_ENABLE
    case ENC_MODE_CUSTOM0:
      mapped_code = retrieve_custom_encoder_config(0, ENC_CUSTOM_CW);
      break;
    case ENC_MODE_CUSTOM1:
      mapped_code = retrieve_custom_encoder_config(1, ENC_CUSTOM_CW);
      break;
    case ENC_MODE_CUSTOM2:
      mapped_code = retrieve_custom_encoder_config(2, ENC_CUSTOM_CW);
      break;
#endif
  }
  return mapped_code;
}

uint16_t handle_encoder_ccw(void) {
	dprintf("Encoder mode: %u\n", encoder_mode);
  uint16_t mapped_code = 0;
  switch(encoder_mode){
    default:
    	break;
    case ENC_MODE_VOLUME:
      mapped_code = KC_VOLD;
      break;
    case ENC_MODE_MEDIA:
      mapped_code = KC_MEDIA_PREV_TRACK;
      break;
    case ENC_MODE_SCROLL:
      mapped_code = KC_WH_U;
      break;
    case ENC_MODE_BACKLIGHT:
      mapped_code = RGB_VAD;
      break;
#ifdef DYNAMIC_KEYMAP_ENABLE
    case ENC_MODE_CUSTOM0:
      mapped_code = retrieve_custom_encoder_config(0, ENC_CUSTOM_CCW);
      break;
    case ENC_MODE_CUSTOM1:
      mapped_code = retrieve_custom_encoder_config(1, ENC_CUSTOM_CCW);
      break;
    case ENC_MODE_CUSTOM2:
      mapped_code = retrieve_custom_encoder_config(2, ENC_CUSTOM_CCW);
      break;
#endif
  }
  return mapped_code;
}