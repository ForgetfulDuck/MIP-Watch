#include "max31331_reg.h"

/* ============ Platform Independent Register R/W ============ */
/**
  * @brief  Read generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to read
  * @param  data  pointer to buffer that store the data read(ptr)
  * @param  len   number of consecutive register to read
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  */
int32_t max31331_read_reg(const max31331_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len) {
    if (ctx == NULL || data == NULL || ctx->read_reg == NULL) {
        return -1;
    }
    return ctx->read_reg(ctx->handle, reg, data, len);
}

/**
  * @brief  Write generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to write
  * @param  data  pointer to data to write in register reg(ptr)
  * @param  len   number of consecutive register to write
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  */
int32_t max31331_write_reg(const max31331_ctx_t *ctx, uint8_t reg, uint8_t *data, uint16_t len) {
    if (ctx == NULL || data == NULL || ctx->write_reg == NULL) {
        return -1;
    }
    return ctx->write_reg(ctx->handle, reg, data, len);
}

/* ============ Helper Functions ============ */
uint8_t bin2dec_bcd(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}
uint8_t dec2bin_bcd(uint8_t dec) {
    return (dec/10) << 4 | (dec%10);
}

/* ============ Register Functions ============ */
int max31331_get_status(const max31331_ctx_t *ctx, uint8_t *status) {
    if (ctx == NULL || status == NULL) return -1;
    return max31331_read_reg(ctx, MAX31331_RTC_STATUS, status, 1);
}

int max31331_reset_rtc(const max31331_ctx_t *ctx) {
    if (ctx == NULL) return -1;
    uint8_t reset = (1 << MAX31331_RESET_SWRST_POS) & MAX31331_RESET_SWRST_MSK;
    int ret = max31331_write_reg(ctx, MAX31331_RTC_RESET, &reset, 1);
    
    if (ret != 0) return ret;

    ctx->mdelay(5);
    reset = (0 << MAX31331_RESET_SWRST_POS) & MAX31331_RESET_SWRST_MSK;
    ret = max31331_write_reg(ctx, MAX31331_RTC_RESET, &reset, 1);
    return ret;
}

int max31331_set_time(const max31331_ctx_t *ctx, uint8_t seconds, uint8_t minutes, uint8_t hours) {
    if (!ctx) return -1;
    if (seconds > 59 || minutes > 59 || hours > 23) return -2;

    uint8_t s = (dec2bin_bcd(seconds) << MAX31331_SECONDS_POS)    & MAX31331_SECONDS_MSK;  /* packed BCD */
    uint8_t m = (dec2bin_bcd(minutes) << MAX31331_MINUTES_POS)    & MAX31331_MINUTES_MSK;
    
    uint8_t h = 0;
    h |= dec2bin_bcd(hours % 10) & MAX31331_HOURS_HOUR_MSK;           // lower 4 bits HOUR
    h |= ((hours / 10) << MAX31331_HOURS_HR10_POS) & MAX31331_HOURS_HR10_MSK; // HR10
    h |= ((hours / 20) << MAX31331_HOURS_HR20_AMPM_POS) & MAX31331_HOURS_HR20_AMPM_MSK; // HR20
    h |= (HOUR24 << MAX31331_TS_F24_12_POS) & MAX31331_TS_F24_12_MSK; // 24-hour mode

    uint8_t time[] = {s, m, h};

    return max31331_write_reg(ctx, MAX31331_SECONDS, time, sizeof(time));
}

int max31331_get_time(const max31331_ctx_t *ctx, uint8_t *seconds, uint8_t *minutes, uint8_t *hours, uint8_t *format) {
	if (!ctx || !seconds || !minutes || !hours || !format) return -1;

    uint8_t time[3] = {0};

    int32_t ret = max31331_read_reg(ctx, MAX31331_SECONDS, time, sizeof(time));
    if (ret != 0) return ret;

    uint8_t s = time[0]; // seconds register
    uint8_t m = time[1]; // minutes register
    uint8_t h = time[2]; // hours register

	*seconds = bin2dec_bcd((s & MAX31331_SECONDS_MSK) >> MAX31331_SECONDS_POS);
    *minutes = bin2dec_bcd((m & MAX31331_MINUTES_MSK) >> MAX31331_MINUTES_POS);
    
    *format  = (h & MAX31331_TS_F24_12_MSK) >> MAX31331_TS_F24_12_POS;
    
	if (*format == HOUR24) {
        *hours = ((h & MAX31331_HOURS_HR20_AMPM_MSK) >> MAX31331_HOURS_HR20_AMPM_POS) * 20
               + ((h & MAX31331_HOURS_HR10_MSK) >> MAX31331_HOURS_HR10_POS) * 10
               + ((h & MAX31331_HOURS_HOUR_MSK) >> MAX31331_HOURS_HOUR_POS);
    } else {
        uint8_t h12 = ((h & MAX31331_HOURS_HR10_MSK) >> MAX31331_HOURS_HR10_POS) * 10
                    + ((h & MAX31331_HOURS_HOUR_MSK) >> MAX31331_HOURS_HOUR_POS);
        uint8_t am_pm = (h & MAX31331_HOURS_HR20_AMPM_MSK) >> MAX31331_HOURS_HR20_AMPM_POS;
        if (am_pm == AM) {          // AM
            *hours = (h12 == 12) ? 0 : h12;
        } else {                    // PM
            *hours = (h12 == 12) ? 12 : h12 + 12;
        }
    }

	return ret;
}

int max31331_set_date(const max31331_ctx_t *ctx, uint8_t day, uint8_t date, uint8_t month, uint8_t century, uint8_t year) {
    if (!ctx) return -1;
    if (date < 1 || date > 31 || month < 1 || month > 12 || day < 1 || day > 7) return -2;

    uint8_t d   = (day                  << MAX31331_DAY_POS) & MAX31331_DAY_MSK;     // day register
    uint8_t dt  = (dec2bin_bcd(date)    << MAX31331_DATE_POS) & MAX31331_DATE_MSK;   // date register (bits 5:0)
    uint8_t m   = ((dec2bin_bcd(month)  << MAX31331_MONTH_POS) & MAX31331_MONTH_MSK) // month register (bits 4:0)
                  | ((century ? 1 : 0)  << MAX31331_CENT_POS);                       // century bit
    uint8_t y   = (dec2bin_bcd(year)    << MAX31331_YEAR_POS) & MAX31331_YEAR_MSK;   // year register

    uint8_t set_date[] = {d, dt, m, y};

    return max31331_write_reg(ctx, MAX31331_DAY, set_date, sizeof(set_date));;
}

int max31331_get_date(const max31331_ctx_t *ctx, uint8_t *day, uint8_t *date, uint8_t *month, uint8_t *century, uint8_t *year) {
    if (!ctx || !day || !date || !month || !year) return -1;

    uint8_t regs[4] = {0};
    int32_t ret = max31331_read_reg(ctx, MAX31331_DAY, regs, sizeof(regs));
    if (ret != 0) return ret;

    uint8_t d  = regs[0];   // day register
    uint8_t dt = regs[1];   // date register
    uint8_t m  = regs[2];   // month + century bit
    uint8_t y  = regs[3];   // year register

    *day     = bin2dec_bcd((d & MAX31331_DAY_MSK)   >> MAX31331_DAY_POS);
    *date    = bin2dec_bcd((dt & MAX31331_DATE_MSK) >> MAX31331_DATE_POS);  // lower 6 bits = date (1–31)
    *month   = bin2dec_bcd((m & MAX31331_MONTH_MSK) >> MAX31331_MONTH_POS); // lower 5 bits = month (1–12)
    *century = (m & MAX31331_CENT_MSK) >> MAX31331_CENT_POS;
    *year    = bin2dec_bcd(y & MAX31331_YEAR_MSK) >> MAX31331_YEAR_POS;        // year (00–99)

    return ret;
}

int max31331_set_clko(const max31331_ctx_t *ctx, uint8_t enable, uint8_t frequency) {
    if (!ctx) return -1;

    uint8_t cfg = 0;
    cfg |= (frequency          << MAX31331_CONFIG2_CLKO_HZ_POS)     & MAX31331_CONFIG2_CLKO_HZ_MSK;
    cfg |= ((enable ? 1 : 0)   << MAX31331_CONFIG2_CLKO_ENCLKO_POS) & MAX31331_CONFIG2_CLKO_ENCLKO_MSK;

    return max31331_write_reg(ctx, MAX31331_RTC_CONFIG2, &cfg, 1);
}

int max31331_calibrate_rtc(const max31331_ctx_t *ctx, float measured_frequency) {
    float error_ppm = ((measured_frequency - 32768.0f) * 1e6f) / 32768.0f;
    int16_t compword = (int16_t)(error_ppm / 0.477f);

    uint8_t hi = (uint8_t)((compword >> 8) & 0xFF);  // high byte
    uint8_t lo = (uint8_t)(compword & 0xFF);         // low byte

    uint8_t set_comp[] = {hi, lo};

    int32_t ret = 0;
    ret = max31331_write_reg(ctx, MAX31331_OFFSET_HIGH, set_comp, sizeof(set_comp));

    return ret;
}

int max31331_set_timer_config(const max31331_ctx_t *ctx, uint8_t enable, uint8_t pause, uint8_t repeat, uint8_t frequency) {
    if (!ctx) return -1;

    uint8_t tcfg = 0;

    tcfg |= ((frequency & 0x03) << MAX31331_TIMER_CFG_TFRQ_POS)     & MAX31331_TIMER_CFG_TFRQ_MSK;
    tcfg |= ((enable ? 1 : 0)   << MAX31331_TIMER_CFG_TEN_POS)      & MAX31331_TIMER_CFG_TEN_MSK;
    tcfg |= ((repeat ? 1 : 0)   << MAX31331_TIMER_CFG_TRPT_POS)     & MAX31331_TIMER_CFG_TRPT_MSK;
    tcfg |= ((pause ? 1 : 0)    << MAX31331_TIMER_CFG_TPAUSE_POS)   & MAX31331_TIMER_CFG_TPAUSE_MSK;

    return max31331_write_reg(ctx, MAX31331_TIMER_CONFIG, &tcfg, 1);
}

int max31331_set_timer_init(const max31331_ctx_t *ctx, uint8_t val) {
    if (!ctx) return -1;
    return max31331_write_reg(ctx, MAX31331_TIMER_INIT, &val, 1);
}

int max31331_set_int_en(const max31331_ctx_t *ctx, uint8_t DOSF, uint8_t PFAILE, uint8_t VBATLOWIE, uint8_t DIE, uint8_t TIE, uint8_t A2IE, uint8_t A1IE) {
    if (!ctx) return -1;

    uint8_t int_en = 0;

    int_en |= ((DOSF      ? 1 : 0) << MAX31331_INT_EN_DOSF_POS)      & MAX31331_INT_EN_DOSF_MSK;
    int_en |= ((PFAILE    ? 1 : 0) << MAX31331_INT_EN_PFAILE_POS)    & MAX31331_INT_EN_PFAILE_MSK;
    int_en |= ((VBATLOWIE ? 1 : 0) << MAX31331_INT_EN_VBATLOWIE_POS) & MAX31331_INT_EN_VBATLOWIE_MSK;
    int_en |= ((DIE       ? 1 : 0) << MAX31331_INT_EN_DIE_POS)       & MAX31331_INT_EN_DIE_MSK;
    int_en |= ((TIE       ? 1 : 0) << MAX31331_INT_EN_TIE_POS)       & MAX31331_INT_EN_TIE_MSK;
    int_en |= ((A2IE      ? 1 : 0) << MAX31331_INT_EN_A2IE_POS)      & MAX31331_INT_EN_A2IE_MSK;
    int_en |= ((A1IE      ? 1 : 0) << MAX31331_INT_EN_A1IE_POS)      & MAX31331_INT_EN_A1IE_MSK;

    return max31331_write_reg(ctx, MAX31331_RTC_INT_EN, &int_en, 1);
}

int max31331_get_int_en(const max31331_ctx_t *ctx, uint8_t *int_en) {
    if (!ctx) return -1;

    return max31331_read_reg(ctx, MAX31331_RTC_INT_EN, int_en, 1);
}