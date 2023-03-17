const express = require('express');
const mongoose = require('mongoose');
const { Nuxt, Builder } = require('nuxt');
const router = require('./api/index.js');

const dotenv = require("dotenv");
dotenv.config();


const app = express();
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

    app.listen(port, host);
    console.log(`server listerning on http://${host}:${port}`);
}

start();
