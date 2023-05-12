const express = require('express');
const mongoose = require('mongoose');
const { Nuxt, Builder } = require('nuxt');
const router = require('./api/index.js');
const { createServer } = require('http');
const { Server } = require('socket.io');
const { Sensor, Warning, Params} = require('./db_schema/index.js')

const app = express();
const httpServer = createServer(app);

const dotenv = require("dotenv");
dotenv.config();
const io = new Server(httpServer);

io.on("connection", (socket) => {
    console.log("socket connection made");
    //connections.push(socket);
})

// --------------//
// -----test-----//
// --------------//
router.post('/fakeReading', async (req, res) => {

    let reading = req.body;
    reading.time = new Date(reading.time)
    console.log("new FAKE sensor reading: ", reading);
    reading = new Sensor(reading);
    try {
        let response = await reading.save();
        console.log(response);

        //broadcast the reading through websockets
        io.emit("newReading", reading)

        res.status(200).send(JSON.stringify({ response: 'Reading accepted'} ));
      } catch (error) {
        res.status(500).send(JSON.stringify({ response: error }));
      }
});

router.post('/deviceReading', async (req, res) => {

    let reading = req.body.data;
    console.log(reading);
    //console.log(message.length%4);
    reading = atob(reading);
    console.log(reading);
    reading = JSON.parse(reading);
    console.log(reading);

    reading.time = new Date(reading.time)
    console.log("new device sensor reading: ", reading);
    reading = new Sensor(reading);
    try {
        let response = await reading.save();
        console.log(response);

        //broadcast the reading through websockets
        io.emit("newReading", reading)

        res.status(200).send(JSON.stringify({ response: 'Reading accepted'} ));
      } catch (error) {
        res.status(500).send(JSON.stringify({ response: error }));
      }
});

router.post('/warning', async (req, res) => {

    let warning = req.body;
    console.log("new warning: ", warning);
    warning = new Warning(warning);
    try {
        await warning.save();
        //send websocket message with data if there is a connection
        io.emit("newWarning", warning)

        response.send(user);
        res.status(200).send(':)');
      } catch (error) {
        res.status(500).send(error);
      }
});

app.use(express.json())

// establish db connection
const mongoDB = process.env.ATLAS_URI;
mongoose.connect(mongoDB, { useNewUrlParser: true, useUnifiedTopology: true });
const db = mongoose.connection;
db.on("error", console.error.bind(console, "MongoDB connection error:"));


// use router
app.use(router)

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

    httpServer.listen(port, host);
    console.log(`server listerning on http://${host}:${port}`);
}

start();
