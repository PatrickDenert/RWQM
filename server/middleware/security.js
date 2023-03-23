var md5 = require('md5');
// var CryptoJS = require('crypto-js');
var aesjs = require('aes-js');

const key = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 ];
const iv = [21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36];

// global variables
const MAC_SIZE = 16;
const failedStatusCode = 401;

module.exports = function() {
    return function secured(req, res, next) {
        //logging for demo
        console.log('received an encryped message');
        console.log('message: ',  Buffer.from(req.body.payload, 'base64').toString('ascii'));
        console.log('decrypting...');

        // reset AES state and decrypt the payload
        var aesCbc = new aesjs.ModeOfOperation.cbc(key, iv);
        var decryptedBytes = aesCbc.decrypt(req.body.payload);



        // change formatting from base64 to ascii
        let decryptedASCIIPayload = Buffer.from(decryptedBytes, 'base64').toString('ascii')

        // seperate MAC from message
        decryptedASCIIPayload = decryptedASCIIPayload.split(''); //split string into array of chars
        let mcuMAC = decryptedASCIIPayload.splice(112 - MAC_SIZE, MAC_SIZE).map((el) => {return el.charCodeAt(0)}); //remove mac and convert from char to int
        let sensorData = decryptedASCIIPayload; // raname to sensorData because the mac has been removed

        // generate new MAC from message
        let serverMAC = gen_MAC(sensorData);

        // compare MACs
        console.log('checking for message authenticity');
        if (MACs_are_equal(mcuMAC, serverMAC)) {

            // convert sensorData from arrray to string with no delimiter
            sensorData = removePadding(sensorData).join('')

            // logging for demo
            console.log("Success, The MACs Match, message is authentic");
            console.log('Decrypted Message: ', sensorData);

            // replace full request body with only the data it needs
            req.body = sensorData

            // pass to request handler
            return next();
        } else {
            console.log('Failure, the MACs do not match, the message is not authentic');
            return res.status(500).send(JSON.stringify({ response: 'Credentials Not Valid' }));
        }
    };
};

function hashStringToArray(string) {
    const arr = new Uint8Array(16)
    for (let i = 0; i < 32; i += 2){
        arr[i/2] = parseInt(string.substr(i, 2), 16);
    }
    return arr;
}

function MACs_are_equal(MAC, newMAC){
    for(let i = 0; i < MAC.length; i++){
        if(MAC[i] !== newMAC[i]){ return false }
    }
    return true;
}


function removePadding(message) {
    while(message[message.length-1] != '}'){
        message.pop()
    }
    return message
}

function gen_MAC(message) {
    let newMAC = Array.from(key);
    message.map((el, i) => {newMAC[i%16] ^= el.charCodeAt(0)});
    return newMAC
}
