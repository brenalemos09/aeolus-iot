const mqtt = require("mqtt");
const banco = require("./db");

function iniciarConsumidorMQTT() {
    const cliente = mqtt.connect(process.env.MQTT_BROKER);

    cliente.on("connect", () => {
        console.log("Backend conectado ao broker MQTT.");

        cliente.subscribe(process.env.MQTT_TOPIC, (erro) => {
            if (erro) {
                console.error("Erro ao assinar tópico MQTT:", erro.message);
                return;
            }

            console.log(`Tópico assinado: ${process.env.MQTT_TOPIC}`);
        });
    });

    cliente.on("message", async (topico, mensagem) => {
        if (topico !== process.env.MQTT_TOPIC) {
            return;
        }

        try {
            const dados = JSON.parse(mensagem.toString());

            const {
                temperatura,
                umidade,
                luminosidade,
                velocidade,
                angulo,
                direcao,
                statusDHT
            } = dados;

            const camposObrigatorios = [
                temperatura,
                umidade,
                luminosidade,
                velocidade,
                angulo,
                direcao,
                statusDHT
            ];

            if (camposObrigatorios.some((campo) => campo === undefined)) {
                throw new Error("Mensagem MQTT com campos ausentes.");
            }

            await banco.query(
                `INSERT INTO leituras_aeolus
                (temperatura, umidade, luminosidade, velocidade, angulo, direcao, status_dht)
                VALUES ($1, $2, $3, $4, $5, $6, $7)`,
                [
                    temperatura,
                    umidade,
                    luminosidade,
                    velocidade,
                    angulo,
                    direcao,
                    statusDHT
                ]
            );

            console.log("Leitura MQTT salva no PostgreSQL.");
        } catch (erro) {
            console.error("Erro ao processar mensagem MQTT:", erro.message);
        }
    });

    cliente.on("error", (erro) => {
        console.error("Erro na conexão MQTT:", erro.message);
    });

    return cliente;
}

module.exports = iniciarConsumidorMQTT;