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
    tds: Number,
});
const Sensor = mongoose.model('Sensor', sensorSchema);

const warningSchema = new mongoose.Schema({
    warningCode: Number,
});
const Warning = mongoose.model('Warning', warningSchema);

const paramSchema = new mongoose.Schema({
    temp: Array,
    ph: Array,
    flow: Array,
    tds: Array,
    cdt: Array,
    tb: Array,
    freq: Number,
});
const Params = mongoose.model('Params', paramSchema);

module.exports = { Sensor, Warning, Params }
