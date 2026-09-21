require("dotenv").config();

const mqtt = require("mqtt");

const cliente = mqtt.connect(process.env.MQTT_BROKER);

cliente.on("connect", () => {
    const leituraTeste = {
        temperatura: 26.5,
        umidade: 68.2,
        luminosidade: 350,
        velocidade: 4.8,
        angulo: 90,
        direcao: "Leste",
        statusDHT: true
    };

    cliente.publish(
        process.env.MQTT_TOPIC,
        JSON.stringify(leituraTeste),
        {},
        (erro) => {
            if (erro) {
                console.error("Erro ao publicar teste:", erro.message);
            } else {
                console.log("Leitura de teste publicada no MQTT.");
            }

            cliente.end();
        }
    );
});

cliente.on("error", (erro) => {
    console.error("Erro na conexão MQTT:", erro.message);
});