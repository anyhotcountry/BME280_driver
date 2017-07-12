#include "mgos_i2c.h"

#include "common/cs_dbg.h"
#include "fw/src/mgos_app.h"
#include "fw/src/mgos_mongoose.h"
#include "fw/src/mgos_utils.h"
#include "fw/src/mgos_hal.h"

#include "mgos_bme280.h"
#include "bme280.h"

static struct bme280_dev *dev = 0;

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    struct mgos_i2c *i2c = mgos_i2c_get_global();
    bool rslt = mgos_i2c_read_reg_n(i2c, dev_id, reg_addr, (size_t)len, data);
    return rslt ? BME280_OK : BME280_E_COMM_FAIL;
}

int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    struct mgos_i2c *i2c = mgos_i2c_get_global();
    bool rslt = mgos_i2c_write_reg_n(i2c, dev_id, reg_addr, (size_t)len, data);
    return rslt ? BME280_OK : BME280_E_COMM_FAIL;
}

void user_delay_ms(uint32_t period)
{
    mgos_usleep(period * 1000);
}

bool mgos_bme280_init()
{
    static struct bme280_dev init_dev;
    dev = &init_dev;
    int8_t rslt = BME280_OK;
    init_dev.id = BME280_I2C_ADDR_PRIM;
    init_dev.interface = BME280_I2C_INTF;
    init_dev.read = user_i2c_read;
    init_dev.write = user_i2c_write;
    init_dev.delay_ms = user_delay_ms;
    init_dev.settings.osr_h = BME280_OVERSAMPLING_4X;
    init_dev.settings.osr_p = BME280_OVERSAMPLING_4X;
    init_dev.settings.osr_t = BME280_OVERSAMPLING_4X;

    rslt = bme280_init(&init_dev);
    if (rslt != BME280_OK)
    {
        init_dev.id = BME280_I2C_ADDR_SEC;
        rslt = bme280_init(&init_dev);
    }

    if (rslt != BME280_OK)
    {
        LOG(LL_ERROR, ("BME280 init failed: %d", rslt));
        return false;
    }

	uint8_t settings_sel = BME280_OSR_PRESS_SEL|BME280_OSR_TEMP_SEL|BME280_OSR_HUM_SEL;
	rslt = bme280_set_sensor_settings(settings_sel, dev);
	rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);
	
    /* Give some delay for the sensor to go into normal mode */
	dev->delay_ms(50);

    return rslt == BME280_OK;
}

bool mgos_bme280_get_sensor_data(sensor_data *data)
{
    if (dev == 0)
    {
        mgos_bme280_init();
    }

    int8_t rslt;
    struct bme280_data comp_data;

    rslt = bme280_get_sensor_data(BME280_PRESS | BME280_HUM | BME280_TEMP, &comp_data, dev);
    data->pressure = 0.00001 * comp_data.pressure;
    data->temperature = 0.01 * comp_data.temperature;
    data->humidity = 0.001 * comp_data.humidity;
    return rslt == BME280_OK && data->pressure > 80;
}

bool mgos_bme280_driver_init(void) {
  return true;
}

