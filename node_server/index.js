
const express = require('express');
const cors = require('cors')
const parser = require('body-parser')
const app = express();
require('dotenv').config();
//CONFIGURACION DE WEBSOCKET
const http = require('http').Server(app);
//////// Configuracion de WebSockets (OPEN) /////////
const io = require('socket.io')(http, {
  cors: {
    origin: ['*'],
    methods: ['GET', 'POST']
  }
});
const Socket = require('./socket');
const socket = new Socket();
socket.initSocket(io);
//////// Configuracion de WebSockets (CLOSE) ///////

const allowedOrigins = ['http://localhost:4200', '192.168.100.114'];
const corsOptions = {
  origin: function (origin, callback) {

    if (allowedOrigins.indexOf(origin) !== -1) {
      console.log("----------------------------------------------------------------------------------")
      console.log("Origen de peticiones: ",origin)
      callback(null, true);
    } else {
      callback(new Error('Acceso denegado por política de CORS'));
    }
  }
};

// app.use(cors(corsOptions))//Definimos los servidores que pueden acceder
app.use(cors())
app.use(parser.urlencoded({ extended: true, limit: '25mb' }))//Limite de datos por peticion
app.use(express.static("./public")); //Para dar acceso a archivos estaticos

app.get("/", (req, res) => {
    res.status(200).send("Hello server");
})

http.listen(process.env.PORT, () => {
    console.log("servidor ejecutandose...");
})