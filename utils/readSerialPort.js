var {SerialPort} = require('serialport');
var md5 = require('md5');
var aesjs = require('aes-js')
var fetch = require('node-fetch')

let message = []

// configuration for the serial port
var port = new SerialPort( {
    path: '/dev/cu.usbmodem141401',
    baudRate: 9600,
})


// listen for data on the serial port
port.on('data', async function (data) {

    // data comes in in small chunks,
    // concatenate data Arrays until full message is received
    message = message.concat(Array.from(data))

    // messages are always 112 bytes, check length of message
    if(message.length === 112){
        console.log(message);

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

//-------------------------//
//---- Deprecated Code ----//
//-------------------------//

// initialize keys and AES module
// const key_good = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
// const key_bad = [1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
// const iv = [ 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 ];
// var aes_bad = new aesjs.ModeOfOperation.cbc(key_bad, iv);

function encryptMessage(message) {
    //console.log(message);
    message = aesjs.utils.utf8.toBytes(message);
    //console.log(message);
    message = pad(message)

    // generate MAC
    const hash = hashStringToArray(md5(message));
    var aes = new aesjs.ModeOfOperation.cbc(key_good, iv); // Reset AES State
    const MAC = aes.encrypt(hash);

    // Concatenate message and MAC and encrypt
    let payload = concat(message, MAC)
    var aes = new aesjs.ModeOfOperation.cbc(key_good, iv); // Reset AES state
    payload = aes.encrypt(payload);
    payload = Array.from(payload)

    //use the bad key to generate a bad payload the server will reject
    const MAC_bad = aes_bad.encrypt(hash);
    const payload_bad = Array.from(aes_bad.encrypt(payload));

    return [payload, payload_bad];

}

function pad(message) {
    let paddingSize = 16 - (message.length % 16)
    padding = new Uint8Array(paddingSize).fill(0)
    var paddedArray = new Uint8Array(message.length + paddingSize);
    paddedArray.set(message);
    paddedArray.set(padding, message.length);
    return paddedArray
}

function hashStringToArray(string) {
    const arr = new Uint8Array(16)
    for (let i = 0; i < 32; i += 2){
        arr[i/2] = parseInt(string.substr(i, 2), 16);
    }
    return arr;
}

function concat(message, MAC) {
    var concatArray = new Uint8Array(message.length + MAC.length);
    concatArray.set(message);
    concatArray.set(MAC, message.length);
    return concatArray
}
