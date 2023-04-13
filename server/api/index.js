const express = require('express');
const router = express.Router();
const { Sensor, Warning, Params} = require('../db_schema/index.js')
const secured = require('../middleware/security.js')
// const ARIMA = require('arima')

//--------------//
//-----test-----//
//--------------//
router.post('/test', async (req, res) => {
    let message = req.body;
    // let reading = JSON.parse(req.body);
    console.log("test message: ", message);
    try {
        // send websocket message with data if there is a connection
        res.status(200).send(JSON.stringify({ response: 'Reading accepted'} ));
      } catch (error) {
        res.status(500).send(JSON.stringify({ response: error }));
      }
});


//---------------------//
//-----Sensor data-----//
//---------------------//
router.post('/mockReading', async (req, res) => {
    let reading = req.body;
    // let reading = JSON.parse(req.body);
    reading.time = new Date(reading.time)
    console.log("new sensor reading: ", reading);
    reading = new Sensor(reading);
    try {
        let response = await reading.save();
        console.log(response);
        // send websocket message with data if there is a connection
        res.status(200).send(JSON.stringify({ response: 'Reading accepted'} ));
      } catch (error) {
        res.status(500).send(JSON.stringify({ response: error }));
      }
});

router.post('/reading', secured(), async (req, res) => {
    // let reading = req.body;
    let reading = JSON.parse(req.body);
    reading.time = new Date(reading.time)
    console.log("new sensor reading: ", reading);
    reading = new Sensor(reading);
    try {
        await reading.save();
        // send websocket message with data if there is a connection
        res.status(200).send(JSON.stringify({ response: 'Reading accepted'} ));
      } catch (error) {
        res.status(500).send(JSON.stringify({ response: error }));
      }
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
        console.log("warnings: ", warnings);
        warningCode = 0
        warnings.map((el, index) => {
            let oldWarningCode = warningCode
            console.log(typeof(warningCode), typeof(oldWarningCode));
            warningCode = warningCode | el.warningCode;
            console.log(`${warningCode} = ${oldWarningCode} || ${el.warningCode}`);
        })
        console.log("warningCode: ", warningCode);
        res.status(200).send(JSON.stringify({warningCode: warningCode}));
    } catch(error) {
        console.log(error)
        res.status(500).send(error);
    }
});

router.delete('/deleteWarnings', async (req, res) => {
    try {
        //change this to make sure it deletes all warnings
        await Warning.deleteMany({});
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
        console.log('params: ', params)
        res.send(params);
    } catch (error)  {
        console.log(error)
        res.status(500).send(error);
    }
});

router.patch('/updateParams', async (req, res) => {
    console.log('updateParams route')
    let newParams = req.body
    try {
        const params = await Params.findOne();
        for (val in newParams) {
            params[val] = newParams[val]
        }
        params.save();
    } catch(error) {
        console.log(error)
        res.status(500).send(error);
    }
});

// router.post('/sarimax', async (req, res) => {
//     console.log('sarimax route');
//     let ts = req.body.ts;
//     console.log(ts.length);
//     const arima = new ARIMA({ auto: true }).fit(ts);
//     console.log(arima);
//     const [pred, errors] = arima.predict(500);
//     res.send(JSON.stringify({pred: pred, errors: errors}))
// });

module.exports = router;
