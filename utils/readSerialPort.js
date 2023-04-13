var {SerialPort} = require('serialport');
var md5 = require('md5');
var aesjs = require('aes-js')
var fetch = require('node-fetch')

let message = [];

// configuration for the serial port
var port = new SerialPort( {
    path: '/dev/cu.usbmodem141101',
    baudRate: 9600,
})


// listen for data on the serial port
port.on('data', async function (data) {

    // data comes in in small chunks,
    // concatenate data Arrays until full message is received
    message = message.concat(Array.from(data))

    // messages are always 112 bytes, check length of message
    if(message.length === 112){
        console.log('read encrypted message from serialport');
        console.log(Buffer.from(message, 'base64').toString('ascii'));

        // convert array of bytes to String of letters
        let result = message.map(x => {return String.fromCharCode(x)}).join('');

        //send to the server
        let res = await postData(message);
        console.log(res);

        // reset the message
        message = [];
    }
})


//--------------------------//
//---- Helper Functions ----//
//--------------------------//

async function postData(payload) {
    try{
        const response = await fetch('http://localhost:3000/reading', {
            method: 'post',
            body: JSON.stringify({payload: payload}),
            headers: {'Content-Type': 'application/json'}
        });
        const data = await response.json();
        return data
    } catch (err) {
        console.log(err)
    }
}
