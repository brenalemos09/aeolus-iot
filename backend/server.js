require("dotenv").config();

const express = require("express");
const cors = require("cors");
const banco = require("./db");
const iniciarConsumidorMQTT = require("./consumidor-mqtt");

const app = express();
const porta = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

app.get("/api/status", async (req, res) => {
    try {
        await banco.query("SELECT 1");

        res.json({
            servidor: "online",
            banco: "conectado"
        });
    } catch (erro) {
        res.status(500).json({
            servidor: "online",
            banco: "erro",
            mensagem: erro.message
        });
    }
});

app.get("/api/leituras", async (req, res) => {
    try {
        const resultado = await banco.query(`
            SELECT *
            FROM (
                SELECT
                    id,
                    temperatura::FLOAT AS temperatura,
                    umidade::FLOAT AS umidade,
                    luminosidade,
                    velocidade::FLOAT AS velocidade,
                    angulo,
                    direcao,
                    status_dht,
                    criado_em
                FROM leituras_aeolus
                ORDER BY criado_em DESC
                LIMIT 100
            ) AS leituras_recentes
            ORDER BY criado_em ASC
        `);

        res.json(resultado.rows);
    } catch (erro) {
        res.status(500).json({
            erro: "Não foi possível consultar o histórico.",
            mensagem: erro.message
        });
    }
});

iniciarConsumidorMQTT();

app.listen(porta, () => {
    console.log(`Servidor Aeolus rodando em http://localhost:${porta}`);
});