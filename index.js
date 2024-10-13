const addon = require('bindings')('build-napi-with-cmake');


module.exports = {
  stop: addon.stopUnit,
  start: addon.startUnit,
  restart: addon.restartUnit
}