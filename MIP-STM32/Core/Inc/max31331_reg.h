#ifndef MAX31331_REG_H
#define MAX31331_REG_H

#include <stdint.h>
#include <stddef.h>
#include <math.h>

/* Function pointer types for platform-dependent I/O */
typedef int32_t (*max31331_write_ptr)(void *handle, uint8_t reg, const uint8_t *data, uint16_t len);
typedef int32_t (*max31331_read_ptr)(void *handle, uint8_t reg, uint8_t *data, uint16_t len);
typedef void (*max31331_mdelay_ptr)(uint32_t milliseconds);

/* Device context structure */
typedef struct
{
  max31331_write_ptr  write_reg;
  max31331_read_ptr   read_reg;
  max31331_mdelay_ptr mdelay;
  void *handle;

  void *priv_data;
} max31331_ctx_t;


#define MAX31331_I2C_ADD                 0xD0U

// Constants
#define HOUR24 0
#define HOUR12 1
#define AM 0
#define PM 1

/* RTC Register Addresses */
#define MAX31331_RTC_STATUS              0x00U
/* ================================================ */
#define MAX31331_STATUS_A1F_POS     0
#define MAX31331_STATUS_A1F_MSK     (0x1U << MAX31331_STATUS_A1F_POS)
#define MAX31331_STATUS_A2F_POS     1
#define MAX31331_STATUS_A2F_MSK     (0x1U << MAX31331_STATUS_A2F_POS)
#define MAX31331_STATUS_TIF_POS     2
#define MAX31331_STATUS_TIF_MSK     (0x1U << MAX31331_STATUS_TIF_POS)
#define MAX31331_STATUS_DIF_POS     3
#define MAX31331_STATUS_DIF_MSK     (0x1U << MAX31331_STATUS_DIF_POS)
#define MAX31331_STATUS_VBATLOW_POS 4
#define MAX31331_STATUS_VBATLOW_MSK (0x1U << MAX31331_STATUS_VBATLOW_POS)
#define MAX31331_STATUS_PFAIL_POS   5
#define MAX31331_STATUS_PFAIL_MSK   (0x1U << MAX31331_STATUS_PFAIL_POS)
#define MAX31331_STATUS_OSF_POS     6
#define MAX31331_STATUS_OSF_MSK     (0x1U << MAX31331_STATUS_OSF_POS)
#define MAX31331_STATUS_PSDECT_POS  7
#define MAX31331_STATUS_PSDECT_MSK  (0x1U << MAX31331_STATUS_PSDECT_POS)


#define MAX31331_RTC_INT_EN               0x01U
/* ================================================ */
#define MAX31331_INT_EN_A1IE_POS      0
#define MAX31331_INT_EN_A1IE_MSK      (0x1U << MAX31331_INT_EN_A1IE_POS)
#define MAX31331_INT_EN_A2IE_POS      1
#define MAX31331_INT_EN_A2IE_MSK      (0x1U << MAX31331_INT_EN_A2IE_POS)
#define MAX31331_INT_EN_TIE_POS       2
#define MAX31331_INT_EN_TIE_MSK       (0x1U << MAX31331_INT_EN_TIE_POS)
#define MAX31331_INT_EN_DIE_POS       3
#define MAX31331_INT_EN_DIE_MSK       (0x1U << MAX31331_INT_EN_DIE_POS)
#define MAX31331_INT_EN_VBATLOWIE_POS 4
#define MAX31331_INT_EN_VBATLOWIE_MSK (0x1U << MAX31331_INT_EN_VBATLOWIE_POS)
#define MAX31331_INT_EN_PFAILE_POS    5
#define MAX31331_INT_EN_PFAILE_MSK    (0x1U << MAX31331_INT_EN_PFAILE_POS)
#define MAX31331_INT_EN_DOSF_POS      6
#define MAX31331_INT_EN_DOSF_MSK      (0x1U << MAX31331_INT_EN_DOSF_POS)


#define MAX31331_RTC_RESET                0x02U
/* ================================================ */
#define MAX31331_RESET_SWRST_POS 0
#define MAX31331_RESET_SWRST_MSK (0x1U << MAX31331_RESET_SWRST_POS)


#define MAX31331_RTC_CONFIG1              0x03U
/* ================================================ */
#define A1AC_Status 0
#define A1AC_10ms   1
#define A1AC_500ms  2
#define A1AC_5s     3
/* ================================================ */
#define MAX31331_CONFIG1_EN_OSC_POS       0
#define MAX31331_CONFIG1_EN_OSC_MSK       (0x1U << MAX31331_CONFIG1_EN_OSC_POS)
#define MAX31331_CONFIG1_I2C_TIMEOUT_POS  1
#define MAX31331_CONFIG1_I2C_TIMEOUT_MSK  (0x1U << MAX31331_CONFIG1_I2C_TIMEOUT_POS)
#define MAX31331_CONFIG1_DIP_POS          3
#define MAX31331_CONFIG1_DIP_MSK          (0x1U << MAX31331_CONFIG1_DIP_POS)
#define MAX31331_CONFIG1_A1AC_POS         4
#define MAX31331_CONFIG1_A1AC_MSK         (0x3U << MAX31331_CONFIG1_A1AC_POS)
#define MAX31331_CONFIG1_EN_IO_POS        6
#define MAX31331_CONFIG1_EN_IO_MSK        (0x1U << MAX31331_CONFIG1_EN_IO_POS)


#define MAX31331_RTC_CONFIG2              0x04U
/* ================================================ */
#define CLKO_1Hz    0
#define CLKO_64Hz   1
#define CLKO_1024Hz 2
#define CLKO_32kHz  3  // Uncompensated (For calibration)
/* ================================================ */
#define MAX31331_CONFIG2_CLKO_HZ_POS          0
#define MAX31331_CONFIG2_CLKO_HZ_MSK          (0x3U << MAX31331_CONFIG2_CLKO_HZ_POS)
#define MAX31331_CONFIG2_CLKO_ENCLKO_POS      2
#define MAX31331_CONFIG2_CLKO_ENCLKO_MSK      (0x1U << MAX31331_CONFIG2_CLKO_ENCLKO_POS)


#define MAX31331_TS_CONFIG                0x05U
/* ================================================ */
#define MAX31331_TS_CONFIG_TSE_POS        0
#define MAX31331_TS_CONFIG_TSE_MSK        (0x1U << MAX31331_TS_CONFIG_TSE_POS)
#define MAX31331_TS_CONFIG_TSR_POS        1
#define MAX31331_TS_CONFIG_TSR_MSK        (0x1U << MAX31331_TS_CONFIG_TSR_POS)
#define MAX31331_TS_CONFIG_TSOW_POS       2
#define MAX31331_TS_CONFIG_TSOW_MSK       (0x1U << MAX31331_TS_CONFIG_TSOW_POS)
#define MAX31331_TS_CONFIG_TSDIN_POS      3
#define MAX31331_TS_CONFIG_TSDIN_MSK      (0x1U << MAX31331_TS_CONFIG_TSDIN_POS)
#define MAX31331_TS_CONFIG_TSPWM_POS      4
#define MAX31331_TS_CONFIG_TSPWM_MSK      (0x1U << MAX31331_TS_CONFIG_TSPWM_POS)
#define MAX31331_TS_CONFIG_TSVLOW_POS     5
#define MAX31331_TS_CONFIG_TSVLOW_MSK     (0x1U << MAX31331_TS_CONFIG_TSVLOW_POS)


#define MAX31331_TIMER_CONFIG              0x06U
/* ================================================ */
#define TIMER_1024Hz    0
#define TIMER_256Hz     1
#define TIMER_64Hz      2
#define TIMER_16Hz      3
/* ================================================ */
#define MAX31331_TIMER_CFG_TFRQ_POS     0
#define MAX31331_TIMER_CFG_TFRQ_MSK     (0x3U << MAX31331_TIMER_CFG_TFRQ_POS)
#define MAX31331_TIMER_CFG_TRPT_POS     2
#define MAX31331_TIMER_CFG_TRPT_MSK     (0x1U << MAX31331_TIMER_CFG_TRPT_POS)
#define MAX31331_TIMER_CFG_TPAUSE_POS   3
#define MAX31331_TIMER_CFG_TPAUSE_MSK   (0x1U << MAX31331_TIMER_CFG_TPAUSE_POS)
#define MAX31331_TIMER_CFG_TEN_POS      4
#define MAX31331_TIMER_CFG_TEN_MSK      (0x1U << MAX31331_TIMER_CFG_TEN_POS)


#define MAX31331_SECONDS_1_128            0x07U
/* ================================================ */
#define MAX31331_SECONDS_128_POS     0
#define MAX31331_SECONDS_128_MSK     (0x7FU << MAX31331_SECONDS_128_POS)


#define MAX31331_SECONDS                  0x08U
/* ================================================ */
#define MAX31331_SECONDS_POS      0
#define MAX31331_SECONDS_MSK      (0x7FU << MAX31331_SECONDS_POS)


#define MAX31331_MINUTES                   0x09U
/* ================================================ */
#define MAX31331_MINUTES_POS      0
#define MAX31331_MINUTES_MSK      (0x7FU << MAX31331_MINUTES_POS)


#define MAX31331_HOURS                     0x0AU
/* ================================================ */
#define MAX31331_HOURS_HOUR_POS       0
#define MAX31331_HOURS_HOUR_MSK       (0xFU << MAX31331_HOURS_HOUR_POS)
#define MAX31331_HOURS_HR10_POS       4
#define MAX31331_HOURS_HR10_MSK       (0x1U << MAX31331_HOURS_HR10_POS)
#define MAX31331_HOURS_HR20_AMPM_POS  5
#define MAX31331_HOURS_HR20_AMPM_MSK  (0x1U << MAX31331_HOURS_HR20_AMPM_POS)
#define MAX31331_HOURS_F24_12_POS     6
#define MAX31331_HOURS_F24_12_MSK     (0x1U << MAX31331_HOURS_F24_12_POS)


#define MAX31331_DAY                       0x0BU
/* ================================================ */
#define MAX31331_DAY_POS          0
#define MAX31331_DAY_MSK          (0x7U << MAX31331_DAY_POS)


#define MAX31331_DATE                      0x0CU
/* ================================================ */
#define MAX31331_DATE_POS         0
#define MAX31331_DATE_MSK         (0x3FU << MAX31331_DATE_POS)


#define MAX31331_MONTH                     0x0DU
/* ================================================ */
#define MAX31331_MONTH_POS        0
#define MAX31331_MONTH_MSK        (0xFU << MAX31331_MONTH_POS)
#define MAX31331_CENT_POS         7
#define MAX31331_CENT_MSK         (0x1U << MAX31331_CENT_POS)


#define MAX31331_YEAR                      0x0EU
/* ================================================ */
#define MAX31331_YEAR_POS       0
#define MAX31331_YEAR_MSK       (0xFFU << MAX31331_YEAR_POS)


#define MAX31331_ALM1_SEC                  0x0FU
/* ================================================ */
#define MAX31331_ALM1_SEC_POS       0
#define MAX31331_ALM1_SEC_MSK       (0x7FU << MAX31331_ALM1_SEC_POS)
#define MAX31331_ALM1_M1_POS        7
#define MAX31331_ALM1_M1_MSK        (0x1U << MAX31331_ALM1_M1_POS)


#define MAX31331_ALM1_MIN                  0x10U
/* ================================================ */
#define MAX31331_ALM1_MIN_POS       0
#define MAX31331_ALM1_MIN_MSK       (0x7FU << MAX31331_ALM1_MIN_POS)
#define MAX31331_ALM1_M2_POS        7
#define MAX31331_ALM1_M2_MSK        (0x1U << MAX31331_ALM1_M2_POS)


#define MAX31331_ALM1_HRS                  0x11U
/* ================================================ */
#define MAX31331_ALM1_HR_POS          0
#define MAX31331_ALM1_HR_MSK          (0x1FU << MAX31331_ALM1_HR_POS)
#define MAX31331_ALM1_HR20_AMPM_POS   5
#define MAX31331_ALM1_HR20_AMPM_MSK   (0x1U << MAX31331_ALM1_HR20_AMPM_POS)
#define MAX31331_ALM1_M3_POS          7
#define MAX31331_ALM1_M3_MSK          (0x1U << MAX31331_ALM1_M3_POS)


#define MAX31331_ALM1_DAY_DATE             0x12U
/* ================================================ */
#define ALM1_DT_MATCH       0
#define ALM1_DY_MATCH       1
/* ================================================ */
#define MAX31331_ALM1_DY_DT_POS         0
#define MAX31331_ALM1_DY_DT_MSK         (0x3FU << MAX31331_ALM1_DY_DT_POS)
#define MAX31331_ALM1_DY_DT_MATCH_POS   6
#define MAX31331_ALM1_DY_DT_MATCH_MSK   (0x1U << MAX31331_ALM1_DY_DT_MATCH_POS)
#define MAX31331_ALM1_M4_POS            7
#define MAX31331_ALM1_M4_MSK            (0x1U << MAX31331_ALM1_M4_POS)


#define MAX31331_ALM1_MON                  0x13U
/* ================================================ */
#define MAX31331_ALM1_MON_POS         0
#define MAX31331_ALM1_MON_MSK         (0x1FU << MAX31331_ALM1_MON_POS)
#define MAX31331_ALM1_M6_POS          6
#define MAX31331_ALM1_M6_MSK          (0x1U << MAX31331_ALM1_M6_POS)
#define MAX31331_ALM1_M5_POS          7
#define MAX31331_ALM1_M5_MSK          (0x1U << MAX31331_ALM1_M5_POS)


#define MAX31331_ALM1_YEAR                 0x14U
/* ================================================ */
#define MAX31331_ALM1_YEAR_POS       0
#define MAX31331_ALM1_YEAR_MSK       (0xFFU << MAX31331_ALM1_YEAR_POS)


#define MAX31331_ALM2_MIN                  0x15U
/* ================================================ */
#define MAX31331_ALM2_MIN_POS       0
#define MAX31331_ALM2_MIN_MSK       (0x7FU << MAX31331_ALM2_MIN_POS)
#define MAX31331_ALM2_M2_POS        7
#define MAX31331_ALM2_M2_MSK        (0x1U << MAX31331_ALM2_M2_POS)


#define MAX31331_ALM2_HRS                  0x16U
/* ================================================ */
#define MAX31331_ALM2_HR_POS          0
#define MAX31331_ALM2_HR_MSK          (0x1FU << MAX31331_ALM2_HR_POS)
#define MAX31331_ALM2_HR20_AMPM_POS   5
#define MAX31331_ALM2_HR20_AMPM_MSK   (0x1U << MAX31331_ALM2_HR20_AMPM_POS)
#define MAX31331_ALM2_M3_POS          7
#define MAX31331_ALM2_M3_MSK          (0x1U << MAX31331_ALM2_M3_POS)


#define MAX31331_ALM2_DAY_DATE             0x17U
/* ================================================ */
#define ALM2_DT_MATCH       0
#define ALM2_DY_MATCH       1
/* ================================================ */
#define MAX31331_ALM2_DY_DT_POS         0
#define MAX31331_ALM2_DY_DT_MSK         (0x3FU << MAX31331_ALM2_DY_DT_POS)
#define MAX31331_ALM2_DY_DT_MATCH_POS   6
#define MAX31331_ALM2_DY_DT_MATCH_MSK   (0x1U << MAX31331_ALM2_DY_DT_MATCH_POS)
#define MAX31331_ALM2_M4_POS            7
#define MAX31331_ALM2_M4_MSK            (0x1U << MAX31331_ALM2_M4_POS)


#define MAX31331_TIMER_COUNT               0x18U
/* ================================================ */
#define MAX31331_TIMER_COUNT_POS     0
#define MAX31331_TIMER_COUNT_MSK     (0xFFU << MAX31331_TIMER_COUNT_POS)


#define MAX31331_TIMER_INIT                0x19U
/* ================================================ */
#define MAX31331_TIMER_INIT_POS      0
#define MAX31331_TIMER_INIT_MSK      (0xFFU << MAX31331_TIMER_INIT_POS)


#define MAX31331_PWR_MGMT                  0x1AU
/* ================================================ */
#define MAX31331_PWR_MGMT_MANUAL_SEL_POS   0
#define MAX31331_PWR_MGMT_MANUAL_SEL_MSK   (0x1U << MAX31331_PWR_MGMT_MANUAL_SEL_POS)
#define MAX31331_PWR_MGMT_VBACK_SEL_POS    1
#define MAX31331_PWR_MGMT_VBACK_SEL_MSK    (0x1U << MAX31331_PWR_MGMT_VBACK_SEL_POS)


#define MAX31331_TRICKLE_REG               0x1BU
/* ================================================ */
#define TRICKLE_3K_OHM_SHOT       0
#define TRICKLE_3K_OHM_SHOT_2     1
#define TRICKLE_6K_OHM_SHOT       2
#define TRICKLE_11K_OHM_SHOT      3
#define TRICKLE_3K_OHM_SHOT_DI    4
#define TRICKLE_3K_OHM_SHOT_DI_2  5
/* ================================================ */
#define MAX31331_TRICKLE_EN_POS        0
#define MAX31331_TRICKLE_EN_MSK        (0x1U << MAX31331_TRICKLE_EN_POS)
#define MAX31331_TRICKLE_RES_POS       1
#define MAX31331_TRICKLE_RES_MSK       (0x7U << MAX31331_TRICKLE_RES_POS)


#define MAX31331_OFFSET_HIGH               0x1DU
/* ================================================ */
#define MAX31331_OFFSET_HIGH_POS     0
#define MAX31331_OFFSET_HIGH_MSK     (0xFFU << MAX31331_OFFSET_HIGH_POS)


#define MAX31331_OFFSET_LOW                0x1EU
/* ================================================ */
#define MAX31331_OFFSET_LOW_POS      0
#define MAX31331_OFFSET_LOW_MSK      (0xFFU << MAX31331_OFFSET_LOW_POS)


/* TS RAM Registers - Timestamp 0 */
#define MAX31331_TS0_SEC_1_128             0x20U
/* ================================================ */
#define MAX31331_TS_SEC_128_POS     0
#define MAX31331_TS_SEC_128_MSK     (0x7FU << MAX31331_TS_SEC_128_POS)

#define MAX31331_TS0_SEC                   0x21U
/* ================================================ */
#define MAX31331_TS_SEC_POS        0
#define MAX31331_TS_SEC_MSK        (0x7FU << MAX31331_TS_SEC_POS)

#define MAX31331_TS0_MIN                   0x22U
/* ================================================ */
#define MAX31331_TS_MIN_POS        0
#define MAX31331_TS_MIN_MSK        (0x7FU << MAX31331_TS_MIN_POS)

#define MAX31331_TS0_HOUR                  0x23U
/* ================================================ */
#define MAX31331_TS_HOUR_POS          0
#define MAX31331_TS_HOUR_MSK          (0x1FU << MAX31331_TS_HOUR_POS)
#define MAX31331_TS_HR20_AMPM_POS     5
#define MAX31331_TS_HR20_AMPM_MSK     (0x1U << MAX31331_TS_HR20_AMPM_POS)
#define MAX31331_TS_F24_12_POS        6
#define MAX31331_TS_F24_12_MSK        (0x1U << MAX31331_TS_F24_12_POS)

#define MAX31331_TS0_DATE                  0x24U
/* ================================================ */
#define MAX31331_TS_DATE_POS        0
#define MAX31331_TS_DATE_MSK        (0x3FU << MAX31331_TS_DATE_POS)

#define MAX31331_TS0_MONTH                 0x25U
/* ================================================ */
#define MAX31331_TS_MONTH_POS       0
#define MAX31331_TS_MONTH_MSK       (0x1FU << MAX31331_TS_MONTH_POS)
#define MAX31331_TS_CENT_POS        7
#define MAX31331_TS_CENT_MSK        (0x1U << MAX31331_TS_CENT_POS)

#define MAX31331_TS0_YEAR                  0x26U
/* ================================================ */
#define MAX31331_TS_YEAR_POS        0
#define MAX31331_TS_YEAR_MSK        (0xFFU << MAX31331_TS_YEAR_POS)

#define MAX31331_TS0_FLAGS                 0x27U
/* ================================================ */
#define MAX31331_TS_FLAGS_DINF_POS   0
#define MAX31331_TS_FLAGS_DINF_MSK   (0x1U << MAX31331_TS_FLAGS_DINF_POS)
#define MAX31331_TS_FLAGS_VCCF_POS   1
#define MAX31331_TS_FLAGS_VCCF_MSK   (0x1U << MAX31331_TS_FLAGS_VCCF_POS)
#define MAX31331_TS_FLAGS_VBATF_POS  2
#define MAX31331_TS_FLAGS_VBATF_MSK  (0x1U << MAX31331_TS_FLAGS_VBATF_POS)
#define MAX31331_TS_FLAGS_VLOWF_POS  3
#define MAX31331_TS_FLAGS_VLOWF_MSK  (0x1U << MAX31331_TS_FLAGS_VLOWF_POS)

/* Timestamp 1-3 registers follow the same pattern as Timestamp 0 */
#define MAX31331_TS1_SEC_1_128             0x28U
#define MAX31331_TS1_SEC                   0x29U
#define MAX31331_TS1_MIN                   0x2AU
#define MAX31331_TS1_HOUR                  0x2BU
#define MAX31331_TS1_DATE                  0x2CU
#define MAX31331_TS1_MONTH                 0x2DU
#define MAX31331_TS1_YEAR                  0x2EU
#define MAX31331_TS1_FLAGS                 0x2FU

#define MAX31331_TS2_SEC_1_128             0x30U
#define MAX31331_TS2_SEC                   0x31U
#define MAX31331_TS2_MIN                   0x32U
#define MAX31331_TS2_HOUR                  0x33U
#define MAX31331_TS2_DATE                  0x34U
#define MAX31331_TS2_MONTH                 0x35U
#define MAX31331_TS2_YEAR                  0x36U
#define MAX31331_TS2_FLAGS                 0x37U

#define MAX31331_TS3_SEC_1_128             0x38U
#define MAX31331_TS3_SEC                   0x39U
#define MAX31331_TS3_MIN                   0x3AU
#define MAX31331_TS3_HOUR                  0x3BU
#define MAX31331_TS3_DATE                  0x3CU
#define MAX31331_TS3_MONTH                 0x3DU
#define MAX31331_TS3_YEAR                  0x3EU
#define MAX31331_TS3_FLAGS                 0x3FU

/* ============ Platform Independent Register R/W ============ */
int32_t max31331_read_reg(const max31331_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);
int32_t max31331_write_reg(const max31331_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len);

/* ============ Register Functions ============ */
int max31331_get_status(const max31331_ctx_t *ctx, uint8_t *val);
int max31331_reset_rtc(const max31331_ctx_t *ctx);

int max31331_set_time(const max31331_ctx_t *ctx, uint8_t seconds, uint8_t minutes, uint8_t hours);
int max31331_get_time(const max31331_ctx_t *ctx, uint8_t *seconds, uint8_t *minutes, uint8_t *hours, uint8_t *format);

int max31331_set_date(const max31331_ctx_t *ctx, uint8_t day, uint8_t date, uint8_t month, uint8_t century, uint8_t year);
int max31331_get_date(const max31331_ctx_t *ctx, uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *century, uint8_t *year);

int max31331_set_clko(const max31331_ctx_t *ctx, uint8_t enable,  uint8_t frequency);
int max31331_calibrate_rtc(const max31331_ctx_t *ctx, float measured_frequency);

int max31331_set_timer_config(const max31331_ctx_t *ctx, uint8_t enable, uint8_t pause, uint8_t repeat, uint8_t frequency);
int max31331_set_timer_init(const max31331_ctx_t *ctx, uint8_t val);

int max31331_set_int_en(const max31331_ctx_t *ctx, uint8_t DOSF, uint8_t PFAILE, uint8_t VBATLOWIE, uint8_t DIE, uint8_t TIE, uint8_t A2IE, uint8_t A1IE);
int max31331_get_int_en(const max31331_ctx_t *ctx, uint8_t *int_en);

/* ============ Helper Functions ============ */
// Convert packed binary to decimal
uint8_t bin2dec_bcd(uint8_t bin); 
// Convert decimal to packed binary
uint8_t dec2bin_bcd(uint8_t bcd);


#endif /* MAX31331_REG_H */