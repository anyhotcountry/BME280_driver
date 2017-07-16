let BME280 = {
  _gsd: ffi('bool mgos_bme280_get_sensor_data(void *data)'),

  read: function() {
    let data = {
		temperature: null,
		pressure: null,
		humidity: null
	};
    let ok = this._gsd(data);
    return ok ? data : null;
  }
};
