const addon = require('bindings')('build-napi-with-cmake');

// console.log(addon.startUnit("nexus-sync"));
// console.log(addon.restartUnit("nexus-sync"));
// console.log(addon.stopUnit("nexus-sync"));


module.exports = {
  stop: addon.stopUnit,
  start: addon.startUnit,
  restart: addon.restartUnit
}