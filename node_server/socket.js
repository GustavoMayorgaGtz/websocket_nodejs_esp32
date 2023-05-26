class Socket {
    constructor() {
    }

    initSocket(io) {
        io.on('connection', async (socket) => {
            console.log("Usuario conectado socket.io");
            const { nameRoom } = socket.handshake.query;
            socket.join(nameRoom);
            const clientIP = socket.handshake.address;
            console.log('IP del cliente:', clientIP);

            // // Unirse a un grupo
            socket.on('joinGroup', (groupName) => {
                const name = groupName.group;
                socket.join(name);
                console.log(`Cliente se unió al grupo: ${name}`);
            });

            // Enviar un mensaje a un grupo específico
            socket.on('sendMessageToGroup', (data) => {
                const { groupName, message } = data;
                io.to(groupName).emit('message', message);
                console.log(`Mensaje enviado al grupo ${groupName}: ${message}`);
            });

            // Dejar un grupo
            socket.on('leaveGroup', (groupName) => {
                socket.leave(groupName);
                console.log(`Cliente abandonó el grupo: ${groupName}`);
            });
            socket.on('disconnect', () => {
                console.log("User disconnect");
            })
        });
    }
}

module.exports = Socket;