let BME280 = {
  _gsd: ffi('bool mgos_bme280_get_sensor_data(void *data)'),
  _gds: ffi('void *get_data_struct(void)'),
  _gtemp: ffi('double get_temperature(void *data)'),
  _gpres: ffi('double get_pressure(void *data)'),
  _ghum: ffi('double get_humidity(void *data)'),
  read: function () {
    let cdata = this._gds();
    let ok = this._gsd(cdata);
    if (ok) {
      let data = {
        temperature: this._gtemp(cdata),
        pressure: this._gpres(cdata),
        humidity: this._ghum(cdata)
      };
      return data
    }

    return null;
  }
};
