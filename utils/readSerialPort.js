var {SerialPort} = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline')

var md5 = require('md5');
var aesjs = require('aes-js')
var fetch = require('node-fetch')
var http = require("http");

// initialize keys and AES module
const key_good = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
const key_bad = [1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15];
const iv = [ 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,35, 36 ];
var aes_bad = new aesjs.ModeOfOperation.cbc(key_bad, iv);

let message = [];

var port = new SerialPort( {
    path: '/dev/cu.usbmodem141401',
    baudRate: 9600,
})

var result = "";

port.on('data', async function (data) {
    for(let i = 0; i < data.length; i++){
        message.push(data[i]);
    }
    if(message[message.length -1 ] === 0x7D){
        console.log('end of message');

     for (let i = 0; i < message.length; i++) {
       result += String.fromCharCode(message[i]);
     }

     console.log('message: ', result)
     let [encryptedMessage, badEncryptedMessage] = encryptMessage(message);
     let response = await sendMessage(encryptedMessage);
     console.log(response);
     response = await sendMessage(badEncryptedMessage);
     console.log(response);

     result = "";
     message = [];
    }
})

async function sendMessage(payload) {
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

function encryptMessage(message) {
    console.log(message);
    message = aesjs.utils.utf8.toBytes(message);
    console.log(message);
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
