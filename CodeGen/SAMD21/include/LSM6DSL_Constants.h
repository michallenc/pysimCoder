#ifndef LSM6DSL_CONSTANTS_H
#define LSM6DSL_CONSTANTS_H

/** registers */
#define LSM6DSL_WHO_AM_I_REG                    0x0F
#define LSM6DSL_ACC_GYRO_RAM_ACCESS_REG         0x01
#define LSM6DSL_ACC_GYRO_CTRL1_XL_REG           0x10
#define LSM6DSL_ACC_GYRO_CTRL2_G_REG			0x11
#define LSM6DSL_ACC_GYRO_CTRL10_C				0x19
#define LSM6DSL_ACC_GYRO_TAP_CFG1				0x58
#define LSM6DSL_ACC_GYRO_INT1_CTRL				0x0D
#define LSM6DSL_ACC_GYRO_CTRL4_C                0x13

#define LSM6DSL_ACC_GYRO_STEP_COUNTER_L			0x4B
#define LSM6DSL_ACC_GYRO_STEP_COUNTER_H			0x4C

#define LSM6DSL_ACC_GYRO_OUTX_L_XL_REG			0x28
#define LSM6DSL_ACC_GYRO_OUTX_H_XL_REG			0x29
#define LSM6DSL_ACC_GYRO_OUTY_L_XL_REG			0x2A
#define LSM6DSL_ACC_GYRO_OUTY_H_XL_REG			0x2B
#define LSM6DSL_ACC_GYRO_OUTZ_L_XL_REG			0x2C
#define LSM6DSL_ACC_GYRO_OUTZ_H_XL_REG			0x2D

#define LSM6DSL_ACC_GYRO_OUT_L_TEMP_REG			0x20
#define LSM6DSL_ACC_GYRO_OUT_H_TEMP_REG			0x21

#define LSM6DSL_ACC_GYRO_OUTX_L_G_REG			0x22
#define LSM6DSL_ACC_GYRO_OUTX_H_G_REG			0x23
#define LSM6DSL_ACC_GYRO_OUTY_L_G_REG			0x24
#define LSM6DSL_ACC_GYRO_OUTY_H_G_REG			0x25
#define LSM6DSL_ACC_GYRO_OUTZ_L_G_REG			0x26
#define LSM6DSL_ACC_GYRO_OUTZ_H_G_REG			0x27

#define LSM6DSL_ACC_GYRO_TAP_CFG                0x58
#define LSM6DSL_ACC_GYRO_WAKE_UP_SRC            0x1B
#define LSM6DSL_ACC_GYRO_WAKE_UP_DUR			0x5C
#define LSM6DSL_ACC_GYRO_FREE_FALL              0x5D
#define LSM6DSL_ACC_GYRO_MD1_CFG                0x5E
#define LSM6DSL_ACC_GYRO_MD2_CFG                0x5F
#define LSM6DSL_ACC_GYRO_WAKE_UP_SRC            0x1B
#define LSM6DSL_ACC_GYRO_TAP_THS_6D             0x59
#define LSM6DSL_ACC_GYRO_INT_DUR2               0x5A
#define LSM6DSL_ACC_GYRO_WAKE_UP_THS            0x5B
#define LSM6DSL_ACC_GYRO_FUNC_SRC1              0x53

/** enums */
typedef enum {
    LSM6DSL_ACC_GYRO_ODR_XL_POWER_DOWN  = 0x00,
    LSM6DSL_ACC_GYRO_ODR_XL_13Hz        = 0x10,
    LSM6DSL_ACC_GYRO_ODR_XL_26Hz        = 0x20,
    LSM6DSL_ACC_GYRO_ODR_XL_52Hz        = 0x30,
    LSM6DSL_ACC_GYRO_ODR_XL_104Hz 		= 0x40,
    LSM6DSL_ACC_GYRO_ODR_XL_208Hz 		= 0x50,
    LSM6DSL_ACC_GYRO_ODR_XL_416Hz 		= 0x60,
    LSM6DSL_ACC_GYRO_ODR_XL_833Hz 		= 0x70,
    LSM6DSL_ACC_GYRO_ODR_XL_1660Hz 		= 0x80,
    LSM6DSL_ACC_GYRO_ODR_XL_3330Hz 		= 0x90,
    LSM6DSL_ACC_GYRO_ODR_XL_6660Hz 		= 0xA0,
} LSM6DSL_ACC_ODR_t;

typedef enum {
    LSM6DSL_ACC_GYRO_FS_XL_2g       = 0x00,
    LSM6DSL_ACC_GYRO_FS_XL_4g       = 0x08,
    LSM6DSL_ACC_GYRO_FS_XL_8g       = 0x0C,
    LSM6DSL_ACC_GYRO_FS_XL_16g      = 0x04,
} LSM6DSL_ACC_FS_t;

typedef enum {
    LSM6DSL_ACC_GYRO_ODR_G_POWER_DOWN   = 0x00,
    LSM6DSL_ACC_GYRO_ODR_G_13Hz         = 0x10,
    LSM6DSL_ACC_GYRO_ODR_G_26Hz         = 0x20,
    LSM6DSL_ACC_GYRO_ODR_G_52Hz         = 0x30,
    LSM6DSL_ACC_GYRO_ODR_G_104Hz 		= 0x40,
    LSM6DSL_ACC_GYRO_ODR_G_208Hz 		= 0x50,
    LSM6DSL_ACC_GYRO_ODR_G_416Hz 		= 0x60,
    LSM6DSL_ACC_GYRO_ODR_G_833Hz 		= 0x70,
    LSM6DSL_ACC_GYRO_ODR_G_1660Hz 		= 0x80,
    LSM6DSL_ACC_GYRO_ODR_G_3330Hz 		= 0x90,
    LSM6DSL_ACC_GYRO_ODR_G_6660Hz 		= 0xA0,
} LSM6DSL_GYRO_ODR_t;


typedef enum {
    LSM6DSL_ACC_GYRO_FS_G_125dps        = 0x02,
    LSM6DSL_ACC_GYRO_FS_G_245dps        = 0x00,
    LSM6DSL_ACC_GYRO_FS_G_500dps        = 0x04,
    LSM6DSL_ACC_GYRO_FS_G_1000dps       = 0x08,
    LSM6DSL_ACC_GYRO_FS_G_2000dps       = 0x0C,
} LSM6DSL_GYRO_FS_t;

#endif
