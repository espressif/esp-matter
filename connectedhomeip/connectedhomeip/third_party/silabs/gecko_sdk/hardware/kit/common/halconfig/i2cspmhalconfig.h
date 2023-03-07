#ifndef I2CSPMHALCONFIG_H
#define I2CSPMHALCONFIG_H

#include "hal-config.h"

#if HAL_IOEXP_ENABLE
  #if (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C0)
  #define I2CSPM_DEFAULT_PERIPHERAL  I2C0
  #elif (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C1)
  #define I2CSPM_DEFAULT_PERIPHERAL  I2C1
  #elif (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C2)
  #define I2CSPM_DEFAULT_PERIPHERAL  I2C2
  #elif (BSP_IOEXP_PERIPHERAL == HAL_I2C_PORT_I2C3)
  #define I2CSPM_DEFAULT_PERIPHERAL  I2C3
  #else
  #error "I2C expander peripheral not configured"
  #endif

  #if defined(_SILICON_LABS_32B_SERIES_0)
  #define I2CSPM_INIT_DEFAULT                                                 \
  {                                                                           \
    I2CSPM_DEFAULT_PERIPHERAL, /* Use I2C instance */                         \
    BSP_IOEXP_SCL_PORT,        /* SCL port */                                 \
    BSP_IOEXP_SCL_PIN,         /* SCL pin */                                  \
    BSP_IOEXP_SDA_PORT,        /* SDA port */                                 \
    BSP_IOEXP_SDA_PIN,         /* SDA pin */                                  \
    BSP_IOEXP_ROUTE_LOC,       /* Location */                                 \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }
  #elif defined(_SILICON_LABS_32B_SERIES_1)
  #define I2CSPM_INIT_DEFAULT                                                 \
  {                                                                           \
    I2CSPM_DEFAULT_PERIPHERAL, /* Use I2C instance */                         \
    BSP_IOEXP_SCL_PORT,        /* SCL port */                                 \
    BSP_IOEXP_SCL_PIN,         /* SCL pin */                                  \
    BSP_IOEXP_SDA_PORT,        /* SDA port */                                 \
    BSP_IOEXP_SDA_PIN,         /* SDA pin */                                  \
    BSP_IOEXP_SCL_LOC,         /* Location of SCL */                          \
    BSP_IOEXP_SDA_LOC,         /* Location of SDA */                          \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }
  #elif defined(_SILICON_LABS_32B_SERIES_2)
  #define I2CSPM_INIT_DEFAULT                                                 \
  {                                                                           \
    I2CSPM_DEFAULT_PERIPHERAL, /* Use I2C instance */                         \
    BSP_IOEXP_SCL_PORT,        /* SCL port */                                 \
    BSP_IOEXP_SCL_PIN,         /* SCL pin */                                  \
    BSP_IOEXP_SDA_PORT,        /* SDA port */                                 \
    BSP_IOEXP_SDA_PIN,         /* SDA pin */                                  \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }
  #endif
#endif // BSP_IOEXP

#if HAL_I2CSENSOR_ENABLE
  #if (BSP_I2CSENSOR_PERIPHERAL == HAL_I2C_PORT_I2C0)
  #define I2CSPM_SENSOR_PERIPHERAL  I2C0
  #elif (BSP_I2CSENSOR_PERIPHERAL == HAL_I2C_PORT_I2C1)
  #define I2CSPM_SENSOR_PERIPHERAL  I2C1
  #elif (BSP_I2CSENSOR_PERIPHERAL == HAL_I2C_PORT_I2C2)
  #define I2CSPM_SENSOR_PERIPHERAL  I2C2
  #elif (BSP_I2CSENSOR_PERIPHERAL == HAL_I2C_PORT_I2C3)
  #define I2CSPM_SENSOR_PERIPHERAL  I2C3
  #else
  #error "I2C sensor peripheral not configured"
  #endif

  #if defined(_SILICON_LABS_32B_SERIES_0)
  #define I2CSPM_INIT_SENSOR                                                  \
  {                                                                           \
    I2CSPM_SENSOR_PERIPHERAL,  /* Use I2C instance */                         \
    BSP_I2CSENSOR_SCL_PORT,    /* SCL port */                                 \
    BSP_I2CSENSOR_SCL_PIN,     /* SCL pin */                                  \
    BSP_I2CSENSOR_SDA_PORT,    /* SDA port */                                 \
    BSP_I2CSENSOR_SDA_PIN,     /* SDA pin */                                  \
    BSP_I2CSENSOR_ROUTE_LOC,   /* Location */                                 \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }
  #elif defined(_SILICON_LABS_32B_SERIES_1)
  #define I2CSPM_INIT_SENSOR                                                  \
  {                                                                           \
    I2CSPM_SENSOR_PERIPHERAL,  /* Use I2C instance */                         \
    BSP_I2CSENSOR_SCL_PORT,    /* SCL port */                                 \
    BSP_I2CSENSOR_SCL_PIN,     /* SCL pin */                                  \
    BSP_I2CSENSOR_SDA_PORT,    /* SDA port */                                 \
    BSP_I2CSENSOR_SDA_PIN,     /* SDA pin */                                  \
    BSP_I2CSENSOR_SCL_LOC,     /* Location of SCL */                          \
    BSP_I2CSENSOR_SDA_LOC,     /* Location of SDA */                          \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }
  #elif defined(_SILICON_LABS_32B_SERIES_2)
  #define I2CSPM_INIT_SENSOR                                                  \
  {                                                                           \
    I2CSPM_SENSOR_PERIPHERAL,  /* Use I2C instance */                         \
    BSP_I2CSENSOR_SCL_PORT,    /* SCL port */                                 \
    BSP_I2CSENSOR_SCL_PIN,     /* SCL pin */                                  \
    BSP_I2CSENSOR_SDA_PORT,    /* SDA port */                                 \
    BSP_I2CSENSOR_SDA_PIN,     /* SDA pin */                                  \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }
  #endif
#endif // BSP_I2CSENSOR

#if !defined(I2CSPM_INIT_DEFAULT) && defined(I2CSPM_INIT_SENSOR)
// Fall back to sensor define for default define
  #define I2CSPM_INIT_DEFAULT   I2CSPM_INIT_SENSOR
#endif

#define I2CSPM_TRANSFER_TIMEOUT 300000

#endif // I2CSPMHALCONFIG_H
