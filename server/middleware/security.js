var md5 = require('md5');
// var CryptoJS = require('crypto-js');
var aesjs = require('aes-js');

// initialize keys and AES module
// var AESKey = '000102030405060708090A0B0C0D0E0F';
// var key = CryptoJS.enc.Hex.parse(AESKey);

const key = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 ];
const iv = [21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36];

// global variables
const MAC_SIZE = 16;
const failedStatusCode = 401;

module.exports = function() {
    return function secured(req, res, next) {
        let ciphertext = req.body.payload;
        var encryptedHex = aesjs.utils.hex.fromBytes(ciphertext);
        var encryptedBytes = aesjs.utils.hex.toBytes(encryptedHex);
        var aesCbc = new aesjs.ModeOfOperation.cbc(key, iv);
        var decryptedBytes = aesCbc.decrypt(encryptedBytes);


        let buff = Buffer.from(decryptedBytes, 'base64');
        let decryptedHex = buff.toString('hex');
        let text = buff.toString('ascii');

        console.log("text: ", text, text.length);
        text = text.split('')


        // seperate MAC from message
        let MAC = text.splice(112 - MAC_SIZE, MAC_SIZE).map((el) => {return el.charCodeAt(0)});
        console.log('oldMAC: ', MAC);
        let message = text;

        // generate new MAC from message
        let newMAC = gen_MAC(message);

        // compare MACs
        if (MACs_MATCH(MAC, newMAC)) {
            console.log("Success, The MACs Match");
            message = removePadding(message).join('')
            console.log('removepadding', message);
            req.body = message
            return next();
        } else {
            console.log('Failure, the MACs do not match');
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

function MACs_MATCH(MAC, newMAC){
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
    console.log("newMAC: ", newMAC);
    return newMAC
}
