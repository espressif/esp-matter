#ifndef SL_DEVICE_INIT_LFXO_CONFIG_H
#define SL_DEVICE_INIT_LFXO_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_DEVICE_INIT_LFXO_MODE> Mode
// <i>
// <cmuLfxoOscMode_Crystal=> Crystal oscillator
// <cmuLfxoOscMode_AcCoupledSine=> AC-coupled buffer
// <cmuLfxoOscMode_External=> External digital clock
// <i> Default: cmuLfxoOscMode_Crystal
#define SL_DEVICE_INIT_LFXO_MODE           cmuLfxoOscMode_Crystal

// <o SL_DEVICE_INIT_LFXO_CTUNE> CTUNE <0-127>
// <i> Default: 63
#define SL_DEVICE_INIT_LFXO_CTUNE          79

// <o SL_DEVICE_INIT_LFXO_PRECISION> LFXO precision in PPM <0-65535>
// <i> Default: 500
#define SL_DEVICE_INIT_LFXO_PRECISION      100

// <<< end of configuration section >>>

#endif // SL_DEVICE_INIT_LFXO_CONFIG_H
