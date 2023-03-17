const express = require('express');
const router = express.Router();
const { Sensor, Warning, Params} = require('../db_schema/index.js')
const secured = require('../middleware/security.js')

//---------------------//
//-----Sensor data-----//
//---------------------//

router.post('/reading', secured(), async (req, res) => {
    let reading = req.body;
    console.log("inside reading route: ", reading)
    // console.log("new sensor reading: ", reading);
    // reading = new Sensor(reading);
    // try {
    //     await reading.save();
    //     //send websocket message with data if there is a connection
    //     response.send(user);
    //     res.status(200).send(':)');
    //   } catch (error) {
    //     res.status(500).send(error);
    //   }
});

router.get('/sensorData', async (req, res) => {
    console.log('sensorData route');
    try {
        const sensorData = await Sensor.find();
        console.log(sensorData[0]);
        res.send(sensorData);
    } catch(error) {
        console.log(error);
        res.status(500).send(error);
    }
});

//------------------//
//---- Warnings ----//
//------------------//

router.post('/warning', async (req, res) => {
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

router.get('/getWarning', async (req, res) => {
    console.log('getWarning route')
    try {
        const warnings = await Warning.find();
        warningCode = { warningCode: 0 } //logical or all warnings
        res.send(warningCode);
    } catch(error) {
        console.log(error)
        res.status(500).send(error);
    }
});

router.delete('/deleteWarnings', async (req, res) => {
    try {
        //change this to make sure it deletes all warnings
        await Warning.delete();
        res.status(200).send(':)');
      } catch (error) {
        res.status(500).send(error);
      }
});


//--------------------//
//---- Parameters ----//
//--------------------//

router.get('/getParams', async (req, res) => {
    console.log('getParams route')
    try {
        const params = await Params.find();
        console.log(params[0])
        res.send(sensorData);
    } catch (error)  {
        console.log(error)
        res.status(500).send(error);
    }
});

router.post('/setParams', async (req, res) => {
    console.log('setParams route')
    let params = req.body;
    params = new Params(params);
    try {
        await params.save();
        response.send(user);
        res.status(200).send(':)');
      } catch (error) {
        res.status(500).send(error);
      }
});

router.patch('/updateParams', async (req, res) => {
    console.log('updateParams route')
    try {
        const sensorData = await Params.find();

        res.send(sensorData);
    } catch(error) {
        console.log(error)
        res.status(500).send(error);
    }
});

module.exports = router;
