const express = require('express');
const mongoose = require('mongoose');
const { Nuxt, Builder } = require('nuxt');
const router = express.Router();
const dotenv = require("dotenv");
dotenv.config();


const app = express();
app.use(express.json())

// establish db connection
const mongoDB = process.env.ATLAS_URI;
mongoose.connect(mongoDB, { useNewUrlParser: true, useUnifiedTopology: true });
const db = mongoose.connection;
db.on("error", console.error.bind(console, "MongoDB connection error:"));


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
    warning: String,
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


//---------------------//
//-----Sensor data-----//
//---------------------//

app.post('/reading', async (req, res) => {
    let reading = req.body;
    console.log("new sensor reading: ", reading);
    reading = new Sensor(reading);
    try {
        await reading.save();
        //send websocket message with data if there is a connection
        response.send(user);
        res.status(200).send(':)');
      } catch (error) {
        res.status(500).send(error);
      }

});

app.get('/sensorData', async (req, res) => {
    console.log('sensorData route')
    try {
        const sensorData = await Sensor.find();
        console.log(sensorData[0])
        res.send(sensorData);
    } catch {
        console.log(error)
        res.status(500).send(error);
    }
});

//------------------//
//---- Warnings ----//
//------------------//

app.post('/warning', async (req, res) => {
    let warning = req.body;
    console.log("new warning: ", warning);
    warning = new Warning(warning);
    try {
        await warning.save();
        //send websocket message with data if there is a connection
        response.send(user);
        res.status(200).send(':)');
      } catch (error) {
        res.status(500).send(error);
      }
});

app.get('/getWarning', async (req, res) => {
    console.log('sensorData route')
    try {
        const sensorData = await Sensor.find();
        console.log(sensorData[0])
        res.send(sensorData);
    } catch {
        console.log(error)
        res.status(500).send(error);
    }
});

app.delete('/deleteWarning', async (req, res) => {
    try {
        //do this better
        await warning.save();
        //send websocket message with data if there is a connection
        response.send(user);
        res.status(200).send(':)');
      } catch (error) {
        res.status(500).send(error);
      }
});


//--------------------//
//---- Parameters ----//
//--------------------//

app.get('/getParams', async (req, res) => {
    console.log('getParams route')
    try {
        const sensorData = await Params.find();
        console.log(sensorData[0])
        res.send(sensorData);
    } catch {
        console.log(error)
        res.status(500).send(error);
    }
});

app.post('/setParams', async (req, res) => {
    console.log('setParams route')
    try {
        const sensorData = await Sensor.find();
        console.log(sensorData[0])
        res.send(sensorData);
    } catch {
        console.log(error)
        res.status(500).send(error);
    }
});

app.patch('/updateParams', async (req, res) => {
    console.log('updateParams route')
    try {
        const sensorData = await Sensor.find();
        console.log(sensorData[0])
        res.send(sensorData);
    } catch {
        console.log(error)
        res.status(500).send(error);
    }
});




// nuxt config
const config = require('../nuxt.config.js');


// boot up the server
async function start() {
    const nuxt = new Nuxt(config);

    const { HOST, PORT } = process.env;
    const host = HOST || (!config.dev ? '0.0.0.0' : 'localhost')
    const port = PORT || (!config.dev ? 80 : 3000);

    app.set('port', port);

    //build only in dev mode
    if(process.env.NODE_ENV === 'development') {
        const builder = new Builder(nuxt);
        await builder.build();
    }

    console.log(host, port);

    // Give nuxt middleware to express
    app.use(nuxt.render);
    app.use(router);

    app.listen(port, host);
    console.log(`server listerning on http://${host}:${port}`);
}

start();
