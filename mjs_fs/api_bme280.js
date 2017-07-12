let BME280 = {
  _gsd: ffi('bool mgos_bme280_get_sensor_data(sensor_data *data)'),

  read: function() {
    let data = {
		temperature: NaN,
		pressure: NaN,
		humidity: NaN
	};
    let ok = this._gsd(data);
    return ok ? data : null;
  }
};
