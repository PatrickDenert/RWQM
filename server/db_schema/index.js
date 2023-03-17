const mongoose = require('mongoose');

//-------------------------//
//---- Database Schema ----//
//-------------------------//

const sensorSchema = new mongoose.Schema({
    time: String,
    temp: Number,
    ph: Number,
    flow: Number,
    cdt: Number,
    tb: Number,
});
const Sensor = mongoose.model('Sensor', sensorSchema);

const warningSchema = new mongoose.Schema({
    warning: Number,
});
const Warning = mongoose.model('Warning', warningSchema);

const paramSchema = new mongoose.Schema({
    time: Array,
    temp: Array,
    ph: Array,
    flow: Array,
    cdt: Array,
    tb: Array,
    freq: Number,
});
const Params = mongoose.model('Params', paramSchema);

module.exports = { Sensor, Warning, Params }
